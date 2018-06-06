#include "headers/compiling.h"

static void update_command(Command cmd, char* cmd_str);
static void update_lines(Command cmd, char* line);
static void split_command_line(Command cmd);
static int verify_dependencies(char* command);


/**
 *  Função para preencher o Notebook com as informações do ficheiro
 *  
 *  @param nb Notebook a preencher
 */
void populate_notebook(Notebook nb){
    if(!nb){
        printf("Notebook not defined\n");
        return;
    }
    char buf[1024] = {0};
    int r;
    int prev_out = FALSE;
    Command* aux;
    nb->commands[nb->command_size] = create_command(); 
    
    while(1){
        // leitura de uma linha do ficheiro com um máximo de 1024
        r = readln(nb->file,buf,1024);
        if ( r == 0) break;
        // Buf contem > no inicio, não vamos guardar informações
        if(buf[0] == '>'){
            prev_out = TRUE;
        }
        // Guardar informações de um comando
        if(buf[0] == '$'){
            update_command(nb->commands[nb->command_size],buf);
            if(nb->command_max * 0.6 < nb->command_size){
                nb->command_max *= 2;
                aux = realloc(nb->commands, sizeof(Command)*nb->command_max);
                if(aux == NULL){
                    nb->command_max *= 0.5;
                    nb->rollback = TRUE;
                    return;
                }
                nb->commands = aux;
            }
            ++nb->command_size;
            nb->commands[nb->command_size] = create_command();
        // Se não for output de comando
        }else if(!prev_out){
            update_lines(nb->commands[nb->command_size],buf);
        }  
        // Buf contem < no inicio, vamos guardar informações;
        if(buf[0] == '<'){
            prev_out = FALSE;
        }

    }
}

/**
 *  Adicionar uma linha da descrião a struct Command
 *  
 *  @param cmd Command a dar update
 *  @param line Linha a adicionar
 */
static void update_lines(Command cmd, char* line){
    char** aux;
    if(!cmd) return;
    // realocar o array das linhas
    if((cmd->l_max * 0.8) < cmd->l_num){
        cmd->l_max *= 2;
        aux = realloc(cmd->lines_before, sizeof(char*) * cmd->l_max);
        if(!aux){
            perror("Realloc error");
            _exit(-1);
        }
        cmd->lines_before = aux;
    }
    char* line_cpy = (char*) malloc(strlen(line) + 1);
    line_cpy = strcpy(line_cpy,line);
    cmd->lines_before[cmd->l_num] = line_cpy;
    ++cmd->l_num; 
}

/**
 *  Update da informação de um comando na struct Command
 *
 *  @param cmd Command a dar update
 *  @param line Linha do comando
 */
static void update_command(Command cmd, char* cmd_str){
    if(!cmd) return;
    cmd->command_line = (char*) malloc(strlen(cmd_str) +1);
    cmd->command_line = strcpy(cmd->command_line, cmd_str);
    split_command_line(cmd);   
    cmd->dep = verify_dependencies(cmd->command_line); 
}

/**
 *  Função auxiliar, que permite a separação da linha do comando 
 *  
 *  @param cmd Command a dar split
 */
static void split_command_line(Command cmd){
    char* token, *string, *to_free;
    char** r_aux;
    int argc = 0, argm = 5, i;

    string = (char*) malloc(strlen(cmd->command_line)+1);
    string = strcpy(string,cmd->command_line);

    cmd->command = malloc(sizeof(char*) * argm);

    if(string){
        to_free = string;
        token = strtok(string," ");
        i = 0;
        while(token !=NULL){
            if(argm * 0.8 < argc){
                argm *= 2;
                r_aux = realloc(cmd->command, sizeof(char*) * argm);
                if(!r_aux){
                    perror("Realloc error on argv");
                    _exit(-1);
                }
                cmd->command = r_aux;
            }
            if(i != 0){
                cmd->command[argc] = (char*) malloc(strlen(token)+1);
                cmd->command[argc] = strcpy(cmd->command[argc],token);
                ++argc;
            }else{
                ++i;
            }
            token = strtok(NULL," ");
        }
        free(to_free);
    }

}

/**
 *  Verificar dependências de um dado comando
 *
 *  @param command Linha do comando
 */
static int verify_dependencies(char* command){
    if(command[1] == '|') return 1;
    if(command[1] == ' ') return 0;
    int i;
    for (i = 1 ; command[i] != '|' ; ++i){
        if(!isdigit(command[i]))
            return -1;
    }
    //convert dependency number from string to int
    char* aux = (char*) malloc(i+1);
    aux = strncpy(aux,command+1,i);
    return strtol(aux,NULL,10);
}

