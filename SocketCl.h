#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <winbase.h>

#include ".\socketcl.h"
#include <string>
using namespace std;
using std::string;

#pragma comment (lib, "Ws2_32.lib") //ccc
#pragma comment (lib, "Mswsock.lib")
//#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 65536

class Socket
{
public:
	Socket();
	Socket(string new_ip, string new_port, unsigned short attempt);
	~Socket(void);
	bool MakeSocket();
	bool SendData(char* sendText);
	bool ShutdownSocket();
	bool ReceiveData();
	char errMsg[100];
	char recvbufAll[DEFAULT_BUFLEN];
	bool IsError();
	bool IsReceiveAll();
	string def_ip;
	string def_port;
	int delay_seconds;
private:
	bool receivedAllData; //ознака отримання цілого повідомлення
	bool error;
	SOCKET ConnectSocket;
	addrinfo *ptrAddrInfo;
};
