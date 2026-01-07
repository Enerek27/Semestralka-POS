
#define _POSIX_C_SOURCE 199309L   


#include <sys/select.h>
#include "socket.h"
#include <netinet/in.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
// pridané kvôli funkcii getaddrinfo a vecí okolo nej
#define __USE_XOPEN2K
#include <netdb.h>
#undef __USE_XOPEN2K


void * cisti_server(void * arg) {
        socket_server_t * server = arg;

    while (atomic_load(&server->server_bezi)) {
        pthread_mutex_lock(&server->mutex);
        int prvy = 0;
        int posledny = server->pocetKlinetov - 1;

        while (prvy <= posledny) {
            if (!atomic_load(&server->klienti[prvy]->bezi_klient)) {
                
                klient_read_t * tmp = server->klienti[prvy];
                server->klienti[prvy] = server->klienti[posledny];
                server->klienti[posledny] = tmp;

                
                server->pocetKlinetov--;
                posledny--; 
            } else {
                prvy++; 
            }
        }
        for (int i = 0; i < server->pocetKlinetov; i++) {
          if (atomic_load(&server->klienti[i]->vypni_server)) {
              atomic_store(&server->server_bezi, 0);
              break;
          }
          if (atomic_load(&server->klienti[i]->prepni_mod)) {
            if (atomic_load(&server->server_info.sumarny_mod)) {
              atomic_store(&server->server_info.sumarny_mod, 0);
            } else {
              atomic_store(&server->server_info.sumarny_mod, 1);
            }
            atomic_store(&server->klienti[i]->prepni_mod, 0);
          }
        }

        pthread_mutex_unlock(&server->mutex);

        
        struct timespec ts = {0, 300 * 1000000}; 
        nanosleep(&ts, NULL);
    }

    pthread_exit(NULL);
}


void * nacuvajklientovi(void * arg) {
  klient_read_t * info_klient = arg; 
 
  while (atomic_load(&info_klient->bezi_klient )) {
    char buf[200];

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(info_klient->socket_pocuvaj.socket, &readfds);
    struct timeval tv = {0, 300000}; // 0.3 s
    int rv = select(info_klient->socket_pocuvaj.socket + 1, &readfds, NULL, NULL, &tv);
     if (rv == -1) {
        if (!atomic_load(&info_klient->bezi_klient)) {
          pthread_exit(NULL);
        }
        perror("select");
        break;
        } else if (rv == 0) {
          continue; 
        } else {
          int n = socket_read(&info_klient->socket_pocuvaj, buf, sizeof(buf));
          
          if (n < 0) {
            printf("klient sa zavrel\n");
            if (!atomic_load(&info_klient->bezi_klient)) {
              pthread_exit(NULL);
            }
          } else if (n == 0) {
            
            atomic_store(&info_klient->bezi_klient, 0);
            socket_destroy(&info_klient->socket_pocuvaj);
            break;
          } else {
            //prijal som spravu spracovanie
              int signal = buf[0] - '0';
              switch (signal) {
                case 0:
                  //signal vypnutie
                  atomic_store(&info_klient->vypni_server, 1);
                  break;
                case 1:
                  //signal prepni mod
                  atomic_store(&info_klient->prepni_mod, 1);
                  
                  break;
                case 2:
                  //signal v prepnutom mode chcem teraz statistiku
                  atomic_store(&info_klient->chcem_statistiku, 1);
                  break;
                case 3:
                  //signal v prepnutom mode chcem teraz kroky
                  atomic_store(&info_klient->chcem_statistiku, 0);
                  break;
                default:
                  break;
              }



            } 
          }


  }

  
  //sem prichadza od klienta info
  pthread_exit(NULL);
}





// Funkcia na inicializáciu soketu, pričom je potrebné uviesť komunikačnú doménu, typ komunikácie a protokol
void socket_init(socket_data_t * this, int domain, int type, int protocol) {
  this->socket = socket(domain, type, protocol);
  if (this->socket < 0) {
    perror("socket_init: zlyhanie vytvorenia soketu!");
    exit(EXIT_FAILURE);
  }
}
// Funkcia na zničenie soketu
void socket_destroy(socket_data_t * this) {
  close(this->socket);
  this->socket = 0;
}
// Funkcia, ktorá vráti informáciu o tom, či je soket validný, teda či jeho popisova má hodnotu väčšiu ako 0
_Bool socket_is_valid(socket_data_t * this) {
  return this->socket > 0;
}
// Funkcia, ktorá obaľuje funkciu bind, pričom je potrebné uviesť aj adresu servera a veľkosť adresy servera
void socket_bind(socket_data_t * this, const struct sockaddr * severAddress, socklen_t severAddressLength) {
  if (bind(this->socket, severAddress, severAddressLength) < 0) {
    perror("socket_bind: zlyhanie funkcie bind!");
    exit(EXIT_FAILURE);
  }
}
// Funkcia, ktorá obaľuje funkciu listen, pričom je potrebné uviesť aj hodnotu maximálneho počtu čakajúcich pripojení
void socket_listen(socket_data_t * this, int backlog) {
  if (listen(this->socket, backlog) < 0) {
    perror("socket_listen: zlyhanie funkcie listen!");
    exit(EXIT_FAILURE);
  }
}
// Funkcia, ktorá obaľuje funkciu accept, pričom je potrebné uviesť aj pasívny soket, miesto, kam sa uloží adresa klienta a miesto, kam sa uloží veľkosť adresy klienta
_Bool socket_accept(socket_data_t * this, const socket_data_t * passiveSocket, struct sockaddr * clientAddress, socklen_t * clientAddressLength, socket_server_t * server) {
  this->socket = accept(passiveSocket->socket, clientAddress, clientAddressLength);
  if (this->socket < 0) {
    if (!atomic_load(&server->server_bezi)) {
      return 1;
    }
    perror("socket_accept: zlyhanie funkcie accept!");
    exit(EXIT_FAILURE);
  }
  return 0;
}
// Funkcia, ktorá obaľuje funkciu connect, pričom je potrebné uviesť aj adresu servera a veľkosť adresy servera
_Bool socket_connect(socket_data_t * this, const struct sockaddr * serverAddress, socklen_t serverAddressLength) {
  int result = connect(this->socket, serverAddress, serverAddressLength);
  printf("%d\n", result);
  if (result < 0) {
    perror("socket_connect: zlyhanie funkcie connect!");
  }
  return result == 0;
}
// Funkcia, ktorá obaľuje funkciu write, pričom je potrebné uviesť aj miesto, kde sa nachádzajú údaje a veľkosť miesta, z ktorého sa číta
void socket_write(socket_data_t * this, const char * buffer, size_t length) {
  int n = write(this->socket, buffer, length);
  if (n < 0) {
    perror("socket_write: zlyhanie zapisu do soketu!");
    exit(EXIT_FAILURE);
  }
}
// Funkcia, ktorá obaľuje funkciu read, pričom je potrebné uviesť aj miesto, kde sa uloží výsledok a maximálnu veľkosť miesta na uloženie
int socket_read(socket_data_t * this, char * buffer, size_t length) {
  int n = read(this->socket, buffer, length);
  return n;
}
// Funkcia na inicializáciu servera, pričom je potrebné uviesť aj port, na ktorom bude server čakať na pripojenia
void socket_server_init(socket_server_t * this, int port) {
  // Adresa servera
  struct sockaddr_in severAddress;
  // Vytvorenie pasívneho soketu pre TCP/IPv4 komunikáciu
  socket_init(&this->passiveSocket, AF_INET, SOCK_STREAM, IPPROTO_TCP);
  // Nastavenie adresy soketu, pričom sa jedná o internetovú komunikáciu
  memset(&severAddress, '\0', sizeof(severAddress));
  // IPv4
  severAddress.sin_family = AF_INET;
  // Použije sa localhost, teda adresa 127.0.0.1 local  alebo 192.168.1.10 lan alebo "10.0.0.5" wifi
  severAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  // Nastaví sa port soketu podľa parametru port, pričom je potrebné brať do úvahy endianitu (htons)
  severAddress.sin_port = htons(port);
  // Samotné zavolanie funkcie bind, pričom sa pretypuje internetová adresa servera na generický typ adresy
  socket_bind(&this->passiveSocket, (const struct sockaddr *) &severAddress, sizeof(severAddress));
  // Premena soketu na pasívny socket, na ktorom sa bude prijímať spojenie, pričom sa nebudú ukladať žiadne ďalšie čakajúce pripojenia do frontu
  socket_listen(&this->passiveSocket, 0);
  this->port = port;
  this->maxPocetKlientov = 5;
  this->pocetKlinetov = 0;
  this->klienti = malloc(this->maxPocetKlientov * sizeof(klient_read_t*));
  for (int i = 0; i < this->maxPocetKlientov; i++) {
    this->klienti[i] = calloc(1, sizeof(klient_read_t));
  }
  if (this->klienti == NULL) {
    perror("Zle inicializovana pamat pre klientov!");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < this->maxPocetKlientov; i++) {
    atomic_store(&this->klienti[i]->bezi_klient, 0);
    atomic_store(&this->klienti[i]->chcem_statistiku, 0);
    atomic_store(&this->klienti[i]->vypni_server, 0);
    atomic_store(&this->klienti[i]->prepni_mod, 0);
  }


  pthread_mutex_init(&this->mutex, NULL);
  this->server_bezi = (atomic_bool)1;
  this->hlavny_klient = 0;
  srv_inf_t server_info;
  atomic_store(&this->server_info.sumarny_mod, 0);
  this->server_info = server_info;
  
}
// Funkcia na akceptovanie pripojenia klientom, pričom sa jedná o blokovacie volanie
void socket_server_accept_connection(socket_server_t * this) {
  // Adresa klienta
  struct sockaddr_in clientAddress;
 
  // Ukladanie veľkosti adresy klienta
  socklen_t clientAddressSize = sizeof(clientAddress);
  socket_data_t tempSocket;
  fd_set readfds;
  if (socket_accept(&tempSocket, &this->passiveSocket, (struct sockaddr * ) &clientAddress, &clientAddressSize, this)) {
    return;
  }
  
  pthread_mutex_lock(&this->mutex);
  // Pripojenie klienta na server, pričom sa nastaví všetko potrebné v adrese klienta, nastaví sa veľkosť adresy a vráti sa popisovač soketu určeného pre komunikáciu
  if (this->maxPocetKlientov == this->pocetKlinetov) {
    int naviac = this->maxPocetKlientov + 5;
    klient_read_t ** tmp = realloc(this->klienti, sizeof(klient_read_t*) * naviac);
    if (tmp == NULL) {
      perror("Chyba zvacsania pamate!");
      exit(EXIT_FAILURE);
    }
    this->klienti = tmp;
    for (int i = this->maxPocetKlientov; i < naviac; i++) {
        this->klienti[i] = calloc(1, sizeof(klient_read_t));
    }
    for (int i = this->maxPocetKlientov; i < naviac; i++) {
      atomic_store(&this->klienti[i]->chcem_statistiku, 0);
      atomic_store(&this->klienti[i]->bezi_klient, 0);
      atomic_store(&this->klienti[i]->vypni_server, 0);
      atomic_store(&this->klienti[i]->prepni_mod, 0);
    }
    
    this->maxPocetKlientov = naviac;
  }
  memset(this->klienti[this->pocetKlinetov], 0, sizeof(*this->klienti[this->pocetKlinetov]));  
  this->klienti[this->pocetKlinetov]->socket_pocuvaj = tempSocket;
  atomic_store(&this->klienti[this->pocetKlinetov]->bezi_klient, 1);
  atomic_store(&this->klienti[this->pocetKlinetov]->vypni_server, 0);
  atomic_store(&this->klienti[this->pocetKlinetov]->chcem_statistiku, 0);
  atomic_store(&this->klienti[this->pocetKlinetov]->prepni_mod, 0);
  this->pocetKlinetov++;
  pthread_t vlakno;
  if (this->pocetKlinetov == 1) {
    this->hlavny_klient = this->pocetKlinetov - 1;
    char buff[2];
    buff[0] = '7';
    buff[1] = '\0';
    socket_write(&this->klienti[this->hlavny_klient]->socket_pocuvaj, buff, strlen(buff));
  } else {
    char buff[2];
    buff[0] = '5';
    buff[1] = '\0';
    socket_write(&this->klienti[this->pocetKlinetov - 1]->socket_pocuvaj, buff, strlen(buff));
    pthread_create(&vlakno, NULL, nacuvajklientovi, this->klienti[this->pocetKlinetov - 1]);
    pthread_detach(vlakno);
  }
    
  pthread_mutex_unlock(&this->mutex);
    
  
}

void client_zavri(socket_server_t * server) {
  pthread_mutex_lock(&server->mutex);
  for (int i = 0; i < server->pocetKlinetov; i++) {
    char buff[4];
    buff[0] = 'o';
    buff[1] = 'f';
    buff[2] = 'f';
    buff[3] = '\0';
    socket_write(&server->klienti[i]->socket_pocuvaj, buff, strlen(buff) + 1);
    atomic_store(&server->klienti[i]->bezi_klient, 0);
  }
  pthread_mutex_unlock(&server->mutex);
}

// Funkcia na zničenie servera, čo momentálne znamená zničenie pasívneho a aktívneho soketu
void socket_server_destroy(socket_server_t * this) {
  atomic_store(&this->server_bezi, 0);

  // Zničenie pasívneho soketu na prijímanie pripojení
  socket_destroy(&this->passiveSocket);
  // Zničenie aktívneho soketu pre komunikáciu s klientom
  for (int i = 0; i < this->pocetKlinetov; i++) {
    socket_destroy(&this->klienti[i]->socket_pocuvaj);
    free(this->klienti[i]);
  }
  
  free(this->klienti);
  
  pthread_mutex_destroy(&this->mutex);
  
  
}
// Funkcia na inicializáciu klienta, pričom je potrebné uviesť aj názov servera a port, na ktorom bude zadaný server čakať na pripojenia
void socket_client_init(socket_client_t * this, char * serverName, char * port) {
  // Štruktúra používaná pre potreby uloženia adries servera
  struct addrinfo * server;
  // Štruktúra používaná na definovanie základných informácií o type komunikácie
  struct addrinfo hints;
  // Internetová komunikácie použitím IP4 aj IP6
  hints.ai_family = AF_INET;
  // Spoľahlivá forma komunikácie
  hints.ai_socktype = SOCK_STREAM;
  // Upresnenie pre protokol TCP/IP
  hints.ai_protocol = IPPROTO_TCP;
  // Aj IP4 aj IP6 adresy
  hints.ai_flags = AI_ALL;
  // Zisťovanie adresy servera podľa mena, portu a uvedených informácií o type komunikácie
  int s = getaddrinfo(serverName, port, &hints, &server);
  if (s != 0) {
    fprintf(stderr, "socket_client_init: zlyhanie funkcie getaddrinfo(%d): %s\n", s, gai_strerror(s));
    exit(EXIT_FAILURE);
  }
  // Pozor! V určitých prípadoch môže existovať aj viac dostupných adries
  for (struct addrinfo * rp = server; rp != NULL; rp = rp->ai_next) {
    // Vytvorenie soketu pre komunikáciu cez internet, pričom sa používa získaná informácia
    socket_init(&this->activeSocket, rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    // Ak nedošlo k vytvoreniu soketu
    if (!socket_is_valid(&this->activeSocket)) {
      // Pokračuj ďalšou adresou
      continue;
    }
    // Pripojenie na server
    printf("Klient sa pripaja na server!\n");
    if (!socket_connect(&this->activeSocket, rp->ai_addr, rp->ai_addrlen)) {
      // Ak zlyhalo, tak sa zatvorí soket a pokračuje sa ďalšou adresou
      printf("Zlyhanie pripojenia!\n");
      socket_destroy(&this->activeSocket);
    } else {
      // Ak sa podarilo pripojiť, uvoľní sa získaná štruktúra z funkcie getaddrinfo a nastaví sa pomocná premenná na úspešné pripojenie
      freeaddrinfo(server);
      this->serverName = serverName;
      this->port = atoi(port);
      atomic_store(&this->klien_bezi, 1);
      pthread_mutex_init(&this->mutex, NULL);
      return;
    }
  }
  // Pokiaľ po prejdení všetkých adries nedošlo k pripojeniu, tak vypíš chybu a skonči
  fprintf(stderr, "socket_client_init: zlyhalo pripojenie na server!\n");
  freeaddrinfo(server);
  exit(EXIT_FAILURE);
}
// Funkcia na zničenie klienta, čo znamená zničenie aktívneho soketu
void socket_client_destroy(socket_client_t * this) {
  // Zničenie aktívneho soketu pre komunikáciu so serverom
  socket_destroy(&this->activeSocket);
  atomic_store(&this->klien_bezi, 0);
  pthread_mutex_destroy(&this->mutex);
}