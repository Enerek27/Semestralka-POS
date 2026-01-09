#ifndef PIPE_H
#define PIPE_H
#include <stdatomic.h>
#include <stddef.h>

// Štruktúra obsahujúca informácie pre prácu s dátovodom
typedef struct PipeData {
    char * path;
    int fD;
} pipe_data_t;
typedef struct server_info_pipe {
    atomic_bool chcem_statistiku;
    atomic_bool vypni_server;
    atomic_bool prepni_mod;
    atomic_bool prazdne;
}server_inf_t ;
typedef struct klient {
    pipe_data_t pip_write;
    pipe_data_t pip_read;
    atomic_bool klien_bezi;
} klient_pipe_t;
typedef struct server {
    pipe_data_t pip_write;
    pipe_data_t pip_read;
    atomic_bool server_bezi;
    server_inf_t server_info;
}server_pipe_t;
// Hlavičky funkcií, ktoré sú verejne dostupné a pracujú s dátovodom
void pipe_init(pipe_data_t * this, char * path, _Bool withCreation);
void pipe_destroy(pipe_data_t * this, _Bool withDeletion, _Bool withNameFree);
void pipe_open_write(pipe_data_t * this);
void pipe_open_read(pipe_data_t * this);
int pipe_read(pipe_data_t * this, char * result, size_t size);
void pipe_write(pipe_data_t * this, const char * data, size_t size);
void pipe_close(pipe_data_t * this);
void * nacuvajklientovi_pipe(void * arg);
#endif