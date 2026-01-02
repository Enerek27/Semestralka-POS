
#include "zdrojove_kody\svet.h"
#include "zdrojove_kody\chodec.h"

int main(int argc, char const *argv[])
{
    svt_t * svetik =  svet_init_normal(10, 10);
    svet_vypis(svetik);
    svet_destroy(svetik);

    return 0;
}
