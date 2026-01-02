
#include "svet.h"

typedef struct Simulacia {
  int pocet_replikacii;
  char * cesta_k_suboru;
  Svet * svet; 
}sim_t;


void simuluj_k_stredu();
void zavriSimulaciu();
sim_t * simulacia_init();
