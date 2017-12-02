#include <stdio.h>   /* printf, stderr, fprintf */
#include <sys/types.h> /* pid_t */
#include <unistd.h>  /* _exit, fork */
#include <stdlib.h>  /* exit */
#include <errno.h>   /* errno */

int main(void)
{
    pid_t pid;
    
    pid2 = fork();
    
    soy = getpid();
    
    
    if (pid2 == -1) //ERROR
    {
        fprintf(stderr, "can't fork, error %d\n", errno);
        exit(EXIT_FAILURE);
    }
    if (pid2 == 0)
    {
        pid3 = fork();
        if(pid3 == 0)
        {
            
        }
        else
        {
            pid4 = fork();
        }
    }
    else
    {
        pid5 = fork();
        if(pid5 == 0)
        {
            waitpid(pid5);
            waitpid(pid4);
            pid6 = fork();
            if(pid6 == 0)
            {
                pid8 = fork();
            }
        }
        else
        {
            pid7 = fork();
        }
    }
    exit(0);
    return 0;
}