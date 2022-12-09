#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include<math.h>

char* getFileText(char* path) {
    FILE* file = fopen(path, "r");

    if (file == NULL) {
        printf("Cannot open file: %s", path);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long long int fileLenght = ftell(file) + 1;
    fseek(file, 0, SEEK_SET);

    char* fileText = malloc(fileLenght * sizeof(char));
    fread(fileText, sizeof(char), fileLenght, file); 
    // using fgets() instead of fread() enables to corretly read files with more than 1 line but it also leaves weird duplicates of the last line in the output file and doesnt covers more than one line to binary
    fclose(file);

    fileText[fileLenght - 1] = '\0';

    return fileText;
}

int writeToFile(char* path, char* text) {
    FILE* file = fopen(path, "w");

    if (file == NULL) {
        printf("Cannot open file: %s", path);
        exit(1);
    }

    fwrite(text, sizeof(char), strlen(text), file);

    fclose(file);
    return 0;
}

char* stringToBinary(char* s) {
    if(s == NULL) return 0; 
    size_t len = strlen(s);
    char *binary = malloc(len*8 + 1);
    binary[0] = '\0';
    for(size_t i = 0; i < len; ++i) {
        char ch = s[i];
        for(int j = 7; j >= 0; --j){
            if(ch & (1 << j)) {
                strcat(binary,"1");
            } else {
                strcat(binary,"0");
            }
        }
    }
    return binary;
}

char* binaryToString(char* b) {
    int i = 0;
    char* string = malloc(strlen(b) * sizeof(char));
    while (b[i] != '\0') {
        char* temp = malloc(9 * sizeof(char));
        for (int j = 0; j < 8; j++) {
            temp[j] = b[i];
            i++;
        }
        temp[8] = '\0';
        int decimal = strtol(temp, NULL, 2);
        char c = decimal;
        string[i / 8 - 1] = c;
    }

    return string;
}

char* binaryToBinaryString(char* b){
    long long int i = 0;
    while (b[i] != '\0') {
        if (b[i] == '0') {
            b[i] = '0';
        } else {
            b[i] = '1';
        }
        i++;
    }
    return b;
}

int main() 
{  
    char* inputMessage = getFileText("plik1.txt");
    char* binaryMessage = stringToBinary(inputMessage);
    char* binaryStringMessage = binaryToBinaryString(binaryMessage);
    char* outputMessage = binaryToString(binaryMessage);

    printf("%d", (strlen(binaryMessage) / 8));
    writeToFile("plik2.txt", binaryStringMessage);
    writeToFile("plik3.txt", outputMessage);

    return 0;
}  

// q: why this code leaves weird artifacts when input file has more than 1 line?
// a: 