#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 32

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
void corrupt_message(unsigned char *encoded_message, size_t lenght);
void print_message(unsigned char *message, size_t lenght);
void copy_bit(unsigned char *byte1, unsigned int source_index, unsigned char *byte2, unsigned int target_index);
unsigned char* encode_message(unsigned char* message, size_t message_lenght);
void correct_bits(unsigned char* transmitted_message, size_t transmitted_message_lenght);
unsigned char* decode_message(unsigned char* encoded_message, size_t encoded_message_length);

int main()
{
    // Seed the random number generator for the random errors
    srand(time(NULL));

    // Read the message from the file
    size_t message_lenght;
    unsigned char *message = read_message_from_file(MESSAGE_INPUT_FILE, &message_lenght);

    // Write message in different formats
    write_message_to_file(BINARY_MESSAGE_INPUT_FILE, message, message_lenght, WRITE_BINARY);
    write_message_to_file(NUMERIC_MESSAGE_INPUT_FILE, message, message_lenght, WRITE_NUMERIC);
    printf("Message read from file in binary:\n");
    print_message(message, message_lenght);

    // Encode the message
    size_t encoded_message_lenght = message_lenght * 2;
    unsigned char *encoded_message = encode_message(message, message_lenght);

    // Write encoded message in different formats
    write_message_to_file(MESSAGE_CODED_FILE, encoded_message, encoded_message_lenght, WRITE_TEXT);
    write_message_to_file(BINARY_MESSAGE_CODED_FILE, encoded_message, encoded_message_lenght, WRITE_BINARY);
    write_message_to_file(NUMERIC_MESSAGE_CODED_FILE, encoded_message, encoded_message_lenght, WRITE_NUMERIC);
    printf("Encoded binary message:\n");
    print_message(encoded_message, encoded_message_lenght);

    // Corrupt the encoded message
    corrupt_message(encoded_message, encoded_message_lenght);

    // Write corrupted encoded message in different formats
    write_message_to_file(MESSAGE_CORRUPTED_FILE, encoded_message, encoded_message_lenght, WRITE_TEXT);
    write_message_to_file(BINARY_MESSAGE_CORRUPTED_FILE, encoded_message, encoded_message_lenght, WRITE_BINARY);
    write_message_to_file(NUMERIC_MESSAGE_CORRUPTED_FILE, encoded_message, encoded_message_lenght, WRITE_NUMERIC);
    printf("Corrupted encoded message:\n");
    print_message(encoded_message, encoded_message_lenght);

    // Read transmitted message from the file
    size_t transmitted_message_lenght = encoded_message_lenght;
    unsigned char* transmitted_message = encoded_message;

    // Correct transmission errors
    correct_bits(transmitted_message, transmitted_message_lenght);

    // Write decoded message in different formats
    write_message_to_file(MESSAGE_CORRECTED_FILE, transmitted_message, transmitted_message_lenght, WRITE_TEXT);
    write_message_to_file(BINARY_MESSAGE_CORRECTED_FILE, transmitted_message, transmitted_message_lenght, WRITE_BINARY);
    write_message_to_file(NUMERIC_MESSAGE_CORRECTED_FILE, transmitted_message, transmitted_message_lenght, WRITE_NUMERIC);
    printf("Corrected message:\n");
    print_message(transmitted_message, transmitted_message_lenght);

    // Decode the message
    size_t decoded_message_lenght = transmitted_message_lenght / 2;
    unsigned char* decoded_message = decode_message(transmitted_message, transmitted_message_lenght);

    // Write decoded message in different formats
    write_message_to_file(MESSAGE_OUTPUT_FILE, decoded_message, decoded_message_lenght, WRITE_TEXT);
    write_message_to_file(BINARY_MESSAGE_OUTPUT_FILE, decoded_message, decoded_message_lenght, WRITE_BINARY);
    write_message_to_file(NUMERIC_MESSAGE_OUTPUT_FILE, decoded_message, decoded_message_lenght, WRITE_NUMERIC);
    printf("Decoded message:\n");
    print_message(decoded_message, decoded_message_lenght);

    // Free the memory
    free(message);
    free(encoded_message);
    free(transmitted_message);
    free(decoded_message);

    printf("Success!\n");
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

    // Close the file
    fclose(file);
}

void corrupt_message(unsigned char* encoded_message, size_t lenght) {
  unsigned int bit_to_swap;

  for (size_t i = 0; i < lenght; i++) {
    // Generate a random number between 0 and 7 (inclusive) to select the bit to swap
    bit_to_swap = rand() % 8;

    // Toggle the selected bit using the bitwise XOR operator (^)
    encoded_message[i] ^= 1 << bit_to_swap;
  }
}

void print_message(unsigned char* message, size_t lenght) {
  // Loop through the array and print each element
  for (size_t i = 0; i < lenght; i++) {
    printf("%c", message[i]);
  }
  printf("\n(%d bytes)\n\n", lenght);
}

void copy_bit(unsigned char *byte_1, unsigned int source_index, unsigned char *byte_2, unsigned int target_index) {
    // Shift the bit in byte_1 to the right, so that it is in the least significant position
    unsigned char bit = (*byte_1 >> source_index) & 1;

    // Shift the bit to the target position in byte_2
    bit = bit << target_index;

    // Mask off the target position in byte_2 with a bitmask that has a 0 in the target position
    // and 1s everywhere else
    unsigned char mask = ~(1 << target_index);
    *byte_2 &= mask;

    // OR the two bytes together to copy the bit from byte_1 to byte_2
    *byte_2 |= bit;
}

unsigned char* encode_message(unsigned char* message, size_t message_lenght)
{
    printf("Encoding message: ");
    print_message(message, message_lenght);

    size_t encoded_message_lenght = message_lenght * 2;
    unsigned char* encoded_message = malloc(encoded_message_lenght * sizeof(unsigned char));
    unsigned char p1, p2, p3, message_byte, byte = 0;

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

void correct_bits(unsigned char* transmitted_message, size_t transmitted_message_lenght) {
  unsigned char message_byte, c1, c2, c3, error_index;

  for (size_t i = 0; i < transmitted_message_lenght; i++) {
    message_byte = transmitted_message[i]; 
    error_index = 0b00000000;

    // Calculate the check bits
    c1 = (((message_byte >> 1) & 1) ^ (message_byte >> 3) & 1) ^ ((message_byte >> 5) & 1) ^ ((message_byte >> 7) & 1);
    c2 = (((message_byte >> 2) & 1) ^ (message_byte >> 3) & 1) ^ ((message_byte >> 6) & 1) ^ ((message_byte >> 7) & 1);
    c3 = (((message_byte >> 4) & 1) ^ (message_byte >> 5) & 1) ^ ((message_byte >> 6) & 1) ^ ((message_byte >> 7) & 1);

    // Calculate the error index
    error_index = c3 << 2 | c2 << 1 | c1;

    // If the error index is not 0, then an error has occurred
    if (error_index != 0) {
      // Correct the error by flipping the bit at the corresponding position in the message byte
      message_byte ^= 1 << (error_index);
    } else {
      message_byte ^= 1 << 0;
    }

    // Update the transmitted message with the corrected message byte
    transmitted_message[i] = message_byte;
  }
}

unsigned char* decode_message(unsigned char* encoded_message, size_t encoded_message_length)
{
    size_t decoded_message_length = encoded_message_length / 2;
    unsigned char* decoded_message = malloc(decoded_message_length * sizeof(unsigned char));
    unsigned char message_byte, byte = 0;

    for(size_t i = 0; i < decoded_message_length; i++){
        message_byte = encoded_message[i * 2];
        byte = 0b00000000;

        // First half of the byte for even indexes
        copy_bit(&message_byte, 3, &byte, 0);
        copy_bit(&message_byte, 5, &byte, 1);
        copy_bit(&message_byte, 6, &byte, 2);
        copy_bit(&message_byte, 7, &byte, 3);

        message_byte = encoded_message[i * 2 + 1];
        // Second half of the byte for odd indexes 
        copy_bit(&message_byte, 3, &byte, 4);
        copy_bit(&message_byte, 5, &byte, 5);
        copy_bit(&message_byte, 6, &byte, 6);
        copy_bit(&message_byte, 7, &byte, 7);

        decoded_message[i] = byte;
    }

    return decoded_message;
}