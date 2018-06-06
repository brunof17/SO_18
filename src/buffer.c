#include "headers/buffer.h"

/**
 *  Criação da estrutura Buffer
 *  
 *  @param fildes File descriptor que se pretende ler
 *  @param nbytes Número máximo de bytes a serem lidos de cada vez
 */
Buffer create_buffer (int fildes, int nbytes){
    Buffer buf = malloc(sizeof(struct buffer_t));
    buf->buf = malloc(nbytes+1);
    buf->fildes = fildes;
    buf->nbytes = nbytes;
    buf->index = 0;
    buf->lidas = 0;    

    return buf;
}

/**
 *  Free da estrutura buffer
 *  
 *  @param buf Buffer a dar free
 */
int destroy_buffer(Buffer buf){
    free(buf->buf);
    free(buf);
    return 0;
}

/**
 *  Return file descritor do buffer
 *
 *  @param buffer Buffer que se pretende obter a informação
 */
int get_fildes(Buffer buffer){
    if(buffer)
        return buffer->fildes;
    return -1;
}

/**
 *  Realizar a leitura de uma linha do Buffer pretendido
 *
 *  @param buffer Estrutura Buffer da qual se pretende fazer a leitura
 *  @param buf Buffer onde se vai guardar a leitura
 *  @param max Número máximo de bytes a ler
 */
int readln(Buffer buffer, char* buf, int max){
    int i = 0;
    if(!buffer){
        perror("Null buffer");
        _exit(-1);
    } 

    while(i < max -1){
        if(buffer->lidas == buffer->index){
            buffer->lidas = read(buffer->fildes, buffer->buf, buffer->nbytes);
            if(buffer->lidas == -1){
                perror("Erro na leitura do ficheiro");
                _exit(-1);
            }
            buffer->index = 0;
        }

        if(buffer->lidas <= 0) break;
        buf[i] = buffer->buf[buffer->index];
        buffer->index ++;
        i++;
        if(buf[i-1] == '\n') break;
    }
    if(i < max){
        buf[i-1] = '\0';
    }
    return buffer->lidas;
}


