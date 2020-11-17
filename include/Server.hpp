#pragma once
#include "Tcp.hpp"
#include <iostream>
#include <pthread.h>
#include <sys/epoll.h>
#include <signal.h>

void saSigaction(int signo, siginfo_t *siginfo, void *data);

class Server : Tcp
{

        friend void *epollLoop(void *arg);
        friend void saSigaction(int signo, siginfo_t *siginfo, void *data);

public:
        struct sigaction sigAction;

        Server()
        {
                initSocket();
                initEpoll();
                sigAction.sa_flags = SA_SIGINFO;
                sigemptyset(&sigAction.sa_mask);
                sigaddset(&sigAction.sa_mask, SIGQUIT);
                sigAction.sa_sigaction = saSigaction;

                if (sigaction(SIGINT, &sigAction, NULL) < 0)
                {
                        perror("sigaction");
                        exit(EXIT_FAILURE);
                }

                if (sigaction(SIGUSR1, &sigAction, NULL) < 0)
                {
                        perror("sigaction");
                        exit(EXIT_FAILURE);
                }

                if (sigaction(SIGUSR2, &sigAction, NULL) < 0)
                {
                        perror("sigaction");
                        exit(EXIT_FAILURE);
                }
        }

        ~Server()
        {
                threadClose();
                serverClose();
                epollClose();
                free(events);
        }

        /*
        *The undering function is about epoll,
        *please check the function info carefully,
        *       
        */
        /*
        * @func         epollAdd: "EPOLL_CTL_ADD" control of epoll_ctl's packed.
        *
        * @P.S           Attention: before you call this function, must call 
        *                this func: 
        *                "setEvent(variable type, int eventType)"
        *                more info about setEvent please goto the setEvent 's usage.
        * 
        * @pram         fd: fd which you need to monitor
        * @return       the same as epoll_ctl
        */
        int epollAdd(int fd);
        /*
        *@func          epollDel: "EPOLL_CTL_DEL" control of epoll_ctl's packed.
        *
        * @P.S           Attention: before you call this function, must call 
        *                this func: 
        *                "setEvent(variable type, int eventType)"
        *                more info about setEvent please goto the setEvent 's usage.
        * 
        * @pram         fd: fd which you need to monitor
        * @return       the same as epoll_ctl
        */
        int epollDel(int fd);
        /*
        * @func         epollAMod: "EPOLL_CTL_MOD" control of epoll_ctl's packed.
        *
        * @P.S           Attention: before you call this function, must call 
        *                this func: 
        *                "setEvent(variable type, int eventType)"
        *                more info about setEvent please goto the setEvent 's usage.
        * 
        * @pram         fd: fd which you need to monitor
        * @return       the same as epoll_ctl
        */
        int epollMod(int fd);
        /*
        * @func         server main loop
        */
        void serverLoop(void);

private:
        pthread_t epollTid = 0; 
        pthread_t databaseTid = 0;
        pthread_t clientTid = 0;

        void initSocket(void);
        void initEpoll(void);
        void setEvent(int fd, int eventType);
        void setEvent(void *ptr, int eventType);
        void setEvent(uint32_t data, int eventType);
        void setEvent(uint64_t data, int eventType);

        __always_inline void serverClose(void)
        {

                if (close(sockfd) == -1)
                {
                        perror("close sockfd failed");
                        exit(EXIT_FAILURE);
                }

#if __DEBUG__
                std::cout << "Close socket successed..." << std::endl;
#endif
        }

        __always_inline void epollClose(void)
        {

                free(events);

                if (close(sockfd) == -1)
                {
                        perror("close epfd failed");
                        exit(EXIT_FAILURE);
                }

#if __DEBUG__
                std::cout << "Close epfd successed..." << std::endl;
#endif
        }

        __always_inline void threadClose(void)
        {

                if (epollTid)
                        pthread_cancel(epollTid);

                if (clientTid)
                        pthread_cancel(clientTid);

                if (databaseTid)
                        pthread_cancel(databaseTid);
        }

protected:
        static int epfd;
        static struct epoll_event *events;
        struct epoll_event event;
};
