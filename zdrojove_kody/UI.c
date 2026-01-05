
#include "UI.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


char * vycisti_obrazovku() {
    printf("\033[H");
    printf("\033[2J");
    fflush(stdout);
}

char * vykresli_svet(svt_t * svet) {


    char * buf;
   vycisti_obrazovku();

   
   printf("\033[32m%s: %d/%d\033[0m\n","Pocet replikacii", svet->original_replikacii, svet->pocet_replikacii);
    for (int i = 0; i < svet->hranica_y; i++)   //vonkajsi for, menej sa opakuje, je to y
    {
        for (int j = 0; j < svet->hranica_x; j++) {    //suradnica x
            //prekazka, chodec, nic, stred
            // 0 reprezentuje prazdne policko, 1 je chodec, 2 je prekazka

            char c;
            char *farba;

            if(svet->stred_x == j && svet->stred_y == i)
            {
                c = '*';
                farba = "\033[32m"; // zelená
            } else if (svet->pole[j][i] == 0)
            {
                c = '-';
                farba = "\033[37m"; // biela 
            }else if (svet->pole[j][i] == 1)
            {
                c = 'C';
                farba = "\033[33m";
            } else{
                //rovna sa 2 == prekazka
                c = 'X';
                farba = "\033[31m"; // červená prekážka

            }  
            printf("%s%c \033[0m", farba, c);
            
        }
        putchar('\n');
        
    }   
    
}


char * svet_vypis_statistiku(svt_t * svet) {
    vycisti_obrazovku();
    
    printf("\033[1;32m--- STATISTIKA PRAVDEPODOBNOSTI ---\033[0m\n");
     for (int i = 0; i < svet->hranica_y; i++) {    
        for (int j = 0; j < svet->hranica_x; j++) {
            char buf[16];
            if (svet->pole[j][i] == 2) {
                printf("\033[31m%7s\033[0m", "X");
            } else {
                printf("%7.2f", svet->pole_pravdepodobnosti[j][i]);
            
            }
            printf(" ");
        }

        printf("\n");

    }

    printf("\n\n");
    printf("\033[1;32m--- STATISTIKA KROKOV ---\033[0m\n");
    for (int i = 0; i < svet->hranica_y; i++) {    
            for (int j = 0; j < svet->hranica_x; j++) {
                char buf[16];
                if (svet->pole[j][i] == 2) {
                    printf("\033[31m%12s\033[0m", "X");
                } else {
                    printf("%12.2f", svet->pole_priemer_krok[j][i]);
                
                }
                printf(" ");
            }
            printf("\n");
        }
}

char * svet_vypis_kroky(svt_t * svet) {

}





void server_vykonavaj_sim(svt_brd_t * data) {
    
    
    for (int i = 0; i < data->svet->original_replikacii; i++) {
        for (int j; j < data->svet->pocet_krokov_K; j++) {
            
            
           
            pthread_mutex_lock(&data->server->mutex);
            int pocet_vlakien = data->server->pocetKlinetov;
            if (!data->server->server_bezi) {
                pthread_mutex_unlock(&data->server->mutex);
                pthread_exit(NULL);
            }
            pthread_mutex_unlock(&data->server->mutex);
          
          
            

            for (int k = 0; k < pocet_vlakien; k++) {
                pthread_t vlakno;
                svt_vp_t * vypisovac;
                vypisovac = calloc(1, sizeof(svt_vp_t));
                if (vypisovac == NULL) {
                perror("Chyba vytvarania vlakien zla pamat");
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
        perror("Chyba alokovania pamate pre vypis");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&data->server->mutex);
    memcpy(posielaj, data->server->activeSocket, sizeof(socket_data_t) * pocet_klientov);
    pthread_mutex_unlock(&data->server->mutex);
    for (int i = 0; i < pocet_klientov; i++) {
        //treba pockat na lydku
        //char *
        socket_write(posielaj[i]);
    };
    free(posielaj);
    free(data);



}


void spusti_menu_klient() {




}