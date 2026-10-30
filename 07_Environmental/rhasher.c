#include "config.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <rhash.h>
#ifdef USE_READLINE
#include <readline/readline.h>
#endif

static void execute(char *line)
{
    char *name = strtok(line, " \t\r\n");
    char *arg;
    unsigned algorithm;
    unsigned char digest[64];
    char output[130];
    int result;

    if (!name)
        return;
    arg = strtok(NULL, " \t\r\n");
    if (!arg) {
        fprintf(stderr, "rhasher: expected algorithm and file or string\n");
        return;
    }
    if (strcasecmp(name, "md5") == 0)
        algorithm = RHASH_MD5;
    else if (strcasecmp(name, "sha1") == 0)
        algorithm = RHASH_SHA1;
    else if (strcasecmp(name, "tth") == 0)
        algorithm = RHASH_TTH;
    else {
        fprintf(stderr, "rhasher: unknown algorithm: %s\n", name);
        return;
    }

    if (*arg == '"') {
        size_t length = strlen(++arg);
        /* A closing quote is optional; the leading quote marks a string. */
        if (length && arg[length - 1] == '"')
            arg[--length] = '\0';
        result = rhash_msg(algorithm, arg, length, digest);
    } else {
        result = rhash_file(algorithm, arg, digest);
    }
    if (result < 0) {
        fprintf(stderr, "rhasher: %s: %s\n", arg, strerror(errno));
        return;
    }
    rhash_print_bytes(output, digest, rhash_get_digest_size(algorithm),
                      isupper((unsigned char)*name) ? RHPR_HEX : RHPR_BASE64);
    puts(output);
}

int main(void)
{
    char *line = NULL;
    size_t capacity = 0;
    int interactive = isatty(STDIN_FILENO);
    int status = EXIT_SUCCESS;

    rhash_library_init();
    for (;;) {
#ifdef USE_READLINE
        if (interactive) {
            line = readline("rhasher> ");
            if (!line)
                break;
            execute(line);
            free(line);
            line = NULL;
            continue;
        }
#endif
        if (interactive) {
            fputs("rhasher> ", stdout);
            fflush(stdout);
        }
        errno = 0;
        if (getline(&line, &capacity, stdin) < 0) {
            if (!feof(stdin)) {
                fprintf(stderr, "rhasher: input: %s\n", strerror(errno));
                status = EXIT_FAILURE;
            }
            break;
        }
        execute(line);
    }
    free(line);
    if (fflush(stdout) == EOF || ferror(stdout))
        status = EXIT_FAILURE;
    return status;
}
