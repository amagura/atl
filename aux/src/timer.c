#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int sec = 0;

void ping()
{
     printf("\r\tElapsed Time - %02d:%02d:%02d",
            (sec / 3600),
            ((sec % 3600)/60),
            (sec % 60));
}

void hdl(int sig)
{
     ++sec;
     ping();
     alarm(1); // sends alarm signal after one second
}

int main(int argc, char **argv)
{
     setbuf(stdout, NULL);
     ping();
     signal(SIGALRM, hdl);
     alarm(1);

     while (true);
     return EXIT_SUCCESS;
}
