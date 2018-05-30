#ifndef __STRUCTURE_H__
#define __STRUCTURE_H__

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include "buffer.h"

#define FALSE 0
#define TRUE !(FALSE)
#define READ_SIZE 1024
#define I_MAX_CMD 5
#define I_MAX_BEFORE 5
#define I_MAX_OUT 5

typedef struct command{
    char** lines_before;
    int l_max;
    int l_num; 
    int dep;
    char* command_line;
    char** command;
    char** output_lines;
    int o_max;
    int o_num;
}* Command;


typedef struct notebook{
    Command* commands;
    char* filename;
    int command_max;
    int command_size;
    Buffer file;
    int rollback;
}* Notebook;


Notebook create_notebook(char* filename);
Command create_command();
void print_notebook(Notebook nb);

#endif
