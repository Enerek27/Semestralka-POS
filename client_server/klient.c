
//TODO
// treba potom zmenit na svet.h


#include "../zdrojove_kody/UI.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../sockety/socket.h"
#include "../zdielanaPamat/pipe.h"

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
        perror("Chybne pripojenie na server!!");
        exit(EXIT_FAILURE);
    }
    
    return navrat;

}

void * vypisujObraz(void * arg) {
    socket_client_t * klient = arg;
    pthread_mutex_lock(&klient->mutex);
    socket_data_t copy = klient->activeSocket;
    pthread_mutex_unlock(&klient->mutex);
    while (atomic_load(&klient->klien_bezi)) {
        char buf[100];
        memset(buf, 0, sizeof(buf));
         
        socket_read(&copy, buf, sizeof(buf));
        printf("%s", buf);
    }
}

_Bool nacitaj_zo_suboru(socket_client_t * socket) {
    _Bool nacitaj_zo_suboru;
     while (1) {
    
    
        char buf [200];
        memset(buf, 0, sizeof(buf));
        printf("Zadaj ci sa svet ma nacitat zo suboru? (1-ano/0-nie)): \n");
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            perror("Chyba nacitavanie textu");
            exit(EXIT_FAILURE);
        }
        char * kontrola;
        int tmp_int;
        tmp_int = strtol(buf, &kontrola, 10);
        if (kontrola == buf) {
            printf("To nie je cislo zadaj znova!!\n");
        } else {
            if (tmp_int == 1 || tmp_int == 0) {
                nacitaj_zo_suboru = (_Bool)tmp_int;
                break;
            } else {
                printf("Zle zadane cislo skus znova");
            }

            
        }
    }
    char cesta_k_suboru[200];
    if (nacitaj_zo_suboru) {
    
        
        memset(cesta_k_suboru, 0, sizeof(cesta_k_suboru));
        while (1) {
        
        
            char buf [200];
            memset(buf, 0, sizeof(buf));
            printf("Zadaj cestu k suboru: \n");
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                perror("Chyba nacitavanie textu");
                exit(EXIT_FAILURE);
            }
            buf[strcspn(buf, "\n")] = '\0';
            memcpy(cesta_k_suboru, buf,strlen(buf) + 1);
            
        }
    }
    if (nacitaj_zo_suboru) {
        socket_data_t copy;
        pthread_mutex_lock(&socket->mutex);
        copy = socket->activeSocket;
        pthread_mutex_unlock(&socket->mutex);
        
        
        char poslat[230];
        memset(poslat, 0, sizeof(poslat));
        poslat[0] = '7';
        poslat[1] = ';';            //7;cestaksuboru
        memcpy(poslat + 2, cesta_k_suboru, strlen(cesta_k_suboru) + 1);
        socket_write(&socket->activeSocket, poslat, strlen(poslat) + 1);
    } else {
        socket_data_t copy;
        pthread_mutex_lock(&socket->mutex);
        copy = socket->activeSocket;
        pthread_mutex_unlock(&socket->mutex);
        char buf[2];
        buf[0] = '5';
        buf[1] = ';';
        socket_write(&copy,buf, 2);
    }

    return nacitaj_zo_suboru;

}

int main(int argc, char const *argv[])
{
   while (1) {
    //tvoje menu
    //bude vracat strukturu
    //
    // switch co lydkine menu vrati podla toho sa bude nieco robit

    switch (hlavne_menu_klient()) {
        case 1:
                ////// NOVA SIMULACIA
                ///////////////////KTORY TYP
                int ktoryTyp;
                char buf [200];
                memset(buf, 0, sizeof(buf));
                printf("Typ simulovaneho sveta: interaktivny -> napis 1 ; sumarny -> napis 2. \n");
                if (fgets(buf, sizeof(buf), stdin) == NULL) {
                    perror("Chyba nacitavania textu.");
                    exit(EXIT_FAILURE);
                }
                char * kontrola;    //ak nieco ostane v nej, znamena, ze zachytilo aspon nejake cislo
                ktoryTyp = strtol(buf, &kontrola, 10);
                if (kontrola == buf) {
                    printf("To nie je cislo zadaj znova!!\n");
                } else {
                    if (ktoryTyp == 1) {
                        ////spusti interaktivny
                        
                    } else {
                        //spusti sumarny
                    }
                }

               
                //NACITAVANIE ZO SUBORU SOM DALA PREC

                //////////////  KOLKO POUZIVATELOV
                int pocetPouzivatelov;
                _Bool dobreZadal = 1;
                while (dobreZadal) {

                printf("Napis pre kolko pouzivatelov ma byt urcena aplikacia:  \n");
                if (fgets(buf, sizeof(buf), stdin) == NULL) {
                    perror("Chyba nacitavania textu.");
                    exit(EXIT_FAILURE);
                }

                pocetPouzivatelov = strtol(buf, &kontrola, 10);
                if (kontrola == buf) {
                    printf("To nie je cislo zadaj znova!!\n");
                } else {
                    if (pocetPouzivatelov == 1) {
                        printf("Aplikacia je nastavena pre %d klienta.\n", pocetPouzivatelov);
                        dobreZadal = 0;
                        break;
                    } else if(pocetPouzivatelov > 1) {
                        printf("Aplikacia je nastavena pre %d klientov.\n", pocetPouzivatelov);
                        dobreZadal = 0;
                        break;
                    } else {
                        dobreZadal = 1;
                    }
                } 

                }  //zatvroka while
                  
            break;
        case 2:
                //pripojenie k simulacii
                //treba si vypytat adresu pripojenia- string
                printf("Napis adresu pripojenia : ");
                char * adresaPripojenia  = fgets(buf, sizeof(buf), stdin);
                if (adresaPripojenia == NULL) {
                    perror("Chyba nacitavanie textu");
                    exit(EXIT_FAILURE);
                } else {
                    printf("Adresa pripojenia je: %s.",adresaPripojenia);
                }   
            break;
        case 3:
                //opatovne spustenie simulacii
                //treba vypytat cestu k suboru
                printf("Napis cestu k suboru:  \n");
                char * nameFile  = fgets(buf, sizeof(buf), stdin);
                if (nameFile == NULL) {
                    perror("Chyba nacitavania textu.");
                    exit(EXIT_FAILURE);
                } else {
                    printf("Nazov suboru je: %s.",nameFile);
                }     
            break;
        case 4:
                //koniec - ukoncenie aplikacie
                //ci chce ukoncit server alebo nie
                //AKO vyjdeme z vajlu, ked sa skonci toooto
                int ciChceUkoncitServer;
                printf("Chces ukoncit aj server -> napis 1, ak nehces -> napis 0. \n");
                
                ciChceUkoncitServer = strtol(buf, &kontrola, 10);
                
                if (kontrola == buf) {
                    printf("To nie je cislo zadaj znova!!\n");
                } else {
                    if (ciChceUkoncitServer == 1) {
                        //ukoncujem server
                    } else {
                        //nechce ukoncit server
                    }
                } 
            break;

    };












   }




    socket_client_t socket_client;

    socket_client_init(&socket_client, "192.168.1.10", "777");
    if (je_klient_hlavny(&socket_client)) {
        if (!nacitaj_zo_suboru(&socket_client)) {
            spusti_initmenu_klient(&socket_client);
        }
        
    }
    
    pthread_t vlakno;
    pthread_create(&vlakno, NULL, vypisujObraz, &socket_client);
    pthread_detach(vlakno);

    while (atomic_load(&socket_client.klien_bezi)) {
        
    }

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));

    socket_read(&socket_client.activeSocket, buffer, sizeof(buffer));

    printf("Sprava co poslal server je: %s\n", buffer);

    socket_client_destroy(&socket_client);
    
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
