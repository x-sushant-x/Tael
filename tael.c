#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_LINES 10
#define POOL_INTERVAL 100

void print_error(char* error) {
    printf("\033[31mERROR:\033[0m");
    printf(" %s\n", error);
}

void print_error_with_instructions(char* error, char* instructions) {
    print_error(error);
    printf("\n%s\n", instructions);
}

long read_file(FILE* file, const unsigned int lines_to_read) {
    if(fseek(file, 0, SEEK_END) != 0) return -1;

    const long file_size = ftell(file);
    if (file_size == -1L) {
        return -1;
    }
    if (file_size == 0) {
        return 0;
    }

    unsigned int new_line_count = 0;
    long pos = file_size - 1;

    while(pos >= 0) {
        if(fseek(file, pos, SEEK_SET) != 0) {
            return -1;
        }

        const int ch = fgetc(file);
        if (ch == EOF) break;

        if (ch == '\n') {
            new_line_count++;
            if (new_line_count == lines_to_read) break;
        }

        pos--;
    }

    const long start_pos = new_line_count == lines_to_read ? pos + 1 : 0;

    if(fseek(file, start_pos, SEEK_SET) != 0) {
        return -1;
    }

    while(1) {
        const int ch = fgetc(file);
        if(ch == EOF) {
            putchar('\n');
            break;
        }

        putchar(ch);
    }

    return file_size - 1;
}

void read_follow(FILE* file, long last_read_pos) {
    for (;;) {
        if(fseek(file, 0, SEEK_END) != 0) return;

        const long new_size = ftell(file);
        if (new_size == -1L) return;

        if (last_read_pos < new_size) {
            if (fseek(file, last_read_pos, SEEK_SET) != 0) return;

            int ch;
            while ((ch = fgetc(file)) != EOF) {
                putchar(ch);
            }

            last_read_pos = new_size;
        }

        usleep(100000);
    }
}

/*
    Usage:
    tael -r <file_name> - Displays last 10 lines
    tael -r10 <file_name> - Displays given lines.
    tael -f <file_name> - Displays last 10 lines and keep printing new if observed. Userful for printing logs.
*/

int main(const int argc, char** argv) {
    // Handle tael -help case here.

    if (argc < 3) {
        char* inst = "Usage: tael <option> <file_name> or run tael -help for more information.";
        print_error_with_instructions("Invalid Command", inst);
        return 0;
    }

    const char* option = argv[1];
    const char* file_name = argv[2];

    FILE* file = fopen(file_name, "rb");
    if (!file) {
        print_error("File not found.");
        return 1;
    }


    if (strcmp(option, "-f") == 0) {
        const int last_read_pos = read_file(file, DEFAULT_LINES);

        if (last_read_pos == -1) {
            print_error("Unable to read file.");
            return 1;
        }

        read_follow(file, last_read_pos);

    } else if (strncmp(option, "-r", 2) == 0) {
        int lines_to_read = DEFAULT_LINES;
        if (strlen(option) > 2) {
            const char* num_str = option + 2;
            char* end_ptr;
            const long num = strtol(num_str, &end_ptr, 10);

            if (end_ptr == num_str || num <= 0) {
                char* ins = "Correct usage: tael -r<lines> file_name or just -r to read 10 lines.";
                print_error_with_instructions("Invalid usage of -r flag.", ins);
                return 0;
            }

            lines_to_read = (int) num;
        }

        if(read_file(file, lines_to_read) == -1) {
            print_error("Unable to read file.");
            return 1;
        }
    }


    return 0;
}