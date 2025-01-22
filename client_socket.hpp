#pragma once
#define NOMINMAX
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

class ClientSocket {
public:
    ClientSocket() : sock(INVALID_SOCKET) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("Failed to initialize Winsock");
        }
    }

    ~ClientSocket() {
        if (sock != INVALID_SOCKET) {
            closesocket(sock);
        }
        WSACleanup();
    }

    bool connect(const std::string& host, int port) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            return false;
        }

        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);

        if (::connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            closesocket(sock);
            sock = INVALID_SOCKET;
            return false;
        }
        return true;
    }

    bool send_data(const std::string& data) {
        if (sock == INVALID_SOCKET) return false;
        return send(sock, data.c_str(), data.length(), 0) != SOCKET_ERROR;
    }

    std::string receive_data() {
        if (sock == INVALID_SOCKET) return "";
        char buffer[1024];
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            return std::string(buffer);
        }
        return "";
    }

private:
    SOCKET sock;
};
