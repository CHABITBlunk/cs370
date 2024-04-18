#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  FILE *file = fopen("/proc/1/stat", "r");
  if (file == NULL) {
    return EXIT_FAILURE;
  }
  char line[256];
  if (fgets(line, sizeof(line), file) == NULL) {
    fclose(file);
    return EXIT_FAILURE;
  }
  char *token = strtok(line, " ");
  for (int i = 1; i < 12; ++i) {
    token = strtok(NULL, " ");
  }
  printf("Page faults: %s\n", token);
  return EXIT_SUCCESS;
}
