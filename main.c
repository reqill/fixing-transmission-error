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

unsigned char* read_message_from_file(const char* file_name, size_t *lenght);
void write_binary_message_to_file(const char* file_name, unsigned char* message, size_t lenght);
void write_message_to_file(const char* file_name, unsigned char* message, size_t lenght);
void corrupt_message(unsigned char* message, size_t lenght);
void generate_ldpc_matrix(unsigned int** matrix, size_t rows, size_t cols, size_t ones_per_row);
void encode_message(unsigned char* message, size_t lenght, unsigned char* codeword, unsigned int** parity_check_matrix, size_t parity_bytes);
void decode_message(unsigned char* codeword, size_t lenght, unsigned char* message, size_t message_lenght, unsigned int** parity_check_matrix, size_t parity_bytes);

int main() {
  // Seed the random number generator
  srand(time(NULL));

  // Allocate memory for the message and the codeword and its lenghts
  size_t message_lenght = 0;
  unsigned char* message = read_message_from_file(MESSAGE_INPUT_FILE, &message_lenght);
  size_t parity_bytes = 8 + message_lenght / 2;
  size_t code_word_lenght = message_lenght + parity_bytes;
  unsigned char* codeword = malloc(code_word_lenght * sizeof(unsigned char));

  // Generate the parity-check matrix for the LDPC code
  unsigned int** parity_check_matrix = malloc(parity_bytes * sizeof(unsigned int*));
  for (int i = 0; i < parity_bytes; i++) {
    parity_check_matrix[i] = malloc(message_lenght * sizeof(unsigned int));
  }
  generate_ldpc_matrix(parity_check_matrix, parity_bytes, message_lenght, 2);

  // Write the parity-check matrix to console as 2D array
  printf("\nParity-check matrix:\n");
  for (int i = 0; i < parity_bytes; i++) {
    printf("[");
    for (int j = 0; j < message_lenght; j++) {
      printf("%d", parity_check_matrix[i][j]);
      if (j < message_lenght - 1) {
        printf(", ");
      }
    }
    printf("]");
    if (i < parity_bytes - 1) {
      printf(",\n");
    }
  }
  printf("(size: %dx%d)\n", parity_bytes, message_lenght);

  // Print the message to console
  printf("\nMessage: ");
  for (int i = 0; i < message_lenght; i++) {
    printf("%hhu ", message[i]);
  }
  printf("(size: %d)\n", message_lenght);

  // Encode the message
  encode_message(message, message_lenght, codeword, parity_check_matrix, parity_bytes);

  // Print the encoded message to console
  printf("\nEncoded message: ");
  for (int i = 0; i < code_word_lenght; i++) {
    printf("%hhu ", codeword[i]);
  }
  printf("(size: %d)\n", code_word_lenght);

  // Write encoded message to the file
  write_binary_message_to_file(MESSAGE_CODED_FILE, codeword, code_word_lenght);

  // Corrupt the encoded message
  corrupt_message(codeword, code_word_lenght);

  // Print the corrupted message to console
  printf("\nCorrupted message: ");
  for (int i = 0; i < code_word_lenght; i++) {
    printf("%hhu ", codeword[i]);
  }
  printf("(size: %d)\n", code_word_lenght);

  // Write the corrupted message to the file
  write_binary_message_to_file(MESSAGE_CORRUPTED_FILE, codeword, code_word_lenght);

  // Decode the corrupted message
  decode_message(codeword, code_word_lenght, message, message_lenght, parity_check_matrix, parity_bytes);

  // Print the decoded message to console
  printf("\nDecoded message: ");
  for (int i = 0; i < message_lenght; i++) {
    printf("%hhu ", message[i]);
  }
  printf("(size: %d)\n", message_lenght);

  // Write the decoded message to the file
  write_binary_message_to_file(MESSAGE_CORRECTED_FILE, message, message_lenght);

  // Write the decoded message to the file
  write_message_to_file(MESSAGE_OUTPUT_FILE, message, message_lenght);

  // Free the memory allocated for the message
  free(message);
  free(codeword);
  free(parity_check_matrix);

  return 0;
}

unsigned char* read_message_from_file(const char* file_name, size_t* lenght) {
  // Initialize variables
  unsigned char* message = NULL;
  size_t buffer_size = 32;
  size_t total_bytes_read = 0;

  // Open the file for reading
  FILE* fp = fopen(file_name, "r");
  if (fp == NULL) {
      perror("Error opening file");
      exit(EXIT_FAILURE);
  }

  // Read the file in increments of buffer_size
  while (1) {
    // Allocate a buffer to hold the message chunk
    unsigned char* chunk = malloc(buffer_size * sizeof(unsigned char));
    if (chunk == NULL) {
      perror("Error allocating memory");
      exit(EXIT_FAILURE);
    }

    // Read a chunk from the file
    size_t bytes_read = fread(chunk, sizeof(unsigned char), buffer_size, fp);
    if (bytes_read == 0) {
      // End of file reached
      break;
    }
    if (bytes_read < buffer_size) {
      // Resize the chunk to the actual number of bytes read
      chunk = realloc(chunk, bytes_read * sizeof(unsigned char));
      if (chunk == NULL) {
          perror("Error allocating memory");
          exit(EXIT_FAILURE);
      }
    }

    // Reallocate the message buffer to make room for the new chunk
    message = realloc(message, (total_bytes_read + bytes_read) * sizeof(unsigned char));
    if (message == NULL) {
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

void write_binary_message_to_file(const char* file_name, unsigned char* message, size_t lenght) {
  // Open the file in text mode for writing
  FILE* file = fopen(file_name, "w");
  if (file == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // Write the binary representation of each element of the message array to the file
  for (size_t i = 0; i < lenght; i++) {
    for (int j = 7; j >= 0; j--) {
      fputc((message[i] & (1 << j)) ? '1' : '0', file);
    }
    if (i < lenght - 1) {
      fputc(' ', file);
    }
  }

  // Close the file
  fclose(file);
}

void write_message_to_file(const char* file_name, unsigned char* message, size_t lenght) {
  // Open the file in text mode for writing
  FILE* file = fopen(file_name, "w");
  if (file == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // Write the message to the file
  for (size_t i = 0; i < lenght; i++) {
    fputc(message[i], file);
  }

  // Close the file
  fclose(file);
}

void corrupt_message(unsigned char* message, size_t lenght) {
  // Loop through the array and swap one bit in the first byte of each element
  for (size_t i = 0; i < lenght; i++) {
    // Generate a random number between 0 and 7 (inclusive) to select the bit to swap
    int bit_to_swap = rand() % 8;

    // Toggle the selected bit using the bitwise XOR operator (^)
    message[i] ^= 1 << bit_to_swap;
  }
}

void encode_message(unsigned char* message, size_t length, unsigned char* codeword, unsigned int** parity_check_matrix, size_t parity_bytes) {
  // Copy the message into the codeword
  memcpy(codeword, message, length);

  // Initialize the parity bytes to 0
  memset(codeword + length, 0, parity_bytes);

  // Compute the parity bytes using the parity check matrix
  for (int i = 0; i < parity_bytes; i++) {
    for (int j = 0; j < length; j++) {
      codeword[length + i] ^= parity_check_matrix[i][j] & message[j];
    }
  }
}

void decode_message(unsigned char* codeword, size_t lenght, unsigned char* message, size_t message_lenght, unsigned int** parity_check_matrix, size_t parity_bytes) {
  // Initialize variables
  unsigned int* syndrome = malloc(parity_bytes * sizeof(unsigned int));
  memset(syndrome, 0, parity_bytes * sizeof(unsigned int));
  int correction_count = 0;
  int iterations = 0;
  size_t error_position = 0;

  // Iteratively decode the message
  while (correction_count > 0 && iterations == 0) {
    correction_count = 0;
    // Recalculate the syndrome
    memset(syndrome, 0, parity_bytes * sizeof(unsigned int));
    for (size_t i = 0; i < parity_bytes; i++) {
      for (size_t j = 0; j < lenght; j++) {
        syndrome[i] ^= codeword[j] & parity_check_matrix[i][j];
      }
    }

    // Correct any errors
    for (size_t i = 0; i < parity_bytes; i++) {
      if (syndrome[i] != 0) {
        correction_count++;
        error_position = 0;
        for (size_t j = 0; j < lenght; j++) {
          error_position += j * parity_check_matrix[i][j];
        }
        codeword[error_position] ^= 1;
      }
    }
    iterations++;
  }

  // Extract the message from the codeword
  for (size_t i = 0; i < message_lenght; i++) {
    message[i] = codeword[i];
  }

  // Free the allocated memory
  free(syndrome);
}

void generate_ldpc_matrix(unsigned int** matrix, size_t rows, size_t cols, size_t ones_per_row) {
  // Initialize the matrix to all zeros
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      matrix[i][j] = 0;
    }
  }

  // For each row, select a number of column indices to place 1s in
  for (int i = 0; i < rows; i++) {
    size_t ones_placed = 0;
    while (ones_placed < ones_per_row) {
      // Choose a random column index
      int j = rand() % cols;

      // Check if a 1 has already been placed at this column index
      if (matrix[i][j] == 0) {
        // Place a 1 at this column index
        matrix[i][j] = 1;
        ones_placed++;
      }
    }
  }
}