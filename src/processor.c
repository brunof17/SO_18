
#include "../headers/processor.h"

int main(int argc, char** argv) {
    int nb_fd;

    if(argc < 2){
        printf("Usage: ./processor <filename>\n");
        _exit(-1);
    }
    
    nb_fd = open(argv[1], O_RDWR);
    if(nb_fd < 0){
        perror("Erro de abertura de ficheiro");
        _exit(-1);
    }

    BUFFER file =  create_buffer(nb_fd, 1024); 

    char buf[2048] = {0};
    int r = readln(file,buf,2048);
    int off = 0;
    char* oi = "bota remedio";

    while(r != 0){
        off += r;
        if(buf[0] == '$'){
            printf("Encontrei um comando: %s\n",buf);
            
           // write(nb_fd,oi,strlen(oi));
        }

    

        r = readln(file,buf,2048);
    } 
     
    close(nb_fd);

    return 0;
}
