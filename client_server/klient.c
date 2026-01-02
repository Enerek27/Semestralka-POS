
#include "svet.h"

int main(int argc, char const *argv[])
{
    svt_t * svetik =  svet_init_prekazky(10, 10, 20);
    svet_vypis(svetik);
    svet_destroy(svetik);

    return 0;
}
