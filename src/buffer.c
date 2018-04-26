#include "../headers/buffer.h"

struct buffer_t{
    char* buf;
    int fildes;
    int nbytes;
    int index;
    int lidas;
};

BUFFER create_buffer (int fildes, int nbytes){
    BUFFER buf = malloc(sizeof(BUFFER));
    buf->buf = malloc(nbytes);
    buf->fildes = fildes;
    buf->nbytes = nbytes;
    buf->index = 0;
    buf->lidas = 0;    

    return buf;
}

int destroy_buffer(BUFFER buf){
    free(buf->buf);
    free(buf);
    return 0;
}

int readln(BUFFER buffer, char* buf, int max){
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
        buf[i-1] = 0;
    }
    return buffer->lidas;
}   
