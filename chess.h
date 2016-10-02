#ifndef CHESS_H
#define CHESS_H


void playchess();

struct stateInfo setupBoard();
struct stateInfo copystate(struct stateInfo state);
struct stateInfo makemove(struct stateInfo state, char *startcell, char *endcell);
struct stateInfo makemovewithPawnPromote(struct stateInfo oldstate, char *startcell, char *endcell, char promotion);
void getmove(struct stateInfo state, char *startcell, char *endcell, char *promotion);

struct stateInfo convertFENtoState(char *FEN);  /* The FEN functions are unused. */
void convertStatetoFEN(struct stateInfo state);
void convertalgebraictocells(char algebraic[50], struct stateInfo state, char startcell[2], char endcell[2], char *promotion);

void displayBoard(char Board[8][8]);
void displayPrompt(struct stateInfo state);

int checkvalidmove(struct stateInfo state, char *startcell, char *endcell);
int checkvalidpawn (struct stateInfo state, int startx, int starty, int endx, int endy);
int checkvalidrook(char Board[8][8], int startx, int starty, int endx, int endy);
int checkvalidknight(int startx, int starty, int endx, int endy);
int checkvalidbishop(char Board[8][8], int startx, int starty, int endx, int endy);
int checkvalidking(struct stateInfo state, char *endcell, int startx, int starty, int endx, int endy);
int checkvalidqueen(char Board[8][8], int startx, int starty, int endx, int endy);

void getallpossiblemoves(struct stateInfo state, int possiblemoves[64][64]);
int iscellbeingattacked(struct stateInfo state, char *cell);
void getcastling(struct stateInfo state, char *castle);

void findkingcell(char Board[8][8], char defendingplayer, char *kingcell);
int isgameover(struct stateInfo state);
int ischeckmate(struct stateInfo state);
int isincheck(struct stateInfo state);
int isnomoves(struct stateInfo state);



#endif
