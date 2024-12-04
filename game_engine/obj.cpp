#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

#define LINE_BUFFER_SIZE 256

bool str_contains(char* str1, char* str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);

    printf("%s\t%i\n%s\t%i\n", str1, len1, str2, len2);

    char* a; 
    char* b; 

    if (len2 > len1) {
        size_t temp_len = len2;
        len2 = len1;
        len1 = temp_len;
        a = str2;
        b = str1;
    } else {
        a = str1;
        b = str2;
    }

    bool contains = false;
    for (int i = 0; i < len1 - len2; i++) {
        for (int j = 0; j < len2; j++ ) {
            contains = a[i + j] == b[j];
        }
        if (contains) {
            return true;
        }
    }
    return false;
}

void load_object_file(const char *file_path) {
    ifstream file(file_path);

    if (!file.is_open()) {
        printf("Error: Failed to open file, exiting...\n");
        exit(0);
    }
    char check_byte;
    char line[LINE_BUFFER_SIZE];
    char token[LINE_BUFFER_SIZE];
    int token_index = 0;
    int vs = 0;
    int fs = 0;

    while (file >> check_byte) {
        file.putback(check_byte);
        file.getline(line, LINE_BUFFER_SIZE);

        while (true) {
            token[token_index] == line[token_index];
            if (line[token_index] == ' ' || token_index + 1 == LINE_BUFFER_SIZE) {
                break;
            }
            token_index++;
        }
        
        if (str_contains(token, "v ")) {
            vs++;
        } else if (str_contains(token, "f ")) {
            fs++;
        }
        token_index = 0;
        
        memset(token, 0, sizeof(char) * LINE_BUFFER_SIZE);
        //printf("%s", line);
        //printf("\n");
    }
    printf("vs: %i, fs: %i\n", vs, fs);
    file.close();
}