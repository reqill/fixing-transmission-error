#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MESSAGE_INPUT_FILE "plik1.txt"
#define MESSAGE_CODED_FILE "plik2.txt"
#define MESSAGE_CORRUPTED_FILE "plik3.txt"
#define MESSAGE_CORRECTED_FILE "plik4.txt"
#define MESSAGE_OUTPUT_FILE "plik5.txt"

#define MESSAGE_LENGTH 12
#define PARITY_BITS 32
#define CODE_WORD_LENGTH (MESSAGE_LENGTH + PARITY_BITS)

unsigned char* read_message_from_file(const char* file_name);
void write_binary_message_to_file(const char* file_name, unsigned char* message, size_t length);
void write_message_to_file(const char* file_name, unsigned char* message, size_t length);
void corrupt_message(unsigned char* message, size_t length);
void encode_message(unsigned char* message, size_t length, unsigned char* codeword, unsigned int parity_check_matrix[PARITY_BITS][CODE_WORD_LENGTH]);
void decode_message(unsigned char* codeword, size_t length, unsigned char* message, unsigned int parity_check_matrix[PARITY_BITS][CODE_WORD_LENGTH]);

int main() {
  // Seed the random number generator
  srand(time(NULL));

  // Allocate memory for the message and the codeword
  unsigned char* message = read_message_from_file(MESSAGE_INPUT_FILE);
  unsigned char* codeword = malloc(CODE_WORD_LENGTH * sizeof(unsigned char));

  // Generate the parity-check matrix for the LDPC code
  unsigned int parity_check_matrix[PARITY_BITS][MESSAGE_LENGTH];
  for (int i = 0; i < PARITY_BITS; i++) {
    for (int j = 0; j < MESSAGE_LENGTH; j++) {
      parity_check_matrix[i][j] = (i * MESSAGE_LENGTH + j) % 2; // Example parity-check matrix
    }
  }

  // Encode the message
  encode_message(message, MESSAGE_LENGTH, codeword, parity_check_matrix);

  // Write encoded message to the file
  write_binary_message_to_file(MESSAGE_CODED_FILE, codeword, CODE_WORD_LENGTH);

  // Corrupt the encoded message
  corrupt_message(codeword, CODE_WORD_LENGTH);

  // Write the corrupted message to the file
  write_binary_message_to_file(MESSAGE_CORRUPTED_FILE, codeword, CODE_WORD_LENGTH);

  // Decode the corrupted message
  decode_message(codeword, CODE_WORD_LENGTH, message, parity_check_matrix);

  // Write the decoded message to the file
  write_binary_message_to_file(MESSAGE_CORRECTED_FILE, message, MESSAGE_LENGTH);

  // Write the decoded message to the file
  write_message_to_file(MESSAGE_OUTPUT_FILE, message, MESSAGE_LENGTH);

  // Free the memory allocated for the message
  free(message);
  free(codeword);

  return 0;
}

unsigned char* read_message_from_file(const char* file_name) {
  // Open the file for reading
  FILE* fp = fopen(file_name, "r");
  if (fp == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // Allocate a buffer to hold the message
  unsigned char* message = malloc(MESSAGE_LENGTH * sizeof(unsigned char));
  if (message == NULL) {
    perror("Error allocating memory");
    exit(EXIT_FAILURE);
  }

  // Read the message from the file all at once
  size_t bytes_read = fread(message, sizeof(unsigned char), MESSAGE_LENGTH, fp);
  if (bytes_read < MESSAGE_LENGTH) {
    fprintf(stderr, "Error reading message from file\n");
    exit(EXIT_FAILURE);
  }

  // Close the file
  fclose(fp);

  return message;
}

void write_binary_message_to_file(const char* file_name, unsigned char* message, size_t length) {
  // Open the file in text mode for writing
  FILE* file = fopen(file_name, "w");
  if (file == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // Write the binary representation of each element of the message array to the file
  for (size_t i = 0; i < length; i++) {
    for (int j = 7; j >= 0; j--) {
      fputc((message[i] & (1 << j)) ? '1' : '0', file);
    }
    if (i < length - 1) {
      fputc(' ', file);
    }
  }

  // Close the file
  fclose(file);
}

void write_message_to_file(const char* file_name, unsigned char* message, size_t length) {
  // Open the file in text mode for writing
  FILE* file = fopen(file_name, "w");
  if (file == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // Write the message to the file
  for (size_t i = 0; i < length; i++) {
    fputc(message[i], file);
  }

  // Close the file
  fclose(file);
}

void corrupt_message(unsigned char* message, size_t length) {
  // Loop through the array and swap one bit in the first byte of each element
  for (size_t i = 0; i < length; i++) {
    // Generate a random number between 0 and 7 (inclusive) to select the bit to swap
    int bit_to_swap = rand() % 8;

    // Toggle the selected bit using the bitwise XOR operator (^)
    message[i] ^= 1 << bit_to_swap;
  }
}

void encode_message(unsigned char* message, size_t length, unsigned char* codeword, unsigned int parity_check_matrix[PARITY_BITS][CODE_WORD_LENGTH]) {
  // Copy the message into the codeword
  for (int i = 0; i < length; i++) {
    codeword[i] = message[i];
  }

  // Calculate the parity bits using the parity-check matrix
  for (int i = 0; i < PARITY_BITS; i++) {
    int parity_bit = 0;
    for (int j = 0; j < length; j++) {
      parity_bit ^= message[j] & parity_check_matrix[i][j];
    }
    codeword[length + i] = parity_bit;
  }
}

void decode_message(unsigned char* codeword, size_t length, unsigned char* message, unsigned int parity_check_matrix[PARITY_BITS][CODE_WORD_LENGTH]) {
  // Initialize an array to store the syndrome of the codeword
  unsigned int syndrome[PARITY_BITS] = {0};

  // Calculate the syndrome of the codeword
  for (int i = 0; i < PARITY_BITS; i++) {
    for (int j = 0; j < length; j++) {
      syndrome[i] ^= parity_check_matrix[i][j] & codeword[j];
    }
  }

  // Check if the syndrome is all zeros, indicating that the codeword is not corrupted
  int is_corrupted = 0;
  for (int i = 0; i < PARITY_BITS; i++) {
    if (syndrome[i] != 0) {
      is_corrupted = 1;
      break;
    }
  }

  if (is_corrupted) {
    // Find the position of the corrupted bit by calculating the dot product of the syndrome and the rows of the parity-check matrix
    int corrupted_bit_position = -1;
    for (int i = 0; i < length; i++) {
      int dot_product = 0;
      for (int j = 0; j < PARITY_BITS; j++) {
        dot_product += syndrome[j] * parity_check_matrix[j][i];
      }
      if (dot_product % 2 != 0) {
        corrupted_bit_position = i;
        break;
      }
    }

    // Flip the corrupted bit
    codeword[corrupted_bit_position] ^= 1;
  }

  // Strip the parity bits from the codeword to obtain the original message
  for (int i = 0; i < MESSAGE_LENGTH; i++) {
    message[i] = codeword[i];
  }
}
