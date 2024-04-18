#include <stdio.h>
#include <stdlib.h>

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
  printf("%s", line);
  return EXIT_SUCCESS;
}
