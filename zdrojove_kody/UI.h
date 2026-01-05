

#include "simulacia.h"
#include "../sockety/socket.h"

typedef struct vypis_sveta {
    svt_t * svet;
    socket_server_t * server;
}svt_vp_t ;

typedef struct  svet_broadcast {
    svt_t * svet;
    socket_server_t * server;
}svt_brd_t;


//ASCII + ANSI 
void vykresli_svet(svt_t * svet);
void svet_vypis_statistiku(svt_t * svet);



void vycisti_obrazovku();




void spusti_menu_klient();

void  server_vykonavaj_sim(svt_brd_t * svet);

void * posli_vsetkym_svet(void * arg);

