#include <string>
#include <iostream> 
#include "packet.hpp"

Packet p;
std::string text = "Hello my name is Abhishek this is a small example datapoint."; 

int main() {
    for (size_t i = 0; i < text.size(); i++) {
        p.data[i] = text[i];
    }

    p.data_length = text.length(); 

    for (size_t i = 0; i < p.data_length; i++) {
        std::cout << p.data[i]; 
    }
    std::cout << "\n"; 
}