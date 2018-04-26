#ifndef __BUFFER_H__
#define __BUFFER_H__


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct buffer_t* BUFFER;

BUFFER create_buffer (int fildes, int nbytes);
int destroy_buffer(BUFFER buf);
int readln(BUFFER buffer, char* buf, int max);

#endif
