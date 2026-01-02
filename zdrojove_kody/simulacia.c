
#include "simulacia.h"






void urobstatistiku_pre_svet(int pocet_pokusov_K, svt_t * svet) {
    svt_t * fiktivny = svet_init_normal(svet->hranica_x, svet->hranica_y, svet->pravdepodobnosti);
    for (int i = 0; i < svet->hranica_x; i++) {
        for (int j = 0; j < svet->hranica_y; j++) {
            fiktivny->pole[i][j] = svet->pole[i][j];
        }
    }
    posun_chodca_na(fiktivny, svet->chodec->x, svet->chodec->y);
    
    for (int i = 0; i < svet->hranica_x; i++) {
        for (int j = 0; j < svet->hranica_y; j++) {
            if (svet->pole[i][j] != 2) {
                svet->pole_pravdepodobnosti[i][j] = simuluj_k_stredu_od_policka_statistika(i, j, pocet_pokusov_K, fiktivny) * 100;
            }
            
        }
    
    }
    svet_destroy(fiktivny);




}


float simuluj_k_stredu_od_policka_statistika(int sur_x, int sur_y, int pocet_pokusov_K, svt_t * svet) {
    
    int pocet_opakovanii_pre_policko = 1000;

    int uspesne_pokusy = 0;

    

    
    
    posun_chodca_na(svet,  sur_x,  sur_y);

    for (int i = 0; i < pocet_opakovanii_pre_policko; i++) {
        for (int j = 0; j < pocet_pokusov_K; j++) {
            
            if (svet->chodec->x == svet->stred_x && svet->chodec->y == svet->stred_y) {
                uspesne_pokusy++;
                break;
            }
            posun_chodca(daj_nahodny_smer_pre_chodca(svet), svet);
            
        }

        posun_chodca_na(svet,  sur_x,  sur_y);
    }

    return (float)uspesne_pokusy / (float)pocet_opakovanii_pre_policko;
    

}