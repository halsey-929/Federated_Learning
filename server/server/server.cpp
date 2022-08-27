#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<cstdlib>
#include<cstring>
#include<winsock2.h>
#include"_Matrix.h"
#include<vector>
#include<thread>
#include<mutex>
#include"_BP.h"
//#include"_Read.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;


bool flag = 1;
char Ip[20][200];
int iConnect = 0;
int updated_client_num = 0;

void str_to_double(const string str, vector<double>& data);

string double_to_str(const double a)
{
	char str[17];
	long long c;
	c = ((long long*)(&a))[0];
	for (int i = 0; i < 16; i++)
	{
		str[15 - i] = (c & ((long long)0x0f)) + 'a';
		c >>= 4;
	}
	str[16] = '\0';
	long long x = 0;
	for (int i = 0; i < 16; i++)
	{
		x = (x << 4) + (int)(str[i] - 'a');
	}
	return string(str);
}

void str_to_double(const string str, vector<double>& data)
{
	for (int i = 0; i < str.length() - 15; i += 16)
	{
		long long tmp = 0;
		for (int j = i; j < i + 16; j++)
		{
			tmp = (tmp << 4) + (int)(str[j] - 'a');
		}
		data.push_back(((double*)(&tmp))[0]);
	}
}
vector<_Matrix> D;
vector<_Matrix> tmp;
struct client_thread
{
	SOCKET mS;
	int id;
	string ip;
};
mutex socket_c;
condition_variable condv;
vector<bool> tag;

bool check()
{
	return flag;
}

int threadpro(client_thread p)
{
	SOCKET hsock = p.mS;
	char buffer[1025];
	char sendBuffer[1024];
	recv(hsock, buffer, 1025, 0);
	p.ip = string(buffer);
	if (hsock != INVALID_SOCKET)
		cout << "Start receive information from IP:" << p.ip <<" Cline id: "<<p.id <<endl << endl;
	/*_Matrix t;
	recv(hsock, (char*)&t, sizeof(t), 0);
	t.show();*/
	while (1)//循环接收发送的内容
	{
		//发送权重
		string str = double_to_str(D.size());
		send(hsock, str.c_str(), 16 + 1, 0);
		recv(hsock, buffer, 1025, 0);
		cout << "Sending Weight to cline" << p.id << "...............";
		if (string(buffer) != string("over"))
			return 0;
		for (int cnt = 0; cnt < D.size(); cnt++)
		{
			//cout << "here";
			str.clear();
			unique_lock<mutex> tlk(socket_c);
			const auto M = D[cnt];
			tlk.unlock();
			str = (double_to_str(M.get_row_num()) + double_to_str(M.get_col_num())).c_str();
			send(hsock, str.c_str(), 2 * 16 + 1, 0);
			recv(hsock, buffer, 1025, 0);
			if (string(buffer) != string("over"))
				return 0;
			int row = M.get_row_num();
			int col = M.get_col_num();
			int total = row * col;
			int currow = 0;
			int curcol = 0;
			while (total > 0)
			{
				str.clear();
				int cur = min(total, 64);
				for (int i = 0; i < cur; i++)
				{
					str += double_to_str(M.get_value(currow, curcol));
					curcol++;
					currow += (curcol >= col) ? (curcol -= col, 1) : 0;
				}
				total -= cur;
				send(hsock, str.c_str(), cur * 16 + 1, 0);
				recv(hsock, buffer, 1025, 0);
				if (string(buffer) != string("over"))
					return 0;
			}
		}
		cout << "Finished!\n";


		vector<_Matrix> data;
		//接收权重
		recv(hsock, buffer, 1025, 0);
		send(hsock, "over", 5, 0);
		cout << "Receiving Weight from cline" << p.id << "...............";
		vector<double> num;
		str_to_double(string(buffer), num);
		//cout << num[0];
		for (int cnt = 0; cnt < num[0]; cnt++)
		{
			//cout << "here";
			recv(hsock, buffer, 1025, 0);
			send(hsock, "over", 5, 0);
			vector<double> s;
			str_to_double(string(buffer), s);
			_Matrix N(s[0], s[1]);
			int row = s[0];
			int col = s[1];
			int total = row * col;
			int currow = 0;
			int curcol = 0;
			//cout << row << ' ' << col << '\n';
			while (total > 0)
			{
				vector<double> tmp;
				recv(hsock, buffer, 1025, 0);
				send(hsock, "over", 5, 0);
				str_to_double(string(buffer), tmp);
				int cur = min(total, 64);
				for (int i = 0; i < cur; i++)
				{
					N.set_value(currow, curcol, tmp[i]);
					curcol++;
					currow += (curcol >= col) ? (curcol -= col, 1) : 0;
				}
				total -= cur;
			}
			//N.show();
			data.push_back(N);
		}
		cout << "Finished\n";

		//cout << tag[p.id];
		unique_lock<mutex> lck(socket_c);
		while (!tag[p.id])
			condv.wait(lck);
		for (int i = 0; i < D.size(); i++)
			tmp[i] += data[i];
		tag[p.id] = 0;
		updated_client_num += 1;
		condv.notify_all();
		while (!tag[p.id])
			condv.wait(lck);
		condv.notify_all();
		lck.unlock();

	}
	return 0;
}


_BP model(784, 10);

void deal(void)
{
	while (1)
	{
		//cout << tag.size();
		//for (int i = 0; i < tag.size(); i++)
		//	cout << tag[i];
		unique_lock<mutex> lck(socket_c);
		condv.wait(lck, []() {bool f = 0; for (auto l : tag)f |= l; return (!f)&(tag.size()!=0); });
		cout << "Averaging Weight...";
		for (int i = 0; i < tag.size(); i++)
		{
			tag[i] = 1;
		}
		if (updated_client_num > 0)
		{
			for (int i = 0; i < D.size(); i++)
			{
				D[i] = tmp[i] * (1.0 / updated_client_num);
				tmp[i] *= 0;
			}
			updated_client_num = 0;
		}
		cout << "Finished!\n";
		lck.unlock();
		condv.notify_all();
	}
}
int main(void)
{
	//model.add(400);
	//model.add(400);
	D = model.get_weight();
	tmp = D;
	for (auto M : tmp)
		M *= 0;

	vector<thread> thread_list;
	thread main_thread(deal);

	/// /// /// /// /// /// /// 
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd))
	{
		printf("Initlalization Error!");
		return -1;
	}
	SOCKET m_SockServer;
	sockaddr_in serveraddr;
	sockaddr_in serveraddrfrom;
	SOCKET m_Server[20];

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(4600);

	serveraddr.sin_addr.S_un.S_addr = inet_addr("192.168.1.103");

	m_SockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int i = bind(m_SockServer, (sockaddr*)&serveraddr, sizeof(serveraddr));
	cout << "bind:" << i << endl << endl;

	int iMaxConnect = 20;
	int iLisRet;
	char buf[] = "THIS IS SERVER\0";
	char WarnBuf[] = "It,is voer Max connect\0";
	int len = sizeof(serveraddr);

	while (1)
	{
		iLisRet = listen(m_SockServer, 0);

		int temp = 0;
		int Len = sizeof(serveraddrfrom);
		m_Server[iConnect] = accept(m_SockServer, (sockaddr*)&serveraddrfrom, &len);
		
		if (m_Server[iConnect] != INVALID_SOCKET)
		{
			if (getsockname(m_Server[iConnect], (struct sockaddr*)&serveraddrfrom, &Len) != -1)
			{
				printf("listen address = %s:%d\n", inet_ntoa(serveraddrfrom.sin_addr), ntohs(serveraddrfrom.sin_port));
				sprintf(Ip[iConnect], "%s", inet_ntoa(serveraddrfrom.sin_addr));
			}
			else
			{
				printf("getsockanme error\n");
				exit(0);
			}
			int ires = send(m_Server[iConnect], buf, sizeof(buf), 0);
			cout << "accept" << ires << endl << endl;

			iConnect++;
			/// /// /// /// /// 

			
			//cout << "locked\n";
			unique_lock<mutex> lck(socket_c);
			tag.push_back(0);
			lck.unlock();
			condv.notify_all();
			//cout << "unlocked\n";
			if (iConnect > iMaxConnect)
			{
				int ires = send(m_Server[iConnect], WarnBuf, sizeof(WarnBuf), 0);
			}
			else
			{
				client_thread tmp;
				tmp.mS = m_Server[iConnect - 1];
				tmp.id = iConnect - 1;
				thread_list.push_back(thread(threadpro, tmp));
			}
		}
		
	}
	main_thread.join();
	WSACleanup();
}