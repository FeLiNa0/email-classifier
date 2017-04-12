#include <stdio.h> 
#include <string.h> 
#include "pearson.h"

int write_row(int d, char* in_file, char* out_file, 
              hash_function hash, unsigned* vector,
              char* rainbow_table_file)
{
    // Initialize the vector
    for (int i = 0; i < d; i++) {
        vector[i] = 0;
    }

    // Open the input file 
    char line[8096];
    FILE* input_fd;
    if (!(input_fd = fopen(in_file, "r"))) {
        printf("Input file %s: fopen failed\n", in_file);
        return 1;
    }

    FILE* rainbow_fd;
    if (rainbow_table_file) {
        if (!(rainbow_fd = fopen(rainbow_table_file, "w"))) {
            printf("Output file %s: fopen failed\n", rainbow_table_file);
            return 1;
        }
    }

    // Add each line of the input file to the vector
    while (fgets(line, sizeof(line), input_fd)) {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';

        // Ignore empty lines and lines that start with //
        if (line[0] != '\0' || (line[0] == '/' && line[1] == '/')) {
            if (rainbow_table_file) {
                fprintf(rainbow_fd, "%u    %s\n", hash(line), line);
            }
            vector[hash(line)]++;
        }
    }

    // Close the input file
    fclose(input_fd);

    // Open the output file
    FILE* output_fd;
    if (!(output_fd = fopen(out_file, "a"))) {
        printf("Output file %s: fopen failed\n", out_file);
        return 1;
    }

    // Write each element of the vector to a row in the CSV file 
    fprintf(output_fd, "%u", vector[0]);
    for (int i = 1; i < d; i++) {
        fprintf(output_fd, ",%u", vector[i]);
    }
    fprintf(output_fd, "\n");

    // Close the output file
    fclose(output_fd);
}

void print_help(char* prog_name)
{
    printf("usage: %s %s", prog_name,
      "meta%d.data %d.data OUT.csv DIMENSION A B\n\n\n"
      "    Use %d to denote the numerical portion of the filename.\n"
      "    DIMENSION must be 128, 256, 512, 1024, 2048, 4096, or 8192 and A < B\n\n"
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
    hash_function hash = pick_hash(d);
    if (hash == NULL) {
        puts("Bad dimension\n");
        print_help(v[0]);
        return 2;
    }

    // Allocate a vector
    unsigned vector[d];

    int r;
    char in_file[1024];
    // Write a row to the CSV file from i=a to i=b
    printf("Writing vectors of dimension %d to output file %s\n", d, out_file);
    for (int i = a; i <= b; i++) {
        sprintf(in_file, in_file_template, i);
        if ((r = write_row(d, in_file, out_file, hash, vector, rainbow_table)) != 0) {
            return r;
        }
        if (i % (b - a) == 0) {
            printf("Appended row %d from %s\n", i, in_file); 
        }
    }
    printf("Finished writing %d rows to %s\n", b - a, out_file);

    return 0;
}
