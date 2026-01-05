
#include "../sockety/socket.h"

#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../zdrojove_kody/UI.h"



void * vlaknoPrijmaniaSpojenia(void * arg) {
    socket_server_t * server = arg;
    while (atomic_load(&server->server_bezi)) {
        socket_server_accept_connection(server);
    }
}


void pockaj_na_prve_pripojenie(socket_server_t * server) {
    while (1) {
        pthread_mutex_lock(&server->mutex);
        if (server->pocetKlinetov > 0) {
            server->hlavny_klient = server->pocetKlinetov - 1;
            pthread_mutex_unlock(&server->mutex);
            break;
        }
        pthread_mutex_unlock(&server->mutex);
    }
}


svt_t * nastav_server(socket_server_t * server) {
    char buf[500];
    pthread_mutex_lock(&server->mutex);
    socket_data_t klient = server->activeSocket[server->hlavny_klient];
    pthread_mutex_unlock(&server->mutex);
    socket_read(&klient, buf, sizeof(buf));

    char * prva  = strchr(buf, ';');

    char  medzi_step[20];                               //rozmerx;rozmery;svetprekazky;pocet_replikacii;...
    strncpy(medzi_step, buf, prva - buf); //potrebujem poslat rozmery sveta x,y   svet prekazky 1 = true , 0 = normal
    medzi_step[prva - buf + 1] = '\0';                  // dalej pocet replikacii (kym niesom v cieli) 
    char * nova_pomocna = prva + 1;                       // max pocet krokov K, pravdepodobnosti
    int rozmer_x = atoi(medzi_step);                 // cesta k suboru
    
    prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna + 1] = '\0';   
    int rozmer_y = atoi(medzi_step);
    nova_pomocna = prva + 1;

    prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna + 1] = '\0';   
    _Bool prekazky = (_Bool)atoi(medzi_step);
    nova_pomocna = prva + 1;

    prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna + 1] = '\0';   
    int pocet_replikacii = atoi(medzi_step);
    nova_pomocna = prva + 1;

    prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna + 1] = '\0';   
    int pocet_krokov_K = atoi(medzi_step);
    nova_pomocna = prva + 1;

    prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna + 1] = '\0';   
    int pravd_hore = atoi(medzi_step);
    nova_pomocna = prva + 1;

    prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna + 1] = '\0';   
    int pravd_dole = atoi(medzi_step);
    nova_pomocna = prva + 1;

     prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna + 1] = '\0';   
    int pravd_vpravo = atoi(medzi_step);
    nova_pomocna = prva + 1;

     prva = strchr(nova_pomocna, ';');
    strncpy(medzi_step, nova_pomocna, prva - nova_pomocna);
    medzi_step[prva - nova_pomocna + 1] = '\0';   
    int pravd_vlavo = atoi(medzi_step);
    nova_pomocna = prva + 1;

     prva = strchr(nova_pomocna, ';');
    char cesta_k_suboru[100];
    strncpy(cesta_k_suboru, nova_pomocna, prva - nova_pomocna);
    cesta_k_suboru[prva - nova_pomocna + 1] = '\0';   
    
    prvd_t pravdepodobnosti;
    pravdepodobnosti.hore = pravd_hore;
    pravdepodobnosti.dole = pravd_dole;
    pravdepodobnosti.vlavo = pravd_vlavo;
    pravdepodobnosti.vpravo = pravd_vpravo;
    svt_t * navrat;
    if (prekazky) {
        navrat = svet_init_prekazky(rozmer_x, rozmer_y, 20, pravdepodobnosti, pocet_krokov_K, pocet_replikacii, cesta_k_suboru);
    } else {
        navrat = svet_init_normal(rozmer_x, rozmer_y, pravdepodobnosti, pocet_krokov_K, pocet_replikacii, cesta_k_suboru);
    }
    return navrat;
    
}




int main(int argc, char const *argv[])
{
    
    srand(time(NULL)); 
    socket_server_t socket_server;
    socket_server_init(&socket_server, 777);
    pthread_t vlakienko;
    pthread_create(&vlakienko, NULL, vlaknoPrijmaniaSpojenia, &socket_server);


    pockaj_na_prve_pripojenie(&socket_server);
    
    svt_t * svet = nastav_server(&socket_server);

    svt_brd_t * svet_vypis;
    svet_vypis = calloc(1, sizeof(svt_brd_t));
    if (svet_vypis == NULL) {
        perror("Chyba vytvarania simulacie pri alokovani pamate");
        exit(EXIT_FAILURE);
    }
    svet_vypis->svet = svet;
    svet_vypis->server = &socket_server;
    //TODO
    server_vykonavaj_sim(svet_vypis);
    free(svet_vypis);
    sleep(1);
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

