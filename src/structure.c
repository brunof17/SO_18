#include "headers/structure.h"

Notebook create_notebook(char* filename){
    int nb_fd = open(filename, O_RDONLY);
    if(nb_fd < 0){
        perror("Erro na abertura do ficheiro");
        _exit(-1);    
    }
    Notebook nb = (Notebook) malloc(sizeof(struct notebook));
    nb->command_max = I_MAX_CMD;
    nb->command_size = 0;
    nb->commands = malloc(sizeof(Command)*nb->command_max);
    nb->file = create_buffer(nb_fd,READ_SIZE);
    nb->filename = (char*)  malloc(strlen(filename)+1);
    nb->filename = strcpy(nb->filename, filename);
    nb->rollback = FALSE;
    return nb;
}

Command create_command(){
    Command cmd = malloc(sizeof(struct command));
    cmd->l_max = I_MAX_BEFORE;
    cmd->o_max = I_MAX_OUT;
    cmd->command_line = NULL;
    cmd->command = NULL;
    cmd->dep = -1;
    cmd->lines_before = malloc(sizeof(char *) * (cmd->l_max));
    for(int i = 0 ; i < cmd->l_max ; ++i) cmd->lines_before[i] = NULL;
    cmd->output_lines = malloc(sizeof(char *) * (cmd->o_max));
    for(int i = 0 ; i < cmd->o_max ; ++i) cmd->output_lines[i] = NULL;
    cmd->l_num = 0;
    cmd->o_num = 0;
    return cmd;
}

void print_notebook(Notebook nb){
    if(!nb) printf("Null notebook, cannot print");
    if(nb->rollback) return;
    for(int i = 0 ; i < nb->command_size ; ++i ){
        print_command(nb->commands[i],nb->rollback);  
    }
}

static void print_command(Command cmd,int rollback){
    if(!cmd) printf("Error printing command");
    for(int i = 0 ; i < cmd->l_num ; ++i){
        printf("%s\n",cmd->lines_before[i]);
    }
    printf("%s\n",cmd->command_line);
    if(cmd->o_num)  printf(">>>");
    for(int i = 0; i < cmd->o_num ; ++i){
        printf("\n%s",cmd->output_lines[i]);
    }
    if(cmd->o_num)  printf("<<<\n");
}
