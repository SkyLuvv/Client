#include "Client.h"
#include <iostream>
#include "FileStuff.h"
#include <chrono>
#include <thread>
#include "Client.h"



bool Client::SetBlocking(bool blocking)
{
    unsigned long nonBlocking = 1;
    unsigned long Blocking = 0;
    //if passed in true, set to blocking. otherwise set to nonblocking
    auto result{ ioctlsocket(sock, FIONBIO, (blocking) ? &Blocking : &nonBlocking) };

    return (result == SOCKET_ERROR) ? false : true;
}
int Client::Poll(int timeout, bool pollout)
{
    static WSAPOLLFD pollfd{};
    pollfd.fd = sock;
    pollfd.events = (pollout) ? POLLOUT : POLLIN;

    auto num_events{ WSAPoll(&pollfd, 1, timeout) };

    if (num_events == 0)
        return 0;
    if (num_events == SOCKET_ERROR)
        return SOCKET_ERROR;
    if (pollfd.revents & ((pollout) ? POLLOUT : POLLIN))
        return 1;
}
bool Client::CreateSocket()
{
    //create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == INVALID_SOCKET)
        return false;

    //create the socket information
    //this tells our socket what info to use to connect to the server
    serverinfo;
    serverinfo.sin_family = AF_INET;
    //htons converts the port number from host to network byte order
    serverinfo.sin_port = htons(port);
    //since in the client we are specifying the address to connect to,  (the server)
    //we need to convert the string into a form we can send which is its numeric binary form
    //and store that in sin_addr struct
    inet_pton(AF_INET, ipAddress.c_str(), &serverinfo.sin_addr);

    return true;
}
bool Client::Init(const std::string& ipaddress, int _port)
{
    ipAddress = ipaddress;
    port = _port;

    return CreateSocket();
}
bool Client::TryToConnect()
{
    if (!isconnected) {

        if (connect(sock, (sockaddr*)&serverinfo, sizeof(serverinfo)) == SOCKET_ERROR)
            return false;
        else
            return  (isconnected = true);
    }
    return isconnected;

}
bool Client::Recv(std::vector<char> & buff, size_t length, Window& outputto)
{
    size_t totalbytesrecv = 0;
    size_t bytesrecv = 0;
    std::string str; str.reserve(200);

    while (totalbytesrecv != length)
    {
        auto result{ Poll(10000,false) };

        if (result == SOCKET_ERROR) {
            outputto.SetText("Client::senddata failed: Poll return SOCKET_ERROR\r\n");
            return false;
        }
        else if (result == 0)           //recv buffer has no data to receive
            continue;
        else if (result == 1) {         //theres data available to be received

            bytesrecv = recv(sock, buff.data() + totalbytesrecv, buff.size() - totalbytesrecv, 0);
            if (bytesrecv == SOCKET_ERROR || bytesrecv == 0)  //0 means connection closed
                return false;
            totalbytesrecv += bytesrecv;


            str += "Received "; str += std::to_string(((float)totalbytesrecv / (float)length) * 100.0f); str += "%";
            outputto.SetText(str);
            str.clear();
        }
    
    }

    return true;
}
bool Client::senddata(std::vector<char>& buffer, Window& outputto)
{
    size_t totalbytessent = 0;
    size_t length = buffer.size();
    size_t bytessent = 0;
    std::string str; str.reserve(200);

    while (true)
    {

        auto result{ Poll(10000,true) };

        if (result == SOCKET_ERROR) {
            outputto.SetText("Server::senddata failed: Poll returned SOCKET_ERROR\r\n");
            return false;
        }
        else if (result == 0)          //means the send buffer has not sent all the data we passed yet and doesnt have space
            continue;
        else if (result == 1)          //means the send buffer has space
        {

            if (totalbytessent != length)
            {
                if ((length - totalbytessent) > maxbytesatatime)
                    bytessent = send(sock, buffer.data() + totalbytessent, maxbytesatatime, 0);
                else
                    bytessent = send(sock, buffer.data() + totalbytessent, length - totalbytessent, 0);

                if (bytessent == SOCKET_ERROR)
                    return false;
                totalbytessent += bytessent;


                str += "Sent "; str += std::to_string(((float)totalbytessent / (float)length) * 100.0f); str += "%";
                str += " to transport layer. The data is being received...";
                outputto.SetText(str);
                str.clear();

            }
            else
                break;
        }

    }

    return true;
}
Client::~Client()
{   
    closesocket(sock);
}
bool Networking::StartWinSock()
{
    WSADATA wsData;
    int version{ MAKEWORD(2, 2) };

    if (WSAStartup(version, &wsData) == 0)
        return true;
    return false;
}
void Networking::CleanupWinSock()
{
    WSACleanup();
}
std::vector<char> Networking::CreateFileDescPacket(const std::string& filename, const uint64_t& filesz)
{
    //create the packet structure we want to send which contains the file
    //information
    fileinfo fileinf{};
    //copy the filename into the struct
    filename.copy((char*)&fileinf, filename.length());
    fileinf.filesize = filesz;
    //now copy the fileinf structure into a vector<char> we can send passing beg and end of the fileinfo struct
    return std::vector<char>((char*)&fileinf, (char*)&fileinf + sizeof(fileinf));
}