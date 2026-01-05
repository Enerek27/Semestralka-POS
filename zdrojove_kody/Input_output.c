

#include "Input_output.h"

void posli_socket_info_svet(svt_t * svet) {
    // komunikacia server - klient
    // server realizuje posiela kopiu klientovi ako foto
    socket_write(socket_data_t *this, const char *buffer, size_t length);

}


svt_t * prijmi_socket_info_svet(svt_t * svet) {
    socket_read(socket_data_t *this, char *buffer, size_t length);
    svt_t * prijaty;
    prijaty = svet_init_normal(int hranica_x, int hranica_y, prvd_t pravdepodobnosti);
}