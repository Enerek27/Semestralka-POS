#ifndef SOCKET_H
#define SOCKET_H


#include <pthread.h>
#include <stddef.h>
#include <sys/socket.h>
#include <stdatomic.h>


void * cisti_server(void * arg);
typedef struct server_info {
    atomic_bool sumarny_mod;
} srv_inf_t;



void * nacuvajklientovi(void * arg);
// Štruktúra obsahujúca informácie pre prácu so soketom
typedef struct SocketData {
    int socket;
} socket_data_t;

// Hlavičky funkcií, ktoré sú verejne dostupné a pracujú so soketom
void socket_init(socket_data_t * this, int domain, int type, int protocol);
void socket_destroy(socket_data_t * this);
_Bool socket_is_valid(socket_data_t * this);
void socket_bind(socket_data_t * this, const struct sockaddr * severAddress, socklen_t severAddressLength);
void socket_listen(socket_data_t * this, int backlog);
void socket_accept(socket_data_t * this, const socket_data_t * passiveSocket, struct sockaddr * clientAddress, socklen_t * clientAddressLength);
_Bool socket_connect(socket_data_t * this, const struct sockaddr * clientAddress, socklen_t clientAddressLength);
void socket_write(socket_data_t * this, const char * buffer, size_t length);
int socket_read(socket_data_t * this, char * buffer, size_t length);

typedef struct klient_read {
    socket_data_t socket_pocuvaj;
    atomic_bool chcem_statistiku;
    atomic_bool bezi_klient;
    atomic_bool vypni_server;
    atomic_bool prepni_mod;
}klient_read_t ;
// Štruktúra obsahujúca informácie pre prácu servera
typedef struct SocketServer {
    //toto je prijimaci socket na pripajanie
    socket_data_t passiveSocket;
    //pole socketov cize ulozeny klienti
    
    klient_read_t ** klienti;
    int pocetKlinetov;
    int maxPocetKlientov;
    int port;
    pthread_mutex_t mutex;
    atomic_bool server_bezi;
    srv_inf_t server_info;
    int hlavny_klient;
} socket_server_t;
void client_zavri(socket_server_t * server);

// Hlavičky funkcií, ktoré sú verejne dostupné a pracujú s informáciami pre server
void socket_server_init(socket_server_t * this, int port);
void socket_server_accept_connection(socket_server_t * this);
void socket_server_destroy(socket_server_t * this);
// Štruktúra obsahujúca informácie pre prácu klienta
typedef struct SocketClient {
    socket_data_t activeSocket;
    char * serverName;
    int port;
    atomic_bool klien_bezi;
    pthread_mutex_t mutex;
} socket_client_t;
// Hlavičky funkcií, ktoré sú verejne dostupné a pracujú s informáciami pre klienta
void socket_client_init(socket_client_t * this, char * serverName, char * port);
void socket_client_destroy(socket_client_t * this);

#endif
