#pragma once
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#include <string>
#include <optional>
#include <vector>
#include "Window.h"

struct fileinfo
{
	char filename[1000];
	uint64_t filesize;
};

namespace Networking
{
	std::vector<char> CreateFileDescPacket(const std::string& filename, const uint64_t& filesz);
	bool StartWinSock();
	void CleanupWinSock();
}
class Client
{
public:
	Client() {};
	int Poll(int timeout, bool pollout);
	bool SetBlocking(bool blocking);
	bool Init(const std::string & ipaddress, int _port);
	bool TryToConnect();
	bool isConnected()const
	{
		return isconnected;
	}
	bool Recv(std::vector<char>& buff, size_t length, Window& outputto);
	bool senddata(std::vector<char>& buffer, Window & outputto);
	~Client();
private:
	bool CreateSocket();
private:
	sockaddr_in serverinfo;
	std::string ipAddress;
	int port;
	bool isconnected = false;  
	SOCKET sock;
	static constexpr size_t maxbytesatatime = 50000000; //mb//mb
	//static constexpr size_t maxbytesatatime = 1; //mb
};