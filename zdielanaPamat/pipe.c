#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#define RED "\033[31m"
#define RESET "\033[0m"


#include "pipe.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Funkcia na otvorenie dátovodu s príslušnými značkami
static void pipe_open_with_flags(pipe_data_t * this, int flags) {
  this->fD = open(this->path, flags);
  if (this->fD == -1) {
    fprintf(stderr, RED "pipe_open_with_flags: zlyhanie otvorenia dátovodu: %s" RESET, this->path);
    perror("");
    exit(EXIT_FAILURE);
  }
}
// Funkcia na inicializáciu dátovotu, pričom je potrebné uviesť jeho názov, resp. cestu a to, či je potrebné dátovod vytvoriť
void pipe_init(pipe_data_t * this, char * path, _Bool withCreation) {
  if (withCreation) {
    if (mkfifo(path, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP) == -1) {
      perror(RED "pipe_init: zlyhanie pri vytvorení dátovodu." RESET);
      exit(EXIT_FAILURE);
    }
  }
  this->path = path;
  this->fD = 0;
}
// Funkcia na zničenie dátovodu, pričom je potrebné uviesť, či má dôjsť aj k jeho vymazaniu a či je potrebné uvoľniť pamäť, v ktorej je vytvorený názov dátovodu
void pipe_destroy(pipe_data_t * this, _Bool withDeletion, _Bool withNameFree) {
  if (withDeletion) {
    if (unlink(this->path) == -1) {
      if (withNameFree) {
        free(this->path);
      }
      perror(RED "pipe_destroy: zlyhanie funkcie unlink pri vymazávani dátovodu." RESET);
      exit(EXIT_FAILURE);
    }
  }
  if (withNameFree) {
    free(this->path);
  }
  this->path = NULL;
  this->fD = 0;
}
// Funkcia na otvorenie dátovodu pre zápis (značka O_WRONLY)
void pipe_open_write(pipe_data_t * this) {
  pipe_open_with_flags(this, O_WRONLY);
}
// Funkcia na otvorenie dátovodu na čítanie (značka O_RDONLY)
void pipe_open_read(pipe_data_t * this) {
  pipe_open_with_flags(this, O_RDONLY);
}
// Funkcia pre čítanie z dátovodu, pričom je potrebné uviesť aj miesto, kde sa uloží výsledok a maximálnu veľkosť miesta na uloženie
int pipe_read(pipe_data_t * this, char * result, size_t size) {
  return read(this->fD, result, size);
   
}
// Funkcia pre zápis do dátovodu, pričom je potrebné uviesť aj miesto, kde sa nachádzajú údaje a veľkosť miesta, z ktorého sa číta
void pipe_write(pipe_data_t * this, const char * data, size_t size) {
  if (write(this->fD, data, size) == -1) {
    perror(RED "pipe_write: zlyhal zápis do dátovodu!" RESET);
  }
}
// Funkcia na zavretie dátovodu
void pipe_close(pipe_data_t * this) {
  if (close(this->fD) == -1) {
    perror(RED "pipe_close: zlyhanie pri zatvárani dátovodu!" RESET);
    exit(EXIT_FAILURE);
  }
  this->fD = 0;
}

void * nacuvajklientovi_pipe(void * arg) {
  server_pipe_t * server = arg; 
  
  while (atomic_load(&server->server_bezi)) {
    
    char buf[200];

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(server->pip_read.fD, &readfds);
    struct timeval tv = {0, 300000}; // 0.3 s
    int rv = select(server->pip_read.fD + 1, &readfds, NULL, NULL, &tv);
     if (rv == -1) {
        if (!atomic_load(&server->server_bezi)) {
          pthread_exit(NULL);
        }
        perror("select");
        break;
        } else if (rv == 0) {
          continue; 
        } else {
          int n = pipe_read(&server->pip_read, buf, sizeof(buf));
          
          if (n < 0) {
            printf("klient sa zavrel\n");
            if (!atomic_load(&server->server_bezi)) {
              pthread_exit(NULL);
            }
          } else if (n == 0) {
            
            atomic_store(&server->server_bezi, 0);
            break;
          } else {
            //prijal som spravu spracovanie
            int signal = buf[0] - '0';
            //printf("prisla sprava %d\n", signal);
              switch (signal) {
                case 0:
                  //signal vypnutie
                  atomic_store(&server->server_bezi, 0);
                  break;
                case 1:
                  //signal prepni mod
                  if (atomic_load(&server->server_info.prepni_mod)) {
                    atomic_store(&server->server_info.prepni_mod, 0);
                  } else {
                  
                    atomic_store(&server->server_info.prepni_mod, 1);
                  }
                  break;
                case 2:
                  //signal v prepnutom mode chcem teraz statistiku
                  atomic_store(&server->server_info.chcem_statistiku, 1);
                  break;
                case 3:
                  //signal v prepnutom mode chcem teraz kroky
                  atomic_store(&server->server_info.chcem_statistiku, 0);
                  break;
                case 4:
                  //odpojim klienta
                  atomic_store(&server->server_info.prepni_mod, 1);
                  atomic_store(&server->server_info.prazdne, 1);
                
                default:
                  break;
              }



            } 
          }
        

  }

  
  //sem prichadza od klienta info
  pthread_exit(NULL);
}

