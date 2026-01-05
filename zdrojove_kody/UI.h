

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
char * vykresli_svet(svt_t * svet);
char * svet_vypis_statistiku(svt_t * svet);
char * svet_vypis_kroky(svt_t * svet);


char * vycisti_obrazovku();




void spusti_menu_klient();

void  server_vykonavaj_sim(svt_brd_t * svet);

void * posli_vsetkym_svet(void * arg);

