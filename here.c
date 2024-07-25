#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <libgen.h>

#ifndef PATH_MAX
# include <linux/limits.h>
#endif

/* This is such a hacky piece of shit */

int main(int argc, char **argv)
{
	char buf[PATH_MAX + 1];
	char *ptr = realpath(argv[0], buf);
	if (ptr == NULL) {
		perror("Could not resolve path to `here'\n");
		goto fail;
#if 0
		switch(errno) {
		case EACCES:
			printf("Lack permissions\n");
			goto fail;
		case EINVAL:
			printf("either path or resolved_path is null\n");
			goto fail;
		case ELOOP:
			printf("too many symbolics\n");
			goto fail;
		case ENAMETOOLONG:
			printf("pathname is too long\n");
			goto fail;
		case ENOENT:
			puts("file doesn't exist\n");
			goto fail;
		case ENOTDIR:
			puts("path prefix is not a directory");
			goto fail;
		}
#endif
	}
	printf("%s\n", dirname(buf));
	/*printf("argv[0]: '%s'\n", argv[0]);*/
	return EXIT_SUCCESS;
fail:
	return EXIT_FAILURE;
}
