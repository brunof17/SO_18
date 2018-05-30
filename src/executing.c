#include "headers/executing.h"

static void write_command(Command cmd, int fd);
static int exec_cmd(Command cmd, Command dep);
static int update_outputs(Command cmd, char* line, int r);

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
                break;
            } 
        }

        if(!exec_cmd(nb->commands[i],prev)){
            nb->rollback = TRUE;
            break;
        }
    }    
}

static int exec_cmd(Command cmd, Command dep){
    int status;
    int r = 0;
    printf("Executing: %s\n", cmd->command_line);

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
                if(!update_outputs(cmd, buf, r)) return FALSE;
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

void override_file(Notebook nb){
    
    if(!nb) return;
    if(nb->rollback) return;
    int fd = open(nb->filename, O_WRONLY | O_TRUNC);
    for(int i = 0 ; i < nb->command_size ; ++i){
        write_command(nb->commands[i],fd);
    }

}

static void write_command(Command cmd, int fd){

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

static int update_outputs(Command cmd, char* line, int r){
    if(!cmd) return FALSE;

    int size;
    char** r_aux;
    if(cmd->o_max * 0.8 < cmd->o_num){
        cmd->o_max *= 2;
        r_aux = realloc(cmd->output_lines, sizeof(char*) * cmd->o_max);
        if(!r_aux){
            cmd->o_max *= 0.5;
            return FALSE;
        }
        cmd->output_lines = r_aux;
    }
    size = cmd->o_num;
    cmd->output_lines[size] = malloc(strlen(line) +1);
    cmd->output_lines[size] = strncpy(cmd->output_lines[size],line, r);
    ++cmd->o_num;
    return TRUE;
}
