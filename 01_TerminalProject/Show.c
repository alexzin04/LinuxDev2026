#include <errno.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void free_lines(char **lines, size_t count)
{
    size_t i;

    for (i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
}

static int read_file(const char *name, char ***lines, size_t *count)
{
    FILE *file;
    char **data = NULL;
    char buffer[4096];
    size_t used = 0;
    size_t size = 0;

    file = fopen(name, "r");
    if (file == NULL) {
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char *line;
        size_t len = strlen(buffer);

        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        if (used == size) {
            char **tmp;
            size_t new_size = size == 0 ? 16 : size * 2;

            tmp = realloc(data, new_size * sizeof(*data));
            if (tmp == NULL) {
                free_lines(data, used);
                fclose(file);
                return -1;
            }
            data = tmp;
            size = new_size;
        }

        line = strdup(buffer);
        if (line == NULL) {
            free_lines(data, used);
            fclose(file);
            return -1;
        }
        data[used++] = line;
    }

    if (ferror(file)) {
        free_lines(data, used);
        fclose(file);
        return -1;
    }

    fclose(file);
    *lines = data;
    *count = used;
    return 0;
}

static void draw_page(WINDOW *win, char **lines, size_t count,
                      size_t first, const char *name)
{
    int height;
    int width;
    int row;

    getmaxyx(win, height, width);
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " %s ", name);

    for (row = 1; row < height - 1; row++) {
        size_t index = first + (size_t)(row - 1);

        if (index >= count) {
            break;
        }
        mvwprintw(win, row, 1, "%.*s", width - 2, lines[index]);
    }

    wrefresh(win);
}

int main(int argc, char **argv)
{
    char **lines = NULL;
    size_t count = 0;
    size_t first = 0;
    int key;
    int height;
    int width;
    WINDOW *win;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        return 1;
    }

    if (read_file(argv[1], &lines, &count) != 0) {
        fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    getmaxyx(stdscr, height, width);
    win = newwin(height, width, 0, 0);
    if (win == NULL) {
        endwin();
        free_lines(lines, count);
        return 1;
    }

    draw_page(win, lines, count, first, argv[1]);

    while ((key = getch()) != 27) {
        int page_height;
        int page_width;

        getmaxyx(win, page_height, page_width);
        (void)page_width;

        if (key == ' ' && first + (size_t)(page_height - 2) < count) {
            first++;
            draw_page(win, lines, count, first, argv[1]);
        }
    }

    delwin(win);
    endwin();
    free_lines(lines, count);
    return 0;
}
