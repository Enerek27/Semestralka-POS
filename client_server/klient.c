
//TODO
// treba potom zmenit na svet.h


#include "../zdrojove_kody/UI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../sockety/socket.h"
#include "../zdielanaPamat/pipe.h"

int main(int argc, char const *argv[])
{

   
   

    socket_client_t socket_client;

    socket_client_init(&socket_client, "127.0.0.1", "777");

    char * sprava = "Posielam spravu na server";
    socket_write(&socket_client.activeSocket, sprava, strlen(sprava));

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));

    socket_read(&socket_client.activeSocket, buffer, sizeof(buffer));

    printf("Sprava co poslal server je: %s\n", buffer);

    socket_client_destroy(&socket_client);
    
 /*
    pipe_data_t klient;

    pipe_init(&klient, "../datovod", 0);

    char * sprava = "Posielam spravu na server";
    pipe_open_write(&klient);
    pipe_write(&klient, sprava, strlen(sprava));
    
    pipe_destroy(&klient, 0, 0);
    */
    return 0;

}
