#pragma once
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class UDPSocket {
public:
    UDPSocket();
    ~UDPSocket();

    bool Create();
    bool Bind(int port);
    bool Send(const std::string& data, const std::string& ip, int port);
    bool Receive(char* buffer, int size, std::string& outIp, int& outPort);
    void Close();

private:
    SOCKET sock;
    struct sockaddr_in serverAddr;
};
