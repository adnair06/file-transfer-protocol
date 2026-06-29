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
    
    Packet errorPacket{}; 
    errorPacket.type = PacketType::ERR; 
    
    std::ifstream input("../sender/send.txt", std::ios::binary); 
    if (!input) {
        std::cout << "Failed to open file." << '\n'; 
        sendto(sender_socket, reinterpret_cast<char*>(&errorPacket), sizeof(errorPacket), 0, reinterpret_cast<sockaddr*>(&receiver_address), receiver_address_size);
        closesocket(sender_socket); 
        WSACleanup();
        return 1; 
    }

    Packet startpacket{}; 
    startpacket.type = PacketType::START; 
    int startBytesSent = sendto(sender_socket, reinterpret_cast<char*>(&startpacket), sizeof(startpacket), 0, reinterpret_cast<sockaddr*>(&receiver_address), receiver_address_size); 
    if (startBytesSent == SOCKET_ERROR) {
        std::cout << "No bytes sent."; 
        sendto(sender_socket, reinterpret_cast<char*>(&errorPacket), sizeof(errorPacket), 0, reinterpret_cast<sockaddr*>(&receiver_address), receiver_address_size);
        closesocket(sender_socket); 
        WSACleanup(); 
        return 1;
    }

    DWORD timeout = 2000; 
    if (setsockopt(sender_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*> (&timeout), sizeof(timeout)) == SOCKET_ERROR) {
        std::cout << "Failed to set socket timeout." << '\n'; 
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

        sockaddr_in ack_receivers_address{}; 
        int ack_receivers_address_size = sizeof(ack_receivers_address); 

        size_t retries = 5; 
        bool acknowledged = false; 
        while (retries--) {
            int bytes_sent = sendto(sender_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0, reinterpret_cast<sockaddr*>(&receiver_address), receiver_address_size); 
            if (bytes_sent == SOCKET_ERROR) {
                std::cout << "No bytes sent."; 
                closesocket(sender_socket); 
                WSACleanup(); 
                return 1; 
            }

            Packet ACKpacket{}; 
            int bytes_received = recvfrom(sender_socket, reinterpret_cast<char*>(&ACKpacket), sizeof(ACKpacket), 0, reinterpret_cast<sockaddr*>(&ack_receivers_address), &ack_receivers_address_size);

            if (bytes_received == SOCKET_ERROR) {
                int error = WSAGetLastError(); 
                if (error == WSAETIMEDOUT) {
                    continue; 
                } else {
                    std::cout << "Failed to receive ACK." << '\n'; 
                    sendto(sender_socket, reinterpret_cast<char*>(&errorPacket), sizeof(errorPacket), 0, reinterpret_cast<sockaddr*>(&receiver_address), receiver_address_size);
                    closesocket(sender_socket); 
                    WSACleanup(); 

                    return 1; 
                }
            }

            if (bytes_received != sizeof(Packet)) {
                std::cout << "Incomplete Packet received." << '\n'; 
                sendto(sender_socket, reinterpret_cast<char*>(&errorPacket), sizeof(errorPacket), 0, reinterpret_cast<sockaddr*>(&receiver_address), receiver_address_size);
                closesocket(sender_socket); 
                WSACleanup(); 
                return 1;
            }

            if (ACKpacket.type == PacketType::ACK && ACKpacket.sequence_number == packet.sequence_number) {
                acknowledged = true; 
                break; 
            } else {
                std::cout << "Corrupted acknowledgement received." << '\n';
                sendto(sender_socket, reinterpret_cast<char*>(&errorPacket), sizeof(errorPacket), 0, reinterpret_cast<sockaddr*>(&receiver_address), receiver_address_size); 
                closesocket(sender_socket); 
                WSACleanup(); 

                return 1; 
            }
        } 

        if (!acknowledged) {
            std::cout << "Packet " << packet.sequence_number << " failed after 5 retries." << '\n'; 
            sendto(sender_socket, reinterpret_cast<char*>(&errorPacket), sizeof(errorPacket), 0, reinterpret_cast<sockaddr*>(&receiver_address), receiver_address_size);
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
        sendto(sender_socket, reinterpret_cast<char*>(&errorPacket), sizeof(errorPacket), 0, reinterpret_cast<sockaddr*>(&receiver_address), receiver_address_size);
        closesocket(sender_socket); 
        WSACleanup(); 
        return 1;
    }

    std::cout << "File sent successfully!\n";
    closesocket(sender_socket); 
    WSACleanup(); 
    return 0; 
}
