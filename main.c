#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 32

#define MESSAGE_INPUT_FILE "data/input/plik1.txt"
#define MESSAGE_CODED_FILE "data/coded/plik2.txt"
#define MESSAGE_CORRUPTED_FILE "data/corrupted/plik3.txt"
#define MESSAGE_CORRECTED_FILE "data/corrected/plik4.txt"
#define MESSAGE_OUTPUT_FILE "data/output/plik5.txt"

#define BINARY_MESSAGE_INPUT_FILE "data/input/plik1b.txt"
#define BINARY_MESSAGE_CODED_FILE "data/coded/plik2b.txt"
#define BINARY_MESSAGE_CORRUPTED_FILE "data/corrupted/plik3b.txt"
#define BINARY_MESSAGE_CORRECTED_FILE "data/corrected/plik4b.txt"
#define BINARY_MESSAGE_OUTPUT_FILE "data/output/plik5b.txt"

#define NUMERIC_MESSAGE_INPUT_FILE "data/input/plik1n.txt"
#define NUMERIC_MESSAGE_CODED_FILE "data/coded/plik2n.txt"
#define NUMERIC_MESSAGE_CORRUPTED_FILE "data/corrupted/plik3n.txt"
#define NUMERIC_MESSAGE_CORRECTED_FILE "data/corrected/plik4n.txt"
#define NUMERIC_MESSAGE_OUTPUT_FILE "data/output/plik5n.txt"

#define WRITE_BINARY "write_binary"
#define WRITE_NUMERIC "write_numeric"
#define WRITE_TEXT "write_text"

void write_message_to_file(const char *file_name, unsigned char *message, size_t lenght, const char *mode);
void corrupt_message(unsigned char *encoded_message, size_t lenght);
void write_print_results(unsigned char *message, size_t lenght, char* title, const char* text_file, const char* binary_file, const char* numeric_file);
void copy_bit(unsigned char *byte1, size_t source_index, unsigned char *byte2, size_t target_index);
void correct_bits(unsigned char* transmitted_message, size_t transmitted_message_lenght);
unsigned char* read_message_from_file(const char *file_name, size_t *lenght);
unsigned char* encode_message(unsigned char* message, size_t message_lenght);
unsigned char* decode_message(unsigned char* encoded_message, size_t encoded_message_length);

int main()
{
    // Seed the random number generator for the random errors
    srand(time(NULL));

    // Read the message from the file
    size_t message_lenght;
    unsigned char *message = read_message_from_file(MESSAGE_INPUT_FILE, &message_lenght);
    write_print_results(message, message_lenght, "Message read from file", NULL, BINARY_MESSAGE_INPUT_FILE, NUMERIC_MESSAGE_INPUT_FILE);

    // Encode the message
    size_t encoded_message_lenght = message_lenght * 2;
    unsigned char *encoded_message = encode_message(message, message_lenght);
    write_print_results(encoded_message, encoded_message_lenght, "Encoded binary message", MESSAGE_CODED_FILE, BINARY_MESSAGE_CODED_FILE, NUMERIC_MESSAGE_CODED_FILE);

    // Corrupt the encoded message
    corrupt_message(encoded_message, encoded_message_lenght);
    write_print_results(encoded_message, encoded_message_lenght, "Corrupted encoded message", MESSAGE_CORRUPTED_FILE, BINARY_MESSAGE_CORRUPTED_FILE, NUMERIC_MESSAGE_CORRUPTED_FILE);

    // Correct transmission errors
    size_t transmitted_message_lenght = encoded_message_lenght;
    unsigned char* transmitted_message = encoded_message;

    correct_bits(transmitted_message, transmitted_message_lenght);
    write_print_results(transmitted_message, transmitted_message_lenght, "Corrected message", MESSAGE_CORRECTED_FILE, BINARY_MESSAGE_CORRECTED_FILE, NUMERIC_MESSAGE_CORRECTED_FILE);

    // Decode the message
    size_t decoded_message_lenght = transmitted_message_lenght / 2;
    unsigned char* decoded_message = decode_message(transmitted_message, transmitted_message_lenght);
    write_print_results(decoded_message, decoded_message_lenght, "Decoded message", MESSAGE_OUTPUT_FILE, BINARY_MESSAGE_OUTPUT_FILE, NUMERIC_MESSAGE_OUTPUT_FILE);

    // Free the memory
    free(message);
    free(encoded_message);
    free(transmitted_message);
    free(decoded_message);

    printf("Success!\n");
    return 0;
}

// Read the message from the file using buffer for memory allocation
unsigned char *read_message_from_file(const char *file_name, size_t *lenght)
{
    unsigned char *message = NULL;
    size_t buffer_size = BUFFER_SIZE;
    size_t total_bytes_read = 0;

    FILE *file = fopen(file_name, "r");
    if (file == NULL)
    {
        printf("Error opening file");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        unsigned char *chunk = malloc(buffer_size * sizeof(unsigned char));
        if (chunk == NULL)
        {
            printf("Error allocating memory");
            exit(EXIT_FAILURE);
        }

        // Read a chunk from the file
        size_t bytes_read = fread(chunk, sizeof(unsigned char), buffer_size, file);
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
                printf("Error allocating memory");
                exit(EXIT_FAILURE);
            }
        }

        // Reallocate the message buffer to make room for the new chunk
        message = realloc(message, (total_bytes_read + bytes_read) * sizeof(unsigned char));
        if (message == NULL)
        {
            printf("Error allocating memory");
            exit(EXIT_FAILURE);
        }

        // Append the chunk to the message buffer
        memcpy(message + total_bytes_read, chunk, bytes_read);
        total_bytes_read += bytes_read;

        // Increase the buffer size for the next iteration
        buffer_size *= 2;
        free(chunk);
    }
    *lenght = total_bytes_read;
    fclose(file);

    return message;
}

// Write the message to the file in binary, numeric or text format
void write_message_to_file(const char *file_name, unsigned char *message, size_t lenght, const char *mode)
{
    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        printf("Error opening file");
        exit(EXIT_FAILURE);
    }

    if(mode == WRITE_BINARY){
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
    } else if (mode == WRITE_TEXT){
        // Write the message to the file
        for (size_t i = 0; i < lenght; i++)
        {
            fputc(message[i], file);
        }
    } else if (mode == WRITE_NUMERIC){
        // Write the message to the file
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

// Correct one random bit in every byte of the message
void corrupt_message(unsigned char* encoded_message, size_t lenght) {
  size_t bit_to_swap;

  for (size_t i = 0; i < lenght; i++) {
    bit_to_swap = rand() % 8;
    encoded_message[i] ^= 1 << bit_to_swap;
  }
}

// Write the message to the console and to the specified files
void write_print_results(unsigned char* message, size_t lenght, char* title, const char* text_file, const char* binary_file, const char* numeric_file) {
    if(text_file != NULL)
        write_message_to_file(text_file, message, lenght, WRITE_TEXT);
    if(binary_file != NULL)
        write_message_to_file(binary_file, message, lenght, WRITE_BINARY);
    if(numeric_file != NULL)
        write_message_to_file(numeric_file, message, lenght, WRITE_NUMERIC);
    
    printf("=== %s ===\n", title);
    for (size_t i = 0; i < lenght; i++) {
        printf("%c", message[i]);
    }
    printf("\n(%d bytes)\n\n", lenght);
}

// Copy a bit from one byte to another
void copy_bit(unsigned char *byte_1, size_t source_index, unsigned char *byte_2, size_t target_index) {
    unsigned char bit = (*byte_1 >> source_index) & 1;
    bit = bit << target_index;

    // Mask off the target position in byte_2 with a bitmask that has a 0 in the target position and 1s everywhere else
    unsigned char mask = ~(1 << target_index);
    *byte_2 &= mask;

    // OR the two bytes together to copy the bit from byte_1 to byte_2
    *byte_2 |= bit;
}

// Encode a message using the Hamming(7,4) code where each byte is encoded as two subsequent bytes
unsigned char* encode_message(unsigned char* message, size_t message_lenght)
{
    size_t encoded_message_lenght = message_lenght * 2;
    unsigned char p1, p2, p3, message_byte, byte = 0;
    unsigned char* encoded_message = malloc(encoded_message_lenght * sizeof(unsigned char));
    if(encoded_message == NULL){
        printf("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    for(size_t i = 0; i < encoded_message_lenght; i++){
        message_byte = message[i / 2];
        byte = 0b00000000;

        if(i % 2 == 0){
            // First half of the byte for even indexes
            copy_bit(&message_byte, 0, &byte, 3);
            copy_bit(&message_byte, 1, &byte, 5);
            copy_bit(&message_byte, 2, &byte, 6);
            copy_bit(&message_byte, 3, &byte, 7);
        } else {
            // Second half of the byte for odd indexes 
            copy_bit(&message_byte, 4, &byte, 3);
            copy_bit(&message_byte, 5, &byte, 5);
            copy_bit(&message_byte, 6, &byte, 6);
            copy_bit(&message_byte, 7, &byte, 7);
        }
        
        // Calculate the parity bits
        p1 = ((byte >> 3) & 1) ^ ((byte >> 5) & 1) ^ ((byte >> 7) & 1);
        p2 = ((byte >> 3) & 1) ^ ((byte >> 6) & 1) ^ ((byte >> 7) & 1);
        p3 = ((byte >> 5) & 1) ^ ((byte >> 6) & 1) ^ ((byte >> 7) & 1);

        // Add the parity bits
        byte |= p1 << 1;
        byte |= p2 << 2;
        byte |= p3 << 4;

        encoded_message[i] = byte;
    }

    return encoded_message;
}

// Corrects the errors in the transmitted message using the Hamming (7,4) code
void correct_bits(unsigned char* transmitted_message, size_t transmitted_message_lenght) {
  unsigned char message_byte, c1, c2, c3, error_index;

  for (size_t i = 0; i < transmitted_message_lenght; i++) {
    message_byte = transmitted_message[i]; 
    error_index = 0b00000000;

    // Calculate the check bits
    c1 = (((message_byte >> 1) & 1) ^ (message_byte >> 3) & 1) ^ ((message_byte >> 5) & 1) ^ ((message_byte >> 7) & 1);
    c2 = (((message_byte >> 2) & 1) ^ (message_byte >> 3) & 1) ^ ((message_byte >> 6) & 1) ^ ((message_byte >> 7) & 1);
    c3 = (((message_byte >> 4) & 1) ^ (message_byte >> 5) & 1) ^ ((message_byte >> 6) & 1) ^ ((message_byte >> 7) & 1);

    // Calculate the error index and flip corrupted bit
    error_index = c3 << 2 | c2 << 1 | c1;
    message_byte ^= 1 << (error_index);

    transmitted_message[i] = message_byte;
  }
}

// Decode the message by removing the parity bits and joining the two halves of each byte from two successive in the encoded message
unsigned char* decode_message(unsigned char* encoded_message, size_t encoded_message_length)
{
    size_t decoded_message_length = encoded_message_length / 2;
    unsigned char message_byte, byte = 0;
    unsigned char* decoded_message = malloc(decoded_message_length * sizeof(unsigned char));
    if(decoded_message == NULL){
        printf("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    for(size_t i = 0; i < decoded_message_length; i++){
        byte = 0b00000000;

        // First half of the byte for even indexes
        message_byte = encoded_message[i * 2];
        copy_bit(&message_byte, 3, &byte, 0);
        copy_bit(&message_byte, 5, &byte, 1);
        copy_bit(&message_byte, 6, &byte, 2);
        copy_bit(&message_byte, 7, &byte, 3);

        // Second half of the byte for odd indexes 
        message_byte = encoded_message[i * 2 + 1];
        copy_bit(&message_byte, 3, &byte, 4);
        copy_bit(&message_byte, 5, &byte, 5);
        copy_bit(&message_byte, 6, &byte, 6);
        copy_bit(&message_byte, 7, &byte, 7);

        decoded_message[i] = byte;
    }

    return decoded_message;
}