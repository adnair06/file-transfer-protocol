#pragma once

#include <array>
#include <cstdint>

enum class PacketType {
    START, 
    DATA, 
    ACK, 
    END, 
    ERR
};

struct Packet {
    PacketType type; 
    uint32_t sequence_number; 
    uint32_t data_length; 
    std::array<char, 4096> data; 
};