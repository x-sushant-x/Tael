#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_LINES 10

void print_error(char* error) {
    printf("\033[31mERROR:\033[0m");
    printf(" %s\n", error);
}

void print_error_with_instructions(char* error, char* instructions) {
    print_error(error);
    printf("\n%s\n", instructions);
}

int read_file(FILE* file, const unsigned int lines_to_read) {

    if(fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return 1;
    }

    const long file_size = ftell(file);
    if (file_size == -1L) {
        fclose(file);
        return 1;
    }

    if (file_size == 0) {
        fclose(file);
        return 0;
    }

    unsigned int new_line_count = 0;
    long pos = file_size - 1;

    while(pos >= 0) {
        if(fseek(file, pos, SEEK_SET) != 0) {
            fclose(file);
            return 1;
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
        fclose(file);
        return 1;
    }

    while(1) {
        const int ch = fgetc(file);
        if(ch == EOF) {
            putchar('\n');
            break;
        }

        putchar(ch);
    }

    fclose(file);

    return 0;
}

/*
    Usage:
    tael <file_name> - Displays last 10 lines
    tael <file_name> -<line_count> - Displays given lines.
*/

int main(const int argc, char** argv) {
    if (argc < 2) {
        char* inst = "Usage: tael <file_name> or run tael --help for more information.";
        print_error_with_instructions("Invalid Command", inst);
        return 0;
    }

    const char* file_name = argv[1];
    int lines_to_read = DEFAULT_LINES;

    // Handles tael <file_name> -<line_count> by calculating lines if specified
    if (argc == 3) {
        const char* c_lines = argv[2];
        if (c_lines != 0) {
            const int i_lines = abs(atoi(c_lines));
            if (i_lines == 0) {
                char* inst = "Usage: tael <file_name> -<lines>";
                print_error_with_instructions("Invalid Command", inst);
                return 0;
            }

            lines_to_read = i_lines;
        }
    }


    FILE* file = fopen(file_name, "rb");
    if (!file) {
        print_error("File not found.");
        return 1;
    }

    printf("Lines to read: %d", lines_to_read);
    if(read_file(file, lines_to_read) != 0) {
        print_error("Unable to read file.");
        return 1;
    }

    return 0;
}