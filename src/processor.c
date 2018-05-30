
#include "headers/processor.h"

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

void update_lines(Command cmd, char* line){
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

int verify_dependencies(char* command){
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

void split_command_line(Command cmd){

    char* token, *string, *to_free, *aux;
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

void update_command(Command cmd, char* cmd_str){
    if(!cmd) return;
    cmd->command_line = (char*) malloc(strlen(cmd_str) +1);
    cmd->command_line = strcpy(cmd->command_line, cmd_str);
    split_command_line(cmd);   
    cmd->dep = verify_dependencies(cmd->command_line); 
}

void update_outputs(Command cmd, char* line, int r){
    if(!cmd) return;

    int size;
    char** r_aux;
    if(cmd->o_max * 0.8 < cmd->o_num){
        cmd->o_max *= 2;
        r_aux = realloc(cmd->output_lines, sizeof(char*) * cmd->o_max);
        if(!r_aux){
            cmd->o_max *= 0.5;
            //TODO CALL ERROR
            return;
        }
        cmd->output_lines = r_aux;
    }
    size = cmd->o_num;
    cmd->output_lines[size] = malloc(strlen(line) +1);
    cmd->output_lines[size] = strncpy(cmd->output_lines[size],line, r);
    ++cmd->o_num;
}

void print_command(Command cmd,int rollback){
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

void print_notebook(Notebook nb){
    if(!nb) printf("Null notebook, cannot print");
    if(nb->rollback) return;
    for(int i = 0 ; i < nb->command_size ; ++i ){
        print_command(nb->commands[i],nb->rollback);  
    }
}

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
    

    //TODO falta resize do array dos Command
    while(1){
        r = readln(nb->file,buf,1024);
        if ( r == 0) break;

        // quando encontrar uma linha que começe por $, adicionar ao commando, e depois passar para o proximo "commando"
        if(buf[0] == '>'){
            prev_out = TRUE;
        }
        if(buf[0] == '$'){
            update_command(nb->commands[nb->command_size],buf);
            if(nb->command_max * 0.6 < nb->command_size){
                nb->command_max *= 2;
                aux = realloc(nb->commands, sizeof(Command)*nb->command_max);
                if(aux == NULL){
                    nb->command_max *= 0.5;
                    nb->rollback = TRUE;
                }
                nb->commands = aux;
            }
            ++nb->command_size;
            nb->commands[nb->command_size] = create_command();
        }else if(!prev_out){
            update_lines(nb->commands[nb->command_size],buf);
        }  
        if(buf[0] == '<'){
            prev_out = FALSE;
        }

    }
}

int exec_cmd(Command cmd, Command dep){
    int status;
    int r = 0;
    printf("Execunting: %s\n", cmd->command_line);

    int pd_in[2], pd_out[2];
    char buf[1024];
    
    if(pipe(pd_in) == -1) perror("Error on pipe creation (in)");
    if(pipe(pd_out) == -1) perror("Error on pipe creation (out)");

    if(fork() == 0){
        //Fechar escrita do pipe in
        close(pd_in[1]);
        if(dep != NULL){
            //Duplicar leitura do pipe in como stdin
            dup2(pd_in[0],0);
        }
        //Fechar entrada do pipe in
        close(pd_in[0]);


        //Fechar a leitura do pipe out
        close(pd_out[0]);
        //Duplicar a escrita do pipe como stdout
        dup2(pd_out[1],1);
        //Fechar a escrita do pipe
        close(pd_out[1]);
        
        r = execvp(cmd->command[0],cmd->command);
        printf("Command %s\n",cmd->command_line);
        perror("Erro no exec");
        _exit(r);

    }else{
        close(pd_in[0]);
        if(dep && dep->output_lines){
            for(int i = 0 ; dep->output_lines[i] != NULL ; ++i){
                write(pd_in[1],dep->output_lines[i], strlen(dep->output_lines[i]));
            }
        }
        close(pd_in[1]);
        
        wait(&status);
        close(pd_out[1]);
        if(WIFEXITED(status) && WEXITSTATUS(status)== 0){
            while((r = read(pd_out[0],buf,1024))){
                if(r <= 0) break;
                buf[r] = '\0';
                update_outputs(cmd, buf, r);
            }
        }else{
            while((r = read(pd_out[0], buf, 1024))){
                if(r <= 0) break;
                buf[r] = '\0';
                printf("%s\n",buf);
            }
            return FALSE;
        }
        close(pd_out[0]);
    }
    return TRUE;
}


void start_exec(Notebook nb){
    int dep;
    Command prev = NULL;
    if(!nb){
        perror("Notebook not existant");
        return;
    }

    for(int i = 0 ; i < nb->command_size ; ++i){
        dep = nb->commands[i]->dep;
        
        if(dep == -1) perror("Erro nas dependencias");

        if(dep != 0){
            // check bondaries
            if( i >= dep ? TRUE : FALSE){
                //Get output from previous as input for the pipe
                prev = nb->commands[i-dep];
            }else{
                nb->rollback = TRUE;
                //TODO rollbackFile()
                break;
            } 
        }

        if(!exec_cmd(nb->commands[i],prev)){
            nb->rollback = TRUE;
            break;
        }
    }    
}

void write_command(Command cmd, int fd){

    for(int i = 0 ; i < cmd->l_num ; ++i){
        write(fd, cmd->lines_before[i], strlen(cmd->lines_before[i]));
        write(fd, "\n", 1);
    }

    write(fd, cmd->command_line, strlen(cmd->command_line));
    write(fd, "\n", 1);

    if(cmd->o_num) write(fd, ">>>", 3);

    for(int i = 0 ; i < cmd->o_num ; ++i){
        write(fd, "\n", 1);
        write(fd, cmd->output_lines[i], strlen(cmd->output_lines[i]));
    }
        
    if(cmd->o_num){
        write(fd, "<<<", 3);
        write(fd, "\n", 1);
    }

    
}

void override_file(Notebook nb){
    
    if(!nb) return;
    if(nb->rollback) return;
    int fd = open(nb->filename, O_WRONLY | O_TRUNC);
    for(int i = 0 ; i < nb->command_size ; ++i){
        write_command(nb->commands[i],fd);
    }

}

int main(int argc, char** argv) {

    if(argc < 2){
        printf("Usage: ./processor <filename>\n");
        _exit(-1);
    }

    //  Create notebook with the corresponding file
    Notebook nb = create_notebook(argv[1]);


    //Populate struct with notebook informations
    populate_notebook(nb);
    //    printf("ANTES EXEC!!!:%s\n",nb->commands[1]->command[0]);

    // Close file, by getting the file descriptor from the buffer_t struct
    int fd = get_fildes(nb->file); 
    close(fd);

//    print_notebook(nb);

    // Reopen file for writing
    start_exec(nb);
    // Exec commands and write outputs
    print_notebook(nb);
    
    override_file(nb);
    
    close(fd);    

    return 0;
}
