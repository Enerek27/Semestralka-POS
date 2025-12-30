#include "chodec.h"
#include <stdlib.h>

chdc_t *chodec_init(int hranica_x, int hranica_y)
{
    chdc_t * chodec;
    chodec = malloc(sizeof(chdc_t));
    //chodec je vpravo dole
    chodec->x = hranica_x - 1;
    chodec->y = hranica_y - 1;

    return chodec;
}
void chodec_destroy(chdc_t * chodec) {
    free(chodec);
}