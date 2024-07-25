#include <getopt.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char **argv)
{
     int *optc = 0;
     int c = 0;
     char *sopts = "t:hf:";

     struct option lopts[] = {
          {"take", required_argument, 0, 't'},
          {"help", no_argument, 0, 'h'},
          {"find", required_argument, 0, 'f'},
          {0, 0, 0, 0},
     };

     while ((c = getopt_long(argc, argv, sopts, lopts, optc)) != EOF) {
          if (optopt != 0)
               return EOF;

          switch (c) {
          case 'f':
          case 't':
               printf("optarg: '%s'\n", optarg);
               break;
          case 'h':
               printf("%s.\n", "this is an example message");
               exit(0);
          }
     }
#define OPTLESS_ARGC ((argc - optind) + 1)
     printf("OPTLESS_ARGC: '%d'\n", OPTLESS_ARGC);
     printf("optind: '%d'\n", optind);
     printf("argc: '%d'\n", argc);
     return optind;
}
