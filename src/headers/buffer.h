#ifndef __BUFFER_H__
#define __BUFFER_H__


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct buffer_t* Buffer;

Buffer create_buffer (int fildes, int nbytes);
int destroy_buffer(Buffer buf);
int get_fildes(Buffer buf);
int readln(Buffer buffer, char* buf, int max);

#endif
