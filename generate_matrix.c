#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h> 
#include <string.h> 
#include <stdint.h>
#include "pearson.h"
#include "cfarmhash/src/cfarmhash.h"

unsigned farm_hash(const char* c)
{
    return (unsigned)cfarmhash(c, strlen(c));
}

int write_row(
        int d, bool dense, int doc_number, char* in_file, char* out_file, hash_function hash, 
        int* table, char* rainbow_table_file)
{
    // Initialize the vector
    for (int i = 0; i < d; i++) {
        table[i] = 0;
    }

    // Open the input file 
    char line[8192];
    FILE* input_fd;
    if (!(input_fd = fopen(in_file, "r"))) {
        printf("Input file %s: fopen failed\n", in_file);
        perror("");
        return 1;
    }

    FILE* rainbow_fd;
    if (rainbow_table_file) {
        if (!(rainbow_fd = fopen(rainbow_table_file, "a"))) {
            printf("Output file %s: fopen failed\n", rainbow_table_file);
            perror("");
            fclose(input_fd);
            return 1;
        }
    }

    // Add each line of the input file to the vector
    int line_number = 0;
    bool is_spam;
    while (fgets(line, sizeof(line), input_fd)) {
        line_number++;
        if (line_number == 1) {
            is_spam = line[0] == '1';
            continue;
        }

        // Remove newline
        line[strcspn(line, "\n")] = '\0';

        // Ignore empty lines and lines that start with //
        if (line[0] != '\0' && line[0] != '/' && line[1] != '/') {
            if (rainbow_table_file) {
                fprintf(rainbow_fd, "%u    (%s:%d)    %s\n", hash(line) % d, in_file, line_number, line);
            }
            // The hashing trick gives poor results...
            // Dimension too low?
            table[hash(line) % d] += 1 - 2 * ((int)hash256(line) % 2);
            // table[hash(line) % d]++;
        }
    }

    // Close the input file
    if (rainbow_table_file) {
        fclose(rainbow_fd);
    }

    fclose(input_fd);

    // Open the output file
    FILE* output_fd;
    if (!(output_fd = fopen(out_file, "a"))) {
        printf("Output file %s: fopen failed\n", out_file);
        perror("");
        return 1;
    }

    // Write each element of the vector to a row in the CSV file 
    // First column is the label
    if (dense) {
        fprintf(output_fd, "%d", is_spam ? 1 : 0);
        for (int i = 0; i < d; i++) {
            fprintf(output_fd, ",%d", table[i]);
        }
    } else {
        fprintf(output_fd, "%d 1 %d\n", 1 + doc_number, is_spam ? 1 : -1);
        for (int i = 0; i < d; i++) {
            if (table[i] != 0)
                fprintf(output_fd, "%d %d %d\n", 1 + doc_number, 2 + i, table[i]);
        }
    }
    fprintf(output_fd, "\n");

    // Close the output file
    fclose(output_fd);
}

void print_help(char* prog_name)
{
    printf("usage: %s %s", prog_name,
      " (sparse|dense) %d.data OUT.csv DIMENSION A B\n\n\n"
      "    Use %d to denote the numerical portion of the filename.\n"
      "    If dense, DIMENSION must be 128, 256, 512, 1024, 2048, 4096, or 8192 and A < B\n\n"
      "    If not dense, DIMENSION should be a prime number or greater than the number of unique features\n\n"
      "    Loads files A.data, (A+1).data, ..., B.data and\n"
      "    writes (B-A) rows to OUT.csv. Designed to be used in conjunction\n"
      "    with extract_features.py\n"
    );
}

int main(int c, char** v)
{
    if (c != 7 && c != 8) {
        print_help(v[0]);
        return 1;
    }

    // Write rainbow table?
    char* rainbow_table = NULL;
    if (c == 8)
        rainbow_table = v[7]; 

    // Dense or sparse?
    bool dense = !strcmp(v[1], "dense");

    // Read filenames
    char* in_file_template = v[2]; 
    char* out_file = v[3]; 

    int matched;

    // Read bounds
    int a = 0;
    matched = sscanf(v[5], "%d", &a);
    if (matched != 1) {
        print_help(v[0]);
        return 3;
    }

    int b = 0;
    matched = sscanf(v[6], "%d", &b);
    if (matched != 1) {
        print_help(v[0]);
        return 3;
    }

    if (a >= b) {
        puts("A >= B\n");
        print_help(v[0]);
        return 3;
    }

    // Read dimension
    int d = 0;
    matched = sscanf(v[4], "%d", &d);
    if (matched != 1) {
        puts("Bad dimension\n");
        print_help(v[0]);
        return 2;
    }

    // Pick the right hash function
    hash_function* hash = NULL;
    if (dense) {
        hash = pick_hash(d);
        if (hash == NULL) {
            puts("Bad dimension\n");
            print_help(v[0]);
            return 2;
        }
    } else {
        hash = &farm_hash; // TODO &farm_hash;
    }

    // Allocate a vector
    int* table = malloc(d * sizeof(int));

    int r;
    char in_file[8192];
    // Write a row to the CSV file from i=a to i=b
    printf("Writing vectors of dimension %d to output file %s\n", d, out_file);
    for (int i = a; i <= b; i++) {
        sprintf(in_file, in_file_template, i);
        if ((r = write_row(d, dense, i - a, in_file, out_file, hash, table, rainbow_table)) != 0) {
            return r;
        }
        if (i % ((b - a) / 10) == 0) {
            printf("Appended row %d from %s\n", i, in_file); 
        }
    }
    printf("Finished writing %d rows to %s\n", b - a, out_file);

    free(table);
    return 0;
}
