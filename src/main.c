#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include "itoa.h"

int readFromFile(char *filepath){    
    //printf("read from %s\n",filepath);
    int fd = open(filepath, O_RDONLY, (mode_t)0600);
    if (fd == -1){
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }
    struct stat fileInfo = {0};
    if (fstat(fd, &fileInfo) == -1){
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }
    if (fileInfo.st_size == 0){
        fprintf(stderr, "Error: File is empty, nothing to do\n");
        exit(EXIT_FAILURE);
    }
    char *map = mmap(0, fileInfo.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED){
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    int res = atoi(map);
    if (munmap(map, fileInfo.st_size) == -1){
        close(fd);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }
    close(fd);
    return res;
}

int writeToFile(char *filepath, int in){
    char *text=malloc(17*sizeof(char));
    itoa(in, text);
    int fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    //printf("write to %s value %d\n",filepath, in);
    if (fd == -1){
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    size_t textsize = strlen(text) + 1; 
    if (lseek(fd, textsize, SEEK_SET) == -1){
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    }
    if (write(fd, "", 1) == -1){
        close(fd);
        perror("Error writing last byte of the file");
        exit(EXIT_FAILURE);
    }
    char *map = mmap(0, textsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED){
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < textsize-1; i++){
        map[i] = text[i];
    }
    if (msync(map, textsize, MS_SYNC) == -1){
        perror("Could not sync the file to disk");
    }
    if (munmap(map, textsize) == -1){
        close(fd);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }
    close(fd);

    return 0;
}
//////////////////Sum func
int sum(int a){
    if(a!=1){
        pid_t pid;
        int res = 0, buff_a = a - 1;
        writeToFile("in", buff_a);
        pid = fork();
        if(pid > 0){    //Родитель
            int status;
            wait(&status);
            //printf(">>%i\n", WEXITSTATUS(status));
            //res = WEXITSTATUS(status);
            res = readFromFile("out");
            return res + a;
        }
        else if(pid < 0){
        	write(STDERR_FILENO, "ERROR: fork\n", sizeof "ERROR: fork\n" - 1);
        	exit(EXIT_FAILURE);
        }
        else{   //Ребенок
            int get = readFromFile("in");
            get = sum(get);
            writeToFile("out", get);
            //exit(get);
            exit(0);
        }
    }
    else
        return a;
}

int main()
{
    int n = 0;
    printf("Enter a sequence number: ");
    scanf("%d", &n);
    if(n<0 || n == 0)
        printf("You should give the number, which is more than zero and zero\n");
    else
        printf("%d\n", sum(n));
    return 0;
}
