/* The main module. The 'hub'. */
/* Includes things for saving/loading weights and their assorted learning decays and such. */
/* Note to self: Compile with gcc -o chessAI main.c chess.c neuralnetwork.c training.c AIbrain.c AIvsHuman.c -lmeschach -lm -Wall */

#include "main.h"


int main() {
  char choice[1];
  int choiceint;
  srand(time(NULL)); /* initialises random time for matrices */
  smrand(time(NULL)); /* initialises random time for matrices */
  /* playchess(); */

  printf("Welcome to a Chess AI.\n\n");

  printf("We hope you enjoy your stay.\n\n");

  printf("Choose one of the options below by typing in the number corresponding to the option:\n");

  printf("1. Train a chess AI against itself from scratch.\n");
  printf("2. Train a chess AI against itself from saved weights.\n");
  printf("3. Train a chess AI from a list of games from scratch.\n");
  printf("4. Train a chess AI from a list of games from saved weights.\n");
  printf("5. Judge the performance of a chess AI from saved weights.\n");
  printf("6. Play against a chess AI (automatically loaded from weights).\n");
  printf("7. Play against another human. (Two-player; No AI)\n");

  getinputofsize(choice, 1);
  
  switch (choice[0]) {
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
    choiceint = choice[0] - '0';
    trainAI(choiceint);
    break;
  case '7':
    playchess();
    break;
  default:
    printf("Error: choice not valid.\n");
  }

  return 0;
}


void getinputofsize(char input[], int size) {
  /* Gets an input of some size. Warning: Does not put \0 at end of string. */
  int i;
  char testinput[size+1];
  char testchar = ' ';
 
  for (i = 0; i <= size; i++) {
    scanf("%c", &testinput[i]);
    if (i < size && testinput[i] == '\n') {
      for (i = i; i <= size; i++) {
	testinput[i] = ' ';
      }
      break;
    }
    else if (i == size && testinput[i] == '\n') {
      break;
    }
    else if (i == size && testinput[size] != '\n') {
      while (testchar != '\n') {
	scanf("%c", &testchar);
      }
    }
  }
 
  strncpy(input, testinput, size);
}
