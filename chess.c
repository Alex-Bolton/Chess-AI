#include "main.h"

/*
Usual chess board:

  | A | B | C | D | E | F | G | H | 
-----------------------------------
8 |   |   |   |   |   |   |   |   |
-----------------------------------
7 |   |   |   |   |   |   |   |   |
-----------------------------------
6 |   |   |   |   |   |   |   |   |
-----------------------------------
5 |   |   |   |   |   |   |   |   |
-----------------------------------
4 |   |   |   |   |   |   |   |   |
-----------------------------------
3 |   |   |   |   |   |   |   |   |
-----------------------------------
2 |   |   |   |   |   |   |   |   |
-----------------------------------
1 |   |   |   |   |   |   |   |   |
-----------------------------------

to be represented in array as:

 x| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 
y----------------------------------
7 |   |   |   |   |   |   |   |   |
-----------------------------------
6 |   |   |   |   |   |   |   |   |
-----------------------------------
5 |   |   |   |   |   |   |   |   |
-----------------------------------
4 |   |   |   |   |   |   |   |   |
-----------------------------------
3 |   |   |   |   |   |   |   |   |
-----------------------------------
2 |   |   |   |   |   |   |   |   |
-----------------------------------
1 |   |   |   |   |   |   |   |   |
-----------------------------------
0 |   |   |   |   |   |   |   |   |
-----------------------------------

Black pieces will be represented by lower case letters; white pieces as capital letters. This how the FEN codes do it.
We are using FEN notation to store states, since it guarantees we have all the information - no point in reinventing the wheel - and also it may be useful for future extensions to the program.

 */




/* int main() { */
/*   char Board[8][8]; */
/*   char *dummy; */

/*   dummy = (char *)malloc(3 * sizeof(char)); */
  
/*   setupBoard(Board, dummy, dummy, dummy); */
/*   /\* displayBoard(Board); *\/ */
/*   /\* convertBoardtoFEN(Board, 'w', "KQ", "kq", "- ", 0, 0); *\/ */
/*   /\* convertFENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkQ - 0 0", Board, whitecastle, blackcastle, enpassant); *\/ */
/*   /\* displayBoard(Board); *\/ */
  
/*   playchess(); */
/*   free(dummy); */
/*   return 0; */
/* } */


void playchess() { /* Function to play Chess (two player; no AI; currently used for testing) */
  struct stateInfo state;
  char startcell[2], endcell[2]; /* The starting and ending positions for a single move. */
  int gameover;
  char promotion;
  /* int i, j; */
  /* int possiblemoves[64][64]; */

  gameover = 0;

  printf("Welcome to fucking chess.\n"); /* Edgy. */
  state = setupBoard();
  while (gameover == 0) { /* Continue until the game is over */
    displayBoard(state.Board);
    printf("Whitecastle: %.2s, Blackcastle: %.2s, enpassant: %.2s\n", state.whitecastle, state.blackcastle, state.enpassant);

    displayPrompt(state);
    getmove(state, startcell, endcell, &promotion); /* Get move from player; checks if move is valid. */
    state = makemovewithPawnPromote(state, startcell, endcell, promotion);

    gameover = isgameover(state); /* Check for gameover */
  }
  displayBoard(state.Board);
  printf("Game over!"); /* Wrap up the game and tell them who won. */
  if (state.player == 'w') {
    printf(" Black won.\n"); /* Because the game ends on the loser. */
  }
  else {
    printf(" White won\n");
  }

}





struct stateInfo setupBoard() { /* Set up the initial board. */
  struct stateInfo state;
  int x, y;
  char piece;

  for (x = 0; x < 8; x++) { /* Initially set it all as empty. */
    for (y = 0; y < 8; y++) {
      state.Board[x][y] = ' ';
    }
  }

  for (x = 0; x < 8; x++) { /* Pawns */
    state.Board[x][6] = 'p';
    state.Board[x][1] = 'P';
  }
  

  for (y = 0; y < 8; y += 7) { /* Rooks */
    piece = 'R';
    if (y == 7) {
      piece = tolower(piece);
    }
    state.Board[0][y] = piece;
    state.Board[7][y] = piece;
  }
  for (y = 0; y < 8; y += 7) { /* Knights */
    piece = 'N';
    if (y == 7) {
      piece = tolower(piece);
    }
    state.Board[1][y] = piece;
    state.Board[6][y] = piece;
  }
  for (y = 0; y < 8; y += 7) { /* Bishops */
    piece = 'B';
    if (y == 7) {
      piece = tolower(piece);
    }
    state.Board[2][y] = piece;
    state.Board[5][y] = piece;
  }
 
  state.Board[3][0] = 'Q'; /* Kings and Queens */
  state.Board[4][0] = 'K';
  state.Board[3][7] = 'q';
  state.Board[4][7] = 'k';

  state.whitecastle[0] = 'K';
  state.whitecastle[1] = 'Q';
  state.blackcastle[0] = 'k';
  state.blackcastle[1] = 'q';
  state.enpassant[0] = '-';
  state.enpassant[1] = '-';

  state.player = 'w';
  state.halfmoveclock = 0;
  state.fullmoveclock = 0;

  return state;
}



struct stateInfo copystate(struct stateInfo state) {
  struct stateInfo copiedstate;
  int x, y;

  for (x = 0; x < 8; x++) {
    for (y = 0; y < 8; y++) {
      copiedstate.Board[x][y] = state.Board[x][y];
    }
  }

  copiedstate.player = state.player;
  
  strncpy(copiedstate.whitecastle, state.whitecastle, 2);
  strncpy(copiedstate.blackcastle, state.blackcastle, 2);
  strncpy(copiedstate.enpassant, state.enpassant, 2);

  copiedstate.halfmoveclock = state.halfmoveclock;
  copiedstate.fullmoveclock = state.fullmoveclock;

  return copiedstate;
}



struct stateInfo makemove (struct stateInfo oldstate, char startcell[2], char endcell[2]) {
  return makemovewithPawnPromote(oldstate, startcell, endcell, 'Q');
}



struct stateInfo makemovewithPawnPromote(struct stateInfo oldstate, char startcell[2], char endcell[2], char promotion) {
  int startx, starty, endx, endy;
  char piece;
  char castle[3];
  struct stateInfo state;

  state = copystate(oldstate);

  startx = startcell[0] - 'a'; /* Convert the inputs into the array-equivalent numbers */
  starty = startcell[1] - '1';
  endx = endcell[0] - 'a';
  endy = endcell[1] - '1';
  piece = oldstate.Board[startx][starty]; /* Get the piece selected by the inputs. */
  state.Board[startx][starty] = ' ';
  state.Board[endx][endy] = piece;

  state.enpassant[0] = '-'; /* Clear any en passants.*/
  state.enpassant[1] = '-';

  state.halfmoveclock += 1;

 

  if (oldstate.Board[endx][endy] != ' ') {
    state.halfmoveclock = 0;
  }

  getcastling(state, castle);

  switch (toupper(piece)) { /* Do some updates based on the piece's type. */
  case 'P': /* Pawn */
    state.halfmoveclock = 0;
    if (starty - endy == 2 || starty - endy == -2) { /* If a big jumpy move has been made, allowing an en passant */
      state.enpassant[0] = startx + 'a';
      state.enpassant[1] = (starty + endy) / 2 + '1'; /* Takes square midway between the move. */
      break;
    }
    else if ((oldstate.enpassant[0] == endcell[0]) && (oldstate.enpassant[1] == endcell[1])) {
      state.Board[endx][starty] = ' '; /* Take the piece that got en passanted. */
    }
    if (state.player == 'w' && endy == 7) {
      state.Board[endx][endy] = toupper(promotion);
    }
    else if (state.player == 'b' && endy == 0) {
      state.Board[endx][endy] = tolower(promotion);
    }
    break;
  case 'R': /* Rook */
    if ((state.player == 'w' && starty == 0) || (state.player == 'b' && starty == 7)) {
      if (startx == 7) { /* Moving from kingside */
      	if (toupper(castle[0]) == 'K' && toupper(castle[1]) == 'Q') {
	  castle[0] = castle[1];
	  castle[1] = '-';
	}
	else if (toupper(castle[0]) == 'K') {
	  castle[0] = '-';
	}
      
      }
      else if (startx == 0) { /* Moving from queenside */
	if (toupper(castle[1]) == 'Q') {
	  castle[1] = '-';
	}
	else if (toupper(castle[0]) == 'Q') {
	  castle[0] = '-';
	}      
      }
    }
    break; 
  case 'K': /* King */
    castle[0] = '-';
    castle[1] = '-';
    if (endx - startx == 2) { /* Move would only happen for kingside castle */
      piece = state.Board[7][starty]; /* Take the rook */
      state.Board[7][starty] = ' ';
      state.Board[5][starty] = piece;
    }
    else if (endx - startx == -2) { /* Move would only happen for queenside castle */
      piece = state.Board[0][starty]; /* Take the rook */
      state.Board[0][starty] = ' ';
      state.Board[3][starty] = piece;
    }
    break;
  }
  if (state.player == 'w') { /* Updating the castles. */
    strncpy(state.whitecastle, castle, 2);
  }
  else {
    strncpy(state.blackcastle, castle, 2);
  }

  
  if (state.player == 'w') {
    strncpy(castle, state.blackcastle, 2);
  }
  else {
    strncpy(castle, state.whitecastle, 2);
  }
  piece = oldstate.Board[endcell[0] - 'a'][endcell[1] - '1']; /* If a rook gets taken. */
  if (toupper(piece) == 'R') {
    if ((state.player == 'w' && endcell[1] == '8') || (state.player == 'b' && endcell[1] == '1')) {
      if (endcell[0] == 'h') { /* Taking kingside castle */
      	if (toupper(castle[0]) == 'K' && toupper(castle[1]) == 'Q') {
	  castle[0] = castle[1];
	  castle[1] = '-';
	}
	else if (toupper(castle[0]) == 'K') {
	  castle[0] = '-';
	}
      
      }
      else if (endcell[0] == 'a') { /* Taking queenside castle */
	if (toupper(castle[1]) == 'Q') {
	  castle[1] = '-';
	}
	else if (toupper(castle[0]) == 'Q') {
	  castle[0] = '-';
	}      
      }
    }
    if (state.player == 'w') { /* Updating the castles. */
      strncpy(state.blackcastle, castle, 2);
    }
    else {
      strncpy(state.whitecastle, castle, 2);
    }
  }
  

  if (state.player == 'w') { /* Switch players */
    state.player = 'b';
  }
  else {
    state.player = 'w';
    state.fullmoveclock += 1;
  }
  return state;
}



void getmove (struct stateInfo state, char startcell[2], char endcell[2], char *promotion) { /* Gets human player's move. */
  int valid;
  char promotionstring[1];
  valid = 0;

  *promotion = '-';
  while (valid == 0) { /* While the move is not valid. */
    printf("Enter piece you wish to move (in the form letternumber, such as a2): ");
    getinputofsize(startcell, 2);
    startcell[0] = tolower(startcell[0]); /* Convert to lower case. */
    printf("\nEnter place you wish to move to (in the form letternumber, such as a4): ");
    getinputofsize(endcell, 2);
    endcell[0] = tolower(endcell[0]);
    valid = checkvalidmove(state, startcell, endcell); /* See if the move is valid. */
    if (valid == 0) {
      printf("INVALID MOVE. PLEASE TRY AGAIN.\n");
      continue;
    }
  } 

  if ((state.player == 'w' && endcell[1] == '8') || (state.player == 'b' && endcell[1] == '1') ) {
    valid = 0;
    while (valid == 0) {
      printf("Please give a pawn promotion (R, N, B or Q): ");
      getinputofsize(promotionstring, 1);
      if (toupper(promotionstring[0]) == 'R' || toupper(promotionstring[0]) == 'N' || toupper(promotionstring[0]) == 'B' || toupper(promotionstring[0]) == 'Q') {
	valid = 1;
	*promotion = promotionstring[0];
      }
      else {
	printf("INVALID PROMOTION. PLEASE TRY AGAIN.\n");
      }
    }
  }
}






struct stateInfo convertFENtoState(char *FEN) { /* Convert FEN notation to a board. */
  struct stateInfo state;
  int i, j, k;
  int x, y;
  int length;
  int settingBoard;
  int extrainfo;

  length = strlen(FEN);

  y = 7;
  x = 0;
  settingBoard = 1; /* Variable to indicate if we are setting up the board. */
  extrainfo = 0; /* Variable which indicates what extra info we want to access; increments by 1 after receiving each of player, castling, enpassant, and clocks. */

  for (i = 0; i < length; i++) { /* Scroll through the whole string. */
    if (settingBoard == 1) { /* Setting up the board */
      if (FEN[i] == '/') {
	y -= 1;
	x = 0;
	continue;
      }
      else if (FEN[i] > '0' && FEN[i] <= '8') {
	for (j = 0; j < FEN[i]; j++) {
	  state.Board[x][y] = ' ';
	  x += 1;
	}
      }
      else {
	state.Board[x][y] = FEN[i];
	x += 1;
      }
      if (FEN[i] == ' ') {
	settingBoard = 0;
      }
    }
    else if (settingBoard == 0) { /* If we have finished setting up the board. */
      if (FEN[i] == ' ') {
	extrainfo += 1;
	continue;
      }

      switch (extrainfo) { /* Setting up the extra bits. */
      case 0 : /* Player */
	state.player = FEN[i];
	break;
      case 1 : /* Castling */
	if (FEN[i] == '-') {
	  state.whitecastle[0] = '-';
	  state.whitecastle[1] = '-';
	  state.blackcastle[0] = '-';
	  state.blackcastle[1] = '-';
	}
	else {
	  for (j = 0; j < 4; j++) { /* Max size of castling 'word' is 4 */
	    if (FEN[i+j] == ' ') { /* If we reach the end of the word, we move on. */
	      i = i + j;
	      extrainfo += 1;
	      break;
	    }
	    else { /* Complicate stuff to separate the castling 'word' in FEN into the two separate white + black castling */
	      if (toupper(FEN[i+j]) == FEN[i+j]) {
		state.whitecastle[j] = FEN[i+j];
		k = j;
	      }
	      else {
		state.blackcastle[j-k] = FEN[i+j];
	      }
	    }
	  }
	}
	break;
      case 2 : /* En passant */
	if (FEN[i] == '-') {
	  state.enpassant[0] = '-';
	}
	else {
	  state.enpassant[0] = FEN[i];
	  state.enpassant[1] = FEN[i+1];
	  i += 1;
	}
	break;
      case 3 : /* Half move clock */
	state.halfmoveclock = FEN[i];
	break;
      case 4 : /* Full move clock */
	state.fullmoveclock = FEN[i];
	break;	
      }
    }
  }

  return state;
}



void convertStatetoFEN(struct stateInfo state) { /* Convert Board into FEN notation. */
  char *FEN;
  char *TempFEN;
  char *line;
  int spacecount; /* Counts the number of empty cells */
  char cell;
  int x, y;
  int linelength;  /* Counts the numbers of chars written in a single FEN row (rows are separated by /). */

  /* Initalise FEN string */
  FEN = (char*)malloc(sizeof(char));
  FEN[0] = '\0';
  printf("%d, %d\n", (int) sizeof(char), (int) strlen(FEN));

  for (y = 7; y >= 0; y--) { /* FEN starts writing from rank 8 and moves down to rank 1. In our code, this is from 7 to 0. */
    /* We build one line at a time, and then add it on to the main FEN string after we have created the line. */
    line = (char*)malloc(10 * sizeof(char)); /* Size 10 so that it is guaranteed to be big enough. (Maximum line is 8 pieces, followed by a '/', followed by the '\0'. This is 10 characters) */
    if (line == NULL) {
      printf("Could not malloc memory to \"line\" in convertStatetoFEN\n");
      exit(EXIT_FAILURE);
    }
    line[0] = '\0';
    linelength = 0;
    spacecount = 0;
 
    for (x = 0; x < 8; x++) {
      if (state.Board[x][y] == ' ') { /* If an empty cell, */
	spacecount += 1; /* Add to the number of empty cells. */
	continue; /* Move along. */
      }
      else if (spacecount > 0) { /* If the next cell isn't empty and there were spaces beforehand, we say how many spaces we just got through. */
	line[linelength] = (char) (spacecount + '0');
	spacecount = 0; /* Reset spaces to 0 */
	linelength += 1;
      }
      cell = state.Board[x][y];
      line[linelength] = cell; /* Fill in the cell. */
      linelength += 1;
    }

    if (spacecount > 0) { /* If we have reached the end of the line and there were empty cells, */
      line[linelength] = spacecount + '0'; /* Write in the empty cells. */
      spacecount = 0;
      linelength += 1;
    }
    if (y != 0) {
      line[linelength] = '/';
      linelength += 1;
    }
    line[linelength] = '\0'; /* Ensure the string terminates. */
    
    TempFEN = (char*)realloc(FEN, (strlen(FEN)+linelength+1) * sizeof(char)); /* Resize the string so we can fit the next ones in. */
    if (TempFEN == NULL) {
      printf("It's all fucked.\n"); /* Error detection and complaint */
    } 
    else {
      FEN = TempFEN; /* Assign the new string if nothing fucked up. */
    }
    strcat(FEN, line); /* Concatenate the FEN and the line. */
    printf("%d, %d\n", (int) sizeof(char), (int) strlen(FEN));
    printf("%s\n", FEN);

    free(line);
  }

  /* Now for the non-board contained information. ie. Whose turn it is, which castling is available, if an enpassant is available, and the half-move and full-move clocks */
  line = (char*)malloc(15 * sizeof(char)); /* Size of 15 is big enough. */

  /* Player */
  line[0] = ' ';
  line[1] = state.player;

  /* Castling */
  line[2] = ' ';
  line[3] = state.whitecastle[0];
  linelength = 4;

  if (state.whitecastle[1] == 'Q') {
    line[linelength] = state.whitecastle[1];
    linelength += 1;
  }
  if (state.blackcastle[0] != '-') {
    line[linelength] = state.blackcastle[0];
    linelength += 1;
  }
  if (state.blackcastle[1] != '-') {
    line[linelength] = state.blackcastle[1];
    linelength += 1;
  }

  /* En passant */
  line[linelength] = ' ';
  linelength += 1;
  line[linelength] = state.enpassant[0];
  linelength += 1;
  if (strlen(state.enpassant) == 2) {
    line[linelength] = state.enpassant[1];
    linelength += 1;
  }

  /* Clocks */
  line[linelength] = ' ';
  linelength += 1;
  line[linelength] = state.halfmoveclock + '0';
  linelength += 1;
  line[linelength] = ' ';
  linelength += 1;
  line[linelength] = state.fullmoveclock + '0';
  line[linelength+1] = '\0';
  printf("%s\n", line);

  TempFEN = (char*)realloc(FEN, (strlen(FEN)+linelength+2) * sizeof(char)); /* Resize FEN length to allow space for additional info */
  if (TempFEN == NULL) {
    printf("It's all fucked.\n"); /* Error catch + complaint */
  }
  else {
    FEN = TempFEN;
  }
  strcat(FEN, line); /* Add on the line */
  printf("%d, %d\n", (int) sizeof(char), (int) strlen(FEN)); /* Used in testing. */
  printf("%s\n", FEN); /* Used in testing */
  free(line); /* Free memory after use */
  free(FEN);
}



void convertalgebraictocells(char algebraic[50], struct stateInfo state, char startcell[2], char endcell[2], char *promotion) { /* NEED TO FINISH SOME TIME - FOR CONVERTING ALGEBRAIC NOTATION OF A MOVE INTO A MOVE REPRESENTED BY NUMBERS */
  char piece;
  int i;
  int endcellindex;
  int possiblemoves;
  char teststartcell[2], possiblestartcell[2];
  int x, y;
  
  *promotion = '-';
  endcell[0] = 'z';
  endcell[1] = '9';
  startcell[0] = 'z';
  startcell[1] = '9';

  if (isupper(algebraic[0])) { /* If piece is first. */
    if (algebraic[0] == 'O') { /* If castling. */
      startcell[0] = 'e';
      if (state.player == 'w') {
	startcell[1] = '1';
	endcell[1] = '1';
      }
      else {
	startcell[1] = '8';
	endcell[1] = '8';
      }
      if (strlen(algebraic) > 4) { /* Queenside castle */
	endcell[0] = 'c';
      }
      else { /* Kingside castle. */
	endcell[0] = 'g';
      }
      return;
    }
    else {
      piece = algebraic[0];
    }
  }
  else { /* If no capital letter to start, and not a castling, must be a pawn move. */
    piece = 'P';
  }


  for (i = 0; i < strlen(algebraic); i++) {
    if (isdigit(algebraic[i])) {
      endcell[0] = algebraic[i-1];
      endcell[1] = algebraic[i]; /* We cycle through all of it. In case we happened to accidentally take the startcell instead. */
      endcellindex = i - 1;
      *promotion = '-';
    }
    /* if (algebraic[i] == '=') { */
    /*   *promotion = algebraic[i+1]; */
    /* } */
    if (algebraic[i] == 'Q' || algebraic[i] == 'N' || algebraic[i] == 'B' || algebraic[i] == 'R') {
      *promotion = algebraic[i];
    }
  }
  if (endcell[0] == 'z' || endcell[1] == '9') {
    return;
  }


  for (i = 0; i < endcellindex; i++) {
    if (islower(algebraic[i]) && algebraic[i] != 'x') {
      startcell[0] = algebraic[i];
    }
    else if (isdigit(algebraic[i])) {
      startcell[1] = algebraic[i];
    }
  }

  possiblemoves = 0;
  for (x = 0; x < 8; x++) {
    if (startcell[0] != 'z') {
      if (startcell[0] != 'a' + x) {
	continue;
      }
    }
    for (y = 0; y < 8; y++) {
      if (startcell[1] != '9') {
	if (startcell[1] != '1' + y) {
	  continue;
	}
      }
      /* printf("On %d, %d.\n", x, y); */
      if ((state.player == 'w' && isupper(state.Board[x][y])) || (state.player == 'b' && islower(state.Board[x][y]))) {
	if (toupper(state.Board[x][y]) == piece) {
	  teststartcell[0] = 'a' + x;
	  teststartcell[1] = '1' + y;
	  /* printf("Checking if %.2s can move to %.2s.\n", teststartcell, endcell); */
	  /* printf("Player: %c\n", state.player); */
	  if (checkvalidmove(state, teststartcell, endcell)) {
	    possiblestartcell[0] = 'a' + x;
	    possiblestartcell[1] = '1' + y;
	    possiblemoves += 1;
	  }
	}
      }
    }
  }
  
  if (possiblemoves == 0) {
    printf("Error in algebraic to cells: No possible moves found: ");
    printf("%s\n", algebraic);
  }
  else if (possiblemoves > 1) {
    printf("Error in algebraic to cells: More than 1 possible move found: ");
    printf("%s\n", algebraic);
  }
  else {
    /* printf("Possible move found.\n"); */
    strncpy(startcell, possiblestartcell, 2);
  }
}






void displayBoard(char Board[8][8]) { /* Display the board to the human. */
  int x, y;

  printf("\n\n  |"); /* Print the top row */
  for (x = 0; x < 8; x++) {
    printf(" ");
    putchar('A' + x);
    printf(" |");
  }
  printf("\n---");
  for (x = 0; x < 8; x++) {
    printf("----");
  }
  printf("\n");

  for (y = 7; y >= 0; y--) { /* Print the rest of the board. */
    putchar('1' + y);
    printf(" |");
    for (x = 0; x < 8; x++) {
      if ((x + y) % 2 == 0) {
	printf(" %c |", Board[x][y]);
      }
      else {
	printf("(%c)|", Board[x][y]);
      }
    }
    printf("\n---");
    for (x = 0; x < 8; x++) {
      printf("----");
    }
    printf("\n");
  }
}



void displayPrompt(struct stateInfo state) {
  if (state.player == 'w') { /* Display whose turn it is. */
    printf("\nIt is White's turn.\n");
  }
  else {
    printf("\nIt is Black's turn.\n");
  }
  if (isincheck(state) == 1) {
    printf("\nYou are in check.\n\n\n");
  }
}







int checkvalidmove(struct stateInfo state, char startcell[2], char endcell[2]) { /* Checks if a move is valid. */
  char startrank, startcol, endrank, endcol;
  int startx, starty, endx, endy;
  char startpiece, endpiece;
  int valid;
  int x, y;
  char castle[2];
  struct stateInfo nextstate;
  
  startcol = startcell[0]; /* Initialising values. */
  startrank = startcell[1];
  endcol = endcell[0];
  endrank = endcell[1];

  startx = startcol - 'a';
  starty = startrank - '1';
  endx = endcol - 'a';
  endy = endrank - '1';

  
  if ((startrank < '1') || (startrank > '8') || (endrank < '1') || (endrank > '8')) { /* If not within ranks */
    return 0;
  }
  if ((startcol < 'a') || (startcol > 'h') || (endcol < 'a') || (endcol > 'h')) { /* If not within columns */
    return 0;
  }

  startpiece = state.Board[startx][starty]; /* The piece which is picked up. */
  endpiece = state.Board[endx][endy]; /* The piece (if any) where we move to. */

  if (startpiece == ' ') { /* If we're trying to move an empty space. */
    return 0;
  }
  if (state.player == 'w') {
    if (startpiece == tolower(startpiece) || (endpiece == toupper(endpiece) && endpiece != ' ')) { /* If piece we're trying to choose is black or if end place has a white piece. */
      return 0;
    }
  }
  else if (state.player == 'b') {
    if (startpiece == toupper(startpiece) || (endpiece == tolower(endpiece) && endpiece != ' ')) { /* If piece we're trying to choose is white or if end place has a black piece. */
      return 0;
    }
  }

  getcastling(state, castle);

  switch (toupper(startpiece)) { /* Do some checks based on the piece's type. */
  case 'P': /* Pawn */
    valid = checkvalidpawn(state, startx, starty, endx, endy);
    break;
  case 'R': /* Rook */
    valid = checkvalidrook(state.Board, startx, starty, endx, endy);
    break; 
  case 'N': /* Knight */
    valid = checkvalidknight(startx, starty, endx, endy);
    break;
  case 'B': /* Bishop */
    valid = checkvalidbishop(state.Board, startx, starty, endx, endy);
    break;
  case 'Q': /* Queen */
    valid = checkvalidqueen(state.Board, startx, starty, endx, endy);
    break;
  case 'K': /* King */
    valid = checkvalidking(state, endcell, startx, starty, endx, endy);
    break;
  }
  if (valid == 0) { /* Final check. */
    return 0;
  }

  nextstate = setupBoard();
  for (x = 0; x < 8; x++) {
    for (y = 0; y < 8; y++) {
      nextstate.Board[x][y] = state.Board[x][y];
    }
  }

  nextstate = makemove(state, startcell, endcell);
  nextstate.player = state.player; /* It's only the "half" nextstate. We want to see if the player would be in check in this 'possible' position. */
  if (isincheck(nextstate) == 1) {
    return 0;
  }
  
  return 1; /* If nobody has raised any concerns, then clearly the move is legal */
}



int checkvalidpawn (struct stateInfo state, int startx, int starty, int endx, int endy) {

  if (state.player == 'w') { /* If white */
    if ( (endy - starty != 1) && (endy - starty != 2) ) { /* Can only move 'forwards'. */
      return 0;
    }
    if ( (endy - starty == 2) && (starty != 1 || endx - startx != 0 || state.Board[startx][2] != ' ') ) { /* Can only move two spaces if started from pawn row. */
      return 0;
    }
  }
  else if (state.player == 'b') { /* If black */
    if ( (endy - starty != -1) && (endy - starty != -2) ) { /* Can only move 'backwards' */
      return 0;
    }
    if ( (endy - starty == -2) && (starty != 6 || endx - startx != 0 || state.Board[startx][5] != ' ') ) { /* Can only move two spaces if started from pawn row */
      return 0;
    }
  }

 
  if (endx - startx == 0) { /* Forward movement */
    if (state.Board[endx][endy] != ' ') {
      return 0;
    }
  }
  else if (endx - startx == -1 || endx - startx == 1) { /* Diagonal movement. */
    if (state.Board[endx][endy] == ' ') { /* If we're trying to diagonal to an empty space: this is only possible for an en passant */
      if (endx != state.enpassant[0] - 'a' || endy != state.enpassant[1] - '1') { /* If there's no enpassant going on. */
	return 0;
      }
    }
  }
  else { /* No other moves are possible */
    return 0;
  }


  return 1; /* Will never be reached due to above 'else' statement. */
}



int checkvalidrook(char Board[8][8], int startx, int starty, int endx, int endy) { /* Checks if a rook move is valid. */
  int i;

  /* If not moving in some horizontal or vertical direction */
  if ( (startx - endx != 0) && (starty - endy != 0) ) {
    return 0;
  }

  /* Vertical movement */
  if (startx - endx == 0) {
    if (starty < endy) { /* Moving 'up' */
      for (i = starty + 1; i < endy; i++) {
	if (Board[startx][i] != ' ') {
	  return 0;
	}
      }
    }
    else if (starty > endy) { /* Moving 'down' */
      for (i = starty - 1; i > endy; i--) {
	if (Board[startx][i] != ' ') {
	  return 0;
	}
      }
    }
  }
  else { /* Horizontal movement */
    if (startx < endx) { /* Moving right */
      for (i = startx + 1; i < endx; i++) {
	if (Board[i][starty] != ' ') {
	  return 0;
	}
      }
    }
    else if (startx > endx) { /* Moving left */
      for (i = startx - 1; i > endx; i--) {
	if (Board[i][starty] != ' ') {
	  return 0;
	}
      }
    }
  }
  
  /* If there were no problems, it's a-okay! */
  return 1;
}


int checkvalidknight(int startx, int starty, int endx, int endy) { /* Knight move check */
  int movex, movey;

  /* The number of squares moved. */
  movex = startx - endx;
  movey = starty - endy;

  /* Make everything positive. */
  if (movex < 0) {
    movex = -movex;
  }
  if (movey < 0) {
    movey = -movey;
  }


  /* We literally only need to check if the move is an 'L' shape. We don't care about any pieces "in the way" and such. */
  if (movex == 1) {
    if (movey != 2) {
      return 0;
    }
  }
  else if (movex == 2) {
    if (movey != 1) {
      return 0;
    }
  }
  else {
    return 0;
  }

  return 1;
}



int checkvalidbishop(char Board[8][8], int startx, int starty, int endx, int endy) { /* Check if a bishop obeys god */
  int numberofsquares;
  int i;
  int directionx, directiony;

  if ( (startx - endx != starty - endy) && (startx - endx != -(starty - endy)) ) { /* Check diagonal movement */
    return 0;
  }

  numberofsquares = startx - endx; /* Could be starty - endy. Doesn't matter due to diagonals; they are equal. */
  if (numberofsquares < 0) {
    numberofsquares = -numberofsquares;
  }

  if (startx < endx) { /* Check if the path is clear. Nothing else matters - the other checks have been done. Has to be rewritten 4 ways for the 4 possible directions. Bit messy. */
    if (starty < endy) {
      directionx = 1;
      directiony = 1;
    }
    else if (starty > endy) {
      directionx = 1;
      directiony = -1;
    }
  }
  else if (startx > endx) {
    if (starty < endy) {
      directionx = -1;
      directiony = 1;
    }
    else if (starty > endy) {
      directionx = -1;
      directiony = -1;
    }
  }
  

  for (i = 1; i < numberofsquares; i++) {
    if (Board[startx + i*directionx][starty + i*directiony] != ' ') {
      return 0;
    }
  }

  return 1;
}



int checkvalidking(struct stateInfo state, char *endcell, int startx, int starty, int endx, int endy) { 
  int movex, movey;
  int kingrank;
  char cell[2], castle[2];

  if (state.player == 'w') {
    kingrank = 0;
  }
  else {
    kingrank = 7;
  }

  if (iscellbeingattacked(state, endcell) == 1) { /* If would be checked in that position. */
    return 0;
  }

  movex = startx - endx;
  movey = starty - endy;
  if (movex < 0) {
    movex = -movex;
  }
  if (movey < 0) {
    movey = -movey;
  }

  if (movey > 1) {
    return 0;
  }

  getcastling(state, castle);
  
  if ((state.player == 'w' && endy == 0) || (state.player == 'b' && endy == 7)) {
    /* printf("%c, %c, %c\n", castle[0], state.whitecastle[0], state.blackcastle[0]); */
    if ((endx == 'g' - 'a') && (toupper(castle[0]) == 'K')) { /* Castling kingside. the char *castle implicitly stores whether the king or rook has moved yet, so we don't need to check those. */
      cell[0] = 'f';
      cell[1] = kingrank + '1';
      if (state.Board[5][kingrank] != ' ' || iscellbeingattacked(state, cell)) {
	return 0;
      }
    }
    else if ((endx == 'c' - 'a') && ((toupper(castle[0]) == 'Q' || toupper(castle[1]) == 'Q'))) { /* Castling queenside. */
      cell[0] = 'd';
      cell[1] = kingrank + '1';
      if ((state.Board[3][kingrank] != ' ' || state.Board[1][kingrank] != ' ') || iscellbeingattacked(state, cell)) {
	return 0;
      }
    }
    else if (movex > 1) {
      /* printf("???\n"); */
      return 0;
    }  
  }
  else if (movex > 1) {
    return 0;
  }    
  return 1;
}



int checkvalidqueen(char Board[8][8], int startx, int starty, int endx, int endy) {  /* The queen is literally just a bishop OR rook. So if either of those are valid, the queen is moving goodly.*/
  int valid;
  
  valid = checkvalidbishop(Board, startx, starty, endx, endy) || checkvalidrook(Board, startx, starty, endx, endy);

  return valid;
}






void getallpossiblemoves(struct stateInfo state, int possiblemoves[64][64]) {
  char *startcell, *endcell;
  int startx, starty, startindex;
  int endx, endy, endindex;
  
  startcell = (char *) malloc(2 * sizeof(char));
  endcell = (char *) malloc(2 * sizeof(char));
  for (startx = 0; startx < 8; startx++) {
    startcell[0] = startx + 'a';
    for (starty = 0; starty < 8; starty++) {
      startcell[1] = starty + '1';
      startindex = 8*starty + startx;
      for (endx = 0; endx < 8; endx++) {
	endcell[0] = endx + 'a';
	for (endy = 0; endy < 8; endy++) {
	  endcell[1] = endy + '1';
	  endindex = 8*endy + endx;
	  if (checkvalidmove(state, startcell, endcell) == 1) {
	    possiblemoves[startindex][endindex] = 1;
	  }
	  else {
	    possiblemoves[startindex][endindex] = 0;
	  }
	}
      }
    }
  }
  free(startcell);
  free(endcell);
}



int iscellbeingattacked(struct stateInfo state, char cell[2]) { /* Iscellbeingattacked. */
  int x, y;
  char currentcell[2];
  int beingattacked = 0;
  char kingcell[2];
  char attackingplayer;

  if (state.player == 'w') {
    attackingplayer = 'b';
  }
  else {
    attackingplayer = 'w';
  }


  kingcell[0] = 'z';
  kingcell[1] = '9';
  findkingcell(state.Board, attackingplayer, kingcell); /* Get the kingcell of the attacking player. */
  if (kingcell[0] == 'z' || kingcell[1] == '9') { /* Stops errors. Can be removed and still have a functioning program, but we dislike errors. */
    return 0;
  }
  
  if (pow(cell[0] - kingcell[0], 2) + pow(cell[1] - kingcell[1], 2) <= 2) { /* Works out euclidean (?) distance between king and target cell. Checks whether king is in range of cell. */
    if (cell[0] != kingcell[0] || cell[1] != kingcell[1]) { /* Check if the cell can be attacked by the king by making sure that it isn't actually the king. We don't care otherwise. */
      return 1;
    }
  }

  for (x = 0; x < 8; x++) { /* Cycle through whole board. */
    for (y = 0; y < 8; y++) {
      currentcell[0] = 'a' + x;
      currentcell[1] = '1' + y;
      if ((currentcell[0] == kingcell[0]) && (currentcell[1] == kingcell[1])) { /* We checked the king earlier. Skip it. DO NOT REMOVE; STOPS INIFITE LOOPINGS. */
	continue;
      }
      state.player = attackingplayer; /* Now to see what the attacking player could do. */
      beingattacked = checkvalidmove(state, currentcell, cell); /* A cell is being attacked if an enemy piece can move there. */

      if (toupper(state.Board[x][y]) == 'P' && state.Board[cell[0] - 'a'][cell[1] - '1'] == ' ') { /* Special exception for pawns; a pawn's possible moves and attacks on empty cells are different. Couldn't think of a cleaner way to write this. */
      	if (beingattacked == 1) { /* This would only be so if the pawn could move forwards */
	  beingattacked = 0;
      	}
      	else if (beingattacked == 0) { /* We ignore all the previous checks and put in some new ones for diagonal attack. */
      	  if (attackingplayer == 'w' && state.Board[x][y] == 'P') {
      	    if ( ((currentcell[0] - cell[0] == 1) || (currentcell[0] - cell[0] == -1)) && (cell[1] - currentcell[1] == 1) ) { /* Diagonal 'up' movement */
      	      beingattacked = 1;
      	    }
      	  }
      	  else if (attackingplayer == 'b' && state.Board[x][y] == 'p') {
      	    if ( ((currentcell[0] - cell[0] == 1) || (currentcell[0] - cell[0] == -1)) && (cell[1] - currentcell[1] == -1) ) { /* Diagonal 'down' movement */
      	      beingattacked = 1;
      	    }
      	  }
      	}
      }
     
      if (beingattacked == 1) { /* No point in waiting until the end if we found something */
        /* printf("Cell %.2s is being attacked by cell %.2s\n", cell, currentcell); */
	return 1; /* Cell is being threatened. */
      }
    }
  }
  return 0;
}



void getcastling(struct stateInfo state, char castle[2]) {
  /* Gets the castling possibilities for the player. */
  if (state.player == 'w') {
    castle[0] = state.whitecastle[0];
    castle[1] = state.whitecastle[1];
  }
  else {
    castle[0] = state.blackcastle[0];
    castle[1] = state.blackcastle[1];
  }
}







void findkingcell(char Board[8][8], char player, char kingcell[2]) {
  int x, y;
  int kingcellx, kingcelly;
          
  kingcellx = -1; /* Assume the king is not on board to begin with. */
  kingcelly = -1;
  
  for (x = 0; x < 8; x++) {
    for (y = 0; y < 8; y++) {
      if (player == 'w') {
	if (Board[x][y] == 'K') {
	  kingcellx = x;
	  kingcelly = y;
	  x = 8;
	  y = 8;
	}
      }
      else if (player == 'b') {
	if (Board[x][y] == 'k') {
	  kingcellx = x;
	  kingcelly = y;
	  x = 8;
	  y = 8;
	}
      }
    }
  }

  if (kingcellx == -1 || kingcelly == -1) { /* If we haven't located the king */
    return;
  }
  else {
    kingcell[0] = 'a' + kingcellx;
    kingcell[1] = '1' + kingcelly;
  }
}



int isgameover(struct stateInfo state) {
  if (isnomoves(state) == 1) {
    return 1;
  }
  else if (state.halfmoveclock >= 50) {
    return 1;
  }
  else {
    return 0;
  }
}



int ischeckmate(struct stateInfo state) {
  if (isnomoves(state) && isincheck(state)) {
    return 1;
  }
  else {
    return 0;
  }
}



int isincheck(struct stateInfo state) {
  char kingcell[2];
  int check;
  
  findkingcell(state.Board, state.player, kingcell);
  check = iscellbeingattacked(state, kingcell);

  return check;
}



int isnomoves(struct stateInfo state) {
  int possiblemoves[64][64];
  int i, j;

  getallpossiblemoves(state, possiblemoves);
  for (i = 0; i < 64; i++) {
    for (j = 0; j < 64; j++) {
      if (possiblemoves[i][j] == 1) {
	return 0;
      }
    }
  }
  return 1;
}
