#include "headers/processor.h"

int main(int argc, char** argv) {

    if(argc < 2){
        printf("Usage: ./processor <filename>\n");
        _exit(-1);
    }
    
    int nbs = argc - 1;

    for(int i = 0 ; i < nbs ; ++i){
        int pid = fork();
        if(pid == 0){
            //Create notebook with the corresponding file
            Notebook nb = create_notebook(argv[i+1]);

            //Populate struct with notebook informations
            populate_notebook(nb);

            //Close file, by getting the file descriptor from the buffer_t struct
            int fd = get_fildes(nb->file); 
            close(fd);
            
            //Execute comands
            start_exec(nb);
    
            //Write new information
            override_file(nb);
    
            close(fd);    

            _exit(0);
        }else{
            printf("Foi criado o processo %d, para executar o notebook \"%s\"\n",pid, argv[i+1]);
        }
    }
    for(int i = 0 ; i < nbs ; ++i){
        wait(NULL);
    }
    return 0;
}
