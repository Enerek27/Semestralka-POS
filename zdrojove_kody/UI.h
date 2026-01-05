

#include "simulacia.h"
#include "../sockety/socket.h"

typedef struct vypis_sveta {
    svt_t * svet;
    socket_server_t * server;
}svt_vp_t ;

//rozmerx;rozmery;svetprekazky;pocet_replikacii;...
//potrebujem poslat rozmery sveta x,y   svet prekazky 1 = true , 0 = normal
               // dalej pocet replikacii (kym niesom v cieli) 
                // max pocet krokov K, pravdepodobnosti
              // cesta k suboru
typedef struct  svet_broadcast {
    svt_t * svet;
    socket_server_t * server;
}svt_brd_t;

typedef struct poslat_server {
    int rozmer_x;
    int rozmer_y;
    _Bool svet_s_prekazkami;
    _Bool nacitaj_zo_suboru;
    int pocet_replikacii;
    int pocet_krokov_K;
    prvd_t pravdepodobnosti;
    char * cesta_k_suboru;
}srv_p_t ;

//ASCII + ANSI 
char * vykresli_svet(svt_t * svet);
char * svet_vypis_statistiku(svt_t * svet);
char * svet_vypis_kroky(svt_t * svet);


char * vycisti_obrazovku();


void inicializuj_server(srv_p_t * data,  socket_client_t * socket);

void spusti_menu_klient();

void  server_vykonavaj_sim(svt_brd_t * svet);

void * posli_vsetkym_svet(void * arg);

