#include "svet.h"


#include <time.h>
#include <stdlib.h>
#include <stdio.h>

svt_t * svet_init_normal(int hranica_x, int hranica_y)
{
    svt_t * svet;
    svet = malloc(sizeof(svt_t));
    svet->chodec = chodec_init(hranica_x, hranica_y);
    // 0 reprezentuje prazdne policko, 1 je chodec, 2 je prekazka
    svet->pole = calloc((hranica_x), sizeof(int*));
    for (int i = 0; i < hranica_x; i++)
    {
        svet->pole[i] = calloc(hranica_y, sizeof(int));
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
    }
    
    free(svet->pole);
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
    return;

}

void svet_uloz_do_suboru(char *cesta_k_suboru, svt_t * svet)
{
    FILE * subor = fopen(cesta_k_suboru, "w");

    if(!subor){
        perror("Chyba:  subor sa zrejme neotvoril. \n ");
        fclose(subor);
        return;
    }

    fprintf(subor, "%d;%d;\n",svet->hranica_x, svet->hranica_y);


    for (int i = 0; i < svet->hranica_y; i++)   //najprv y = 0, a x budem zvysovat == idem po riadku
    {
        for (int j = 0; j < svet->hranica_x; j++)
        {
            fprintf(subor, "%d;",svet->pole[j][i]);
            
        }
        fprintf(subor, "\n");
    }
    
    fclose(subor);
}

svt_t * svet_nacitaj_zo_suboru(char *cesta_k_suboru)
{
    FILE* subor = fopen(cesta_k_suboru, "r");
    // najpprv sirku, potom vysku, potom vsetko po riadkoch
    /*char ciselkoSirka1 = fgetc(subor);
    int ciselkoSirka = ciselkoSirka1 - '0';
    fgetc(subor);
    char ciselkoVyska1 = fgetc(subor);
    int ciselkoVyska = ciselkoVyska1 - '0';
    fgetc(subor);
    int uloziskoMapy[ciselkoSirka][ciselkoVyska];*/

    
        
        int ciselkoSirka;
        int ciselkoVyska;
        fscanf(subor, "%d;%d;", ciselkoSirka, ciselkoVyska);


        svt_t * svetNacitany = svet_init_normal(ciselkoSirka, ciselkoVyska);


        for (int t = 0; t < ciselkoVyska; t++)
        {
            for (int m = 0; m < ciselkoSirka; m++)
            {
                //budem nacitavat
                //fread a strchr, u mna ;
            }
            
        }


        
        fclose(subor);
        return svetNacitany;
        



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

svt_t * svet_init_prekazky(int hranica_x, int hranica_y,  int sanca_na_prekazku) {
    svt_t * svet;
    svet = malloc(sizeof(svt_t));
    srand(time(NULL));

    svet->stred_x = hranica_x/2;
    svet->stred_y = hranica_y/2;
    svet->chodec = chodec_init(hranica_x, hranica_y);
    // 0 reprezentuje prazdne policko, 1 je chodec, 2 je prekazka
    svet->pole = calloc((hranica_x), sizeof(int*));
    for (int i = 0; i < hranica_x; i++)
    {
        svet->pole[i] = calloc(hranica_y, sizeof(int));
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
