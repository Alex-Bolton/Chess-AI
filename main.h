#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <meschach/matrix.h>
#include <time.h>
#include <pthread.h>


struct stateInfo {
  char Board[8][8];
  char player;
  char whitecastle[2], blackcastle[2], enpassant[2];
  int halfmoveclock, fullmoveclock;
};

struct neuralNetObjects {
  VEC *DESIREDOUTPUT;
  VEC **LAYERS;
  MAT **WEIGHTS;
  MAT **LASTWEIGHTCHANGES;

  int layernumber, *layersizes;
  double weightregularisation, momentum, learningrate;
};

struct learningParameters {
  double baselearningrate, learningratedecay;
  double timediscount;
  int searchdepth;
  int maxgames;
};


struct judgePerformance_multithread_struct {
  struct neuralNetObjects neuralnet;
  struct learningParameters learningparams;
  char colour;
};


#include "chess.h"
#include "training.h"
#include "AIbrain.h"
#include "neuralnetwork.h"


void getinputofsize(char input[], int size);


#endif
