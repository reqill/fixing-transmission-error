#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 32
#define BLOCK_SIZE 4

#define MESSAGE_INPUT_FILE "plik1.txt"
#define MESSAGE_CODED_FILE "plik2.txt"
#define MESSAGE_CORRUPTED_FILE "plik3.txt"
#define MESSAGE_CORRECTED_FILE "plik4.txt"
#define MESSAGE_OUTPUT_FILE "plik5.txt"

#define BINARY_MESSAGE_INPUT_FILE "plik1b.txt"
#define BINARY_MESSAGE_CODED_FILE "plik2b.txt"
#define BINARY_MESSAGE_CORRUPTED_FILE "plik3b.txt"
#define BINARY_MESSAGE_CORRECTED_FILE "plik4b.txt"
#define BINARY_MESSAGE_OUTPUT_FILE "plik5b.txt"

#define NUMERIC_MESSAGE_INPUT_FILE "plik1n.txt"
#define NUMERIC_MESSAGE_CODED_FILE "plik2n.txt"
#define NUMERIC_MESSAGE_CORRUPTED_FILE "plik3n.txt"
#define NUMERIC_MESSAGE_CORRECTED_FILE "plik4n.txt"
#define NUMERIC_MESSAGE_OUTPUT_FILE "plik5n.txt"

#define WRITE_BINARY "write_binary"
#define WRITE_NUMERIC "write_numeric"
#define WRITE_TEXT "write_text"

unsigned char* read_message_from_file(const char *file_name, size_t *lenght);
void write_message_to_file(const char *file_name, unsigned char *message, size_t lenght, const char *mode);
void corrupt_message(unsigned char *message, size_t lenght);
unsigned char* encode_message(unsigned char *message, size_t message_lenght size_t encoded_message_lenght);
unsigned char* decode_message(unsigned char *encoded_message, size_t encoded_message_lenght);

int main()
{
    // Seed the random number generator for the random errors
    srand(time(NULL));

    // Read the message from the file
    size_t message_lenght;
    unsigned char *message = read_message_from_file(MESSAGE_INPUT_FILE, &message_lenght);

    // Write message to files in different formats
    write_message_to_file(BINARY_MESSAGE_INPUT_FILE, message, message_lenght, WRITE_BINARY);
    write_message_to_file(NUMERIC_MESSAGE_INPUT_FILE, message, message_lenght, WRITE_NUMERIC);

    // Encode the message
    size_t encoded_message_lenght = message_lenght * BLOCK_SIZE;
    unsigned char *coded_message = encode_message(message, message_lenght, encoded_message_lenght);

    return 0;
}

unsigned char *read_message_from_file(const char *file_name, size_t *lenght)
{
    // Initialize variables
    unsigned char *message = NULL;
    size_t buffer_size = 32;
    size_t total_bytes_read = 0;

    // Open the file for reading
    FILE *fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read the file in increments of buffer_size
    while (1)
    {
        // Allocate a buffer to hold the message chunk
        unsigned char *chunk = malloc(buffer_size * sizeof(unsigned char));
        if (chunk == NULL)
        {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }

        // Read a chunk from the file
        size_t bytes_read = fread(chunk, sizeof(unsigned char), buffer_size, fp);
        if (bytes_read == 0)
        {
            // End of file reached
            break;
        }
        if (bytes_read < buffer_size)
        {
            // Resize the chunk to the actual number of bytes read
            chunk = realloc(chunk, bytes_read * sizeof(unsigned char));
            if (chunk == NULL)
            {
                perror("Error allocating memory");
                exit(EXIT_FAILURE);
            }
        }

        // Reallocate the message buffer to make room for the new chunk
        message = realloc(message, (total_bytes_read + bytes_read) * sizeof(unsigned char));
        if (message == NULL)
        {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }

        // Append the chunk to the message buffer
        memcpy(message + total_bytes_read, chunk, bytes_read);
        total_bytes_read += bytes_read;

        // Increase the buffer size for the next iteration
        buffer_size *= 2;

        // Free the chunk
        free(chunk);
    }

    // Set the lenght of the message
    *lenght = total_bytes_read;

    // Close the file
    fclose(fp);

    return message;
}

void write_message_to_file(const char *file_name, unsigned char *message, size_t lenght, const char *mode)
{
    // Open the file in text mode for writing
    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    switch (mode)
    {
    case WRITE_BINARY:
        // Write binary representation of the message to the file
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
        break;
    case WRITE_TEXT:
        // Write the message to the file
        for (size_t i = 0; i < lenght; i++)
        {
            fputc(message[i], file);
        }
        break;
    case WRITE_NUMERIC:
        // Write the message to the file
        for (size_t i = 0; i < lenght; i++)
        {
            fprintf(file, "%d ", message[i]);
        }
        break;
    default:
        printf("Invalid mode");
        exit(EXIT_FAILURE);
    }

    // Close the file
    fclose(file);
}

unsigned char *encode_message(unsigned char *message, size_t lenght, size_t encoded_message_lenght)
{
    unsigned char *encoded_message = malloc(encoded_message_lenght * sizeof(unsigned char));

    return encoded_message;
}

unsigned char *decode_message(unsigned char *encoded_message, size_t encoded_message_lenght)
{
    unsigned char *decoded_message = malloc(encoded_message_lenght * sizeof(unsigned char));

    return decoded_message;
}
