#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 32

#define MESSAGE_INPUT_FILE "plik1.txt"
#define MESSAGE_CODED_FILE "plik2.txt"
#define MESSAGE_CORRUPTED_FILE "plik3.txt"

#define BINARY_MESSAGE_INPUT_FILE "plik1b.txt"
#define BINARY_MESSAGE_CODED_FILE "plik2b.txt"
#define BINARY_MESSAGE_CORRUPTED_FILE "plik3b.txt"

#define NUMERIC_MESSAGE_INPUT_FILE "plik1n.txt"
#define NUMERIC_MESSAGE_CODED_FILE "plik2n.txt"
#define NUMERIC_MESSAGE_CORRUPTED_FILE "plik3n.txt"

#define WRITE_BINARY "write_binary"
#define WRITE_NUMERIC "write_numeric"
#define WRITE_TEXT "write_text"

unsigned char* read_message_from_file(const char *file_name, size_t *lenght);
void write_message_to_file(const char *file_name, unsigned char *message, size_t lenght, const char *mode);
void corrupt_message(unsigned char *encoded_message, size_t lenght);
void print_message(unsigned char *message, size_t lenght);
int check_parity(unsigned char byte, int power, unsigned char parity_mask[4]);
unsigned char* encode_message(unsigned char* message, size_t message_lenght, unsigned char parity_mask[4]);
void encode_byte(unsigned char* byte_1, unsigned char* byte_2, unsigned char parity_mask[4]);

int main()
{
    // Seed the random number generator for the random errors
    srand(time(NULL));

    // Create the parity mask
    unsigned char parity_mask[4] = {255, 85, 51, 15};

    // Read the message from the file
    size_t message_lenght;
    unsigned char *message = read_message_from_file(MESSAGE_INPUT_FILE, &message_lenght);

    // Write message in different formats
    write_message_to_file(BINARY_MESSAGE_INPUT_FILE, message, message_lenght, WRITE_BINARY);
    write_message_to_file(NUMERIC_MESSAGE_INPUT_FILE, message, message_lenght, WRITE_NUMERIC);
    printf("Message read from file:\n");
    print_message(message, message_lenght);

    // Encode the message
    size_t encoded_message_lenght = message_lenght * 2;
    unsigned char *encoded_message = encode_message(message, message_lenght, parity_mask);

    // Write encoded message in different formats
    write_message_to_file(MESSAGE_CODED_FILE, encoded_message, encoded_message_lenght, WRITE_TEXT);
    write_message_to_file(BINARY_MESSAGE_CODED_FILE, encoded_message, encoded_message_lenght, WRITE_BINARY);
    write_message_to_file(NUMERIC_MESSAGE_CODED_FILE, encoded_message, encoded_message_lenght, WRITE_NUMERIC);
    printf("Encoded message:\n");
    print_message(encoded_message, encoded_message_lenght);

    // Corrupt the encoded message
    corrupt_message(encoded_message, encoded_message_lenght);

    // Write corrupted encoded message in different formats
    write_message_to_file(MESSAGE_CORRUPTED_FILE, encoded_message, encoded_message_lenght, WRITE_TEXT);
    write_message_to_file(BINARY_MESSAGE_CORRUPTED_FILE, encoded_message, encoded_message_lenght, WRITE_BINARY);
    write_message_to_file(NUMERIC_MESSAGE_CORRUPTED_FILE, encoded_message, encoded_message_lenght, WRITE_NUMERIC);
    printf("Corrupted encoded message:\n");
    print_message(encoded_message, encoded_message_lenght);

    free(message);
    free(encoded_message);

    printf("Success! Press any key to exit...");
    getchar();
    return 0;
}

unsigned char *read_message_from_file(const char *file_name, size_t *lenght)
{
    unsigned char *message = NULL;
    size_t buffer_size = 32;
    size_t total_bytes_read = 0;

    FILE *fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        unsigned char *chunk = malloc(buffer_size * sizeof(unsigned char));
        if (chunk == NULL)
        {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }

        size_t bytes_read = fread(chunk, sizeof(unsigned char), buffer_size, fp);
        if (bytes_read == 0)
        {
            // End of file reached
            break;
        }
        if (bytes_read < buffer_size)
        {
            chunk = realloc(chunk, bytes_read * sizeof(unsigned char));
            if (chunk == NULL)
            {
                perror("Error allocating memory");
                exit(EXIT_FAILURE);
            }
        }
        message = realloc(message, (total_bytes_read + bytes_read) * sizeof(unsigned char));
        if (message == NULL)
        {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }
        memcpy(message + total_bytes_read, chunk, bytes_read);
        total_bytes_read += bytes_read;

        buffer_size *= 2;
        free(chunk);
    }
    *lenght = total_bytes_read;

    fclose(fp);
    return message;
}

void write_message_to_file(const char *file_name, unsigned char *message, size_t lenght, const char *mode)
{
    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    if(mode == WRITE_BINARY){
        for (size_t i = 0; i < lenght; i++)
        {
            for (int j = 7; j >= 0; j--)
            {
                fputc((message[i] & (1 << j)) ? '1' : '0', file);
            }
            if (i < lenght - 1)
            {
                fputc(' ', file);
            }
        }
    } else if (mode == WRITE_TEXT){
        for (size_t i = 0; i < lenght; i++)
        {
            fputc(message[i], file);
        }
    } else if (mode == WRITE_NUMERIC){
        for (size_t i = 0; i < lenght; i++)
        {
            fprintf(file, "%d ", message[i]);
        }
    } else {
        printf("Invalid mode");
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void corrupt_message(unsigned char* encoded_message, size_t lenght) {
  for (size_t i = 0; i < lenght; i++) {
    int bit_to_swap = rand() % 8;
    encoded_message[i] ^= 1 << bit_to_swap;
  }
}

void print_message(unsigned char* message, size_t lenght) {
  for (size_t i = 0; i < lenght; i++) {
    printf("%c", message[i]);
  }
  printf("\n(%d bytes)\n\n", lenght);
}

unsigned char* encode_message(unsigned char* message, size_t message_lenght, unsigned char parity_mask[4])
{
    size_t encoded_message_lenght = message_lenght * 2;
    unsigned char* encoded_message = malloc(encoded_message_lenght * sizeof(unsigned char));

    for (size_t i = 0; i < message_lenght; i++)
    {
        unsigned char byte_1 = message[i];
        unsigned char byte_2 = 0;

        encode_byte(&byte_1, &byte_2, parity_mask);

        encoded_message[i * 2] = byte_1;
        encoded_message[i * 2 + 1] = byte_2;
    }
    return encoded_message;
}

void encode_byte(unsigned char* byte_1, unsigned char* byte_2, unsigned char parity_mask[4])
{
    unsigned char tmp_byte_1 = 0b00000000;
    unsigned char tmp_byte_2 = 0b00000000;

    tmp_byte_1 |= ((*byte_1 & 0b00000001) & 0b00001000);
    tmp_byte_1 |= ((*byte_1 & 0b00000010) & 0b00100000);
    tmp_byte_1 |= ((*byte_1 & 0b00000100) & 0b01000000);
    tmp_byte_1 |= ((*byte_1 & 0b00001000) & 0b10000000);

    tmp_byte_2 |= ((*byte_1 & 0b00010000) & 0b00001000);
    tmp_byte_2 |= ((*byte_1 & 0b00100000) & 0b00100000);
    tmp_byte_2 |= ((*byte_1 & 0b01000000) & 0b01000000);
    tmp_byte_2 |= ((*byte_1 & 0b10000000) & 0b10000000);

    tmp_byte_1 |= ((check_parity(*byte_1, 3, parity_mask)) & 0b00010000);
    tmp_byte_1 |= ((check_parity(*byte_1, 2, parity_mask)) & 0b00000100);
    tmp_byte_1 |= ((check_parity(*byte_1, 1, parity_mask)) & 0b00000010);
    tmp_byte_1 |= ((check_parity(*byte_1, 0, parity_mask)) & 0b00000001);
    
    tmp_byte_2 |= ((check_parity(*byte_1, 3, parity_mask)) & 0b00010000);
    tmp_byte_2 |= ((check_parity(*byte_1, 2, parity_mask)) & 0b00000100);
    tmp_byte_2 |= ((check_parity(*byte_1, 1, parity_mask)) & 0b00000010);
    tmp_byte_2 |= ((check_parity(*byte_1, 0, parity_mask)) & 0b00000001);

    *byte_1 = tmp_byte_1;
    *byte_2 = tmp_byte_2;
}

int check_parity(unsigned char byte, int power, unsigned char parity_mask[4])
{
    int sum = 0;
    for(unsigned int i = 0; i < 8; i++)
    {
        if((parity_mask[power] >> i) & 1)
        { 
            sum += (byte >> i) & 1;
        }
    }
    return sum % 2;
}

