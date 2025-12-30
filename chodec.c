#include "chodec.h"

chdc_t *chodec_init(int hranica_x, int hranica_y)
{
    chdc_t chodec;
    //chodec je vpravo dole
    chodec.x = hranica_x - 1;
    chodec.y = hranica_y - 1;

    return &chodec;
}
