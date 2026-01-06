
#include "UI.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


char * vycisti_obrazovku() {

    char * buff;
   char zatial[10];
   int aktualPocetZnakov = 0;
   int maxPocetZnakov = 5;
   buff = calloc(maxPocetZnakov, sizeof(char));
   if (buff == NULL)
   {
    perror("Chyba pamate v inicializacii, vo vykresleni sveta.");
    exit(EXIT_FAILURE);
   }

   int len = snprintf(zatial, sizeof(zatial), "\033[H");

   if (aktualPocetZnakov + len >= maxPocetZnakov)
   {
    int novyMax = maxPocetZnakov + 10;
    char * zatial1 = realloc(buff, novyMax * sizeof(char));
    if (zatial1 == NULL)
    {
        perror("Chyba pamate v inicializacii  realokacia.");
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff, zatial, len);
   aktualPocetZnakov += len;
    //printf("\033[H");   //ako clear


   len = snprintf(zatial, sizeof(zatial), "\033[2J");

   if (aktualPocetZnakov + len >= maxPocetZnakov)
   {
    int novyMax = maxPocetZnakov + 10;
    char * zatial1 = realloc(buff, novyMax * sizeof(char));
    if (zatial1 == NULL)
    {
        perror("Chyba pamate v inicializacii  realokacia.");
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
    perror("Chyba pamate v inicializacii, vo vykresleni sveta.");
    exit(EXIT_FAILURE);
   }

   char * vycistenieObr = vycisti_obrazovku(); // ona mi posle char, co mam do zaciatku buffru prilepit  TODO
   int len = snprintf(zatial, sizeof(zatial), "%s", vycistenieObr);

   if (aktualPocetZnakov + len >= maxPocetZnakov)
   {
    int novyMax = maxPocetZnakov + 50;
    char * zatial1 = realloc(buff, novyMax * sizeof(char));
    if (zatial1 == NULL)
    {
        perror("Chyba pamate v inicializacii  realokacia.");
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff + aktualPocetZnakov, zatial, len);
   aktualPocetZnakov += len;
   free(vycistenieObr);


   
   len = snprintf(zatial, sizeof(zatial), "\033[32m%s: %d/%d\033[0m\n","Pocet replikacii", svet->original_replikacii, svet->pocet_replikacii );

   if (aktualPocetZnakov + len >= maxPocetZnakov)
   {
    int novyMax = maxPocetZnakov + 50;
    char * zatial1 = realloc(buff, novyMax * sizeof(char));
    if (zatial1 == NULL)
    {
        perror("Chyba pamate v inicializacii  realokacia.");
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff, zatial, len);
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
                    perror("Chyba pamate v inicializacii realokacia.");
                    exit(EXIT_FAILURE);
                }
                buff = zatial1;
                maxPocetZnakov = novyMax;
                
            }
            memcpy(buff, zatial, len);
            aktualPocetZnakov += len;
        }
        //putchar('\n');   
        len = snprintf(zatial, sizeof(zatial), "\n") + 1;
            if (aktualPocetZnakov + len >= maxPocetZnakov)
            {
                int novyMax = maxPocetZnakov + 5;
                char * zatial1 = realloc(buff, novyMax * sizeof(char));
                if (zatial1 == NULL)
                {
                    perror("Chyba pamate v inicializacii realokacia.");
                    exit(EXIT_FAILURE);
                }
                buff = zatial1;
                maxPocetZnakov = novyMax;
                
            }
            memcpy(buff, zatial, len);
            aktualPocetZnakov += len;
        }

          

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
    perror("Chyba pamate v inicializacii, vo vykresleni sveta.");
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
        perror("Chyba pamate v inicializacii  realokacia.");
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff + aktualPocetZnakov, zatial, len);
   aktualPocetZnakov += len;
   free(vycistenieObr);


   //printf("\033[1;32m--- STATISTIKA PRAVDEPODOBNOSTI ---\033[0m\n");  
    len = snprintf(zatial, sizeof(zatial), "\033[1;32m--- STATISTIKA PRAVDEPODOBNOSTI ---\033[0m\n");

   if (aktualPocetZnakov + len >= maxPocetZnakov)
   {
    int novyMax = maxPocetZnakov + 50;
    char * zatial1 = realloc(buff, novyMax * sizeof(char));
    if (zatial1 == NULL)
    {
        perror("Chyba pamate v inicializacii  realokacia.");
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff + aktualPocetZnakov, zatial, len);
   aktualPocetZnakov += len;


   
     for (int i = 0; i < svet->hranica_y; i++) {    
        for (int j = 0; j < svet->hranica_x; j++) {
            char buf[16];
            if (svet->pole[j][i] == 2) {

                //printf("\033[31m%7s\033[0m", "X");
                 len = snprintf(zatial, sizeof(zatial), "\033[31m%7s\033[0m", "X");
                if (aktualPocetZnakov + len >= maxPocetZnakov)
                    {
                        int novyMax = maxPocetZnakov + 50;
                        char * zatial1 = realloc(buff, novyMax * sizeof(char));
                        if (zatial1 == NULL)
                        {
                            perror("Chyba pamate v inicializacii  realokacia.");
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
                            perror("Chyba pamate v inicializacii  realokacia.");
                            exit(EXIT_FAILURE);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
                        }
                        buff = zatial1;
                        maxPocetZnakov = novyMax;
                    }
                    memcpy(buff + aktualPocetZnakov, zatial, len);
                    aktualPocetZnakov += len;
            
            }
            //printf(" ");
            len = snprintf(zatial, sizeof(zatial), " ");
                if (aktualPocetZnakov + len >= maxPocetZnakov)
                    {
                        int novyMax = maxPocetZnakov + 5;
                        char * zatial1 = realloc(buff, novyMax * sizeof(char));
                        if (zatial1 == NULL)
                        {
                            perror("Chyba pamate v inicializacii  realokacia.");
                            exit(EXIT_FAILURE);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
                        }
                        buff = zatial1;
                        maxPocetZnakov = novyMax;
                    }
                    memcpy(buff + aktualPocetZnakov, zatial, len);
                    aktualPocetZnakov += len;
        }

        //printf("\n");
        len = snprintf(zatial, sizeof(zatial), " \n");
                if (aktualPocetZnakov + len >= maxPocetZnakov)
                    {
                        int novyMax = maxPocetZnakov + 5;
                        char * zatial1 = realloc(buff, novyMax * sizeof(char));
                        if (zatial1 == NULL)
                        {
                            perror("Chyba pamate v inicializacii  realokacia.");
                            exit(EXIT_FAILURE);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
                        }
                        buff = zatial1;
                        maxPocetZnakov = novyMax;
                    }
                    memcpy(buff + aktualPocetZnakov, zatial, len);
                    aktualPocetZnakov += len;

    }

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
    perror("Chyba pamate v inicializacii, vo vykresleni sveta.");
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
        perror("Chyba pamate v inicializacii  realokacia.");
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff + aktualPocetZnakov, zatial, len);
   aktualPocetZnakov += len;

   free(vycistenieObr);


    //printf("\033[1;32m--- STATISTIKA KROKOV ---\033[0m\n");
    len = snprintf(zatial, sizeof(zatial), "\033[1;32m--- STATISTIKA KROKOV ---\033[0m\n");

   if (aktualPocetZnakov + len >= maxPocetZnakov)
   {
    int novyMax = maxPocetZnakov + 50;
    char * zatial1 = realloc(buff, novyMax * sizeof(char));
    if (zatial1 == NULL)
    {
        perror("Chyba pamate v inicializacii  realokacia.");
        exit(EXIT_FAILURE);
    }
    buff = zatial1;
    maxPocetZnakov = novyMax;
   }
   memcpy(buff + aktualPocetZnakov, zatial, len);
   aktualPocetZnakov += len;



    for (int i = 0; i < svet->hranica_y; i++) {    
            for (int j = 0; j < svet->hranica_x; j++) {
                char buf[16];
                if (svet->pole[j][i] == 2) {
                    //printf("\033[31m%12s\033[0m", "X");
                    len = snprintf(zatial, sizeof(zatial), "\033[31m%12s\033[0m", "X");

                    if (aktualPocetZnakov + len >= maxPocetZnakov)
                    {
                        int novyMax = maxPocetZnakov + 50;
                        char * zatial1 = realloc(buff, novyMax * sizeof(char));
                        if (zatial1 == NULL)
                        {
                            perror("Chyba pamate v inicializacii  realokacia.");
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
                            perror("Chyba pamate v inicializacii  realokacia.");
                            exit(EXIT_FAILURE);
                        }
                        buff = zatial1;
                        maxPocetZnakov = novyMax;
                    }
                    memcpy(buff + aktualPocetZnakov, zatial, len);
                    aktualPocetZnakov += len;
                
                }
                //printf(" ");   TO
                len = snprintf(zatial, sizeof(zatial), " ");

                    if (aktualPocetZnakov + len >= maxPocetZnakov)
                    {
                        int novyMax = maxPocetZnakov + 50;
                        char * zatial1 = realloc(buff, novyMax * sizeof(char));
                        if (zatial1 == NULL)
                        {
                            perror("Chyba pamate v inicializacii  realokacia.");
                            exit(EXIT_FAILURE);
                        }
                        buff = zatial1;
                        maxPocetZnakov = novyMax;
                    }
                    memcpy(buff + aktualPocetZnakov, zatial, len);
                    aktualPocetZnakov += len;
            }
            //printf("\n");    
            len = snprintf(zatial, sizeof(zatial), "\n");

                    if (aktualPocetZnakov + len >= maxPocetZnakov)
                    {
                        int novyMax = maxPocetZnakov + 50;
                        char * zatial1 = realloc(buff, novyMax * sizeof(char));
                        if (zatial1 == NULL)
                        {
                            perror("Chyba pamate v inicializacii  realokacia.");
                            exit(EXIT_FAILURE);
                        }
                        buff = zatial1;
                        maxPocetZnakov = novyMax;
                    }
                    memcpy(buff + aktualPocetZnakov, zatial, len);
                    aktualPocetZnakov += len;
        }
        return buff;
}




void server_vykonavaj_sim(svt_brd_t * data) {
    
    
    for (int i = 0; i < data->svet->original_replikacii; i++) {
        for (int j; j < data->svet->pocet_krokov_K; j++) {
            
            
           
            pthread_mutex_lock(&data->server->mutex);
            int pocet_vlakien = data->server->pocetKlinetov;
            if (!data->server->server_bezi || !data->server->server_info.zobraz_pole) {
                pthread_mutex_unlock(&data->server->mutex);
                pthread_exit(NULL);
            }
            pthread_mutex_unlock(&data->server->mutex);
          
          
            

            for (int k = 0; k < pocet_vlakien; k++) {
                pthread_t vlakno;
                svt_vp_t * vypisovac;
                vypisovac = calloc(1, sizeof(svt_vp_t));
                if (vypisovac == NULL) {
                perror("Chyba vytvarania vlakien zla pamat.");
                exit(EXIT_FAILURE);
                }
                pthread_create(&vlakno, NULL, posli_vsetkym_svet, &vypisovac);
                pthread_detach(vlakno);
            }
            posun_chodca(daj_nahodny_smer_pre_chodca(data->svet), data->svet);
            data->svet->pocet_replikacii--;
            sleep(1);
        }
    }
}

void * posli_vsetkym_svet(void * arg) {
    svt_vp_t * data = arg;
    pthread_mutex_lock(&data->server->mutex);
    int pocet_klientov = data->server->pocetKlinetov;
    pthread_mutex_unlock(&data->server->mutex);
    socket_data_t * posielaj;
    posielaj = calloc(pocet_klientov, sizeof(socket_data_t));
    if (posielaj == NULL) {
        perror("Chyba alokovania pamate pre vypis.");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&data->server->mutex);
    memcpy(posielaj, data->server->activeSocket, sizeof(socket_data_t) * pocet_klientov);
    pthread_mutex_unlock(&data->server->mutex);
    for (int i = 0; i < pocet_klientov; i++) {
        //treba pockat na lydku
        char * buf = vykresli_svet(data->svet);
        socket_write(&posielaj[i], buf,strlen(buf));
        free(buf);
    };
    free(posielaj);
    free(data);



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
        perror("Chyba pamate v inicializacii.");
        exit(EXIT_FAILURE);
    }
    int len = snprintf(tmp, sizeof(tmp), "%d;",data->rozmer_x);
    if (aktual_znakov + len >= max) {
        int novy_max = max + 50;
        char * tmp1 = realloc(buf, novy_max * sizeof(char));
        if (tmp1 == NULL) {
            perror("Chyba pamate v inicializacii realokacia.");
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
            perror("Chyba pamate v inicializacii realokacia.");
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
            perror("Chyba pamate v inicializacii realokacia.");
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
            perror("Chyba pamate v inicializacii realokacia.");
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
            perror("Chyba pamate v inicializacii realokacia.");
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
            perror("Chyba pamate v inicializacii realokacia.");
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
            perror("Chyba pamate v inicializacii realokacia.");
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
            perror("Chyba pamate v inicializacii realokacia.");
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
            perror("Chyba pamate v inicializacii realokacia.");
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
            perror("Chyba pamate v inicializacii realokacia.");
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
        printf("Zadaj sirku pola(rozmer_x): \n");
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            perror("Chyba nacitavania textu.");
            exit(EXIT_FAILURE);
        }
        char * kontrola;
        rozmer_x = strtol(buf, &kontrola, 10);
        if (kontrola == buf) {
            printf("To nie je cislo zadaj znova!!\n");
        } else {
            break;
        }
    }

    while (1) {
    
    
        char buf [200];
        memset(buf, 0, sizeof(buf));
        printf("Zadaj vysku pola(rozmer_y): \n");
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            perror("Chyba nacitavania textu.");
            exit(EXIT_FAILURE);
        }
        char * kontrola;
        rozmer_y = strtol(buf, &kontrola, 10);
        if (kontrola == buf) {
            printf("To nie je cislo zadaj znova!!\n");
        } else {
            break;
        }
    }

    while (1) {
    
    
        char buf [200];
        memset(buf, 0, sizeof(buf));
        printf("Zadaj pocet replikacii simulacie: \n");
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            perror("Chyba nacitavania textu.");
            exit(EXIT_FAILURE);
        }
        char * kontrola;
        pocet_replikacii = strtol(buf, &kontrola, 10);
        if (kontrola == buf) {
            printf("To nie je cislo zadaj znova!!\n");
        } else {
            break;
        }
    }

    while (1) {
        printf("Zadaj pravdepodobnosti v tvare s desatinou bodkou.\n");
        printf("Ich súčet musí byť 1.\n");
    
        while (1) {
        
            char buf [200];
            memset(buf, 0, sizeof(buf));
            printf("Zadaj pravdepodobnost pohybu smer hore: \n");
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                perror("Chyba nacitavania textu.");
                exit(EXIT_FAILURE);
            }
            char * kontrola;
            pravdepodobnosti.hore = strtof(buf, &kontrola);
            if (kontrola == buf) {
                printf("To nie je cislo zadaj znova!!\n");
            } else {
                break;
            }
        }

        while (1) {
        
            char buf [200];
            memset(buf, 0, sizeof(buf));
            printf("Zadaj pravdepodobnost pohybu smer dole: \n");
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                perror("Chyba nacitavania textu.");
                exit(EXIT_FAILURE);
            }
            char * kontrola;
            pravdepodobnosti.dole = strtof(buf, &kontrola);
            if (kontrola == buf) {
                printf("To nie je cislo zadaj znova!!\n");
            } else {
                break;
            }
        }

        while (1) {
        
            char buf [200];
            memset(buf, 0, sizeof(buf));
            printf("Zadaj pravdepodobnost pohybu smer vpravo: \n");
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                perror("Chyba nacitavania textu.");
                exit(EXIT_FAILURE);
            }
            char * kontrola;
            pravdepodobnosti.vpravo = strtof(buf, &kontrola);
            if (kontrola == buf) {
                printf("To nie je cislo zadaj znova!!\n");
            } else {
                break;
            }
        }

        while (1) {
        
            char buf [200];
            memset(buf, 0, sizeof(buf));
            printf("Zadaj pravdepodobnost pohybu smer vlavo: \n");
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                perror("Chyba nacitavania textu.");
                exit(EXIT_FAILURE);
            }
            char * kontrola;
            pravdepodobnosti.vlavo = strtof(buf, &kontrola);
            if (kontrola == buf) {
                printf("To nie je cislo zadaj znova!!\n");
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
            printf("Sucet nie je 1 !!!!\n");
        }
    }

    while (1) {
    
    
        char buf [200];
        memset(buf, 0, sizeof(buf));
        printf("Zadaj hodnotu K maximalny pocet krokov chodca: \n");
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            perror("Chyba nacitavania textu.");
            exit(EXIT_FAILURE);
        }
        char * kontrola;
        pocet_krokov_K = strtol(buf, &kontrola, 10);
        if (kontrola == buf) {
            printf("To nie je cislo zadaj znova!!\n");
        } else {
            break;
        }
    }
    while (1) {
    
    
        char buf [200];
        memset(buf, 0, sizeof(buf));
        printf("Zadaj ci sa ma byt svet s prekazkami(1) alebo bez(0) (1/0): \n");
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            perror("Chyba nacitavania textu.");
            exit(EXIT_FAILURE);
        }
        char * kontrola;
        int tmp_int;
        tmp_int = strtol(buf, &kontrola, 10);
        if (kontrola == buf) {
            printf("To nie je cislo zadaj znova!!\n");
        } else {
            if (tmp_int == 1 || tmp_int == 0) {
                svet_s_prekazkami = (_Bool)tmp_int;
                break;
            } else {
                printf("Zle zadane cislo skus znova.");
            }

            
        }
    }

    while (1) {
    
    
        char buf [200];
        memset(buf, 0, sizeof(buf));
        printf("Zadaj cestu k suboru na ulozenie: \n");
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            perror("Chyba nacitavania textu.");
            exit(EXIT_FAILURE);
        }
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

void hlavne_menu_klient(){
    while (1) {
    
        printf("Vitaj v aplikacii menom Nahodna pochodzka.\n K dispozicii mas tento manual, v ktorom si mozes vybrat priebeh pochodzky.\n\n");
        printf("MENU:\n 1. NOVA SIMULACIA \n 2. PRIPOJENIE K SIMULACII\n 3. OPATOVNE SPUSTENIE SIMULACIE\n 4. KONIEC\n");
        printf("Tvoja volba je: ");
        int moznost;
        char buf [200];
        memset(buf, 0, sizeof(buf));
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            perror("Chyba nacitavania textu.");
            exit(EXIT_FAILURE);
        }
        char * kontrola;    //ak nieco ostane v nej, znamena, ze zachytilo aspon nejake cislo
        moznost = strtol(buf, &kontrola, 10);
        if (kontrola == buf) {
            printf("To nie je cislo zadaj znova!!\n");
        } else {
            switch (moznost) {
            case 1:     ////// NOVA SIMULACIA
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

                //////////////// KTORY SUBOR
                printf("Zo suboru -> stlac 1; od nuly -> stlac 0. \n");
                if (fgets(buf, sizeof(buf), stdin) == NULL) {
                    perror("Chyba nacitavania textu.");
                    exit(EXIT_FAILURE);
                }

                int ciChceZoSuboru;
                ciChceZoSuboru = strtol(buf, &kontrola, 10);
                char * nazovSuboru;
                if (kontrola == buf) {
                    printf("To nie je cislo zadaj znova!!\n");
                } else {
                    if (ciChceZoSuboru == 0) {
                        // TO DO : idem pustit z tomiho srandy
                    } else if (ciChceZoSuboru == 1) {
                        printf("Napis cestu k suboru, odkial sa nacita svet. \n");
                        nazovSuboru = fgets(buf, sizeof(buf), stdin);
                        if (nazovSuboru == NULL) {
                            perror("Chyba nacitavania textu.");
                            exit(EXIT_FAILURE);
                        } else{
                            //nic sa nedeje, idem ho pouzit
                            printf("NAcitavam zo suboru: %s.", nazovSuboru);
                        }   
                    }
                }


                //////////////  KOLKO POUZIVATELOV
                int pocetPouzivatelov;
                _Bool dobreZadal = 1;
                while (dobreZadal) {

                printf("Napis pre kolko pouzivatelov ma byt urcena aplikacia. \n");
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
                printf("Napis adresu pripojenia. \n");
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
                printf("Napis cestu k suboru. \n");
                char * nameFile  = fgets(buf, sizeof(buf), stdin);
                if (nameFile == NULL) {
                    perror("Chyba nacitavania textu.");
                    exit(EXIT_FAILURE);
                } else {
                    printf("NAzov suboru je: %s.",nameFile);
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
            }

        }

        printf("Spsustame simulaciu.\n");

    } //zatvorka pre while


}
