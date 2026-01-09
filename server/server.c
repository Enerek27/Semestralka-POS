#define _POSIX_C_SOURCE 199309L 


#include "../sockety/socket.h"
#include <signal.h>
#include <sys/prctl.h>


#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "../server_lib/pomoc_server.h"



int main(int argc, char const *argv[])
{
    prctl(PR_SET_PDEATHSIG, SIGTERM);
    srand(time(NULL)); 
    socket_server_t socket_server;
    socket_server_init(&socket_server, 2000);
    socket_server_accept_connection(&socket_server);
    svt_t * skuska = server_info_subor(&socket_server);
    svt_t * svet;
    
    if (skuska == NULL) {
        svet = nastav_server(&socket_server);
        
        svt_vp_t  vypisovac;
        
        vypisovac.server = &socket_server;
        vypisovac.svet = svet;
       
        
        posli_vsetkym_svet(&vypisovac);
        sleep(1);
        generuj_pravdepodobnost(svet);
       
        generuj_priem_krok(svet);
        
       
    } else {
        svet = skuska;
    }
    pthread_t vlakienko[3];
    //treba spravit nekonecny loop kde sa bude posielat ci sa ma vypnut alebo nie bude to aj cakaci loop
    
    pthread_create(&vlakienko[0], NULL, vlaknoPrijmaniaSpojenia, &socket_server);
    
    pthread_create(&vlakienko[1], NULL, nacuvajklientovi, socket_server.klienti[socket_server.hlavny_klient]);
    
    pthread_create(&vlakienko[2], NULL, cisti_server, &socket_server);
    
    svt_brd_t * svet_vypis;
    svet_vypis = calloc(1, sizeof(svt_brd_t));
    if (svet_vypis == NULL) {
        perror(RED "Chyba vytvárania simulácie pri alokovaní pamäte." RESET);
        exit(EXIT_FAILURE);
    }
    svet_vypis->svet = svet;
    svet_vypis->server = &socket_server;
    //treba dorobit ukoncenie a posielanie a skoncenie v sledovaci
    
    while (atomic_load(&socket_server.server_bezi)) {
        if (atomic_load(&socket_server.server_info.sumarny_mod)) {
            
            
            posli_vsetkym_statistiku(svet_vypis);
            
        } else {
            server_vykonavaj_sim(svet_vypis);
            if (!atomic_load(&socket_server.server_info.sumarny_mod)) {
                atomic_store(&socket_server.server_bezi, 0);
            } 
        }
    }
    
    client_zavri(&socket_server);
    
    struct timespec ts = {0, 400 * 1000000}; 
    nanosleep(&ts, NULL);
   shutdown(socket_server.passiveSocket.socket, SHUT_RDWR);
   
    pthread_join(vlakienko[0], NULL);
    
    socket_destroy(&socket_server.passiveSocket);
   
    
    
    pthread_join(vlakienko[1], NULL);
   
    pthread_join(vlakienko[2], NULL);
   
    free(svet_vypis);
    
    socket_posli_klientom_end(&socket_server);
    nanosleep(&ts, NULL);
    svet_uloz_do_suboru(svet);
    svet_destroy(svet);
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

