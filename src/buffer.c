#include "headers/buffer.h"

Buffer create_buffer (int fildes, int nbytes){
    Buffer buf = malloc(sizeof(struct buffer_t));
    buf->buf = malloc(nbytes+1);
    buf->fildes = fildes;
    buf->nbytes = nbytes;
    buf->index = 0;
    buf->lidas = 0;    

    return buf;
}

int destroy_buffer(Buffer buf){
    free(buf->buf);
    free(buf);
    return 0;
}

int get_fildes(Buffer buffer){
    if(buffer)
        return buffer->fildes;
    return -1;
}

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


