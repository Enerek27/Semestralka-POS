#define _POSIX_C_SOURCE 200809L
#define UZIVATELFARBA "\033[38;5;14m"
#define GREEN "\033[32m"
#define ORANGE "\033[38;5;9m"
#define RESET "\033[0m"

#include <sched.h>
#include <stdatomic.h>
#include <time.h>

#include <sys/wait.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>





#include "../klient_lib/pomoc_klient.h" 


//TODO treba resetovat vsetko pred dalsou simulaciuo skusit nacitanie a ulozenie do suboru a viacej klientov 
// pridat moznost iba odist z simulacie a znova sa pripojit

static void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}


int main()
{
    _Bool idem = 1;
    
    struct sigaction sa = {0};
    sa.sa_handler = sigchld_handler;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

   while (idem) {
        // switch co lydkine menu vrati podla toho sa bude nieco robit
        
        int odpoved = hlavne_menu_klient();

        switch (odpoved) {
            case 1:
                    ////// NOVA SIMULACIA
                
                    //////////////  KOLKO POUZIVATELOV
                    {
                    char buf [200];
                    memset(buf, 0, sizeof(buf));
                    
                    char * kontrola; 
                    int pocetPouzivatelov;
                    _Bool dobreZadal = 1;
                    while (dobreZadal) {

                        printf(GREEN "Vyber si: simulácia pre jedného(1), pre N klientov (2): " RESET);
                        printf(UZIVATELFARBA);
                        if (fgets(buf, sizeof(buf), stdin) == NULL) {
                            printf(RESET);
                            perror(RED "Chyba načitavania textu." RESET);
                            exit(EXIT_FAILURE);
                        }
                        printf(RESET);
                        
                        pocetPouzivatelov = strtol(buf, &kontrola, 10);
                        if (kontrola == buf) {
                            printf(ORANGE "To nie je číslo, zadaj znova.\n" RESET);
                        } else {
                            if (pocetPouzivatelov == 1) {
                                
                                pid_t pid = fork();
                                if (pid == 0) {
                                    //treba zmenit na execl
                                    execl("../server/server_pipe","server_pipe", NULL);
                                    perror(RED "Chyba pri spustení servera." RESET);
                                    _exit(EXIT_FAILURE);
                                } else if (pid > 0) {
                                    //tu bezi klient
                                    pipe_data_t pip_read;
                                    pipe_data_t pip_write;
                                    klient_pipe_t klient;
                                    atomic_store(&klient.klien_bezi, 1);
                                    
                                    sleep(2);
                                  
                                    
                                    pipe_init(&pip_read, "../pipe_read", 0);
                                    pipe_init(&pip_write, "../pipe_write", 0);
                                    
                                    pipe_open_write(&pip_write);
                                    pipe_open_read(&pip_read);
                                   
                                    klient.pip_write = pip_write;
                                    klient.pip_read = pip_read;
                                   
                                    
                                    char buf[2];
                                    buf[0] = '5';
                                    buf[1] = ';';
                                    
                                    pipe_write(&klient.pip_write, buf, 2);
                                    spusti_initmenu_klient(NULL,&pip_write, 1);
                                       
                                    //tu som skoncil

                                    pthread_t vlakno;
                                    pthread_create(&vlakno, NULL, vypisujObraz_pipe, &klient);
                                    
                                    
                                    while (atomic_load(&klient.klien_bezi)) {
                                        
                                        klient_odpovedaj(NULL, &klient, 1);
                                    }
                                    pipe_destroy(&klient.pip_read, 0,0);
                                    pipe_destroy(&klient.pip_write, 0,0);
                                   
                                    pthread_join(vlakno, NULL);
                                   
                                    
                                    
                                    
                                    
                                    
                                } else {
                                    perror(RED "Chyba vytvorenia procesu." RESET);
                                    exit(EXIT_FAILURE);
                                }
                                dobreZadal = 0;
                                break;
                            } else if(pocetPouzivatelov > 1) {
                                char port[50];
                                memset(port, 0, sizeof(port));
                                printf(GREEN "Zadaj cislo portu pre vytvorenie:  " RESET);
                                printf(UZIVATELFARBA);
                                if (fgets(port, sizeof(port), stdin) == NULL) {
                                    printf(RESET);
                                    perror(RED "Chyba načitavania textu." RESET);
                                    exit(EXIT_FAILURE);
                                    }       
                                port[strcspn(port, "\n")] = '\0';
                                pid_t pid = fork();
                                if (pid == 0) {
                                    //treba zmenit na execl
                                    execl("../server/server","server", port, NULL);
                                    perror(RED "Chyba pri spustení servera." RESET);
                                    _exit(EXIT_FAILURE);
                                } else if (pid > 0) {
                                    //tu bezi klient
                                    sleep(2);
                                    socket_client_t * socket_client;
                                    socket_client = malloc(sizeof(socket_client_t));
                                    if (socket_client == NULL) {
                                        perror("Chyba alokacie pamate");
                                        exit(EXIT_FAILURE);
                                    }

                                    
                                    if (!socket_client_init(socket_client, "127.0.0.1", port)) {
                                        free(socket_client);
                                        break;
                                    }

                                    if (je_klient_hlavny(socket_client)) {
                                        char buf[2];
                                        buf[0] = '5';
                                        buf[1] = ';';
                                        socket_write(&socket_client->activeSocket,buf, 2);
                                        spusti_initmenu_klient(socket_client, NULL, 0);
                                          
                                    }
            
                                    pthread_t vlakno;
                                    pthread_create(&vlakno, NULL, vypisujObraz, socket_client);
                                    
                                    
                                    while (atomic_load(&socket_client->klien_bezi)) {
                                        
                                        klient_odpovedaj(socket_client, NULL, 0);
                                    }
                                    
                                    
                                    pthread_join(vlakno, NULL);
                                    socket_client_destroy(socket_client);
                                    free(socket_client);
                                    
                                } else {
                                    perror(RED "Chyba vytvorenia procesu." RESET);
                                    exit(EXIT_FAILURE);
                                }
                                dobreZadal = 0;
                                break;
                            } else {
                                dobreZadal = 1;
                            }
                            
                        } 

                    }  

                }
                break;




                    
                
            case 2:
            //TODO prestavit ako 1 aby bolo dobre :)
                    { char buf[200];
                    //treba upravit vypinanie aby tam bola dalsia moznost
                    char * kontrola; 
                    int pocetPouzivatelov;
                    _Bool dobreZadal = 1;
                    while (dobreZadal) {

                        printf(GREEN "Vyber si: simulácia pre jedného(1), pre N klientov (2): " RESET);
                        if (fgets(buf, sizeof(buf), stdin) == NULL) {
                            perror(RED "Chyba načitavania textu." RESET);
                            exit(EXIT_FAILURE);
                        }
                        
                        pocetPouzivatelov = strtol(buf, &kontrola, 10);
                        if (kontrola == buf) {
                            printf(ORANGE "To nie je číslo, zadaj znova.\n" RESET);
                        } else {
                            if (pocetPouzivatelov == 1) {
                               //tu bezi klient
                                    pipe_data_t pip_read;
                                    pipe_data_t pip_write;
                                    klient_pipe_t klient;
                                    atomic_store(&klient.klien_bezi, 1);
                                    
                                    
                                  
                                    
                                    pipe_init(&pip_read, "../pipe_read", 0);
                                    pipe_init(&pip_write, "../pipe_write", 0);
                                    
                                    pipe_open_write(&pip_write);
                                    pipe_open_read(&pip_read);
                                   
                                    klient.pip_write = pip_write;
                                    klient.pip_read = pip_read;
                                   
                                   
                                    //tu som skoncil

                                    pthread_t vlakno;
                                    pthread_create(&vlakno, NULL, vypisujObraz_pipe, &klient);
                                    
                                    
                                    while (atomic_load(&klient.klien_bezi)) {
                                        
                                        klient_odpovedaj(NULL, &klient, 1);
                                    }
                                    pipe_destroy(&klient.pip_read, 0,0);
                                    pipe_destroy(&klient.pip_write, 0,0);
                                   
                                    pthread_join(vlakno, NULL);
                                   
                                    
                                    
                                dobreZadal = 0;
                                break;
                            } else if (pocetPouzivatelov > 1) {
                            
                                printf(GREEN "Napíš port pripojenia: " RESET);
                                printf(UZIVATELFARBA);
                                char * adresaPripojenia  = fgets(buf, sizeof(buf), stdin);
                                adresaPripojenia[strcspn(adresaPripojenia, "\n")] = '\0';
                                if (adresaPripojenia == NULL) {
                                printf(RESET);
                                    perror(RED "Chyba načitavania textu." RESET);
                                    exit(EXIT_FAILURE);
                                } 
                                printf(RESET);
                                socket_client_t socket_client;
                                if (!socket_client_init(&socket_client, "127.0.0.1", adresaPripojenia)) {
                                        break;
                                }
                                je_klient_hlavny(&socket_client);
                                pthread_t vlakno;
                                pthread_create(&vlakno, NULL, vypisujObraz, &socket_client);
                                
            
                                while (atomic_load(&socket_client.klien_bezi)) {
                                    klient_odpovedaj(&socket_client, NULL, 0);
            
                                }
                                pthread_join(vlakno, NULL);
                                socket_client_destroy(&socket_client);
                            }
                            dobreZadal = 0;
                            break;
                        } 
                    }
                    
                    
                }
                break;
            case 3:
                //treba upravit vypinanie aby tam bola dalsia moznost
                {
                char cesta_k_suboru[200];
                memset(cesta_k_suboru, 0, sizeof(cesta_k_suboru));
                while (1) {
                
                
                    char buf [200];
                    memset(buf, 0, sizeof(buf));
                    printf(GREEN "Zadaj cestu k súboru na uloženie(ukončená musi byť '.txt'): " RESET);
                    printf(UZIVATELFARBA);
                    if (fgets(buf, sizeof(buf), stdin) == NULL) {
                        printf(RESET);
                        perror(RED "Chyba načitavania textu." RESET);
                        exit(EXIT_FAILURE);
                    }
                    printf(RESET);
                    buf[strcspn(buf, "\n")] = '\0';
                    memcpy(cesta_k_suboru, buf,strlen(buf) + 1);
                    break;
                }
                char buf [200];
                    memset(buf, 0, sizeof(buf));
                    
                    char * kontrola; 
                    int pocetPouzivatelov;
                    _Bool dobreZadal = 1;
                    while (dobreZadal) {

                        printf(GREEN "Vyber si: simulácia pre jedného(1), pre N klientov (2): " RESET);
                        if (fgets(buf, sizeof(buf), stdin) == NULL) {
                            perror(RED "Chyba načitavania textu." RESET);
                            exit(EXIT_FAILURE);
                        }
                        
                        pocetPouzivatelov = strtol(buf, &kontrola, 10);
                        if (kontrola == buf) {
                            printf(ORANGE "To nie je číslo, zadaj znova.\n" RESET);
                        } else {
                            if (pocetPouzivatelov == 1) {
                                
                                pid_t pid = fork();
                                if (pid == 0) {
                                    //treba zmenit na execl
                                    execl("../server/server_pipe","server_pipe", NULL);
                                    perror(RED "Chyba pri spustení servera." RESET);
                                    _exit(EXIT_FAILURE);
                                } else if (pid > 0) {
                                   //tu bezi klient
                                    pipe_data_t pip_read;
                                    pipe_data_t pip_write;
                                    klient_pipe_t klient;
                                    atomic_store(&klient.klien_bezi, 1);
                                    
                                    sleep(2);
                                  
                                    
                                    pipe_init(&pip_read, "../pipe_read", 0);
                                    pipe_init(&pip_write, "../pipe_write", 0);
                                    
                                    pipe_open_write(&pip_write);
                                    pipe_open_read(&pip_read);
                                   
                                    klient.pip_write = pip_write;
                                    klient.pip_read = pip_read;
                                   
                                    if (!nacitaj_zo_suboru_pipe(&pip_write, cesta_k_suboru)) {
                                        
                                        spusti_initmenu_klient(NULL,&pip_write, 1);
                                    }  
                                    //tu som skoncil

                                    pthread_t vlakno;
                                    pthread_create(&vlakno, NULL, vypisujObraz_pipe, &klient);
                                    
                                    
                                    while (atomic_load(&klient.klien_bezi)) {
                                        
                                        klient_odpovedaj(NULL, &klient, 1);
                                    }
                                    pipe_destroy(&klient.pip_read, 0,0);
                                    pipe_destroy(&klient.pip_write, 0,0);
                                   
                                    pthread_join(vlakno, NULL);
                                   
                                    
                                    
                                    
                                } else {
                                    perror(RED "Chyba vytvorenia procesu." RESET);
                                    exit(EXIT_FAILURE);
                                }
                                dobreZadal = 0;
                                break;
                            } else if(pocetPouzivatelov > 1) {
                                char port[50];
                                memset(port, 0, sizeof(port));
                                printf(GREEN "Zadaj cislo portu pre vytvorenie:  " RESET);
                                printf(UZIVATELFARBA);
                                if (fgets(port, sizeof(port), stdin) == NULL) {
                                    printf(RESET);
                                    perror(RED "Chyba načitavania textu." RESET);
                                    exit(EXIT_FAILURE);
                                    }       
                                port[strcspn(port, "\n")] = '\0';
                                pid_t pid = fork();
                                if (pid == 0) {
                                    //treba zmenit na execl
                                    execl("../server/server","server",port, NULL);
                                    perror(RED "Chyba pri spustení servera." RESET);
                                    _exit(EXIT_FAILURE);
                                } else if (pid > 0) {
                                    //tu bezi klient
                                    sleep(1);
                                    socket_client_t socket_client;
                                    if (!socket_client_init(&socket_client, "127.0.0.1", port)) {
                                        break;
                                    }
                                    if (je_klient_hlavny(&socket_client)) {
                                        if (!nacitaj_zo_suboru(&socket_client, cesta_k_suboru)) {
                                            spusti_initmenu_klient(&socket_client, NULL, 0);
                                        }   
                                    }
            
                                    pthread_t vlakno;
                                    pthread_create(&vlakno, NULL, vypisujObraz, &socket_client);
                                    
                                    
                                    while (atomic_load(&socket_client.klien_bezi)) {
                                        
                                        klient_odpovedaj(&socket_client, NULL, 0);
                                    }
                                    
                                    
                                    pthread_join(vlakno, NULL);
                                    socket_client_destroy(&socket_client);
                                    
                                    
                                } else {
                                    perror(RED "Chyba vytvorenia procesu." RESET);
                                    exit(EXIT_FAILURE);
                                }
                                dobreZadal = 0;
                                break;
                            } else {
                                dobreZadal = 1;
                            }
                            
                        } 

                    }  
            }
                break;
            case 4:
                    idem = 0;
                break;
            

        };
    }



    

   

        
 /*
    pipe_data_t klient;

    pipe_init(&klient, "../datovod", 0);

    char * sprava = "Posielam spravu na server";
    pipe_open_write(&klient);
    pipe_write(&klient, sprava, strlen(sprava));
    
    pipe_destroy(&klient, 0, 0);
    */
    return 0;

}
