
#include "../sockety/socket.h"
#include <stdio.h>
#include <string.h>
#include "../zdielanaPamat/pipe.h"

int main(int argc, char const *argv[])
{
    
    
    socket_server_t socket_server;
    socket_server_init(&socket_server, 777);

    socket_server_accept_connection(&socket_server);

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));


    socket_read(&socket_server.activeSocket, buffer, 256);

    printf("Prijata sprava: %s\n", buffer);

    char * odpoved = "dobre dosli";
    socket_write(&socket_server.activeSocket, odpoved, strlen(odpoved));
    socket_server_destroy(&socket_server);
    /*
    pipe_data_t server;

    pipe_init(&server, "../datovod", 1);

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));

    pipe_open_read(&server);
    pipe_read(&server, buffer, sizeof(buffer) );

    printf("Prijata sprava: %s\n", buffer);
    
    pipe_destroy(&server, 1, 0);
    */
    return 0; 

}