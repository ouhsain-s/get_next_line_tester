// tst_bonus_with_yourfiles.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "../get_next_line_bonus.h" /* header from project root (parent dir) */

static void print_colored(int fd, int line_num, const char *line)
{
    printf("\033[1;36m[FD %d | Line %02d]\033[0m %s", fd, line_num, line);
    if (line[0] && line[strlen(line) - 1] != '\n')
        printf("\n");
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s file1 file2 ...\n", argv[0]);
        return 1;
    }

    int *fds = malloc(sizeof(int) * (argc - 1));
    int *line_count = malloc(sizeof(int) * (argc - 1));
    if (!fds || !line_count)
        return perror("malloc failed"), free(fds), free(line_count), 1;

    for (int i = 0; i < argc - 1; i++)
    {
        fds[i] = open(argv[i + 1], O_RDONLY);
        if (fds[i] < 0)
        {
            perror(argv[i + 1]);
            line_count[i] = -1;
        }
        else
            line_count[i] = 0;
    }

    int end = 0;
    printf("\n=== MULTI-FD TEST ===\n\n");

    while (!end)
    {
        end = 1;
        for (int i = 0; i < argc - 1; i++)
        {
            if (fds[i] < 0) continue;

            char *line = get_next_line_bonus(fds[i]);
            if (line)
            {
                end = 0;
                line_count[i]++;
                print_colored(fds[i], line_count[i], line);
                free(line);
            }
        }
    }

    printf("\n\n=== END OF FILES ===\n");
    for (int i = 0; i < argc - 1; i++)
    {
        if (fds[i] >= 0)
        {
            close(fds[i]);
            printf("Closed FD %d after %d lines.\n", fds[i], line_count[i]);
        }
    }

    free(fds);
    free(line_count);

    printf("\nRun with Valgrind: valgrind --leak-check=full ./tst_bonus_with_yourfiles ../file1 ../file2 ...\n");

    return 0;
}

