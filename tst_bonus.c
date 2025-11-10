// tst_bonus.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

#include "../get_next_line_bonus.h" /* header from project root (parent dir) */

#define FILE_SMALL "bonus_test_small.txt"
#define FILE_BIG "bonus_test_big.txt"
#define FILE_EMPTY "bonus_test_empty.txt"
#define FILE_NONEWLINE "bonus_test_nonewline.txt"

double get_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0);
}

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
    fprintf(f, "no_newline_here");
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

void run_single_test(const char *name, const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0) { perror("open"); return; }

    char *line;
    int count = 0;

    printf("\n--- %s ---\n", name);
    while ((line = get_next_line_bonus(fd)))
    {
        if (!line) break;
        if (count < 5)
            print_line(fd, line);
        free(line);
        count++;
    }
    printf("Total lines read: %d\n", count);
    close(fd);
}

void run_perf_test(void)
{
    int fd = open(FILE_BIG, O_RDONLY);
    if (fd < 0) { perror("open big"); return; }

    double start = get_time_ms();
    char *line;
    int count = 0;
    while ((line = get_next_line_bonus(fd)))
    {
        if (!line) break;
        free(line);
        count++;
    }
    double end = get_time_ms();
    close(fd);
    printf("\n--- PERFORMANCE TEST ---\n");
    printf("Read %d lines in %.2f ms\n", count, end - start);
}

void run_invalid_fd_test(void)
{
    char *line;
    printf("\n--- INVALID FD TEST ---\n");
    line = get_next_line_bonus(-1);
    if (!line)
        printf("✅ Returned NULL for invalid FD\n");
    else
    {
        printf("❌ Returned non-NULL!\n");
        free(line);
    }
}

void run_tiny_buffer_test(void)
{
    int fd = open(FILE_SMALL, O_RDONLY);
    if (fd < 0) { perror("open small"); return; }

    char *line;
    int count = 0;
    printf("\n--- TINY BUFFER TEST (BUFFER_SIZE=%d) ---\n", BUFFER_SIZE);
    while ((line = get_next_line_bonus(fd)))
    {
        if (!line) break;
        print_line(fd, line);
        free(line);
        count++;
    }
    close(fd);
    printf("Total lines read with tiny buffer: %d\n", count);
}

int main(void)
{
    printf("=== BONUS GNL TESTER ===\n");

    make_test_files();

    run_single_test("SMALL FILE", FILE_SMALL);
    run_single_test("EMPTY FILE", FILE_EMPTY);
    run_single_test("NO NEWLINE FILE", FILE_NONEWLINE);

    run_invalid_fd_test();
    run_tiny_buffer_test();
    run_perf_test();

    remove_test_files();

    printf("\n=== ALL TESTS COMPLETE ===\n");
    printf("Run with Valgrind: valgrind --leak-check=full ./tst_bonus\n");

    return 0;
}

