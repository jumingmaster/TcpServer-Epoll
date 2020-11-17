#include "IPSet.hpp"

void IPSet::setIP (std::string ipAddr) {
        Tcp::serverIP = ipAddr;
}

void IPSet::setPort (in_port_t port) {
        Tcp::serverPort = port;
}

void IPSet::setFamily (sa_family_t family) {
        Tcp::famliy = family;
}

void IPSet::setMaxWaitClient (uint8_t num) {
        Tcp::maxWaitClient = num;
}

void IPSet::setMaxClient (uint32_t num) {
        Tcp::maxClient = num;
}