#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string>
#include <arpa/inet.h>
#include <cstddef>
#include <errno.h>


class Tcp {

friend void packing (uint8_t msgType, void *data, struct dataPacket &packet);
friend void *unpacking (struct dataPacket &packet);


public:
        
        enum msgTypeENUM { P2P, getData, SaveData, DelData};

        struct dataPacket {
                int packet_size;
                uint8_t msgType;
                void * data;
        }__attribute__((packed));


protected:
        std::string serverIP = "192.168.34.2";
        in_port_t serverPort = 6666;
        uint8_t maxWaitClient = 20;
        uint32_t maxClient = 1000;
        sa_family_t famliy = AF_INET;
        static int sockfd;
        static int clients;
        struct sockaddr_in addr;
        
};
