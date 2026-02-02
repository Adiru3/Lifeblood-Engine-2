#include "Network/UDPSocket.h"
#include <iostream>

UDPSocket::UDPSocket() : sock(INVALID_SOCKET) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

UDPSocket::~UDPSocket() {
    Close();
    WSACleanup();
}

bool UDPSocket::Create() {
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        return false;
    }
    
    // Non-blocking mode
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);
    return true;
}

bool UDPSocket::Bind(int port) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

bool UDPSocket::Send(const std::string& data, const std::string& ip, int port) {
    sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &dest.sin_addr);

    int sent = sendto(sock, data.c_str(), (int)data.size(), 0, (struct sockaddr*)&dest, sizeof(dest));
    return sent != SOCKET_ERROR;
}

bool UDPSocket::Receive(char* buffer, int size, std::string& outIp, int& outPort) {
    sockaddr_in sender;
    int senderLen = sizeof(sender);
    
    int received = recvfrom(sock, buffer, size, 0, (struct sockaddr*)&sender, &senderLen);
    
    if (received > 0) {
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &sender.sin_addr, ipStr, INET_ADDRSTRLEN);
        outIp = std::string(ipStr);
        outPort = ntohs(sender.sin_port);
        return true;
    }
    return false;
}

void UDPSocket::Close() {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
}
