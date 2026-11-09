#include "config.h"
#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _(s) gettext(s)

int main(void) {
  int low = 1, high = 100;
  char *line = NULL;
  size_t capacity = 0;
  ssize_t length;

  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  bind_textdomain_codeset(PACKAGE, "UTF-8");
  textdomain(PACKAGE);

  puts(_("Think of a number from 1 to 100."));
  while (low < high) {
    int middle = low + (high - low) / 2;
    printf(_("Is your number greater than %d? (%s/%s): "), middle, _("Yes"),
           _("No"));
    fflush(stdout);
    length = getline(&line, &capacity, stdin);
    if (length < 0) {
      fputs(ferror(stdin) ? _("Error: cannot read standard input.\n")
                          : _("Error: standard input is closed.\n"),
            stderr);
      free(line);
      return EXIT_FAILURE;
    }
    if (length > 0 && line[length - 1] == '\n')
      line[--length] = '\0';
    /* Compare the whole line, including any embedded NUL bytes. */
    if ((size_t)length == strlen(_("Yes")) &&
        memcmp(line, _("Yes"), (size_t)length) == 0)
      low = middle + 1;
    else if ((size_t)length == strlen(_("No")) &&
             memcmp(line, _("No"), (size_t)length) == 0)
      high = middle;
    else
      fprintf(stderr, _("Error: enter exactly '%s' or '%s'.\n"), _("Yes"),
              _("No"));
  }
  printf(_("Your number is %d!\n"), low);
  free(line);
  return EXIT_SUCCESS;
}
