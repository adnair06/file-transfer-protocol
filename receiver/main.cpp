#include <iostream> 

#include <winsock2.h> //Contains all functions to create a socket, bind to an IP address and recieve from. 
#include <ws2tcpip.h> //Contains helper functions to work with IP addresses. 

int main() {
    WSADATA wsaData; 

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cout << "WSAStartup failed." << "\n"; 
        return 1; 
    }

    SOCKET reciever_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    /*
    SOCKET tells windows to make a new network endpoint. 
    AF_INET -> IPv4 
    SOCK_DGRAM -> UDP protocol 
    IPPROTO_UDP -> Use UDP  
    */

    sockaddr_in reciever_address{}; 
    reciever_address.sin_family = AF_INET; 
    
}

