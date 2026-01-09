



#include "../zdrojove_kody/UI.h"




#define RED "\033[31m"
#define RESET "\033[0m"


svt_t * server_info_subor(socket_server_t * server);
void * vlaknoPrijmaniaSpojenia(void * arg);
svt_t * nastav_server(socket_server_t * server);