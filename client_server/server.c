#define _POSIX_C_SOURCE 199309L 

#define RED "\033[31m"
#define RESET "\033[0m"

#include "../sockety/socket.h"
#include <signal.h>
#include <sys/prctl.h>


#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "../zdrojove_kody/UI.h"



void * vlaknoPrijmaniaSpojenia(void * arg) {
    socket_server_t * server = arg;
    while (atomic_load(&server->server_bezi)) {
        socket_server_accept_connection(server);
        
    }
    pthread_exit(NULL);
}





svt_t * nastav_server(socket_server_t * server) {
    char buf[500];
    
    pthread_mutex_lock(&server->mutex);
    socket_data_t klient = server->klienti[server->hlavny_klient]->socket_pocuvaj;
    pthread_mutex_unlock(&server->mutex);
    socket_read(&klient, buf, sizeof(buf));
    char * prva  = strchr(buf, ';');

    char  medzi_step[20];                               //rozmerx;rozmery;svetprekazky;pocet_replikacii;...
    strncpy(medzi_step, buf, prva - buf); //potrebujem poslat rozmery sveta x,y   svet prekazky 1 = true , 0 = normal
    medzi_step[prva - buf] = '\0';                  // dalej pocet replikacii (kym niesom v cieli) 
    char * nova_pomocna = prva + 1;                       // max pocet krokov K, pravdepodobnosti
    int rozmer_x = atoi(medzi_step);                 // cesta k suboru
    
    prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna] = '\0';   
    int rozmer_y = atoi(medzi_step);
    nova_pomocna = prva + 1;

    prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna] = '\0';   
    _Bool prekazky = (_Bool)atoi(medzi_step);
    nova_pomocna = prva + 1;

    prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna] = '\0';   
    int pocet_replikacii = atoi(medzi_step);
    nova_pomocna = prva + 1;

    

    prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna] = '\0';   
    int pocet_krokov_K = atoi(medzi_step);
    nova_pomocna = prva + 1;

    prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna] = '\0';   
    float pravd_hore = strtof(medzi_step, NULL);
    nova_pomocna = prva + 1;

    prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna] = '\0';   
    float pravd_dole = strtof(medzi_step, NULL);
    nova_pomocna = prva + 1;

     prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna] = '\0';   
    float pravd_vpravo = strtof(medzi_step, NULL);
    nova_pomocna = prva + 1;

     prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna] = '\0';   
    float pravd_vlavo = strtof(medzi_step, NULL);
    nova_pomocna = prva + 1;

     prva = strchr(nova_pomocna, ';');
    char cesta_k_suboru[100];
    strncpy(cesta_k_suboru, nova_pomocna, prva - nova_pomocna);
    cesta_k_suboru[prva - nova_pomocna] = '\0';   
    nova_pomocna = prva + 1;

   
    
    prvd_t pravdepodobnosti;
    pravdepodobnosti.hore = pravd_hore;
    pravdepodobnosti.dole = pravd_dole;
    pravdepodobnosti.vlavo = pravd_vlavo;
    pravdepodobnosti.vpravo = pravd_vpravo;
    svt_t * navrat;
        //printf("pravdepodobnost hore: %f\n", pravdepodobnosti.hore);
        //printf("pravdepodobnost dole: %f\n", pravdepodobnosti.dole);
        //printf("pravdepodobnost vpravo: %f\n", pravdepodobnosti.vpravo);
        //printf("pravdepodobnost vlavo: %f\n", pravdepodobnosti.vlavo);
        //sleep(5);
    if (prekazky) {
        navrat = svet_init_prekazky(rozmer_x, rozmer_y, 20, pravdepodobnosti, pocet_krokov_K, pocet_replikacii, cesta_k_suboru);
    } else {
        navrat = svet_init_normal(rozmer_x, rozmer_y, pravdepodobnosti, pocet_krokov_K, pocet_replikacii, cesta_k_suboru);
    }

   
    return navrat;
    
}

svt_t * server_info_subor(socket_server_t * server) {
    char buf[250];
    socket_read(&server->klienti[server->hlavny_klient]->socket_pocuvaj, buf, sizeof(buf));
    if (buf[0] - '0' == 7) {
        //true treba subor
        char cesta_k_suboru[100];
        memcpy(cesta_k_suboru, buf + 2, strlen(buf + 2) + 1);
        return svet_nacitaj_zo_suboru(cesta_k_suboru);
        
    } else if (buf[0] - '0' == 5) {
        return NULL;
    } else {
        perror(RED "Chyba komunikácie server-klient." RESET);
        exit(EXIT_FAILURE);
    }

}


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

