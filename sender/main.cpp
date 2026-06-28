#include <iostream> 
#include <string> 
#include <fstream>
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

    sockaddr_in receiver_address{}; 
    receiver_address.sin_family = AF_INET; 
    receiver_address.sin_port = htons(5000); 
    receiver_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
    int receiver_address_size = sizeof(receiver_address); 
    
    std::ifstream input("../sender/send.txt", std::ios::binary); 
    if (!input) {
        std::cout << "Failed to open file." << '\n'; 
        closesocket(sender_socket); 
        WSACleanup();
        return 1; 
    }

    Packet startpacket{}; 
    startpacket.type = PacketType::START; 
    int startBytesSent = sendto(sender_socket, reinterpret_cast<char*>(&startpacket), sizeof(startpacket), 0, reinterpret_cast<sockaddr*>(&receiver_address), receiver_address_size); 
    if (startBytesSent == SOCKET_ERROR) {
        std::cout << "No bytes sent."; 
        closesocket(sender_socket); 
        WSACleanup(); 
        return 1;
    }

    uint32_t sequence_number = 0; 
    while (true) {
        Packet packet{}; 
        input.read(packet.data.data(), packet.data.size()); 
        std::streamsize bytes_read = input.gcount(); 
        if (bytes_read == 0) {
            break; 
        }
        packet.type = PacketType::DATA; 
        packet.sequence_number = sequence_number++; 
        packet.data_length = static_cast<uint32_t>(bytes_read);

        int bytes_sent = sendto(sender_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0, reinterpret_cast<sockaddr*>(&receiver_address), receiver_address_size); 
        if (bytes_sent == SOCKET_ERROR) {
            std::cout << "No bytes sent."; 
            closesocket(sender_socket); 
            WSACleanup(); 
            return 1; 
        }
    }

    Packet endpacket{}; 
    endpacket.type = PacketType::END; 
    int endBytesSent = sendto(sender_socket, reinterpret_cast<char*>(&endpacket), sizeof(endpacket), 0, reinterpret_cast<sockaddr*>(&receiver_address), receiver_address_size);
    if (endBytesSent == SOCKET_ERROR) {
        std::cout << "No bytes sent."; 
        closesocket(sender_socket); 
        WSACleanup(); 
        return 1;
    }

    std::cout << "File sent successfully!\n";
    closesocket(sender_socket); 
    WSACleanup(); 
    return 0; 
}
