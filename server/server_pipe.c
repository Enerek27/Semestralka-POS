#define _POSIX_C_SOURCE 199309L 
#include <sched.h>



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
    
    server_pipe_t server;
    atomic_store(&server.server_info.vypni_server, 0);
    atomic_store(&server.server_info.chcem_statistiku, 0);
    atomic_store(&server.server_info.prepni_mod, 0);
    atomic_store(&server.server_info.prazdne, 0);
    
    pipe_init(&server.pip_read, "../pipe_write", 1);
    pipe_init(&server.pip_write, "../pipe_read", 1);
    
    pipe_open_read(&server.pip_read);
   
    pipe_open_write(&server.pip_write);
    
    atomic_store(&server.server_bezi, 1);
    
    svt_t * skuska = server_info_subor(NULL, &server, 1);
    svt_t * svet;
    
    
    if (skuska == NULL) {
        svet = nastav_server(NULL, &server, 1);
        
         vypis_pipe_t vypisovac;
        
        vypisovac.server = &server;
        vypisovac.svet = svet;
       
        
        posli_vsetkym_svet(NULL, &vypisovac, 1);
        sleep(1);
        generuj_pravdepodobnost(svet);
       
        generuj_priem_krok(svet);
        
       
    } else {
        svet = skuska;
    }
    pthread_t vlakienko;
    //treba spravit nekonecny loop kde sa bude posielat ci sa ma vypnut alebo nie bude to aj cakaci loop
    
   
    pthread_create(&vlakienko, NULL, nacuvajklientovi_pipe, &server);
    
   
    
    vypis_pipe_t * svet_vypis;
    svet_vypis = calloc(1, sizeof(vypis_pipe_t));
    if (svet_vypis == NULL) {
        perror(RED "Chyba vytvárania simulácie pri alokovaní pamäte." RESET);
        exit(EXIT_FAILURE);
    }
    svet_vypis->svet = svet;
    svet_vypis->server = &server;
    //treba dorobit ukoncenie a posielanie a skoncenie v sledovaci
    
    sleep(4);
    while (atomic_load(&server.server_bezi)) {
        if (atomic_load(&server.server_info.prepni_mod)) {
            
            
            posli_vsetkym_statistiku(NULL, svet_vypis, 1);
            
        } else {
            server_vykonavaj_sim(NULL, svet_vypis, 1);
            if (!atomic_load(&server.server_info.prepni_mod)) {
                atomic_store(&server.server_bezi, 0);
            } 
        }
    }
    
   
   
    pthread_join(vlakienko, NULL);
    

   
    
    free(svet_vypis);
    
    
    char buff[4];
    buff[0] = 'o';
    buff[1] = 'f';
    buff[2] = 'f';
    buff[3] = '\0';
  
    pipe_write(&server.pip_write, buff, 4);
    
  
    struct timespec ts = {0, 300 * 1000000}; 
    nanosleep(&ts, NULL);
    svet_uloz_do_suboru(svet);
    svet_destroy(svet);
    
    pipe_destroy(&server.pip_read, 1,0);
    pipe_destroy(&server.pip_write, 1,0);
   

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

