
#include "chodec.h"

typedef enum Smer {
  Hore,
  Dole,
  Vpravo,
  Vlavo,
}smer_t;

typedef struct Pravdepodobnost {
  float hore;
  float dole;
  float vpravo;
  float vlavo;
}prvd_t ;

typedef struct Svet {
  int ** pole;
  float ** pole_pravdepodobnosti;
  chdc_t * chodec;
  int hranica_x;
  int hranica_y;
  int stred_x; //je to index stedu
  int stred_y;
  prvd_t pravdepodobnosti;
}svt_t; 


svt_t * svet_init_normal(int hranica_x, int hranica_y, prvd_t pravdepodobnosti);
svt_t * svet_init_prekazky(int hranica_x, int hranica_y, int sanca_na_prekazku, prvd_t pravdepodobnosti); //od 0-100%
_Bool je_svet_ok(svt_t * svet);
void posun_chodca_na(svt_t * svet, int sur_x, int sur_y);
//pomocna funkcia
void je_policko_ok(svt_t * svet, int x, int y, _Bool navstivene[svet->hranica_x][svet->hranica_y]);
void svet_destroy(svt_t * svet);
//posunie chodca v smere
void posun_chodca(smer_t smer_posunu, svt_t * svet);  
void uloz_do_suboru(char * cesta_k_suboru);
//vypise svet do stdout
void svet_vypis(svt_t * svet);
//vráti nahodny smer pre chodca ktorý je možný, nevráti smer do steny
smer_t daj_nahodny_smer_pre_chodca(svt_t * svet);
//vypise statistiku pre svet v percentach
void svet_vypis_statistiku(svt_t * svet);
//vytvor kopiu sveta
svt_t * svet_copy(svt_t * svet_nakopirovanie);

//nevieme este co bude vracat
void naformatuj_svet_na_poslanie(); 

