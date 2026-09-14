#include <curses.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Offsets {
  long *data;
  int count;
  int size;
};

void free_lines(char **lines, int count) {
  int i;

  for (i = 0; i < count; i++) {
    free(lines[i]);
  }
}

char *read_line(FILE *file) {
  char buffer[4096];
  size_t len;

  if (fgets(buffer, sizeof(buffer), file) == NULL) {
    return NULL;
  }

  len = strlen(buffer);
  if (len > 0 && buffer[len - 1] == '\n') {
    buffer[len - 1] = '\0';
  }

  return strdup(buffer);
}

int has_next_char(FILE *file) {
  int ch;
  long pos = ftell(file);

  if (pos < 0) {
    return 0;
  }

  ch = fgetc(file);
  if (ch == EOF) {
    return 0;
  }

  if (fseek(file, pos, SEEK_SET) != 0) {
    return 0;
  }

  return 1;
}

int add_offset(struct Offsets *offsets, long value) {
  long *tmp;
  int new_size;

  if (offsets->count < offsets->size) {
    offsets->data[offsets->count++] = value;
    return 0;
  }

  new_size = offsets->size == 0 ? 32 : offsets->size * 2;
  tmp = realloc(offsets->data, (size_t)new_size * sizeof(*offsets->data));
  if (tmp == NULL) {
    return -1;
  }

  offsets->data = tmp;
  offsets->size = new_size;
  offsets->data[offsets->count++] = value;
  return 0;
}

int ensure_offset(FILE *file, struct Offsets *offsets, int line) {
  char buffer[4096];

  while (offsets->count <= line) {
    if (fseek(file, offsets->data[offsets->count - 1], SEEK_SET) != 0) {
      return -1;
    }
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
      return 0;
    }
    if (!has_next_char(file)) {
      return 0;
    }
    if (add_offset(offsets, ftell(file)) != 0) {
      return -1;
    }
  }

  return 1;
}

int load_page(FILE *file, char **lines, int old_count, int max_lines,
              struct Offsets *offsets, int first_line) {
  int count = 0;

  if (ensure_offset(file, offsets, first_line) != 1) {
    return old_count;
  }

  free_lines(lines, old_count);
  if (fseek(file, offsets->data[first_line], SEEK_SET) != 0) {
    return 0;
  }

  while (count < max_lines) {
    lines[count] = read_line(file);
    if (lines[count] == NULL) {
      break;
    }
    if (offsets->count == first_line + count + 1 && has_next_char(file)) {
      if (add_offset(offsets, ftell(file)) != 0) {
        free_lines(lines, count + 1);
        return 0;
      }
    }
    count++;
  }

  return count;
}

void draw_page(WINDOW *win, char **lines, int count, const char *name) {
  int width;
  int row;

  width = getmaxx(win);
  werase(win);
  box(win, 0, 0);
  mvwprintw(win, 0, 2, " %s ", name);

  for (row = 0; row < count; row++) {
    mvwprintw(win, row + 1, 1, "%.*s", width - 2, lines[row]);
  }

  wrefresh(win);
}

int move_to_line(FILE *file, char **lines, int count, int max_lines,
                 struct Offsets *offsets, int *first_line, int new_line) {
  int new_count;

  if (new_line < 0) {
    new_line = 0;
  }

  new_count = load_page(file, lines, count, max_lines, offsets, new_line);
  if (new_count == 0 && new_line != 0) {
    return count;
  }

  *first_line = new_line;
  return new_count;
}

int main(int argc, char **argv) {
  FILE *file;
  char **lines;
  int count = 0;
  int key;
  int height;
  int width;
  int max_lines;
  int first_line = 0;
  WINDOW *win;
  struct Offsets offsets = {NULL, 0, 0};

  if (argc != 2) {
    fprintf(stderr, "Usage: %s FILE\n", argv[0]);
    return 1;
  }

  file = fopen(argv[1], "r");
  if (file == NULL) {
    fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
    return 1;
  }

  if (add_offset(&offsets, 0) != 0) {
    fclose(file);
    return 1;
  }

  initscr();
  cbreak();
  noecho();

  getmaxyx(stdscr, height, width);
  max_lines = height - 2;
  if (max_lines < 1) {
    endwin();
    free(offsets.data);
    fclose(file);
    return 1;
  }

  lines = calloc((size_t)max_lines, sizeof(*lines));
  if (lines == NULL) {
    endwin();
    free(offsets.data);
    fclose(file);
    return 1;
  }

  win = newwin(height, width, 0, 0);
  if (win == NULL) {
    endwin();
    free(lines);
    free(offsets.data);
    fclose(file);
    return 1;
  }
  keypad(win, TRUE);

  count = load_page(file, lines, count, max_lines, &offsets, first_line);
  draw_page(win, lines, count, argv[1]);

  while ((key = wgetch(win)) != 27) {
    int new_line = first_line;

    if (key == ' ' || key == KEY_DOWN) {
      new_line = first_line + 1;
    } else if (key == KEY_UP) {
      new_line = first_line - 1;
    } else if (key == KEY_NPAGE) {
      new_line = first_line + max_lines;
    } else if (key == KEY_PPAGE) {
      new_line = first_line - max_lines;
    } else {
      continue;
    }

    count = move_to_line(file, lines, count, max_lines, &offsets, &first_line,
                         new_line);
    draw_page(win, lines, count, argv[1]);
  }

  delwin(win);
  endwin();
  free_lines(lines, count);
  free(lines);
  free(offsets.data);
  fclose(file);
  return 0;
}
