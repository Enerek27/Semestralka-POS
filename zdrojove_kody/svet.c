#include "svet.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

svt_t * svet_init_normal(int hranica_x, int hranica_y, prvd_t pravdepodobnosti, int pocet_krokov_K, int pocet_replikacii, char * cesta_k_suboru)
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

    svet->pole_pravdepodobnosti = calloc((hranica_x), sizeof(float*));
    for (int i = 0; i < hranica_x; i++)
    {
        svet->pole_pravdepodobnosti[i] = calloc(hranica_y, sizeof(float));
    }
    
    svet->pole_priemer_krok = calloc((hranica_x), sizeof(float*));
    for (int i = 0; i < hranica_x; i++)
    {
        svet->pole_priemer_krok[i] = calloc(hranica_y, sizeof(float));
    }

    svet->hranica_x = hranica_x;
    svet->hranica_y = hranica_y;
    svet->pole[hranica_x - 1][hranica_y - 1] = 1;
    svet->stred_x = hranica_x/2;
    svet->stred_y = hranica_y/2;
    strcpy(svet->cesta_k_suboru, cesta_k_suboru);
    svet->pocet_krokov_K = pocet_krokov_K;
    svet->pocet_replikacii = pocet_replikacii;
    svet->original_replikacii = pocet_replikacii;
    return svet;
}

void svet_destroy(svt_t * svet)
{
    for (int i = 0; i < svet->hranica_x; i++)
    {
        free(svet->pole[i]);
        free(svet->pole_pravdepodobnosti[i]);
        free(svet->pole_priemer_krok[i]);
    }

    
    free(svet->pole_priemer_krok);
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

void svet_uloz_do_suboru(svt_t * svet)
//okrem chodca vsetko
{
    FILE * subor = fopen(svet->cesta_k_suboru, "w");

    if(!subor){
        perror("Chyba:  subor sa zrejme neotvoril. \n ");
        fclose(subor);
        return;
    }

   
    //fprintf nepridava ziadne dalsie znaky, zapise presne to, co je uvedene vo formate

    fprintf(subor, "%d;%d;%d;%d;", svet->hranica_x, svet->hranica_y, svet->stred_x, svet->stred_y);
    fprintf(subor, "%d;%d;", svet->pocet_krokov_K, svet->original_replikacii);

    //ukladam pole_pravdepodobnosti
    for (int i = 0; i < svet->hranica_y; i++)
    {
        for (int j = 0; j < svet->hranica_x; j++)
        {
            fprintf(subor, "%f;", svet->pole_pravdepodobnosti[j][i]);
        }
    }

    //ukladam pole_priemer_krok
    for (int i = 0; i < svet->hranica_y; i++)
    {
        for (int j = 0; j < svet->hranica_x; j++)
        {
            fprintf(subor, "%f;", svet->pole_priemer_krok[j][i]);
        }
    }

        //ukladam strukturu pravdepodobnosti   
        fprintf(subor, "%f;", svet->pravdepodobnosti.hore);
        fprintf(subor, "%f;", svet->pravdepodobnosti.dole);
        fprintf(subor, "%f;", svet->pravdepodobnosti.vpravo);
        fprintf(subor, "%f;", svet->pravdepodobnosti.vlavo);


    //ukladam pole
    for (int i = 0; i < svet->hranica_y; i++)   //najprv y = 0, a x budem zvysovat == idem po riadku
    {
        for (int j = 0; j < svet->hranica_x; j++)
        {
            fprintf(subor, "%d;",svet->pole[j][i]); 
        }
        //fprintf(subor, "\n");  kvoli Tomaskovi
    }

    fclose(subor);
}

svt_t * svet_nacitaj_zo_suboru(char *cesta_k_suboru)
{
    FILE* subor = fopen(cesta_k_suboru, "r");
 
    //hranica x, hranica y, stred x, stred y, pocet krokov K, svet origninal replikacii
    //pole pravdepodobnosti, priemer krok, struktura preavdepodobnosti, pole
        int ciselkoHranicaX;
        int ciselkoHranicaY;
        int stredX;
        int stredY;

        int pocetKrokov;
        int svetOriginalReplikacii;


        fscanf(subor, "%d;%d;%d;%d;", &ciselkoHranicaX, &ciselkoHranicaY, &stredX, &stredY);
        fscanf(subor, "%d;%d;", &pocetKrokov, &svetOriginalReplikacii);

        //nacitam pole_pravepodobnosti  -float
        float ** pole_pravdepodobnosti;
        pole_pravdepodobnosti = calloc((ciselkoHranicaX), sizeof(float*));
        for (int i = 0; i < ciselkoHranicaX; i++)
        {
            pole_pravdepodobnosti[i] = calloc((ciselkoHranicaY), sizeof(float));
        }
        

        for (int i = 0; i < ciselkoHranicaY; i++)
        {
            for (int j = 0; j < ciselkoHranicaX; j++)
            {
            fscanf(subor, "%f;", &pole_pravdepodobnosti[j][i]);
            }
        }

        //nacitam priemer_krok  -float
        float ** pole_priemerKrok;
        pole_priemerKrok = calloc((ciselkoHranicaX), sizeof(float*));
        for (int i = 0; i < ciselkoHranicaX; i++)
        {
            pole_priemerKrok[i] = calloc((ciselkoHranicaY), sizeof(float));
        }
        

        for (int i = 0; i < ciselkoHranicaY; i++)
        {
            for (int j = 0; j < ciselkoHranicaX; j++)
            {
            fscanf(subor, "%f;", &pole_priemerKrok[j][i]);
            }
        }


        //nacitam struktura pravdepodobnosti

        prvd_t pravedpodobnosti;

        fscanf(subor, "%f;", &pravedpodobnosti.hore);
        fscanf(subor, "%f;", &pravedpodobnosti.dole);
        fscanf(subor, "%f;", &pravedpodobnosti.vpravo);
        fscanf(subor, "%f;", &pravedpodobnosti.vlavo);


        //nacitam pole  - char
        svt_t * svetNacitany = svet_init_normal(ciselkoHranicaX, ciselkoHranicaY, pravedpodobnosti, pocetKrokov, svetOriginalReplikacii, cesta_k_suboru);


        char znak;
        char oddelovac;
        int sur_x_chodec;
        int sur_y_chodec;
        for (int t = 0; t < ciselkoHranicaY; t++)
        {
            for (int m = 0; m < ciselkoHranicaX; m++)
            {
                //budem nacitavat cez fread

                fread(&znak,sizeof(char), 1, subor);
                fread(&oddelovac,sizeof(char), 1, subor);
                //pom = strchr(buffer, ';');     //vracia adresu tohto znaku ;

                svetNacitany->pole[m][t] = znak - '0';
                if (svetNacitany->pole[m][t] == 1) {
                    sur_x_chodec = m;
                    sur_y_chodec = t;
                }
            }
        }


        //inicializacia sveta
        for (int i = 0; i < ciselkoHranicaY; i++)
        {
            for (int j = 0; j < ciselkoHranicaX; j++)
            {
                free(svetNacitany->pole_pravdepodobnosti[i]);
                free(svetNacitany->pole_priemer_krok[i]);
            }
        }
        free(svetNacitany->pole_priemer_krok);
        free(svetNacitany->pole_pravdepodobnosti);
        svetNacitany->pole_priemer_krok = pole_priemerKrok;
        svetNacitany->pole_pravdepodobnosti = pole_pravdepodobnosti;

        posun_chodca_na(svetNacitany, sur_x_chodec, sur_y_chodec);




        fclose(subor);
        return svetNacitany;
        

}



svt_t * svet_init_prekazky(int hranica_x, int hranica_y,  int sanca_na_prekazku, prvd_t pravdepodobnosti, int pocet_krokov_K, int pocet_replikacii, char * cesta_k_suboru) {
    svt_t * svet;
    svet = malloc(sizeof(svt_t));
    
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
    
    svet->pole_pravdepodobnosti = calloc((hranica_x), sizeof(float*));
    for (int i = 0; i < hranica_x; i++)
    {
        svet->pole_pravdepodobnosti[i] = calloc(hranica_y, sizeof(float));
    }

     svet->pole_priemer_krok = calloc((hranica_x), sizeof(float*));
    for (int i = 0; i < hranica_x; i++)
    {
        svet->pole_priemer_krok[i] = calloc(hranica_y, sizeof(float));
    }
    strcpy(svet->cesta_k_suboru, cesta_k_suboru);
    svet->pocet_krokov_K = pocet_krokov_K;
    svet->pocet_replikacii = pocet_replikacii;
    svet->hranica_x = hranica_x;
    svet->hranica_y = hranica_y;
    svet->original_replikacii = pocet_replikacii;
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
    je_policko_ok(svet, x-1, y, navstivene);
    je_policko_ok(svet, x+1, y, navstivene);
    je_policko_ok(svet, x, y-1, navstivene);
    je_policko_ok(svet, x, y+1, navstivene);
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





svt_t * svet_copy(svt_t * svet_nakopirovanie) {
    svt_t * fiktivny = svet_init_normal(svet_nakopirovanie->hranica_x, svet_nakopirovanie->hranica_y, svet_nakopirovanie->pravdepodobnosti);
    for (int i = 0; i < svet_nakopirovanie->hranica_x; i++) {
        for (int j = 0; j < svet_nakopirovanie->hranica_y; j++) {
            fiktivny->pole[i][j] = svet_nakopirovanie->pole[i][j];
        }
    }
    posun_chodca_na(fiktivny, svet_nakopirovanie->chodec->x, svet_nakopirovanie->chodec->y);
    return fiktivny;
}


