#include "main.h"

/**
 * main - main point
 * @ac: argument count
 * @argv: argument vector
 *
 * Return: 0 on success and -1 on error
 */

int main(int ac, char **argv){
  char *prompt = "(kingNco) :) ";
  char *lineptr;
  size_t n = 0;

  /* declaring void variables */
  (void)ac; (void)argv;

  printf("%s", prompt);
  getline(&lineptr, &n, stdin);
  printf("%s\n", lineptr);

  free(lineptr);
  return (0);
}
