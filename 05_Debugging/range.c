#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static long long number(const char *text)
{
    char *end;
    errno = 0;
    long long value = strtoll(text, &end, 10);
    if (errno || end == text || *end) {
        fprintf(stderr, "Invalid integer: %s\n", text);
        exit(EXIT_FAILURE);
    }
    return value;
}

/* A stable breakpoint before each element is printed. */
static void element(long long start, long long stop, long long step,
                    long long current)
{
    (void)start;
    (void)stop;
    (void)step;
    printf("%lld\n", current);
}

int main(int argc, char **argv)
{
    if (argc == 1) {
        puts("Usage: range [START] STOP [STEP]\n"
             "Print integers from START (default 0) to STOP (excluded).\n"
             "STEP defaults to 1 and must not be zero.");
        return EXIT_SUCCESS;
    }
    if (argc > 4) {
        fputs("Usage: range [START] STOP [STEP]\n", stderr);
        return EXIT_FAILURE;
    }
    long long start = argc == 2 ? 0 : number(argv[1]);
    long long stop = number(argv[argc == 2 ? 1 : 2]);
    long long step = argc == 4 ? number(argv[3]) : 1;
    if (step == 0) {
        fputs("STEP must not be zero\n", stderr);
        return EXIT_FAILURE;
    }
    for (long long current = start;
         step > 0 ? current < stop : current > stop;) {
        element(start, stop, step, current);
        /* Stop before signed overflow; the next value is outside the range. */
        if ((step > 0 && current > LLONG_MAX - step) ||
            (step < 0 && current < LLONG_MIN - step))
            break;
        current += step;
    }
    return EXIT_SUCCESS;
}
