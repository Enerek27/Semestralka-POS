
//TODO
// treba potom zmenit na svet.h
#define _POSIX_C_SOURCE 200809L
#include <time.h>

#define RED "\033[31m"
#define GREEN "\033[31m"
#define ORANGE "\033[38;5;9m"
#define RESET "\033[0m"

#include "../zdrojove_kody/UI.h"


#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "../sockety/socket.h"
#include "../zdielanaPamat/pipe.h"
#include <sys/select.h>
#include <sys/wait.h>

_Bool je_klient_hlavny(socket_client_t * socket) {
    char buff[20];
    memset(buff, 0, sizeof(buff));
    socket_read(&socket->activeSocket, buff, sizeof(buff));
    int nacitane = atoi(buff);

    _Bool navrat;
    if (nacitane == 7) {
        //je to hlavny klient
        navrat = 1;
    } else if (nacitane == 5) {
        //nie je to hlavny klient
        navrat = 0;
    } else {
        perror(RED "Chybne pripojenie na server." RESET);
        exit(EXIT_FAILURE);
    }
    
    return navrat;

}

//TODO treba resetovat vsetko pred dalsou simulaciuo skusit nacitanie a ulozenie do suboru a viacej klientov 
// pridat moznost iba odist z simulacie a znova sa pripojit



void * vypisujObraz(void * arg) {
    socket_client_t * klient = arg;
    
    char * buf;
    char nacitaj[50];
    int maxVelkost = 100;
    
    int aktualVelkost = 0;
    buf = calloc(maxVelkost, sizeof(char));
    if (buf == NULL) {
        perror(RED "Chyba alokovania pamäte." RESET);
        exit(EXIT_FAILURE);
    }
    
    while (atomic_load(&klient->klien_bezi)) {
        
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(klient->activeSocket.socket, &readfds);
    struct timeval tv = {0, 300000}; // 0.3 s
    int rv = select(klient->activeSocket.socket + 1, &readfds, NULL, NULL, &tv);
     if (rv == -1) {
        perror("select");
        break;
        } else if (rv == 0) {
          continue; 
        } else {
                int n = socket_read(&klient->activeSocket, nacitaj, 50);
            if (n <= 0) {
                //chyba alebo sa zavrej socket
                atomic_store(&klient->klien_bezi, 0);
                break;
            }

           
            for (int i = 0; i < n; i++) {
                    if (maxVelkost - 1 <= aktualVelkost) {
                        int novaVelkost = maxVelkost + 50;
                        char * tmp = realloc(buf, novaVelkost);
                        if (tmp == NULL) {
                            perror(RED "Chyba pri zväčšení pamäte." RESET);
                            free(buf);
                            exit(EXIT_FAILURE);
                        }
                        buf = tmp;
                        maxVelkost = novaVelkost;

                        
                    }
                    buf[aktualVelkost] = nacitaj[i];
                    aktualVelkost++;
                    //treba odstranit iba na test
                // printf("%s", buf);
                    if (strstr(buf, "off") != NULL) {
                        atomic_store(&klient->klien_bezi, 0);
                        break;
                    }
                    if (buf[aktualVelkost - 1] == '\0') {
                        printf("%s", buf);
                        fflush(stdout);
                        memset(buf, 0, maxVelkost);
                        aktualVelkost = 0;
                    }

            }
        }

    }
    free(buf);
    pthread_exit(NULL);
}

_Bool nacitaj_zo_suboru(socket_client_t * socket, char * mozno_cesta_subor) {
    _Bool nacitaj_zo_suboru;
    if (mozno_cesta_subor == NULL) {
        while (1) {
    
    
        char buf [200];
        memset(buf, 0, sizeof(buf));
        printf(GREEN "Zadaj, či sa svet ma načítať zo súboru(1) alebo nie(0)?: " RESET);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            perror("Chyba načitavania textu");
            exit(EXIT_FAILURE);
        }
        char * kontrola;
        int tmp_int;
        tmp_int = strtol(buf, &kontrola, 10);
        if (kontrola == buf) {
            printf(RED "To nie je číslo, zadaj znova.\n" RESET);
        } else {
            if (tmp_int == 1 || tmp_int == 0) {
                nacitaj_zo_suboru = (_Bool)tmp_int;
                break;
            } else {
                printf(ORANGE "Zle zadané číslo, skús znova." RESET);
            }

            
        }
    }
    char cesta_k_suboru[200];
    if (nacitaj_zo_suboru) {
    
        
        memset(cesta_k_suboru, 0, sizeof(cesta_k_suboru));
        while (1) {
        
        
            char buf [200];
            memset(buf, 0, sizeof(buf));
            printf(GREEN "Zadaj cestu k súboru: " RESET);
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                perror(RED "Chyba načitavania textu." RESET);
                exit(EXIT_FAILURE);
            }
            buf[strcspn(buf, "\n")] = '\0';
            memcpy(cesta_k_suboru, buf,strlen(buf) + 1);
            break;
        }
    }
    if (nacitaj_zo_suboru) {
        
      
        
        
        char poslat[230];
        memset(poslat, 0, sizeof(poslat));
        poslat[0] = '7';
        poslat[1] = ';';            //7;cestaksuboru
        memcpy(poslat + 2, cesta_k_suboru, strlen(cesta_k_suboru) + 1);
        socket_write(&socket->activeSocket, poslat, strlen(poslat) + 1);
    } else {
        
        char buf[2];
        buf[0] = '5';
        buf[1] = ';';
        socket_write(&socket->activeSocket,buf, 2);
    }

    return nacitaj_zo_suboru;
    } else {
       
        
        
        char poslat[230];
        memset(poslat, 0, sizeof(poslat));
        poslat[0] = '7';
        poslat[1] = ';';            //7;cestaksuboru
        memcpy(poslat + 2, mozno_cesta_subor, strlen(mozno_cesta_subor) + 1);
        socket_write(&socket->activeSocket, poslat, strlen(poslat) + 1);
        return 1;
    }
     

}
static void sigchld_handler(int sig)
{
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}


int main(int argc, char const *argv[])
{
    _Bool idem = 1;
    
    struct sigaction sa = {0};
    sa.sa_handler = sigchld_handler;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

   while (idem) {
        //tvoje menu
        //
        //
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

                        printf(GREEN "Napíš pre koľko používateľov má byť určená aplikácia:  " RESET);
                        if (fgets(buf, sizeof(buf), stdin) == NULL) {
                            perror(RED "Chyba načitavania textu." RESET);
                            exit(EXIT_FAILURE);
                        }
                        
                        pocetPouzivatelov = strtol(buf, &kontrola, 10);
                        if (kontrola == buf) {
                            printf(ORANGE "To nie je číslo, zadaj znova.\n" RESET);
                        } else {
                            if (pocetPouzivatelov == 1) {
                            
                                dobreZadal = 0;
                                break;
                            } else if(pocetPouzivatelov > 1) {
                            
                                dobreZadal = 0;
                                break;
                            } else {
                                dobreZadal = 1;
                            }
                        } 

                    }  

                    


                    pid_t pid = fork();
                    if (pid == 0) {
                        //treba zmenit na execl
                        execl("./server","server", NULL);
                        perror(RED "Chyba pri spustení servera." RESET);
                        _exit(EXIT_FAILURE);
                    } else if (pid > 0) {
                        //tu bezi klient
                        sleep(1);
                        socket_client_t socket_client;
                        socket_client_init(&socket_client, "127.0.0.1", "2000");
                        if (je_klient_hlavny(&socket_client)) {
                            if (!nacitaj_zo_suboru(&socket_client, NULL)) {
                                spusti_initmenu_klient(&socket_client);
                            }   
                        }

                        pthread_t vlakno;
                        pthread_create(&vlakno, NULL, vypisujObraz, &socket_client);
                        
                        
                        while (atomic_load(&socket_client.klien_bezi)) {
                            
                                


                                //bude tu fgets s prevodom na cislo a kontrolou prevodu a bude tu switch podla cisla 
                                //je to reakcia na menu ktore lydka robiiiii stlacenie klavesnice 
                                //reakcia na menu ktore sa robilo na 2 krat to nepochopene menu ktore robit mala 
                                //lubim ju :)
                            
                            fd_set fds;
                            FD_ZERO(&fds);
                            FD_SET(STDIN_FILENO, &fds);

                            struct timeval tv;
                            tv.tv_sec = 0;
                            tv.tv_usec = 300000; 

                            int rv = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);

                            if (rv == -1) {
                                perror("select stdin");
                                break;
                            }

                           
                            if (!atomic_load(&socket_client.klien_bezi)) {
                                break; 
                            }

                            if (rv == 0) {
                                continue; 
                            }

                            if (FD_ISSET(STDIN_FILENO, &fds)) {
                                char stlacene[10];
                                int stlacenePismeno;
                                char * endptr;
                                
                                if (fgets(stlacene, sizeof(stlacene), stdin) == NULL) {
                                    printf(ORANGE "Nezadal si číslo, skús znova.\n" RESET);
                                    break;
                                }
                                stlacene[strcspn(stlacene, "\n")] = '\0';
                                stlacenePismeno = strtol(stlacene, &endptr, 10);
                                if (stlacene == endptr) {
                                    printf(ORANGE "To nie je číslo, zadaj znova.\n" RESET);
                                    break;
                                }
                                if (!atomic_load(&socket_client.klien_bezi)) {
                                    break;
                                }
                                char buff[2];
                                switch (stlacenePismeno) {
                                    case 1:
                                        //signal vypnutie
                                        buff[0] = '0';
                                        buff[1] = '\0';
                                        
                                        pthread_mutex_lock(&socket_client.mutex);
                                        socket_write(&socket_client.activeSocket, buff , strlen(buff));
                                        pthread_mutex_unlock(&socket_client.mutex);
                                    
                                        sleep(1);
                                        atomic_store(&socket_client.klien_bezi, 0);
                                        break;
                                    case 2:
                                        //signal prepni mod
                                        buff[0] = '1';
                                        buff[1] = '\0';
                                        
                                        pthread_mutex_lock(&socket_client.mutex);
                                        socket_write(&socket_client.activeSocket, buff , strlen(buff));
                                        pthread_mutex_unlock(&socket_client.mutex);
                                    
                                        break;
                                    case 3:
                                        //signal v prepnutom mode chcem teraz statistiku
                                        buff[0] = '2';
                                        buff[1] = '\0';
                                        
                                        pthread_mutex_lock(&socket_client.mutex);
                                        socket_write(&socket_client.activeSocket, buff , strlen(buff));
                                        pthread_mutex_unlock(&socket_client.mutex);
                                        break;
                                    case 4:
                                        //signal v prepnutom mode chcem teraz kroky
                                        buff[0] = '3';
                                        buff[1] = '\0';
                                        
                                        pthread_mutex_lock(&socket_client.mutex);
                                        socket_write(&socket_client.activeSocket, buff , strlen(buff));
                                        pthread_mutex_unlock(&socket_client.mutex);
                                        break;
                                    case 5:
                                        //signal na odpojenie
                                        buff[0] = '4';
                                        buff[1] = '\0';
                                        
                                        pthread_mutex_lock(&socket_client.mutex);
                                        socket_write(&socket_client.activeSocket, buff , strlen(buff));
                                        pthread_mutex_unlock(&socket_client.mutex);
                                        atomic_store(&socket_client.klien_bezi, 0);
                                        break;
                                    default:
                                        printf(ORANGE "Také číslo nie je uvedené.\n" RESET);
                                        break;
                                }
                            }
                            



                        }
                        
                        
                        pthread_join(vlakno, NULL);
                        socket_client_destroy(&socket_client);
                        
                        
                    } else {
                        perror(RED "Chyba vytvorenia procesu." RESET);
                        exit(EXIT_FAILURE);
                    }
                }
                break;




                    
                
            case 2:
            //TODO prestavit ako 1 aby bolo dobre :)
                    { char buf[200];
                    //treba upravit vypinanie aby tam bola dalsia moznost
                    printf(GREEN "Napíš adresu pripojenia: " RESET);
                    char * adresaPripojenia  = fgets(buf, sizeof(buf), stdin);
                    adresaPripojenia[strcspn(adresaPripojenia, "\n")] = '\0';
                    if (adresaPripojenia == NULL) {
                        perror(RED "Chyba načitavania textu." RESET);
                        exit(EXIT_FAILURE);
                    } 
                    socket_client_t socket_client;
                    socket_client_init(&socket_client, adresaPripojenia, "2000");
                    je_klient_hlavny(&socket_client);
                    pthread_t vlakno;
                    pthread_create(&vlakno, NULL, vypisujObraz, &socket_client);
                    

                    while (atomic_load(&socket_client.klien_bezi)) {
                                //bude tu fgets s prevodom na cislo a kontrolou prevodu a bude tu switch podla cisla 
                                //je to reakcia na menu ktore lydka robiiiii stlacenie klavesnice 
                                //reakcia na menu ktore sa robilo na 2 krat to nepochopene menu ktore robit mala 
                                //lubim ju :)
                            
                            fd_set fds;
                            FD_ZERO(&fds);
                            FD_SET(STDIN_FILENO, &fds);

                            struct timeval tv;
                            tv.tv_sec = 0;
                            tv.tv_usec = 300000; 

                            int rv = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);

                            if (rv == -1) {
                                perror("select stdin");
                                break;
                            }

                           
                            if (!atomic_load(&socket_client.klien_bezi)) {
                                break; 
                            }

                            if (rv == 0) {
                                continue; 
                            }

                            if (FD_ISSET(STDIN_FILENO, &fds)) {
                                char stlacene[10];
                                int stlacenePismeno;
                                char * endptr;
                                
                                if (fgets(stlacene, sizeof(stlacene), stdin) == NULL) {
                                    printf(ORANGE "Nezadal si číslo, skús znova.\n" RESET);
                                    break;
                                }
                                stlacene[strcspn(stlacene, "\n")] = '\0';
                                stlacenePismeno = strtol(stlacene, &endptr, 10);
                                if (stlacene == endptr) {
                                    printf(ORANGE "Nie je to číslo.\n" RESET);
                                    break;
                                }
                                if (!atomic_load(&socket_client.klien_bezi)) {
                                    break;
                                }
                                char buff[2];
                                switch (stlacenePismeno) {
                                    case 1:
                                        //signal vypnutie
                                        buff[0] = '0';
                                        buff[1] = '\0';
                                        
                                        pthread_mutex_lock(&socket_client.mutex);
                                        socket_write(&socket_client.activeSocket, buff , strlen(buff));
                                        pthread_mutex_unlock(&socket_client.mutex);
                                    
                                        sleep(1);
                                        atomic_store(&socket_client.klien_bezi, 0);
                                        break;
                                    case 2:
                                        //signal prepni mod
                                        buff[0] = '1';
                                        buff[1] = '\0';
                                        
                                        pthread_mutex_lock(&socket_client.mutex);
                                        socket_write(&socket_client.activeSocket, buff , strlen(buff));
                                        pthread_mutex_unlock(&socket_client.mutex);
                                    
                                        break;
                                    case 3:
                                        //signal v prepnutom mode chcem teraz statistiku
                                        buff[0] = '2';
                                        buff[1] = '\0';
                                        
                                        pthread_mutex_lock(&socket_client.mutex);
                                        socket_write(&socket_client.activeSocket, buff , strlen(buff));
                                        pthread_mutex_unlock(&socket_client.mutex);
                                        break;
                                    case 4:
                                        //signal v prepnutom mode chcem teraz kroky
                                        buff[0] = '3';
                                        buff[1] = '\0';
                                        
                                        pthread_mutex_lock(&socket_client.mutex);
                                        socket_write(&socket_client.activeSocket, buff , strlen(buff));
                                        pthread_mutex_unlock(&socket_client.mutex);
                                        break;
                                    case 5:
                                        //signal na odpojenie
                                        buff[0] = '4';
                                        buff[1] = '\0';
                                        
                                        pthread_mutex_lock(&socket_client.mutex);
                                        socket_write(&socket_client.activeSocket, buff , strlen(buff));
                                        pthread_mutex_unlock(&socket_client.mutex);
                                        atomic_store(&socket_client.klien_bezi, 0);
                                        break;
                                    default:
                                        printf(ORANGE "Také číslo nie je uvedené.\n" RESET);
                                        break;
                                }
                            }
                            

                    }
                    pthread_join(vlakno, NULL);
                    socket_client_destroy(&socket_client);
                    
                    
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
                    printf(GREEN "Zadaj cestu k súboru: \n" RESET);
                    if (fgets(buf, sizeof(buf), stdin) == NULL) {
                        perror(RED "Chyba načitavania textu." RESET);
                        exit(EXIT_FAILURE);
                    }
                    buf[strcspn(buf, "\n")] = '\0';
                    memcpy(cesta_k_suboru, buf,strlen(buf) + 1);
                    break;
                }
                pid_t pid1 = fork();
                if (pid1 == 0) {
                    //treba zmenit na execl
                    execl("./server","server", NULL);
                    perror(RED "Chyba pri spustení servera." RESET);
                    _exit(EXIT_FAILURE);
                } else if (pid1 > 0) {
                    //tu bezi klient
                    sleep(1);
                    socket_client_t socket_client;
                    socket_client_init(&socket_client, "127.0.0.1", "2000");
                    if (je_klient_hlavny(&socket_client)) {
                        if (!nacitaj_zo_suboru(&socket_client, cesta_k_suboru)) {
                            printf("Subor sa nenasiel\n");
                            break;
                        }   
                    }

                    pthread_t vlakno;
                    pthread_create(&vlakno, NULL, vypisujObraz, &socket_client);
                    
                    while (atomic_load(&socket_client.klien_bezi)) {

                              //bude tu fgets s prevodom na cislo a kontrolou prevodu a bude tu switch podla cisla 
                                //je to reakcia na menu ktore lydka robiiiii stlacenie klavesnice 
                                //reakcia na menu ktore sa robilo na 2 krat to nepochopene menu ktore robit mala 
                                //lubim ju :)
                            
                            fd_set fds;
                            FD_ZERO(&fds);
                            FD_SET(STDIN_FILENO, &fds);

                            struct timeval tv;
                            tv.tv_sec = 0;
                            tv.tv_usec = 300000; 

                            int rv = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);

                            if (rv == -1) {
                                perror("select stdin");
                                break;
                            }

                           
                            if (!atomic_load(&socket_client.klien_bezi)) {
                                break; 
                            }

                            if (rv == 0) {
                                continue; 
                            }

                            if (FD_ISSET(STDIN_FILENO, &fds)) {
                                char stlacene[10];
                                int stlacenePismeno;
                                char * endptr;
                                
                                if (fgets(stlacene, sizeof(stlacene), stdin) == NULL) {
                                    printf(ORANGE "Nezadal si číslo, skús znova.\n" RESET);
                                    break;
                                }
                                stlacene[strcspn(stlacene, "\n")] = '\0';
                                stlacenePismeno = strtol(stlacene, &endptr, 10);
                                if (stlacene == endptr) {
                                    printf(ORANGE "Nie je to číslo.\n" RESET);
                                    break;
                                }
                                if (!atomic_load(&socket_client.klien_bezi)) {
                                    break;
                                }
                                char buff[2];
                                switch (stlacenePismeno) {
                                    case 1:
                                        //signal vypnutie
                                        buff[0] = '0';
                                        buff[1] = '\0';
                                        
                                        pthread_mutex_lock(&socket_client.mutex);
                                        socket_write(&socket_client.activeSocket, buff , strlen(buff));
                                        pthread_mutex_unlock(&socket_client.mutex);
                                    
                                        sleep(1);
                                        atomic_store(&socket_client.klien_bezi, 0);
                                        break;
                                    case 2:
                                        //signal prepni mod
                                        buff[0] = '1';
                                        buff[1] = '\0';
                                        
                                        pthread_mutex_lock(&socket_client.mutex);
                                        socket_write(&socket_client.activeSocket, buff , strlen(buff));
                                        pthread_mutex_unlock(&socket_client.mutex);
                                    
                                        break;
                                    case 3:
                                        //signal v prepnutom mode chcem teraz statistiku
                                        buff[0] = '2';
                                        buff[1] = '\0';
                                        
                                        pthread_mutex_lock(&socket_client.mutex);
                                        socket_write(&socket_client.activeSocket, buff , strlen(buff));
                                        pthread_mutex_unlock(&socket_client.mutex);
                                        break;
                                    case 4:
                                        //signal v prepnutom mode chcem teraz kroky
                                        buff[0] = '3';
                                        buff[1] = '\0';
                                        
                                        pthread_mutex_lock(&socket_client.mutex);
                                        socket_write(&socket_client.activeSocket, buff , strlen(buff));
                                        pthread_mutex_unlock(&socket_client.mutex);
                                        break;
                                    case 5:
                                        //signal na odpojenie
                                        buff[0] = '4';
                                        buff[1] = '\0';
                                        
                                        pthread_mutex_lock(&socket_client.mutex);
                                        socket_write(&socket_client.activeSocket, buff , strlen(buff));
                                        pthread_mutex_unlock(&socket_client.mutex);
                                        atomic_store(&socket_client.klien_bezi, 0);
                                        break;
                                    default:
                                        printf(ORANGE "Take číslo nie je uvedené.\n" RESET);
                                        break;
                                }
                            }
                    }
                    pthread_join(vlakno, NULL);
                    socket_client_destroy(&socket_client);
                    
                    
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
