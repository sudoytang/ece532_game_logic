#pragma once

#ifdef _WIN32

#ifndef TCP_CLIENT_HPP
#define TCP_CLIENT_HPP



#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")


class TCPClient {
private:
    SOCKET clientSocket;
    std::string serverIP;
    int serverPort;

public:
    TCPClient(const std::string& ip, int port) : serverIP(ip), serverPort(port) {
        clientSocket = INVALID_SOCKET;
    }

    bool connect() {
        // Initialize Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cout << "WSAStartup failed" << std::endl;
            return false;
        }

        // Create socket
        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "Socket creation failed" << std::endl;
            WSACleanup();
            return false;
        }

        // Set server address
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort);
        inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

        // Connect to server
        if (::connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cout << "Connection failed" << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return false;
        }

        return true;
    }

    bool sendData(const char* data, int length) {
        if (send(clientSocket, data, length, 0) == SOCKET_ERROR) {
            std::cout << "Send failed" << std::endl;
            return false;
        }
        return true;
    }

    std::string receiveData(int bufferSize = 1024) {
        char* buffer = new char[bufferSize];
        int bytesReceived = recv(clientSocket, buffer, bufferSize, 0);

        std::string response;
        if (bytesReceived > 0) {
            response = std::string(buffer, bytesReceived);
        }

        delete[] buffer;
        return response;
    }

    void disconnect() {
        closesocket(clientSocket);
        WSACleanup();
    }

    ~TCPClient() {
        disconnect();
    }
};

#endif // TCP_CLIENT_HPP
#endif // _WIN32
