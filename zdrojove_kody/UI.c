
#include "UI.h"
#include <stdio.h>


void vycisti_obrazovku() {
    printf("\033[H");
    printf("\033[2J");
    fflush(stdout);
}

void vykresli_svet(svt_t * svet) {

   vycisti_obrazovku();

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


void svet_vypis_statistiku(svt_t * svet) {
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
