#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<cstdlib>
#include<time.h>
#include<WinSock2.h>
#include<vector>
#include"_Matrix.h"
#include"_BP.h"
#include"_Read.h"
#include<random>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
#define DATA_RATE 0.3
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
int main(void)
{
	char ip[] = "192.168.1.103";
	WSADATA wsd;
	int cur;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	SOCKET m_SockClient;

	sockaddr_in clientaddr;
	
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(4600);
	clientaddr.sin_addr.S_un.S_addr = inet_addr("192.168.1.103");
	m_SockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int i = connect(m_SockClient, (sockaddr*)&clientaddr, sizeof(clientaddr));
	cout << "Connection status  " << i << endl;
	char buffer[1025];
	char inBuf[1024];
	int num;
	num = recv(m_SockClient, buffer, 1024, 0);
	send(m_SockClient, ip, sizeof(ip), 0);
	if (num > 0)
	{
		cout << "Receive from server" << buffer << endl;
		_BP model(784, 10, 0.005);

		vector<_Matrix> train;
		vector<_Matrix> label;
		cout << "Reading Data.......";
		test(train, label);
		cout << "Finished!\n";

		_Matrix Weight;


		//send(m_SockClient, (char*)&Weight, sizeof(Weight), 0);

		while (1)
		{
			cout << "Random_shuffling...............";
			vector<int> id(train.size());
			for (int i = 0; i < train.size(); i++)
				id[i] = i;
			shuffle(id.begin(), id.end(), mt19937(1331));
			for (int i = 0; i < id.size(); i++)
			{
				swap(train[i], train[id[i]]);
				swap(label[i], label[id[i]]);
			}
			cout << "Finished!\n";
			cout << "Receiving weights...............";
			vector<_Matrix> data;
			//接收权重
			{
				recv(m_SockClient, buffer, 1025, 0);
				send(m_SockClient, "over", 5, 0);
				vector<double> num;
				str_to_double(string(buffer), num);
				//cout << num[0];
				for (int cnt = 0; cnt < num[0]; cnt++)
				{
					//cout << "here";
					recv(m_SockClient, buffer, 1025, 0);
					send(m_SockClient, "over", 5, 0);
					vector<double> s;
					str_to_double(string(buffer), s);
					_Matrix N(s[0], s[1]);
					int row = s[0];
					int col = s[1];
					int total = row * col;
					int currow = 0;
					int curcol = 0;
					cout << row << ' ' << col << '\n';
					while (total > 0)
					{
						vector<double> tmp;
						recv(m_SockClient, buffer, 1025, 0);
						send(m_SockClient, "over", 5, 0);
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
			}
			cout << "Finished!\n";
			cout << "Fiting...............";
			model.set_weights(data);
			model.fit(train, label, DATA_RATE);
			data = model.get_weight();
			cout << "Finished.\n";
			cout << "Sending weight...............";
			//发送权重
			{
				string str = double_to_str(data.size());
				send(m_SockClient, str.c_str(), 16 + 1, 0);
				recv(m_SockClient, buffer, 1025, 0);
				if (string(buffer) != string("over"))
					return 0;
				for (int cnt = 0; cnt < data.size(); cnt++)
				{
					//cout << "here";
					str.clear();
					const auto& M = data[cnt];
					str = (double_to_str(M.get_row_num()) + double_to_str(M.get_col_num())).c_str();
					send(m_SockClient, str.c_str(), 2 * 16 + 1, 0);
					recv(m_SockClient, buffer, 1025, 0);
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
						send(m_SockClient, str.c_str(), cur * 16 + 1, 0);
						recv(m_SockClient, buffer, 1025, 0);
						if (string(buffer) != string("over"))
							return 0;
					}
				}
			}
			cout << "Finish!\n";
			cout << "Calculating Accuracy...............\n";
			int num = train.size();
			int cnt = 0;
			for (int i = 0; i < num*DATA_RATE; i++)
			{
				_Matrix ans = model.predict(train[i]);
				double mx = 0;
				int id = 0;
				for (int i = 0; i < 10; i++)
				{
					if (mx < ans.get_value(0, i))
					{
						mx = ans.get_value(0, i);
						id = i;
					}
				}
				if (label[i].get_value(0, id) == 1)
					cnt++;
			}
			cout << "acc: ";
			cout << double(cnt) / (num * DATA_RATE) * 100 << "%\n";
		}
	}
	printf("\n\n???");
	system("pause");
}