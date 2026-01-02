
#include "simulacia.h"






void urobstatistiku_pre_svet(int pocet_pokusov_K, svt_t * svet) {
    
    
    for (int i = 0; i < svet->hranica_x; i++) {
        for (int j = 0; j < svet->hranica_y; j++) {
            if (svet->pole[i][j] != 2) {
                svet->pole_pravdepodobnosti[i][j] = simuluj_k_stredu_od_policka_statistika(i, j, pocet_pokusov_K, svet) * 100;
            }
            
        }
    
    }
    




}


float simuluj_k_stredu_od_policka_statistika(int sur_x, int sur_y, int pocet_pokusov_K, svt_t * svet) {
    
    int pocet_opakovanii_pre_policko = 1500;

    int uspesne_pokusy = 0;

    
    svt_t * kopia_sveta = svet_copy(svet);
    posun_chodca_na(kopia_sveta,  sur_x, sur_y);
    
    
    

    for (int i = 0; i < pocet_opakovanii_pre_policko; i++) {
        for (int j = 0; j < pocet_pokusov_K; j++) {
            
            if (kopia_sveta->chodec->x == kopia_sveta->stred_x && kopia_sveta->chodec->y == kopia_sveta->stred_y) {
                uspesne_pokusy++;
                break;
            }
            posun_chodca(daj_nahodny_smer_pre_chodca(kopia_sveta), kopia_sveta);
            if (kopia_sveta->chodec->x == kopia_sveta->stred_x && kopia_sveta->chodec->y == kopia_sveta->stred_y) {
                uspesne_pokusy++;
                break;
            }
        }
        svet_destroy(kopia_sveta);
        kopia_sveta = svet_copy(svet);
        posun_chodca_na(kopia_sveta,  sur_x, sur_y);
    }
    svet_destroy(kopia_sveta);

    return (float)uspesne_pokusy / (float)pocet_opakovanii_pre_policko;
    

}