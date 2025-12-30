
#include "chodec.h"

typedef enum Smer {
  Hore,
  Dole,
  Vpravo,
  Vlavo,
};


typedef struct Svet {
  int ** pole;
  Chodec * chodec;
}svt_t; 


svt_t * svet_init(int hranica_x, int hranica_y);
void svet_destroy();
void posun_chodca(Smer smer_posunu);
void uloz_do_suboru(char * cesta_k_suboru);

//nevieme este co bude vracat
void naformatuj_svet_na_poslanie();

