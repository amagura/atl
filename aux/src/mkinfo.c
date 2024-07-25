#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <regex.h>
#include <libgen.h>
#include <limits.h>
#include <assert.h>
#include <mcheck.h>
#include <getopt.h>

#ifndef PATH_MAX
# include <linux/limits.h>
#endif

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlIO.h>
#include <libxml/xinclude.h>

#define COMNR_DEBUG 0
#define PROGNAME "mkinfo"
#define COMNR_PROGNAME "mkinfo"
#include <commoner.h>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "mkinfo.h"

#include "xml.h"

int bdinit(struct btl_build_data *bd)
{
     if (bd == NULL)
          return 1;
     bd->xpath = malloc(24); // size of "/top/release/target\0" + 4
     bd->v[0] = NULL;
     bd->v[1] = NULL;
     bd->used[0] = false;
     bd->used[1] = false;
     return 0;
}

int bdfree(struct btl_build_data *bd)
{
     if (bd == NULL)
          return 1;
     if (bd->xpath)
          free(bd->xpath);

     for (int idx = 0; idx < 2; ++idx) {
          if (!bd->used[idx])
               continue;
          if (bd->v[idx]) {
               free(bd->v[idx]);
          }
     }
     return 0;
}

int cxinit(struct changes *ch)
{
     if (ch == NULL)
          return 1;
     ch->value = NULL;
     ch->xpath = malloc(24); //the length of the longest possible xpath + 1
     ch->chid = -1;
     ch->xml = NULL;
     ch->dry = false;
     ch->deploy = true;
     return 0;
}

int cxfree(struct changes *ch)
{
     if (ch == NULL)
          return 1;

     if (ch->value)
          free(ch->value);
     if (ch->xpath)
          free(ch->xpath);
     return 0;
}

void inform(const char *msg, ...) __attribute__((sentinel));

void inform(const char *msg, ...)
{
     const char *s0;
     fprintf(stderr, "%s: %s\n", PNAME, msg);
     va_list args;
     va_start(args, msg);
     while ((s0 = va_arg(args, const char *))) {
          fprintf(stderr, "%s\n", s0);
     }
     va_end(args);
}

/* XXX
 * -1 = no match
 *  0 = bash_tools
 *  1 = navigation
 */
int idfile(FILE *fp)
{
     const char *regs[3] = {"^BASH_TOOLS_VERSION='.*'", "^NG_VERSION='.*'"};
     int err;
     /* just long enough for navigation or bash_tools plus terminator */
     char *line = NULL;
     size_t len = 0;
     ssize_t read;
     regex_t rgx0;
     regex_t rgx1;
     int r = -1;

     err = regcomp(&rgx0, regs[0], 0);
     if (err) {
          fprintf(stderr, "could not compile regex `%s'\n",
                  regs[0]);
          exit(1);
     }
     err = regcomp(&rgx1, regs[1], 0);
     if (err) {
          fprintf(stderr, "could not compile regex `%s'\n",
                  regs[1]);
          exit(1);
     }

     while ((read = getline(&line, &len, fp)) != EOF) {
          /* remove line-endings */
          line[strcspn(line, "\r\n")] = 0;

          COMNR_DBG("retrieved line of length: %zu:\n\t'%s'\n",
                    read, line);
          err = regexec(&rgx0, line, 0, NULL, 0);
          if (!err) {
               comnr_log("Bash Tools Match");
               r = 0;
               goto end;
          }
          err = regexec(&rgx1, line, 0, NULL, 0);
          if (!err) {
               comnr_log("Navigation Match");
               r = 1;
               goto end;
          }
     }
end:
     if (line)
          free(line);
     regfree(&rgx0);
     regfree(&rgx1);
     return r;
}

int main(int argc, char **argv)
{
     int ret = EXIT_SUCCESS;
     char *btl_build = NULL;
     int c, *optc = 0;
     c = 0;
     struct changes ch;
     cxinit(&ch);

     char *sopts = "hvf:x:ndSs";

     struct option lopts[] = {
          {"help", no_argument, 0, 'h'},
          {"version", no_argument, 0, 'v'},
          {"xml", required_argument, 0, 'f'},
          {"dryrun", no_argument, 0, 'd'},
          {"dry-run", no_argument, 0, 'd'},
          {0, 0, 0, 0}
     };

     while ((c = getopt_long(argc, argv, sopts, lopts, optc)) != EOF) {
          if (optopt != 0)
               goto fail;

          switch (c) {
          case 'h':
               printf("Usage: %s [OPTION]... COMMAND\n", PROGNAME);
               printf("  or:  %s [OPTION]... get TARGET\n", PROGNAME);
               printf("  or:  %s [OPTION]... ship FILE\n", PROGNAME);
               printf("  or:  %s [OPTION]... update FILE\n", PROGNAME);
               printf("\n%s", "COMMAND may be one of `update', `ship', or `get'\n");
               printf("%s\n", " TARGET may be one of `build' or `release'\n");
               comnr_arg("-h, --help", "print this message and exit", "\t\t");
               comnr_arg("-v, --version", "print program version and exit", "\t\t");
               comnr_arg("-f, --xml=FILE", "use FILE instead of default xml file", "\t");
               comnr_arg("-n, --dryrun", "don't update or modify the target xml file", "\t");
               printf("\n%s\n", "[1mNote[0;0m");
               printf("%s\n%s\n", "Use `update' to mark FILE as the latest build of FILE;",
                      "`ship' marks FILE as the latest release of FILE");
               goto end;
          case 'v':
               printf("%s\n", "0.0.4a");
               goto end;
          case 'f': /* let's have both -x and -f be the same option */
          case 'x':
               btl_build = malloc(PATH_MAX);
               int err = 0;
               char *tmp = getdir(&err, optarg);
               if (err != 0) {
                    printf("%s: fatal: %s: `%s'\n",
                           argv[0], strerror(err), optarg);
                    free(btl_build);
                    goto fail;
               }
               rstrdup(btl_build, PATH_MAX, tmp);
               free(tmp);
               break;
          case 'd':
          case 'n':
               ch.dry = true;
               break;
          case 's':
          case 'S':
               ch.deploy = false;
               break;
          }
     }

#define BTL_BUILD_PTR (btl_build == NULL ? BTL_BUILD : btl_build)

     comnr_log("optopt: '%d'\n", optopt);
     comnr_log("opterr: '%d'\n", opterr);
     comnr_log("optind: '%d'\n", optind);
     comnr_log("argc: '%d'\n", argc);
     comnr_log("argv[optind]: '%s'\n", argv[optind]);

     if (argc == optind || argc == 1 || argc == (optind - 1))
          kys("%s: missing command operand\n", PROGNAME);

#if COMNR_DEBUG
     for (int index = optind; index < argc; ++index) {
          fprintf(stderr, "Non-option argument '%s'\n", argv[index]);
     }
#endif

     FILE *fp;
     int err;
     char *buf = NULL;
     char *target = "release";
     char *cache = NULL;
     int argind = optind;

     /* if we're updating the XML we need at least two args:
      * 1. command
      * 2. file
      */

     /* okay, so first let's figure out what we're doing.  */
     // optind is the index for the first non-option at this point
     int test = cmpstrs(argv[argind++], 2, "update", "ship", "get", "set", NULL);

     switch (test) {
     case 1:
          target = "build";
     case 2:
          if (argind == argc)
               ribbt("missing file operand", NULL);

          size_t tmpsz = strlen("/top/" "/target");
          tmpsz += strlen(target);
          catl((char *)ch.xpath, tmpsz, "/top/", target, "/target");

          for (int idx = argind; idx < argc; ++idx) {
               err = 0;
               buf = getdir(&err, argv[idx]);
               if (err != 0) {
                    printf("%s: warning: %s: `%s'\n",
                            argv[0], strerror(err), argv[idx]);
                    continue;
               }

               if ((fp = fopen(buf, "r")) == NULL) {
                    err = errno;
                    printf("%s: cannot open file `%s': %s\n",
                            argv[0], argv[idx], strerror(err));
                    ret = EXIT_FAILURE;
                    goto cleanup;
               }
               int tmpid = idfile(fp);
               fclose(fp);
               switch (tmpid) {
               case -1:
                    free(buf);
                    kys("%s: %s: '%s'\n", PROGNAME, "file cannot be identified", argv[idx]);
                    break;
               case 0:
               case 1:
                    ch.chid = tmpid;
                    ch.value = memdup(buf, PATH_MAX);
                    free(buf);

                    COMNR_DBG("ch.xml will be '%s'\n", BTL_BUILD_PTR);
                    ch.xml = BTL_BUILD_PTR;
                    modxml(ch);
                    cxfree(&ch);
                    goto end;
               }
          }
          break;
     case 3:
          if (argind == argc)
               ribbt("missing target operand", NULL);

          target = argv[argind];

          struct btl_build_data bd;
          memset(&bd, 0, sizeof(bd));
          bool release = true;
          // we have already read from optind; from now on, we need to read from optind + 1
          test = cmpstrs(target, 4, "build", "release", NULL, NULL);
          switch (test) {
          case -1:
               /* XXX support for custom targets is under development */
               kys("%s: invalid target: '%s'\n", PROGNAME, argv[argind]);
               break;
          case 1:
               release = false;
               target = "build";
          case 2:
               COMNR_DBG("argv[%d]: '%s'\n", argind - 1, argv[argind - 1]);
               COMNR_DBG("doc will be '%s'\n", BTL_BUILD_PTR);
               xmlDocPtr doc = xopen(BTL_BUILD_PTR);
               buf = malloc(22);
               catl(buf, 22, "/top/", target, "/target");
               char **v = getv_xpath(doc, (xmlChar *)buf);
               if (v == NULL) {
                    printf("%s: could not find `%s' in XML: '`%s'\n", PROGNAME, buf, BTL_BUILD_PTR);
                    ret = EXIT_FAILURE;
                    xclose(doc);
                    goto cleanup;
               }
               comnr_ping;

               for (int cnt = 0; cnt < 2; ++cnt) {
                    COMNR_DBG("v[%d]: '%s'\n", cnt, v[cnt]);
                    if (!v[cnt])
                         continue;
                    COMNR_DBG("(pre: %d) v[%d]: '%s'\n", cnt, cnt, v[cnt]);
                    if (!release) {
                         err = 0;
                         cache = getdir(&err, v[cnt]);
                         if (err != 0) {
                              printf("%s: warning: %s: `%s'\n", PROGNAME, strerror(err), v[cnt]);
                              free(v[cnt]);
                              continue;
                         }
                         free(v[cnt]);
                    } else {
                         if (strncmp(STARTDIR, v[cnt], 39) != 0) {
                              COMNR_DBG("v[%d] does not contain STARTDIR\n", cnt);
                              COMNR_DBG("v[%d][37-45]: '%c%c%c%c%c%c%c%c%c'\n", cnt,
                                        v[cnt][37], v[cnt][38], v[cnt][39],
                                        v[cnt][40], v[cnt][41], v[cnt][42],
                                        v[cnt][43], v[cnt][44], v[cnt][45]);;
                              COMNR_DBG("(%d): STARTDIR[37-39]: '%c%c%c'\n", cnt,
                                        STARTDIR[37], STARTDIR[38], STARTDIR[39]);
                              cache = malloc(PATH_MAX);
                              rstrdup(cache, PATH_MAX, STARTDIR, v[cnt]);
                              free(v[cnt]);
                         } else {
                              cache = strdup(v[cnt]);
                              free(v[cnt]);
                         }
                    }
                    COMNR_DBG("(post: %d) cache: '%s'\n", cnt, cache);
                    printf("%s\n", cache);
                    free(cache);
               }
               comnr_pong;
               xclose(doc);
               free(v);
               break;
          }
          break;
     case 4:
          /* allow setting arbitrary fields in the xml file, like a beta release */
          printf("%s: [1mset[0;0m feature currently under development\n", PROGNAME);
          goto fail;
          break;
     }
     if (buf == NULL)
          buf = realloc(buf, 42);
cleanup:
     free(buf);
end:
     return ret;
fail:
     return EXIT_FAILURE;
}
