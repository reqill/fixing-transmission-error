#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 256
#define MESSAGE_INPUT_FILE "plik1.txt"
#define MESSAGE_CODED_FILE "plik2.txt"
#define MESSAGE_CORRUPTED_FILE "plik3.txt"
#define MESSAGE_CORRECTED_FILE "plik4.txt"
#define MESSAGE_OUTPUT_FILE "plik5.txt"

int* read_message_from_file(const char* file_name, size_t* length) {
  int buffer_size = BUFFER_SIZE;

  // Open the file for reading
  FILE* fp = fopen(file_name, "r");
  if (fp == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // Allocate a buffer to hold the message
  int* message = malloc(buffer_size * sizeof(int));
  if (message == NULL) {
    perror("Error allocating memory");
    exit(EXIT_FAILURE);
  }

  // Read the message from the file one character at a time
  size_t index = 0;
  int c;
  while ((c = fgetc(fp)) != EOF) {
    // Resize the buffer if necessary
    if (index == buffer_size) {
      buffer_size *= 2;
      message = realloc(message, buffer_size * sizeof(int));
      if (message == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
      }
    }

    // Add the character to the message
    message[index++] = c;
  }

  // Set the length of the message
  *length = index;

  // Close the file
  fclose(fp);

  // Return the message
  return message;
}

int main() {
  // Read the message from the file
  size_t length;
  unsigned int* message = read_message_from_file(MESSAGE_INPUT_FILE, &length);

  // Print the message
  for (size_t i = 0; i < length; i++) {
    putchar(message[i]);
  }
  printf("\n");

  // Free the memory allocated for the message
  free(message);

  printf("\nProgram finished successfully. Press any key to exit.\n");
  getchar();
  return 0;
}