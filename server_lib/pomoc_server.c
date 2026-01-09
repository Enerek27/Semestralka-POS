

#include "pomoc_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>


svt_t * nastav_server(socket_server_t * server, server_pipe_t * server_pipe, _Bool pouziPipe) {
    char buf[500];
    if (pouziPipe) {
        pipe_read(&server_pipe->pip_read, buf, sizeof(buf));
    } else {
        pthread_mutex_lock(&server->mutex);
        socket_data_t klient = server->klienti[server->hlavny_klient]->socket_pocuvaj;
        pthread_mutex_unlock(&server->mutex);
        socket_read(&klient, buf, sizeof(buf));
    }
    
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


void * vlaknoPrijmaniaSpojenia(void * arg) {
    socket_server_t * server = arg;
    while (atomic_load(&server->server_bezi)) {
        socket_server_accept_connection(server);
        
    }
    pthread_exit(NULL);
}




svt_t * server_info_subor(socket_server_t * server, server_pipe_t * server_pipe, _Bool pouziPipe) {
    char buf[250];
    if (pouziPipe) {
        pipe_read(&server_pipe->pip_read, buf, sizeof(buf));
    } else {
        socket_read(&server->klienti[server->hlavny_klient]->socket_pocuvaj, buf, sizeof(buf));
    }
    
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

