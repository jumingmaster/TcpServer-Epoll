#include "Server.hpp"
#include "IPSet.hpp"

int main(int argc, char **argv)
{
        IPSet setting;
        setting.setIP();
        setting.setPort ();

        Server *server = new Server();
        server->serverLoop ();
        
        delete (server);
        return 0;
}
