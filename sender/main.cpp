#include <iostream> 
#include <string> 
#include <algorithm>

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

    SOCKET sender_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    if (sender_socket == INVALID_SOCKET) {
        std::cout << "Failed to create socket" << '\n'; 
        WSACleanup(); 
        return 1;
    }

    Packet packet; 
    std::string data = "Hello, world!"; 
    packet.data_length = data.length();
    packet.data.fill(0); 
    std::copy(data.begin(), data.end(), packet.data.begin()); 
    packet.type = PacketType::DATA; 
    packet.sequence_number = 0; 

    sockaddr_in receiver_address{}; 
    receiver_address.sin_family = AF_INET; 
    receiver_address.sin_port = htons(5000); 
    receiver_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
    
    int receiver_address_size = sizeof(receiver_address); 
    int bytes_sent = sendto(sender_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0, reinterpret_cast<sockaddr*>(&receiver_address), receiver_address_size);

    if (bytes_sent == SOCKET_ERROR) {
        std::cout << "Send failed." << '\n'; 

        closesocket(sender_socket); 
        WSACleanup(); 
        return 1; 
    }

    std::cout << "Packet sent successfully!\n";
    closesocket(sender_socket); 
    WSACleanup(); 
    return 0; 
}
