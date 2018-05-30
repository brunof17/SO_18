#include "headers/processor.h"

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
