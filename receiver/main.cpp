#include <iostream> 

#include <winsock2.h> //Contains all functions to create a socket, bind to an IP address and recieve from. 
#include <ws2tcpip.h> //Contains helper functions to work with IP addresses. 

#include "packet.hpp"

int main() {
    WSADATA wsaData; 

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cout << "WSAStartup failed." << '\n'; 
        return 1; 
    }

    SOCKET receiver_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    if (receiver_socket == INVALID_SOCKET) {
        std::cout << "Failed to create socket." << '\n'; 
        WSACleanup(); 
        return 1; 
    }
    /*
    SOCKET tells windows to make a new network endpoint. 
    AF_INET -> IPv4 
    SOCK_DGRAM -> UDP protocol 
    IPPROTO_UDP -> Use UDP  
    */

    sockaddr_in receiver_address{}; 
    receiver_address.sin_family = AF_INET; 
    receiver_address.sin_port = htons(5000); 
    receiver_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
    
    int bind_result = bind(receiver_socket, reinterpret_cast<sockaddr*>(&receiver_address), sizeof(receiver_address)); 
    if (bind_result == SOCKET_ERROR) {
        std::cout << "Bind failed." << '\n'; 
        closesocket(receiver_socket); 
        WSACleanup(); 
        return 1; 
    }

    std::cout << "Listening on 127.0.0.1:5000...\n";

    Packet packet; 

    sockaddr_in sender_address{}; 
    int sender_address_size = sizeof(sender_address); 

    int bytes_received = recvfrom(receiver_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0, reinterpret_cast<sockaddr*>(&sender_address), &sender_address_size); 
    if (bytes_received == SOCKET_ERROR) {
        std::cout << "Receive failed." << '\n'; 
        closesocket(receiver_socket); 
        WSACleanup(); 
        return 1; 
    }

    if (bytes_received != sizeof(packet)) {
        std::cout << "Incomplete packet received." << '\n'; 
        closesocket(receiver_socket);
        WSACleanup();
        return 1; 
    }

    std::cout << "Received packet!\n";
    std::cout << "Type: " << static_cast<int>(packet.type) << '\n';
    std::cout << "Sequence: " << packet.sequence_number << '\n';
    std::cout << "Length: " << packet.data_length << '\n';

    for (size_t i = 0; i < packet.data_length; i++)
    {
        std::cout << packet.data[i];
    }

    std::cout << '\n';
    closesocket(receiver_socket);
    WSACleanup();

    return 0;
}

