
#include "chodec.h"

typedef enum Smer {
  Hore,
  Dole,
  Vpravo,
  Vlavo,
}smer_t;


typedef struct Svet {
  int ** pole;
  chdc_t * chodec;
  int hranica_x;
  int hranica_y;
  int stred_x; //je to index stedu
  int stred_y;
}svt_t; 


svt_t * svet_init_normal(int hranica_x, int hranica_y);
svt_t * svet_init_prekazky(int hranica_x, int hranica_y, int sanca_na_prekazku); //od 0-100
_Bool je_svet_ok(svt_t * svet);
void je_policko_ok(svt_t * svet, int x, int y, _Bool navstivene[svet->hranica_x][svet->hranica_y]);
void svet_destroy(svt_t * svet);
void posun_chodca(smer_t smer_posunu, svt_t * svet);
void uloz_do_suboru(char * cesta_k_suboru);


//nevieme este co bude vracat
void naformatuj_svet_na_poslanie(); 

