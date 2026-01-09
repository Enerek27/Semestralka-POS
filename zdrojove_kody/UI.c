
#define _POSIX_C_SOURCE 199309L   

#define RED "\033[31m"
#define MAGENTA "\033[35m"
#define GREEN "\033[32m"
#define ORANGE "\033[38;5;9m"
#define BLUE "\033[34m"
#define UZIVATELFARBA "\033[38;5;14m"
#define ZLATA "\033[38;5;222m"
#define RESET "\033[0m"

#include "UI.h"


#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>




char * vrat_menu_klient() {
    char * buff;
    int maxPocetZnakov = 250;
    buff = calloc(maxPocetZnakov, sizeof(char));
    if (buff == NULL)
   {
    perror(RED "Chyba pamäte v inicializácii, vo vykresleni sveta." RESET);
    exit(EXIT_FAILURE);
   }

   int len = 0;
   
   snprintf(buff, maxPocetZnakov,
    ZLATA "MENU:\n"
            ZLATA "1. VYPNI SIMULACIU\n"
            ZLATA "2. ZMEN MOD SIMULACIE\n"
            ZLATA "3. ZOBRAZ STATISTIKU\n"
            ZLATA "4. ZOBRAZ KROKY\n"
            ZLATA "5. ODPOJ SA OD SIMULACIE\n" RESET
            GREEN "Tvoje odpoved je: " RESET );
   return buff;
}

char * vycisti_obrazovku() {

    char * buff;
   char zatial[10];
   int aktualPocetZnakov = 0;
   int maxPocetZnakov = 5;
   buff = calloc(maxPocetZnakov, sizeof(char));
   if (buff == NULL)
   {
    perror(RED "Chyba pamäte v inicializácii, vo vykresleni sveta." RESET);
    exit(EXIT_FAILURE);
   }

   int len = snprintf(zatial, sizeof(zatial), "\033[2J");

   if (aktualPocetZnakov + len >= maxPocetZnakov)
   {
    int novyMax = maxPocetZnakov + 10;
    char * zatial1 = realloc(buff, novyMax * sizeof(char));
    if (zatial1 == NULL)
    {
        perror(RED "Chyba pamäte v inicializácii  realokacia." RESET);
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff, zatial, len);
   aktualPocetZnakov += len;
    //printf("\033[H");   //ako clear


   len = snprintf(zatial, sizeof(zatial), "\033[H");

   if (aktualPocetZnakov + len >= maxPocetZnakov)
   {
    int novyMax = maxPocetZnakov + 10;
    char * zatial1 = realloc(buff, novyMax * sizeof(char));
    if (zatial1 == NULL)
    {
        perror(RED "Chyba pamäte v inicializácii  realokacia." RESET);
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff + aktualPocetZnakov, zatial, len);
   aktualPocetZnakov += len;
    
    //printf("\033[2J");  //kurzor posunie do laveho horneho rohu
    //fflush(stdout);
   
    return buff;
}

char * vykresli_svet(svt_t * svet) {
    char * buff;
   char zatial[128];
   int aktualPocetZnakov = 0;
   int maxPocetZnakov = 100;
   buff = calloc(maxPocetZnakov, sizeof(char));

   if (buff == NULL)
   {
    perror(RED "Chyba pamäte v inicializácii, vo vykresleni sveta." RESET);
    exit(EXIT_FAILURE);
   }

   char * vycistenieObr = vycisti_obrazovku(); // ona mi posle char, co mam do zaciatku buffru prilepit
   int len = snprintf(zatial, sizeof(zatial), "%s", vycistenieObr);

   if (aktualPocetZnakov + len >= maxPocetZnakov)
   {
    int novyMax = maxPocetZnakov + 50;
    char * zatial1 = realloc(buff, novyMax * sizeof(char));
    if (zatial1 == NULL)
    {
        perror(RED "Chyba pamäte v inicializácii  realokácia." RESET);
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff + aktualPocetZnakov, zatial, len);
   aktualPocetZnakov += len;
   free(vycistenieObr);


   
   len = snprintf(zatial, sizeof(zatial), "\033[90m%s: %d/\033[75m %d\033[0m\n","Počet replikácii", svet->original_replikacii, svet->pocet_replikacii );

   if (aktualPocetZnakov + len >= maxPocetZnakov)
   {
    int novyMax = maxPocetZnakov + 50;
    char * zatial1 = realloc(buff, novyMax * sizeof(char));
    if (zatial1 == NULL)
    {
        perror(RED "Chyba pamäte v inicializácii  realokácia." RESET);
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff + aktualPocetZnakov, zatial, len);
   aktualPocetZnakov += len;

   len = snprintf(zatial, sizeof(zatial), "\033[93m%s: %d/%d\033[0m\n","Počet krokov", svet->pocet_krokov_K_origo, svet->pocet_krokov_K);

   if (aktualPocetZnakov + len >= maxPocetZnakov)
   {
    int novyMax = maxPocetZnakov + 50;
    char * zatial1 = realloc(buff, novyMax * sizeof(char));
    if (zatial1 == NULL)
    {
        perror(RED "Chyba pamäte v inicializácii  realokácia." RESET);
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff + aktualPocetZnakov, zatial, len);
   aktualPocetZnakov += len;
   


   
    for (int i = 0; i < svet->hranica_y; i++)   //vonkajsi for, menej sa opakuje, je to y
    {
        for (int j = 0; j < svet->hranica_x; j++) {    //suradnica x
            //prekazka, chodec, nic, stred
            // 0 reprezentuje prazdne policko, 1 je chodec, 2 je prekazka

            char c;
            char *farba;
    

            if(svet->stred_x == j && svet->stred_y == i)    //stred
            {
                c = '*';
                farba = "\033[32m"; // zelená
            } else if (svet->pole[j][i] == 0)   //prazdne
            {
                c = '-';
                farba = "\033[37m"; // biela 
            }else if (svet->pole[j][i] == 1)    //chodec
            {
                c = 'C';
                farba = "\033[33m";
            } else{
                //rovna sa 2 == prekazka
                c = 'X';
                farba = "\033[31m"; // červená prekážka

            }  
            //printf("%s%c \033[0m", farba, c);
            len = snprintf(zatial, sizeof(zatial), "%s%c \033[0m", farba, c);
            if (aktualPocetZnakov + len >= maxPocetZnakov)
            {
                int novyMax = maxPocetZnakov + 50;
                char * zatial1 = realloc(buff, novyMax * sizeof(char));
                if (zatial1 == NULL)
                {
                    perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
                    exit(EXIT_FAILURE);
                }
                buff = zatial1;
                maxPocetZnakov = novyMax;
                
            }
            memcpy(buff + aktualPocetZnakov, zatial, len);
            aktualPocetZnakov += len;
        }
        //putchar('\n');   
        len = snprintf(zatial, sizeof(zatial), "\n");
            if (aktualPocetZnakov + len >= maxPocetZnakov)
            {
                int novyMax = maxPocetZnakov + 5;
                char * zatial1 = realloc(buff, novyMax * sizeof(char));
                if (zatial1 == NULL)
                {
                    perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
                    exit(EXIT_FAILURE);
                }
                buff = zatial1;
                maxPocetZnakov = novyMax;
                
            }
            memcpy(buff + aktualPocetZnakov, zatial, len);
            aktualPocetZnakov += len;
        }



            char * opatovneMenu = vrat_menu_klient();
            len = snprintf(zatial, sizeof(zatial), "%s", opatovneMenu) + 1;

            if (aktualPocetZnakov + len >= maxPocetZnakov)
            {
                int novyMax = maxPocetZnakov + 200;
                char * zatial1 = realloc(buff, novyMax * sizeof(char));
                if (zatial1 == NULL)
                {
                    perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
                    exit(EXIT_FAILURE);
                }
                buff = zatial1;
                maxPocetZnakov = novyMax;
            }
            memcpy(buff + aktualPocetZnakov, zatial, len);
            aktualPocetZnakov += len;
            free(opatovneMenu);
          

    return buff;
    
}


char * svet_vypis_statistiku(svt_t * svet) {
    //vycisti_obrazovku();
     char * buff;
   char zatial[128];
   int aktualPocetZnakov = 0;
   int maxPocetZnakov = 100;
   buff = calloc(maxPocetZnakov, sizeof(char));

   if (buff == NULL)
   {
    perror(RED "Chyba pamäte v inicializácii, vo vykresleni sveta." RESET);
    exit(EXIT_FAILURE);
   }

   char * vycistenieObr = vycisti_obrazovku(); 
   int len = snprintf(zatial, sizeof(zatial), "%s", vycistenieObr);

   if (aktualPocetZnakov + len >= maxPocetZnakov)
   {
    int novyMax = maxPocetZnakov + 50;
    char * zatial1 = realloc(buff, novyMax * sizeof(char));
    if (zatial1 == NULL)
    {
        perror(RED "Chyba pamäte v inicializácii  realokácia." RESET);
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff + aktualPocetZnakov, zatial, len);
   aktualPocetZnakov += len;
   free(vycistenieObr);


   //printf("\033[1;32m--- STATISTIKA PRAVDEPODOBNOSTI ---\033[0m\n");  
    len = snprintf(zatial, sizeof(zatial), "\033[1;32m--- ŠTATISTIKA PRAVDEPODOBNOSTI ---\033[0m\n");

   if (aktualPocetZnakov + len >= maxPocetZnakov)
   {
    int novyMax = maxPocetZnakov + 50;
    char * zatial1 = realloc(buff, novyMax * sizeof(char));
    if (zatial1 == NULL)
    {
        perror(RED "Chyba pamäte v inicializácii  realokácia." RESET);
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff + aktualPocetZnakov, zatial, len);
   aktualPocetZnakov += len;


   
     for (int i = 0; i < svet->hranica_y; i++) {    
        for (int j = 0; j < svet->hranica_x; j++) {
            if (svet->pole[j][i] == 2) {

                //printf("\033[31m%7s\033[0m", "X");
                 len = snprintf(zatial, sizeof(zatial), "\033[31m%7s\033[0m", "X");
                if (aktualPocetZnakov + len >= maxPocetZnakov)
                    {
                        int novyMax = maxPocetZnakov + 50;
                        char * zatial1 = realloc(buff, novyMax * sizeof(char));
                        if (zatial1 == NULL)
                        {
                            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
                            exit(EXIT_FAILURE);
                        }
                        buff = zatial1;
                        maxPocetZnakov = novyMax;
                    }
                    memcpy(buff + aktualPocetZnakov, zatial, len);
                    aktualPocetZnakov += len;
                
            } else {
                //printf("%7.2f", svet->pole_pravdepodobnosti[j][i]);
                len = snprintf(zatial, sizeof(zatial), "%7.2f", svet->pole_pravdepodobnosti[j][i]);
                if (aktualPocetZnakov + len >= maxPocetZnakov)
                    {
                        int novyMax = maxPocetZnakov + 50;
                        char * zatial1 = realloc(buff, novyMax * sizeof(char));
                        if (zatial1 == NULL)
                        {
                            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
                            exit(EXIT_FAILURE);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
                        }
                        buff = zatial1;
                        maxPocetZnakov = novyMax;
                    }
                    memcpy(buff + aktualPocetZnakov, zatial, len);
                    aktualPocetZnakov += len;
            
            }
            //printf(" ");
            len = snprintf(zatial, sizeof(zatial), "%s", " ");
                if (aktualPocetZnakov + len >= maxPocetZnakov)
                    {
                        int novyMax = maxPocetZnakov + 5;
                        char * zatial1 = realloc(buff, novyMax * sizeof(char));
                        if (zatial1 == NULL)
                        {
                            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
                            exit(EXIT_FAILURE);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
                        }
                        buff = zatial1;
                        maxPocetZnakov = novyMax;
                    }
                    memcpy(buff + aktualPocetZnakov, zatial, len);
                    aktualPocetZnakov += len;
        }

        //printf("\n");
        len = snprintf(zatial, sizeof(zatial), " %s", "\n");
                if (aktualPocetZnakov + len >= maxPocetZnakov)
                    {
                        int novyMax = maxPocetZnakov + 5;
                        char * zatial1 = realloc(buff, novyMax * sizeof(char));
                        if (zatial1 == NULL)
                        {
                            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
                            exit(EXIT_FAILURE);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
                        }
                        buff = zatial1;
                        maxPocetZnakov = novyMax;
                    }
                    memcpy(buff + aktualPocetZnakov, zatial, len);
                    aktualPocetZnakov += len;

    }

    char * opatovneMenu = vrat_menu_klient();
    len = snprintf(zatial, sizeof(zatial), "%s", opatovneMenu) + 1;

    if (aktualPocetZnakov + len >= maxPocetZnakov)
    {
        int novyMax = maxPocetZnakov + 200;
        char * zatial1 = realloc(buff, novyMax * sizeof(char));
        if (zatial1 == NULL)
        {
            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
            exit(EXIT_FAILURE);
        }
        buff = zatial1;
        maxPocetZnakov = novyMax;
    }
    memcpy(buff + aktualPocetZnakov, zatial, len);
    aktualPocetZnakov += len;
    free(opatovneMenu);

    return buff;
}
char *  svet_vypis_kroky(svt_t * svet) {
     //vycisti_obrazovku();
     char * buff;
   char zatial[128];
   int aktualPocetZnakov = 0;
   int maxPocetZnakov = 100;
   buff = calloc(maxPocetZnakov, sizeof(char));

   if (buff == NULL)
   {
    perror(RED "Chyba pamäte v inicializácii, vo vykresleí sveta." RESET);
    exit(EXIT_FAILURE);
   }

   char * vycistenieObr = vycisti_obrazovku(); 
   int len = snprintf(zatial, sizeof(zatial), "%s", vycistenieObr);

   if (aktualPocetZnakov + len >= maxPocetZnakov)
   {
    int novyMax = maxPocetZnakov + 100;
    char * zatial1 = realloc(buff, novyMax * sizeof(char));
    if (zatial1 == NULL)
    {
        perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff + aktualPocetZnakov, zatial, len);
   aktualPocetZnakov += len;

   free(vycistenieObr);


    //printf("\033[1;32m--- STATISTIKA KROKOV ---\033[0m\n");
    len = snprintf(zatial, sizeof(zatial), "%s", "\033[1;32m--- ŠTATISTIKA KROKOV ---\033[0m\n");

   if (aktualPocetZnakov + len >= maxPocetZnakov)
   {
    int novyMax = maxPocetZnakov + 50;
    char * zatial1 = realloc(buff, novyMax * sizeof(char));
    if (zatial1 == NULL)
    {
        perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff + aktualPocetZnakov, zatial, len);
   aktualPocetZnakov += len;



    for (int i = 0; i < svet->hranica_y; i++) {    
            for (int j = 0; j < svet->hranica_x; j++) {
                
                if (svet->pole[j][i] == 2) {
                    //printf("\033[31m%12s\033[0m", "X");
                    len = snprintf(zatial, sizeof(zatial), "\033[31m%12s\033[0m", "X");

                    if (aktualPocetZnakov + len >= maxPocetZnakov)
                    {
                        int novyMax = maxPocetZnakov + 100;
                        char * zatial1 = realloc(buff, novyMax * sizeof(char));
                        if (zatial1 == NULL)
                        {
                            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
                            exit(EXIT_FAILURE);
                        }
                        buff = zatial1;
                        maxPocetZnakov = novyMax;
                    }
                    memcpy(buff + aktualPocetZnakov, zatial, len);
                    aktualPocetZnakov += len;


                } else {
                    //printf("%12.2f", svet->pole_priemer_krok[j][i]); 
                    len = snprintf(zatial, sizeof(zatial), "%12.2f", svet->pole_priemer_krok[j][i]);

                    if (aktualPocetZnakov + len >= maxPocetZnakov)
                    {
                        int novyMax = maxPocetZnakov + 50;
                        char * zatial1 = realloc(buff, novyMax * sizeof(char));
                        if (zatial1 == NULL)
                        {
                            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
                            exit(EXIT_FAILURE);
                        }
                        buff = zatial1;
                        maxPocetZnakov = novyMax;
                    }
                    memcpy(buff + aktualPocetZnakov, zatial, len);
                    aktualPocetZnakov += len;
                
                }
                //printf(" ");   TO
                len = snprintf(zatial, sizeof(zatial), "%s", " ");

                    if (aktualPocetZnakov + len >= maxPocetZnakov)
                    {
                        int novyMax = maxPocetZnakov + 50;
                        char * zatial1 = realloc(buff, novyMax * sizeof(char));
                        if (zatial1 == NULL)
                        {
                            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
                            exit(EXIT_FAILURE);
                        }
                        buff = zatial1;
                        maxPocetZnakov = novyMax;
                    }
                    memcpy(buff + aktualPocetZnakov, zatial, len);
                    aktualPocetZnakov += len;
            }
            //printf("\n");    
            len = snprintf(zatial, sizeof(zatial), "%s", "\n");

                    if (aktualPocetZnakov + len >= maxPocetZnakov)
                    {
                        int novyMax = maxPocetZnakov + 50;
                        char * zatial1 = realloc(buff, novyMax * sizeof(char));
                        if (zatial1 == NULL)
                        {
                            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
                            exit(EXIT_FAILURE);
                        }
                        buff = zatial1;
                        maxPocetZnakov = novyMax;
                    }
                    memcpy(buff + aktualPocetZnakov, zatial, len);
                    aktualPocetZnakov += len;
        }


    char * opatovneMenu = vrat_menu_klient();
    len = snprintf(zatial, sizeof(zatial), "%s", opatovneMenu) + 1;

    if (aktualPocetZnakov + len >= maxPocetZnakov)
    {
        int novyMax = maxPocetZnakov + 200;
        char * zatial1 = realloc(buff, novyMax * sizeof(char));
        if (zatial1 == NULL)
        {
            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
            exit(EXIT_FAILURE);
        }
        buff = zatial1;
        maxPocetZnakov = novyMax;
    }
    memcpy(buff + aktualPocetZnakov, zatial, len);
    aktualPocetZnakov += len;

    free(opatovneMenu);

        return buff;
}




void server_vykonavaj_sim(svt_brd_t * data) {
    
    
    for (int i = 0; i < data->svet->original_replikacii; i++) {
        
        for (int j = 0; j < data->svet->pocet_krokov_K_origo; j++) {
            
            
            if (!atomic_load(&data->server->server_bezi)|| atomic_load(&data->server->server_info.sumarny_mod)) {
                
                return;
            }

            if (data->svet->pocet_krokov_K == 0 && data->svet->pocet_replikacii == 0) {
                return;
            }
            if (data->svet->chodec->x == data->svet->stred_x && data->svet->chodec->y == data->svet->stred_y) {
                printf(ZLATA "Chodec Dosiahol stred!!\n" RESET);
                sleep(1);
                break;
            }
            svt_vp_t vypisovac;
            
            
            vypisovac.server = data->server;
            vypisovac.svet = data->svet;
            posli_vsetkym_svet( &vypisovac);
            posun_chodca(daj_nahodny_smer_pre_chodca(data->svet), data->svet);
            data->svet->pocet_krokov_K--;
            
            struct timespec ts = {0, 500 * 1000000}; 
            nanosleep(&ts, NULL);
        }
        data->svet->pocet_krokov_K = data->svet->pocet_krokov_K_origo;
        data->svet->pocet_replikacii--;
    }
}

void  posli_vsetkym_svet(svt_vp_t * data) {
    
    
    pthread_mutex_lock(&data->server->mutex);
    int pocet_klientov = data->server->pocetKlinetov;
    pthread_mutex_unlock(&data->server->mutex);
    socket_data_t * posielaj;
    posielaj = calloc(pocet_klientov, sizeof(socket_data_t));
    if (posielaj == NULL) {
        perror(RED "Chyba alokovania pamäte pre výpis." RESET);
        exit(EXIT_FAILURE);
    }
    //treba dorobit
    pthread_mutex_lock(&data->server->mutex);
    for (int i = 0; i < pocet_klientov; i++) {
        memcpy(&posielaj[i], &data->server->klienti[i]->socket_pocuvaj, sizeof(socket_data_t));
    }
    
    pthread_mutex_unlock(&data->server->mutex);
    char * buf = vykresli_svet(data->svet);
    for (int i = 0; i < pocet_klientov; i++) {
        //treba pockat na lydku
        if (atomic_load(&data->server->klienti[i]->bezi_klient)) {
            pthread_mutex_lock(&data->server->mutex);
            socket_write(&posielaj[i], buf,strlen(buf) + 1);
            pthread_mutex_unlock(&data->server->mutex);
        }
    };
    sleep(1);
    free(buf);
    free(posielaj);
    
    
    
}
void posli_vsetkym_statistiku(svt_brd_t * data) {
    while (atomic_load(&data->server->server_bezi)) {
    
        
       
        if (!data->server->server_bezi || !atomic_load(&data->server->server_info.sumarny_mod)) {
            
            break;
        }
        
        
        
        
        svt_vp_t  vypisovac;
       
        vypisovac.server = data->server;
        vypisovac.svet = data->svet;
        posli_vsetkym_stat(&vypisovac);
            
        
        sleep(1);
    }
    
}

void  posli_vsetkym_stat(svt_vp_t * data) {

     pthread_mutex_lock(&data->server->mutex);
    int pocet_klientov = data->server->pocetKlinetov;
    pthread_mutex_unlock(&data->server->mutex);
    klient_read_t * klienti;
    klienti = calloc(pocet_klientov, sizeof(klient_read_t));
    if (klienti == NULL) {
        perror(RED "Chyba alokovania pamäte pre vypis." RESET);
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&data->server->mutex);
    for (int i = 0 ; i < pocet_klientov; i++) {
        
        memcpy(&klienti[i], &data->server->klienti[i], sizeof(klient_read_t));
    }
    pthread_mutex_unlock(&data->server->mutex);
    char * buf1;
    char * buf;
    
    
    buf = svet_vypis_kroky(data->svet);
    
    
    buf1 = svet_vypis_statistiku(data->svet);
    for (int i = 0; i < pocet_klientov; i++) {
        //treba pockat na lydku

        //printf("Idem poslat klientovi cislo: %d\n", i + 1);
        
        if (atomic_load(&data->server->klienti[i]->chcem_statistiku)) {
           
            if (atomic_load(&data->server->klienti[i]->bezi_klient)) {
                 pthread_mutex_lock(&data->server->mutex);
                // printf("Lokol som sa idem poslat stat\n");
                 socket_write(&data->server->klienti[i]->socket_pocuvaj, buf1, strlen(buf1) + 1);
                pthread_mutex_unlock(&data->server->mutex);
               // printf("unlock\n");
            }
        } else {
            if (atomic_load(&data->server->klienti[i]->bezi_klient)) {
                pthread_mutex_lock(&data->server->mutex);
                //printf("Lokol som sa idem poslat kroky\n");
                socket_write(&data->server->klienti[i]->socket_pocuvaj, buf, strlen(buf) + 1);
                pthread_mutex_unlock(&data->server->mutex);
               // printf("unlock\n");
            }
        }
        
        
    }
    free(buf);
    free(buf1);
    free(klienti);
    

}


//rozmerx;rozmery;svetprekazky;pocet_replikacii;...
//potrebujem poslat rozmery sveta x,y   svet prekazky 1 = true , 0 = normal
               // dalej pocet replikacii (kym niesom v cieli) 
                // max pocet krokov K, pravdepodobnosti
              // cesta k suboru
              //ci sa ma nacitat zo suboru
void inicializuj_server(srv_p_t * data, socket_client_t * socket) {
    char * buf;
    char tmp[128];
    int aktual_znakov = 0;
    int max = 100;
    buf = calloc(max, sizeof(char));
    if (buf == NULL) {
        perror(RED "Chyba pamäte v inicializácii." RESET);
        exit(EXIT_FAILURE);
    }
    int len = snprintf(tmp, sizeof(tmp), "%d;",data->rozmer_x);
    if (aktual_znakov + len >= max) {
        int novy_max = max + 50;
        char * tmp1 = realloc(buf, novy_max * sizeof(char));
        if (tmp1 == NULL) {
            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
            exit(EXIT_FAILURE);
        }
        buf = tmp1;
        max = novy_max;
    }
    memcpy(buf, tmp, len);
    aktual_znakov += len;

    len = snprintf(tmp,sizeof(tmp), "%d;", data->rozmer_y);
     if (aktual_znakov + len >= max) {
        int novy_max = max + 50;
        char * tmp1 = realloc(buf, novy_max * sizeof(char));
        if (tmp1 == NULL) {
            perror(RED "Chyba paäate v inicializácii realokácia." RESET);
            exit(EXIT_FAILURE);
        }
        buf = tmp1;
        max = novy_max;
    }
    memcpy(buf + aktual_znakov, tmp, len);
    aktual_znakov += len;

    len = 2;
     if (aktual_znakov + len >= max) {
        int novy_max = max + 50;
        char * tmp1 = realloc(buf, novy_max * sizeof(char));
        if (tmp1 == NULL) {
            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
            exit(EXIT_FAILURE);
        }
        buf = tmp1;
        max = novy_max;
    }
    
    if (data->svet_s_prekazkami) {
        tmp[0] = '1';
        tmp[1] = ';';
        memcpy(buf + aktual_znakov, tmp, len);
       
    } else {
        tmp[0] = '0';
        tmp[1] = ';';
         memcpy(buf + aktual_znakov, tmp, len);
    }
    aktual_znakov += len;

    
    len = snprintf(tmp, sizeof(tmp), "%d;", data->pocet_replikacii);
    if (aktual_znakov + len >= max) {
        int novy_max = max + 50;
        char * tmp1 = realloc(buf, novy_max * sizeof(char));
        if (tmp1 == NULL) {
            perror(RED"Chyba pamäte v inicializácii realokácia." RESET);
            exit(EXIT_FAILURE);
        }
        buf = tmp1;
        max = novy_max;
    }
    
    memcpy(buf + aktual_znakov, tmp, len);
    aktual_znakov += len;
//rozmerx;rozmery;svetprekazky;pocet_replikacii;...
//potrebujem poslat rozmery sveta x,y   svet prekazky 1 = true , 0 = normal
               // dalej pocet replikacii (kym niesom v cieli) 
                // max pocet krokov K, pravdepodobnosti
              // cesta k suboru
              //ci sa ma nacitat zo suboru
    len = snprintf(tmp, sizeof(tmp), "%d;", data->pocet_krokov_K);
    if (aktual_znakov + len >= max) {
        int novy_max = max + 50;
        char * tmp1 = realloc(buf, novy_max * sizeof(char));
        if (tmp1 == NULL) {
            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
            exit(EXIT_FAILURE);
        }
        buf = tmp1;
        max = novy_max;
    }
    memcpy(buf + aktual_znakov, tmp, len);
    aktual_znakov += len;
    // hore dole vpravo vlavo
    len = snprintf(tmp, sizeof(tmp), "%f;", data->pravdepodobnosti.hore);
    if (aktual_znakov + len >= max) {
        int novy_max = max + 50;
        char * tmp1 = realloc(buf, novy_max * sizeof(char));
        if (tmp1 == NULL) {
            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
            exit(EXIT_FAILURE);
        }
        buf = tmp1;
        max = novy_max;
    }
    memcpy(buf + aktual_znakov, tmp, len);
    aktual_znakov += len;

     len = snprintf(tmp, sizeof(tmp), "%f;", data->pravdepodobnosti.dole);
    if (aktual_znakov + len >= max) {
        int novy_max = max + 50;
        char * tmp1 = realloc(buf, novy_max * sizeof(char));
        if (tmp1 == NULL) {
            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
            exit(EXIT_FAILURE);
        }
        buf = tmp1;
        max = novy_max;
    }
    memcpy(buf + aktual_znakov, tmp, len);
    aktual_znakov += len;

    len = snprintf(tmp, sizeof(tmp), "%f;", data->pravdepodobnosti.vpravo);
    if (aktual_znakov + len >= max) {
        int novy_max = max + 50;
        char * tmp1 = realloc(buf, novy_max * sizeof(char));
        if (tmp1 == NULL) {
            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
            exit(EXIT_FAILURE);
        }
        buf = tmp1;
        max = novy_max;
    }
     memcpy(buf + aktual_znakov, tmp, len);
    aktual_znakov += len;

    len = snprintf(tmp, sizeof(tmp), "%f;", data->pravdepodobnosti.vlavo);
    if (aktual_znakov + len >= max) {
        int novy_max = max + 50;
        char * tmp1 = realloc(buf, novy_max * sizeof(char));
        if (tmp1 == NULL) {
            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
            exit(EXIT_FAILURE);
        }
        buf = tmp1;
        max = novy_max;
    }
    memcpy(buf + aktual_znakov, tmp, len);
    aktual_znakov += len;

    // cesta k suboru
              //ci sa ma nacitat zo suboru
    len = snprintf(tmp,sizeof(tmp), "%s;", data->cesta_k_suboru);
    if (aktual_znakov + len >= max) {
        int novy_max = max + 50;
        char * tmp1 = realloc(buf, novy_max * sizeof(char));
        if (tmp1 == NULL) {
            perror(RED "Chyba pamäte v inicializácii realokácia." RESET);
            exit(EXIT_FAILURE);
        }
        buf = tmp1;
        max = novy_max;
    }
     memcpy(buf + aktual_znakov, tmp, len);
    aktual_znakov += len;

    socket_write(&socket->activeSocket, buf, aktual_znakov);
    free(buf);
}



void spusti_initmenu_klient(socket_client_t * socket) {
    srv_p_t vstup;
    char cesta_k_suboru[200];
    int pocet_krokov_K;
    int pocet_replikacii;
    prvd_t pravdepodobnosti;
    int rozmer_x;
    int rozmer_y;
    _Bool svet_s_prekazkami;
    
    while (1) {
    
    
        char buf [200];
        memset(buf, 0, sizeof(buf));
        printf(GREEN "Zadaj jedno číslo - šírku poľa(rozmer x): " RESET);
        printf(UZIVATELFARBA);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf(RESET);
            perror(RED "Chyba načitavania textu." RESET);
            exit(EXIT_FAILURE);
        }
        printf(RESET);
        char * kontrola;
        rozmer_x = strtol(buf, &kontrola, 10);
        if (kontrola == buf) {
            printf(ORANGE "To nie je číslo, zadaj znova.\n" RESET);
        } else {
            break;
        }
    }

    while (1) {
    
    
        char buf [200];
        memset(buf, 0, sizeof(buf));
        printf(GREEN "Zadaj jedno číslo - výšku poľa(rozmer y): " RESET);
        printf(UZIVATELFARBA);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf(RESET);
            perror(RED "Chyba načitavania textu." RESET);
            exit(EXIT_FAILURE);
        }
        printf(RESET);
        char * kontrola;
        rozmer_y = strtol(buf, &kontrola, 10);
        if (kontrola == buf) {
            printf(ORANGE "To nie je číslo, zadaj znova.\n" RESET);
        } else {
            break;
        }
    }

    while (1) {
    
    
        char buf [200];
        memset(buf, 0, sizeof(buf));
        printf(GREEN "Zadaj počet replikácii simulácie: " RESET);
        printf(UZIVATELFARBA);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf(RESET);
            perror(RED "Chyba načitavania textu." RESET);
            exit(EXIT_FAILURE);
        }
        printf(RESET);
        char * kontrola;
        pocet_replikacii = strtol(buf, &kontrola, 10);
        
        
        if (kontrola == buf) {
            printf(ORANGE "To nie je číslo, zadaj znova.\n" RESET);
        } else {
            break;
        }
    }

    
    

    while (1) {
        printf(GREEN "\nZadaj pravdepodobnosti v tvare s desatinnou bodkou.\n");
        printf( "Ich súčet MUSÍ BYŤ 1.\n" RESET);
    
        while (1) {
        
            char buf [200];
            memset(buf, 0, sizeof(buf));
            printf(GREEN "Zadaj pravdepodobnosť pohybu -> smer hore: " RESET);
            printf(UZIVATELFARBA);
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                printf(RESET);
                perror(RED "Chyba načitavania textu." RESET);
                exit(EXIT_FAILURE);
            }
            printf(RESET);
            char * kontrola;
            pravdepodobnosti.hore = strtof(buf, &kontrola);
            if (kontrola == buf) {
                printf(ORANGE "To nie je číslo, zadaj znova.\n" RESET);
            } else {
                break;
            }
        }

        while (1) {
        
            char buf [200];
            memset(buf, 0, sizeof(buf));
            printf(GREEN "Zadaj pravdepodobnosť pohybu -> smer dole: " RESET);
            printf(UZIVATELFARBA);
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                printf(RESET);
                perror(RED "Chyba načítavania textu." RESET);
                exit(EXIT_FAILURE);
            }
            printf(RESET);
            char * kontrola;
            pravdepodobnosti.dole = strtof(buf, &kontrola);
            if (kontrola == buf) {
                printf(ORANGE "To nie je číslo, zadaj znova.\n" RESET);
            } else {
                break;
            }
        }

        while (1) {
        
            char buf [200];
            memset(buf, 0, sizeof(buf));
            printf(GREEN"Zadaj pravdepodobnosť pohybu -> smer vpravo: " RESET);
            printf(UZIVATELFARBA);
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                printf(RESET);
                perror(RED "Chyba načitavania textu." RESET);
                exit(EXIT_FAILURE);
            }
            printf(RESET);
            char * kontrola;
            pravdepodobnosti.vpravo = strtof(buf, &kontrola);
            if (kontrola == buf) {
                printf(ORANGE "To nie je číslo, zadaj znova.\n" RESET);
            } else {
                break;
            }
        }

        while (1) {
        
            char buf [200];
            memset(buf, 0, sizeof(buf));
            printf(GREEN "Zadaj pravdepodobnosť pohybu -> smer vlavo: " RESET);
            printf(UZIVATELFARBA);
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                printf(RESET);
                perror(RED "Chyba načitavania textu." RESET);
                exit(EXIT_FAILURE);
            }
            printf(RESET);
            char * kontrola;
            pravdepodobnosti.vlavo = strtof(buf, &kontrola);
            if (kontrola == buf) {
                printf(ORANGE "To nie je číslo, zadaj znova.\n"RESET);
            } else {
                break;
            }
        }

        float sucet = 0;
        sucet += pravdepodobnosti.dole;
        sucet += pravdepodobnosti.hore;
        sucet += pravdepodobnosti.vpravo;
        sucet += pravdepodobnosti.vlavo;
        if (sucet == 1.0) {
            break;
        } else {
            printf(ORANGE"Súčet sa nerová 1. Porozmýšľaj a skús znova.\n" RESET);
        }
    }

    while (1) {
    
    
        char buf [200];
        memset(buf, 0, sizeof(buf));
        printf(GREEN "Zadaj hodnotu K -> maximálny počet krokov chodca: " RESET);
        printf(UZIVATELFARBA);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf(RESET);
            perror(RED "Chyba načitavania textu." RESET);
            exit(EXIT_FAILURE);
        }
        printf(RESET);
        char * kontrola;
        pocet_krokov_K = strtol(buf, &kontrola, 10);
        if (kontrola == buf) {
            printf(ORANGE "To nie je číslo, zadaj znova.\n" RESET);
        } else {
            break;
        }
    }
    while (1) {
    
    
        char buf [200];
        memset(buf, 0, sizeof(buf));
        printf(GREEN "Ak súhlasíš, že chceš svet s prekážkami zadaj 1, inak zadaj 0: " RESET);
        printf(UZIVATELFARBA);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf(RESET);
            perror(RED "Chyba načitavania textu." RESET);
            exit(EXIT_FAILURE);
        }
        printf(RESET);
        char * kontrola;
        int tmp_int;
        tmp_int = strtol(buf, &kontrola, 10);
        if (kontrola == buf) {
            printf(ORANGE "To nie je číslo, zadaj znova.\n" RESET);
        } else {
            if (tmp_int == 1 || tmp_int == 0) {
                svet_s_prekazkami = (_Bool)tmp_int;
                break;
            } else {
                printf(ORANGE "Zle zadané číslo, skús znova." RESET);
            }
        }
    }

    while (1) {
        char buf [200];
        memset(buf, 0, sizeof(buf));
        printf(GREEN "Zadaj cestu k súboru na uloženie(ukončená musi byť '.txt'): \n" RESET);
        printf(UZIVATELFARBA);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf(RESET);
            perror(RED "Chyba načitavania textu." RESET);
            exit(EXIT_FAILURE);
        }
        printf(RESET);
        buf[strcspn(buf, "\n")] = '\0';
        memcpy(cesta_k_suboru, buf, strlen(buf));
        break;
    }


    vstup.svet_s_prekazkami = svet_s_prekazkami;
    vstup.rozmer_y = rozmer_y;
    vstup.rozmer_x = rozmer_x;
    vstup.pravdepodobnosti = pravdepodobnosti; 
    vstup.pocet_replikacii = pocet_replikacii;
    vstup.pocet_krokov_K = pocet_krokov_K;
    vstup.cesta_k_suboru = cesta_k_suboru;

    inicializuj_server(&vstup, socket);

}


int hlavne_menu_klient(){
    while (1) {

        printf("\033[2J");  //vymaze obrazovku
        printf("\033[H");   //posun kurzora do lava hore
        
        fflush(stdout);
        printf("\033[38;5;128m Vitaj v aplikácii menom Náhodna pochôdzka.\n K dispozícii máš tento manuál, v ktorom si môžeš vybrať priebeh pochôdzky.\n\n" RESET);
        printf("\033[38;5;172m MENU:\n 1. NOVÁ SIMULÁCIA \n 2. PRIPOJENIE K SIMULÁCII\n 3. OPATOVNÉ SPUSTENIE SIMULÁCIE\n 4. KONIEC\n" RESET);
        printf(" \033[1;32m Tvoja voľba je: \033[0m" );
        
        int moznost;
        char buf [200];
        memset(buf, 0, sizeof(buf));
        printf(UZIVATELFARBA);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf(RESET);
            perror(RED "Chyba načitavania textu." RESET);
            exit(EXIT_FAILURE);
        }
        printf(RESET);
        char * kontrola;    //ak nieco ostane v nej, znamena, ze zachytilo aspon nejake cislo
        moznost = strtol(buf, &kontrola, 10);
        if (kontrola == buf) {
            printf(ORANGE "To nie je číslo, zadaj znova.\n" RESET);
        } else if(moznost == 1) {
            return 1;
        } else if (moznost == 2) {
            return 2;
        }  else if (moznost == 3) {
            return 3;
        } else {
            return 4;
        }
    } //zatvorka pre while
}
