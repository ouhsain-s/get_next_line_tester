// tst_mandatory.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "../get_next_line.h" /* header from project root (parent dir) */

#define FILE_SMALL "test_small.txt"
#define FILE_BIG "test_big.txt"
#define FILE_EMPTY "test_empty.txt"
#define FILE_NONEWLINE "test_nonewline.txt"

void make_test_files(void)
{
    FILE *f;

    f = fopen(FILE_SMALL, "w");
    if (!f) { perror("fopen small"); exit(1); }
    fprintf(f, "line1\nline2\nline3\n");
    fclose(f);

    f = fopen(FILE_BIG, "w");
    if (!f) { perror("fopen big"); exit(1); }
    for (int i = 0; i < 10000; i++)
        fprintf(f, "line%d\n", i);
    fclose(f);

    f = fopen(FILE_EMPTY, "w");
    if (!f) { perror("fopen empty"); exit(1); }
    fclose(f);

    f = fopen(FILE_NONEWLINE, "w");
    if (!f) { perror("fopen nonewline"); exit(1); }
    fprintf(f, "last_line_no_newline");
    fclose(f);
}

void remove_test_files(void)
{
    remove(FILE_SMALL);
    remove(FILE_BIG);
    remove(FILE_EMPTY);
    remove(FILE_NONEWLINE);
}

void print_line(int fd, char *line)
{
    if (line)
        printf("[FD %d] %s", fd, line);
    else
        printf("[FD %d] (NULL)\n", fd);
}

void run_file_test(const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0) { perror("open"); return; }

    char *line;
    int count = 0;

    printf("\n--- Testing %s ---\n", filename);
    while ((line = get_next_line(fd)))
    {
        print_line(fd, line);
        free(line);
        count++;
    }
    printf("Total lines read: %d\n", count);

    close(fd);
}

void run_invalid_fd_test(void)
{
    char *line;
    printf("\n--- INVALID FD TEST ---\n");
    line = get_next_line(-1);
    if (!line)
        printf("✅ Returned NULL for invalid FD\n");
    else
    {
        printf("❌ Returned non-NULL!\n");
        free(line);
    }
}

void run_perf_test(void)
{
    int fd = open(FILE_BIG, O_RDONLY);
    if (fd < 0) { perror("open big"); return; }

    char *line;
    int count = 0;
    while ((line = get_next_line(fd)))
    {
        free(line);
        count++;
    }
    close(fd);
    printf("\n--- PERFORMANCE TEST ---\n");
    printf("Read %d lines from BIG file\n", count);
}

int main(void)
{
    printf("=== MANDATORY GNL TESTER ===\n");

    make_test_files();

    run_file_test(FILE_SMALL);
    run_file_test(FILE_EMPTY);
    run_file_test(FILE_NONEWLINE);
    run_file_test(FILE_BIG);

    run_invalid_fd_test();
    run_perf_test();

    remove_test_files();

    printf("\n=== ALL TESTS COMPLETE ===\n");
    printf("Run with Valgrind: valgrind --leak-check=full ./tst_mandatory\n");

    return 0;
}

