
//TODO
// treba potom zmenit na svet.h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../zdrojove_kody/simulacia.h"

int main(int argc, char const *argv[])
{
     srand(time(NULL));
    prvd_t pravdepodobnosti;
    pravdepodobnosti.dole = 0.25;
    pravdepodobnosti.vpravo = 0.25;
    pravdepodobnosti.hore = 0.25;
    pravdepodobnosti.vlavo = 0.25;
    svt_t * svetik =  svet_init_prekazky(10, 10, 20,pravdepodobnosti);
    svet_vypis(svetik);
    printf("\n");
    generuj_pravdepodobnost(50, svetik);
    generuj_priem_krok(svetik);
    svet_vypis_statistiku(svetik);
    printf("\n");
    svet_vypis_priem_krok(svetik);
    svet_destroy(svetik);
    svt_t * svetik1 = svet_init_normal(10, 10, pravdepodobnosti);
    svet_vypis(svetik1);
    printf("\n");
    generuj_pravdepodobnost(50, svetik1);
    generuj_priem_krok(svetik1);
    svet_vypis_statistiku(svetik1);
    printf("\n");
    svet_vypis_priem_krok(svetik1);
    svet_destroy(svetik1);
    return 0;
}
