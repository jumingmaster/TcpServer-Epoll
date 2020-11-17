#include "Server.hpp"
#include <string.h>

int Tcp::sockfd = -1;
int Server::epfd = -1;
int Tcp::clients = 0;
struct epoll_event *Server::events = NULL;

void saSigaction(int signo, siginfo_t *siginfo, void *data)
{

        std::cout
            << "Recv the sign:\v"
            << sys_siglist[signo]
            << std::endl
            << "si_code:"
            << siginfo->si_code
            << std::endl;
        switch (signo)
        {

        case SIGUSR1:
                std::cout << "SIGUSR1: " << siginfo->si_value.sival_ptr << std::endl;

        case SIGINT:
                close(Tcp::sockfd);
                close(Server::epfd);
                free(Server::events);
                std::cout << "The Server Closed." << std::endl;
                exit(EXIT_FAILURE);

        case SIGUSR2:
                std::cout << "SIGUSR2: " << (char *)siginfo->si_value.sival_ptr << std::endl;
                break;

        default:
                break;
        }
}

void *epollLoop(void *arg)
{

        Server *server = (Server *)arg;
        struct Tcp::dataPacket packet;
        int nr_events = 0;
        ssize_t recv_size;

        while (1)
        {

                nr_events = epoll_wait(server->epfd, server->events, server->maxClient, -1);
                if (nr_events < 0)
                        perror("epoll_wait");

                for (int i = 0; i < nr_events; i++)
                {

                        if (server->events[i].data.fd == server->sockfd)
                        {

                                struct sockaddr_in client_addr;
                                socklen_t len = sizeof(client_addr);
                                struct epoll_event ev;
                                ev.events = EPOLLIN;

                                int clientfd = accept(server->sockfd, (struct sockaddr *)&client_addr, &len);

                                ev.data.fd = clientfd;
                                Tcp::clients++;

                                server->events = 
                                (struct epoll_event *)realloc (
                                        server->events, 
                                        sizeof(struct epoll_event) * Tcp::clients);

                                if (!server->events)
                                {
                                        perror("realloc");
                                        /*
                                        * send a signal
                                        * serious mistake
                                        */
                                        sigval_t info;
                                        std::string err = "Allocate Memory for events: ";
                                        err += strerror(errno);
                                        info.sival_ptr = (void *)err.c_str();
                                        sigqueue(getpid(), SIGUSR1, info);
                                        break;
                                }

                                if (epoll_ctl(server->epfd, EPOLL_CTL_ADD, clientfd, &ev) == -1)
                                {

                                        perror("epoll_ctl");
                                        sigval_t info;
                                        std::string err = "call epoll_ctl failed: ";
                                        err += strerror(errno);
                                        info.sival_ptr = (void *)err.c_str();
                                        sigqueue(getpid(), SIGUSR2, info);
                                }

                                printf("new connection[%s]\n", inet_ntoa(client_addr.sin_addr));
                        }

                        else if (server->events[i].events & EPOLLIN)
                        {

                                memset(&packet, 0, sizeof(Tcp::dataPacket));
                                recv_size = recv(
                                    server->events[i].data.fd, &packet, sizeof(Tcp::dataPacket), 0);

                                if (recv_size == 0)
                                {
                                        
                                        //printf("clinet disconnected.\n");
                                        epoll_ctl(
                                            server->epfd, EPOLL_CTL_DEL, server->events[i].data.fd, server->events);
                                        close(server->events[i].data.fd);
                                        
                                        sigval_t info; 
                                        info.sival_ptr = new char[32];
                                        strcpy ((char *)info.sival_ptr, "client disconnected.");
                                        sigqueue(getpid(), SIGUSR2, info);
                                }

                                else if (recv_size == -1)
                                        perror ("recv");
                                
                                
                                //under is the func
                                
                        }
                }
        }
}

void Server::initSocket(void)
{

        using std::cout;
        using std::endl;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd == -1)
        {
                cout
                    << "In " << __FILE__
                    << " [" << __LINE__ << "]:"
                    << __FUNCTION__ << "\v: ";
                
                perror("socket");

                exit(EXIT_FAILURE);
        }
#if __DEBUG__
        cout << "Create socket successed..." << endl;
#endif
        addr.sin_port = serverPort;
        addr.sin_family = famliy;
        addr.sin_addr.s_addr = inet_addr(serverIP.c_str());

        if (bind(
                sockfd,
                (struct sockaddr *)&addr,
                sizeof(struct sockaddr)) == -1)
        {
                cout
                    << "In " << __FILE__
                    << " [" << __LINE__ << "]:"
                    << __FUNCTION__ << "\v: ";
                perror("socket");
                exit(EXIT_FAILURE);
        }

#if __DEBUG__
        cout << "Bind socket successed..." << endl;
#endif

        if (listen(sockfd, maxWaitClient) == -1)
        {

                cout
                    << "In " << __FILE__
                    << " [" << __LINE__ << "]:"
                    << __FUNCTION__ << "\v: ";
                perror("socket");
                exit(EXIT_FAILURE);
        }

#if __DEBUG__
        cout
            << "Starting listen socket...\n"
            << "The Server Address: \"tcp://"
            << inet_ntoa(addr.sin_addr)
            << ":"
            << addr.sin_port
            << "\""
            << endl;
#endif
}

void Server::initEpoll(void)
{

        epfd = epoll_create1(0);

        if (epfd == -1)
        {
                perror("close sockfd failed");
                exit(EXIT_FAILURE);
        }

        events = (struct epoll_event *)malloc(sizeof(struct epoll_event));

        if (!events)
        {
                perror("malloc");
                exit(EXIT_FAILURE);
        }

        setEvent(sockfd, EPOLLIN | EPOLLOUT);
        if (epollAdd(sockfd) == -1)
        {
                perror("epoll event ctl failed");
                exit(EXIT_FAILURE);
        }
}

int Server::epollAdd(int fd)
{
        return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
}

int Server::epollDel(int fd)
{
        return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event);
}

int Server::epollMod(int fd)
{
        return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
}

void Server::setEvent(int fd, int eventType)
{
        event.data.fd = fd;
        event.events = eventType;
}

void Server::setEvent(void *ptr, int eventType)
{
        event.data.ptr = ptr;
        event.events = eventType;
}

void Server::setEvent(uint32_t data, int eventType)
{
        event.data.u32 = data;
        event.events = eventType;
}

void Server::setEvent(uint64_t data, int eventType)
{
        event.data.u64 = data;
        event.events = eventType;
}

void Server::serverLoop(void)
{

        if (pthread_create(&epollTid, NULL, epollLoop, (void *)this))
        {
                perror("Pthread Create");
                exit(EXIT_FAILURE);
        }

        while (1)
                ;
}