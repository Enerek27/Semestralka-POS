
//TODO
// treba potom zmenit na svet.h
#include "../zdrojove_kody/svet.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
    prvd_t pravdepodobnosti;
    pravdepodobnosti.dole = 0.50;
    pravdepodobnosti.vpravo = 0.50;
    pravdepodobnosti.hore = 0.25;
    pravdepodobnosti.vlavo = 0.25;
    svt_t * svetik =  svet_init_prekazky(10, 10, 20,pravdepodobnosti);
    svet_vypis(svetik);
    printf("\n");
    svet_vypis_statistiku(svetik);
    svet_destroy(svetik);
    svt_t * svetik1 = svet_init_normal(10, 10, pravdepodobnosti);
    svet_vypis(svetik1);
    printf("\n");
    svet_vypis_statistiku(svetik1);
    svet_destroy(svetik1);
    return 0;
}
