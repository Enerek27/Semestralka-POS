

#include "simulacia.h"
#include "../sockety/socket.h"
#include "../zdielanaPamat/pipe.h"
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
    int pocet_replikacii;
    int pocet_krokov_K;
    prvd_t pravdepodobnosti;
    char * cesta_k_suboru;
}srv_p_t ;

typedef struct pipe_svet_vypis {
    svt_t * svet;
    server_pipe_t * server;
}vypis_pipe_t;

//ASCII + ANSI 
char * vykresli_svet(svt_t * svet);
char * svet_vypis_statistiku(svt_t * svet);
char * svet_vypis_kroky(svt_t * svet);

char * vrat_menu_klient();

char * vycisti_obrazovku();


void inicializuj_server(srv_p_t * data,  socket_client_t * socket, pipe_data_t * pip_write, _Bool pouziPipe);

void spusti_initmenu_klient(socket_client_t * socket, pipe_data_t * pip_write, _Bool pouziPipe);

void  server_vykonavaj_sim(svt_brd_t * svet, vypis_pipe_t * data_pipe, _Bool pouziPipe);

void  posli_vsetkym_svet(svt_vp_t * data, vypis_pipe_t * data_pipe, _Bool pouziPipe);


int hlavne_menu_klient();

void posli_vsetkym_statistiku(svt_brd_t * data, vypis_pipe_t * data_pipe, _Bool pouziPipe);

void  posli_vsetkym_stat(svt_vp_t * data, vypis_pipe_t * data_pipe, _Bool pouziPipe);
