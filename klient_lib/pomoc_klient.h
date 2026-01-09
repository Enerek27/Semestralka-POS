

#include "../sockety/socket.h"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define ORANGE "\033[38;5;9m"
#define RESET "\033[0m"



_Bool je_klient_hlavny(socket_client_t * socket);
void * vypisujObraz(void * arg);
_Bool nacitaj_zo_suboru(socket_client_t * socket, char * mozno_cesta_subor);
void klient_odpovedaj(socket_client_t * socket_client);