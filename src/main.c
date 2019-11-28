#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h>
#include <stdbool.h>

int sum(int a){
    if(a!=1){
        pid_t pid;
        int pipe_1[2], pipe_2[2], res=0, buff_a = a-1;
        if((pipe(pipe_1) == -1)){
        	write(STDERR_FILENO, "ERROR: pipe\n", sizeof "ERROR: pipe\n" - 1);
        	exit(EXIT_FAILURE);
        }
        pid = fork();
        if(pid > 0){    //Родитель
            close(pipe_1[0]);
            write(pipe_1[1], &buff_a, sizeof(buff_a));
            close(pipe_1[1]);
            int status;
            wait(&status);
            printf(">>%i\n", WEXITSTATUS(status));
            res = WEXITSTATUS(status);
            return res+a;
        }
        else if(pid < 0){
        	write(STDERR_FILENO, "ERROR: fork\n", sizeof "ERROR: fork\n" - 1);
        	exit(EXIT_FAILURE);
        }
        else{   //Ребенок
            int get = 0;
            close(pipe_1[1]);
            read(pipe_1[0], &get, sizeof(get));
            close(pipe_1[0]);
            get = sum(get);
            exit(get);
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
