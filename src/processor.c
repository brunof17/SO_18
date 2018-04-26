
#include "../headers/processor.h"

#define READ_SIZE 1024
#define I_MAX_CMD 5
#define I_MAX_BEFORE 5
#define I_MAX_OUT 5

typedef struct comand{
    char** lines_before;
    int l_max;
    int l_num; 
    char* comand;
    char** output_lines;
    int o_max;
    int o_num;
}* Comand;


typedef struct notebook{
    Comand * comands;
    char* filename;
    int comand_max;
    int comand_size;
    Buffer file;
}* Notebook;

Comand create_comand(){
    Comand cmd = malloc(sizeof(Comand));
    cmd->l_max = I_MAX_BEFORE;
    cmd->o_max = I_MAX_OUT;
    cmd->comand = malloc(READ_SIZE);
    cmd->lines_before = malloc(sizeof(char*) * cmd->l_max);
    cmd->output_lines = malloc(sizeof(char*) * cmd->o_max);
    cmd->l_num = 0;
    cmd->o_num = 0;
    return cmd;
}

void update_lines(Comand cmd, char* line){
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
    char* line_cpy = malloc(strlen(line) + 1);
    line_cpy = strcpy(line_cpy,line);
    cmd->lines_before[cmd->l_num] = line_cpy;
    ++cmd->l_num; 
}

void update_comand(Comand cmd, char* cmd_str){
    if(!cmd) return;
    char* cmd_cpy = malloc(strlen(cmd_str) + 1);    
    cmd_cpy = strcpy(cmd_cpy, cmd_str);
    cmd->comand = cmd_cpy;
}

void update_outputs(Comand cmd, char* line){
}

void print_comand(Comand cmd){
    if(!cmd) printf("No current comand");
    for(int i = 0 ; i < cmd->l_num ; ++i){
        printf("%s\n",cmd->lines_before[i]);
    }
    printf("%s\n",cmd->comand);
    for(int i = 0; i < cmd->o_num ; ++i){
        printf("%s\n",cmd->output_lines[i]);
    }
}

void print_notebook(Notebook nb){
    if(!nb) printf("Null notebook, cannot print");
    printf("Notebook name: %s\n",nb->filename);
    for(int i = 0 ; i < nb->comand_size ; ++i ){
        print_comand(nb->comands[i]);  
    }
}

Notebook create_notebook(char* filename){
    int nb_fd = open(filename, O_RDONLY);
    if(nb_fd < 0){
        perror("Erro na abertura do ficheiro");
        _exit(-1);    
    }
    Notebook nb = malloc(sizeof(Notebook));
    nb->comand_max = I_MAX_CMD;
    nb->comand_size = 0;
    nb->comands = malloc(sizeof(Comand)*5);
    nb->file = create_buffer(nb_fd,READ_SIZE);
    nb->filename = malloc(strlen(filename)+1);
    nb->filename = strcpy(nb->filename, filename);
    return nb;
}

Notebook populate_notebook(Notebook nb){
    if(!nb){
        printf("Notebook not defined\n");
        return nb;
    }
    char buf[1024] = {0};
    int r, cmd_n = 0;
    nb->comands[cmd_n] = create_comand(); 

    //TODO falta resize do array dos Comand
    while(1){
        r = readln(nb->file,buf,1024);
        if ( r == 0) break;
        
        // quando encontrar uma linha que começe por $, adicionar ao comando, e depois passar para o proximo "comando"
        if(buf[0] == '$'){
            update_comand(nb->comands[cmd_n],buf);
            ++cmd_n;
            nb->comand_size++;
            nb->comands[cmd_n] = create_comand();
        }else{
            update_lines(nb->comands[cmd_n],buf);
        }  

    }
    return nb;
}

int main(int argc, char** argv) {

    if(argc < 2){
        printf("Usage: ./processor <filename>\n");
        _exit(-1);
    }
    
    /*
     * Create notebook with the corresponding file
     * */    
    Notebook nb = create_notebook(argv[1]);

    //Populate struct with notebook informations
    nb = populate_notebook(nb);

    // Close file
    int fd = get_fildes(nb->file); 
    close(fd);

    print_notebook(nb);
    
    // Reopen file for writing

    // Exec comands and write outputs

    return 0;
}
