#pragma once

#include "Tcp.hpp"
#include <string>

class IPSet : Tcp {

public:
        void setIP (std::string ipAddr = "127.0.0.1");
        void setPort (in_port_t port = 6666);
        void setFamily (sa_family_t family = AF_INET);
        void setMaxWaitClient (uint8_t num = 20);
        void setMaxClient (uint32_t num = 1000);
};