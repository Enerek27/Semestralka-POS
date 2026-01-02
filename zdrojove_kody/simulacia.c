
#include "simulacia.h"






void generuj_pravdepodobnost(int pocet_pokusov_K, svt_t * svet) {
    
    
    for (int i = 0; i < svet->hranica_x; i++) {
        for (int j = 0; j < svet->hranica_y; j++) {
            if (svet->pole[i][j] != 2) {
                svet->pole_pravdepodobnosti[i][j] = simuluj_k_stredu_od_policka_statistika(i, j, pocet_pokusov_K, svet) * 100;
            }
            
        }
    
    }
    




}


float simuluj_k_stredu_od_policka_statistika(int sur_x, int sur_y, int pocet_pokusov_K, svt_t * svet) {
    
    int pocet_opakovanii_pre_policko = 1000;

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



void generuj_priem_krok(svt_t * svet) {
    for (int i = 0; i < svet->hranica_x; i++) {
        for (int j = 0; j < svet->hranica_y; j++) {
            if (svet->pole[i][j] != 2) {
                svet->pole_priemer_krok[i][j] = daj_priem_krok_policko(i, j, svet) * 100;
            }
            
        }
    
    }
}

float daj_priem_krok_policko(int sur_x, int sur_y, svt_t * svet) {
    int pocet_simulacii = 1000;

    svt_t * kopia_sveta = svet_copy(svet);
    posun_chodca_na(kopia_sveta,  sur_x, sur_y);
    int pocet_krokov_aktual = 0;
    int pocet_krokov_celkovo = 0;

    for (int i = 0; i < pocet_simulacii; i++) {
        while (kopia_sveta->stred_x != kopia_sveta->chodec->x || kopia_sveta->stred_y != kopia_sveta->chodec->y) {
            posun_chodca(daj_nahodny_smer_pre_chodca(kopia_sveta), kopia_sveta);
            pocet_krokov_aktual++;
        }
        pocet_krokov_celkovo += pocet_krokov_aktual;
        pocet_krokov_aktual = 0;
        svet_destroy(kopia_sveta);
        kopia_sveta = svet_copy(svet);
        posun_chodca_na(kopia_sveta,  sur_x, sur_y);
    }
    svet_destroy(kopia_sveta);
    return  (float)pocet_krokov_celkovo / (float)pocet_simulacii;


}
