#include "svet.h"
#include "chodec.h"

svt_t *svet_init(int hranica_x, int hranica_y)
{
    svt_t svet;
    svet.chodec = chodec_init(hranica_x, hranica_y);
    // 0 reprezentuje prazdne policko, 1 je chodec, 2 je prekazka
    svet.pole = calloc((hranica_x), sizeof(int));
    for (int i = 0; i < hranica_x; i++)
    {
        svet.pole[i] = calloc(hranica_y, sizeof(int));
    }
    
    svet.hranica_x = hranica_x;
    svet.hranica_y = hranica_y;
    svet.pole[hranica_x - 1][hranica_y - 1] = 1;
    return &svet;
}

void svet_destroy(svt_t * svet)
{
    for (int i = 0; i < svet->hranica_x; i++)
    {
        free(svet->pole[i]);
    }
    
    free(svet->pole);
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
            if (aktualna_poloha_x = 0)
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
                nova_poloha_x = svet->hranica_x + 1;
            }
            
            break;
        case Hore:
            if (aktualna_poloha_y = 0)
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
