#include "svet.h"


#include <stdio.h>
#include <time.h>
#include <stdlib.h>

svt_t * svet_init_normal(int hranica_x, int hranica_y, prvd_t pravdepodobnosti)
{
    svt_t * svet;
    
    svet = malloc(sizeof(svt_t));
    svet->pravdepodobnosti = pravdepodobnosti;
    svet->chodec = chodec_init(hranica_x, hranica_y);
    // 0 reprezentuje prazdne policko, 1 je chodec, 2 je prekazka
    svet->pole = calloc((hranica_x), sizeof(int*));
    for (int i = 0; i < hranica_x; i++)
    {
        svet->pole[i] = calloc(hranica_y, sizeof(int));
    }

    svet->pole_pravdepodobnosti = calloc((hranica_x), sizeof(int*));
    for (int i = 0; i < hranica_x; i++)
    {
        svet->pole_pravdepodobnosti[i] = calloc(hranica_y, sizeof(int));
    }
    
    svet->hranica_x = hranica_x;
    svet->hranica_y = hranica_y;
    svet->pole[hranica_x - 1][hranica_y - 1] = 1;
    svet->stred_x = hranica_x/2;
    svet->stred_y = hranica_y/2;
    return svet;
}

void svet_destroy(svt_t * svet)
{
    for (int i = 0; i < svet->hranica_x; i++)
    {
        free(svet->pole[i]);
        free(svet->pole_pravdepodobnosti[i]);
    }

    
    
    free(svet->pole);
    free(svet->pole_pravdepodobnosti);
    chodec_destroy(svet->chodec);
    free(svet);
    return;
}

void posun_chodca(smer_t smer_posunu, svt_t * svet)
{
    //[00] je v strede, treba zmenit inicializaciu, osetrit konce
    int aktualna_poloha_x = svet->chodec->x;
    int aktualna_poloha_y = svet->chodec->y;
    
    int nova_poloha_x = aktualna_poloha_x;
    int nova_poloha_y = aktualna_poloha_y;
    switch (smer_posunu)
    {
        case Vlavo:
            if (aktualna_poloha_x == 0)
            {
                nova_poloha_x = svet->hranica_x - 1;
            } else{
                nova_poloha_x = aktualna_poloha_x - 1;
            }
            
            break;
        case Vpravo:
            if (aktualna_poloha_x == svet->hranica_x - 1)
            {
                nova_poloha_x = 0;
            } else {
                nova_poloha_x = aktualna_poloha_x + 1;
            }
            
            break;
        case Hore:
            if (aktualna_poloha_y == 0)
            {
                nova_poloha_y = svet->hranica_y - 1;
            } else {
                nova_poloha_y = aktualna_poloha_y -1;
            }
            
            break;
        default:
            //dole
            if (aktualna_poloha_y == svet->hranica_y - 1)
            {
                nova_poloha_y = 0;
            } else {
                nova_poloha_y = aktualna_poloha_y + 1;
            }
            
            break;
    }

    svet->pole[aktualna_poloha_x][aktualna_poloha_y] = 0;
    svet->pole[nova_poloha_x][nova_poloha_y] = 1;
    svet->chodec->x = nova_poloha_x;
    svet->chodec->y = nova_poloha_y;
    return;

}

void svet_vypis(svt_t *svet)
{
    for (int i = 0; i < svet->hranica_y; i++)   //vonkajsi for, menej sa opakuje, je to y
    {
        for (int j = 0; j < svet->hranica_x; j++) {    //suradnica x
            //prekazka, chodec, nic, stred
            // 0 reprezentuje prazdne policko, 1 je chodec, 2 je prekazka
            if(svet->stred_x == j && svet->stred_y == i)
            {
                printf("%s ", "*");
            } else if (svet->pole[j][i] == 0)
            {
                printf("%s ", "-");
            }else if (svet->pole[j][i] == 1)
            {
                printf("%s ", "C");
            } else{
                //rovna sa 2 == prekazka
                printf("%s ", "X");
            }  
            
        }
        printf("\n");
        
    }
    

}

svt_t * svet_init_prekazky(int hranica_x, int hranica_y,  int sanca_na_prekazku, prvd_t pravdepodobnosti) {
    svt_t * svet;
    svet = malloc(sizeof(svt_t));
    srand(time(NULL));
    svet->pravdepodobnosti = pravdepodobnosti;
    svet->stred_x = hranica_x/2;
    svet->stred_y = hranica_y/2;
    svet->chodec = chodec_init(hranica_x, hranica_y);
    // 0 reprezentuje prazdne policko, 1 je chodec, 2 je prekazka
    svet->pole = calloc((hranica_x), sizeof(int*));
    for (int i = 0; i < hranica_x; i++)
    {
        svet->pole[i] = calloc(hranica_y, sizeof(int));
    }
    
    svet->pole_pravdepodobnosti = calloc((hranica_x), sizeof(int*));
    for (int i = 0; i < hranica_x; i++)
    {
        svet->pole_pravdepodobnosti[i] = calloc(hranica_y, sizeof(int));
    }

    svet->hranica_x = hranica_x;
    svet->hranica_y = hranica_y;
    svet->pole[hranica_x - 1][hranica_y - 1] = 1;
    do {



        for (int i = 0; i < hranica_x; i++) {
                for (int j = 0; j < hranica_y; j++) {
                    if (!(i == hranica_x - 1 && j == hranica_y - 1) &&  !(i == svet->stred_x && j == svet->stred_y)) {
                
                        
                        int generator = (rand() % 100) + 1;
                        if (generator <= sanca_na_prekazku) {
                            svet->pole[i][j] = 2;
                        } else {
                            svet->pole[i][j] = 0;
                        }
                    }
                }
            }
    } while (!je_svet_ok(svet));
        
    return svet;
}

_Bool je_svet_ok(svt_t * svet) {
    _Bool navstivene_policka[svet->hranica_x][svet->hranica_y];
    for (int i = 0; i < svet->hranica_x; i++) {
        for (int j = 0; j < svet->hranica_y; j++) {
            navstivene_policka[i][j] = 0;
        }
    }
    je_policko_ok(svet, svet->stred_x ,svet->stred_y, navstivene_policka);

    for (int i = 0; i < svet->hranica_x; i++) {
        for (int j = 0; j < svet->hranica_y; j++) {
            if (!(i == svet->hranica_x - 1 && j == svet->hranica_y - 1) &&  !(i == svet->stred_x && j == svet->stred_y)) {
 
                if (!navstivene_policka[i][j] && svet->pole[i][j] != 2) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

void je_policko_ok(svt_t * svet, int x, int y,  _Bool navstivene[svet->hranica_x][svet->hranica_y]) {
    if (x < 0 || x > svet->hranica_x - 1) {
        return;
    }
    if (y < 0 || y > svet->hranica_y -1) {
        return;
    }
    if (svet->pole[x][y] == 2) {
        return;
    }
    if (navstivene[x][y] == 1) {
        return;
    }

    navstivene[x][y] = 1;
    int nove_x = x - 1;
    int nove_y = y - 1;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            je_policko_ok(svet, nove_x, nove_y, navstivene);
            nove_y++;
        }
        nove_y = y - 1;
        nove_x++;
    }
    return;
}

void posun_chodca_na(svt_t * svet, int sur_x, int sur_y) {
    for (int i = 0; i < svet->hranica_x; i++) {
        for (int j = 0; j < svet->hranica_y; j++) {
           if (svet->pole[i][j] == 1) {
                svet->pole[i][j] = 0;
           } 
        }
    }
    svet->pole[sur_x][sur_y] = 1;
    svet->chodec->x = sur_x;
    svet->chodec->y = sur_y;
}

smer_t daj_nahodny_smer_pre_chodca(svt_t * svet) {
    srand(time(NULL));
    int aktual_x = svet->chodec->x;
    int aktual_y =svet->chodec->y;
    

    //vpravo
    int sur_x_vpravo;
    int sur_y_vpravo = aktual_y;
    if (aktual_x + 1 == svet->hranica_x) {
        sur_x_vpravo = 0;
    } else {
        sur_x_vpravo = aktual_x + 1;
    }
    //vlavo
    int sur_x_vlavo;
    int sur_y_vlavo = aktual_y;
    if (aktual_x - 1 < 0) {
        sur_x_vlavo = svet->hranica_x - 1;
    } else {
        sur_x_vlavo = aktual_x - 1;
    }
    //hore
    int sur_x_hore = aktual_x;
    int sur_y_hore;
    if (aktual_y - 1 < 0) {
        sur_y_hore = svet->hranica_y - 1;
    } else {
        sur_y_hore = aktual_y - 1;
    }

    //dole
    int sur_x_dole = aktual_x;
    int sur_y_dole;
    if (aktual_y + 1 == svet->hranica_y) {
        sur_y_dole = 0;
    } else {
        sur_y_dole = aktual_y + 1;
    }

    _Bool dole = svet->pole[sur_x_dole][sur_y_dole] == 0;
    _Bool vpravo= svet->pole[sur_x_vpravo][sur_y_vpravo] == 0;;
    _Bool vlavo= svet->pole[sur_x_vlavo][sur_y_vlavo] == 0;;
    _Bool hore= svet->pole[sur_x_hore][sur_y_hore] == 0;;
    
    float hore_ak_viac = svet->pravdepodobnosti.hore * 100;
    float dole_ak_viac = svet->pravdepodobnosti.dole * 100 + hore_ak_viac;
    float vpravo_ak_viac = dole_ak_viac + svet->pravdepodobnosti.vpravo * 100;

    
    while (1) 
    
     {
     int generovane = (rand() % 100) + 1;
            
            if (generovane <= hore_ak_viac && hore) {
                //ide hore
                return Hore;
            } else if (generovane > hore_ak_viac && generovane < dole_ak_viac && dole) {
                //ide dole
                return Dole;
            } else if (generovane >= dole_ak_viac && generovane < vpravo_ak_viac && vpravo) {
                //ide vpravo
                return Vpravo;
            } else if (vlavo) {
                // ide vlavo
                return Vlavo;
            }
        } 
    

}


void svet_vypis_statistiku(svt_t * svet) {
     for (int i = 0; i < svet->hranica_y; i++) {    
        for (int j = 0; j < svet->hranica_x; j++) {
            printf("%3.2f ", svet->pole_pravdepodobnosti[j][i]);
        }
        printf("\n");
    }
}