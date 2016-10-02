/* The training module. Used to train the AI. This happens by running through a 'sample game', improving as it goes. */

#include "main.h"


void trainAI(int menuoption) {
  /* Initiates the AI parts and executes the desired function based on the option chosen in main.c */

  struct neuralNetObjects neuralnetobjs;
  struct learningParameters learningparams;
  
  int i;
  int iteration = 0;

  initialiseLearningParameters(&learningparams);

  switch (menuoption) {
  case 1: /* Against itself from scratch. */
    loadRandomWeights(&neuralnetobjs);
    learnFromAgainstSelf(neuralnetobjs, learningparams, iteration);
    break;
  case 2: /* Against itself from saved. */
    loadWeights(&neuralnetobjs, &iteration);
    learnFromAgainstSelf(neuralnetobjs, learningparams, iteration);
    break;
  case 3: /* From games from scratch. */
    loadRandomWeights(&neuralnetobjs);
    readfrompgn(neuralnetobjs, learningparams, iteration);
    return;
    break;
  case 4: /* From games from saved. */
    loadWeights(&neuralnetobjs, &iteration);
    readfrompgn(neuralnetobjs, learningparams, iteration);
    break;
  case 5: /* Judge performance against random player. */
    loadWeights(&neuralnetobjs, &iteration);
    judgePerformance(neuralnetobjs, learningparams, iteration);
    break;
  case 6: /* Play human against AI from saved. */
    loadWeights(&neuralnetobjs, &iteration);
    playHumanAgainstAI(neuralnetobjs, learningparams);
    break;
  default:
    printf("Error: choice not valid for training.\n");
    break;
  }
  

  /* Freeing the memory after use */
  V_FREE(neuralnetobjs.DESIREDOUTPUT);
  for (i = 0; i < neuralnetobjs.layernumber; i++) {
    V_FREE(neuralnetobjs.LAYERS[i]);
  }
  free(neuralnetobjs.LAYERS);
  for (i = 0; i < neuralnetobjs.layernumber-1; i++) {
    M_FREE(neuralnetobjs.WEIGHTS[i]);
  }
  free(neuralnetobjs.WEIGHTS);
  for (i = 0; i < neuralnetobjs.layernumber-1; i++) {
    M_FREE(neuralnetobjs.LASTWEIGHTCHANGES[i]);
   }
  free(neuralnetobjs.LASTWEIGHTCHANGES);
  
  free(neuralnetobjs.layersizes);
}







void learnFromAgainstSelf(struct neuralNetObjects neuralnet, struct learningParameters learningparams, int iteration) {
  /* Trains AI against itself. */

  char startcell[2], endcell[2]; /* For moving from one board position to the next. */
  struct stateInfo state, nextstate;
  int i, j;
  int actions[64][64];   /* Takes values of 0 or 1; identity of possible moves. */
  double alpha, beta;    /* For negamax algorithm. */
  double currentValue, bestValue;
  int action[2];         /* Stores two integers from 0 to 63. First index represents startcell; second represents the endcell. */
  double policy[64][64]; /* The probability of moving from one cell to the next. */

  for (iteration = iteration; iteration < learningparams.maxgames; iteration++) {
    if (iteration % 1000 == 0 && iteration > 0) {
      /* Every 1000th move, we see how good the AI is. */
      judgePerformance(neuralnet, learningparams, iteration);
    }
    printf("\nIteration %d\n", iteration);
    saveWeights(neuralnet, iteration);
    neuralnet.learningrate = learningparams.baselearningrate / (1 + iteration*learningparams.learningratedecay); /* Calculate the learning rate for this iteration. Rather crude method of rate = base/(1 + time*decay). */
    state = setupBoard(); /* Get the initial state. */
    while (isgameover(state) != 1) {
      getallpossiblemoves(state, actions); /* Gets possible actions/moves */
      bestValue = -100;
      for (i = 0; i < 64; i++) { /* Cycle through startcells. */
	for (j = 0; j < 64; j++) { /* Cycle through endcells. */
	  if (actions[i][j] == 0) { /* Skip any impossible (illegal) actions. */
	    continue;
	  }
	  alpha = -100; /* Initiate alpha and beta to -infinity and +infinity (or good enough approximations.) */
	  beta = 100;
	  startcell[0] = 'a' + i % 8; /* Get the representation of the startcell and encell in letter-number format. */
	  startcell[1] = '1' + i / 8;
	  endcell[0] = 'a' + j % 8;
	  endcell[1] = '1' + j / 8;
	  nextstate = makemove(state, startcell, endcell); /* Get the nextstate. */
	  currentValue = -negamaxwithpruning(learningparams.searchdepth, neuralnet, learningparams, nextstate, &alpha, &beta); /* Get the value of the state from the negative of the value of the nextstate. */
	  if (currentValue > bestValue) { /* bestValue = max(currentValue, bestValue) */
	    bestValue = currentValue;
	  }
	}
      }
      
      convertStatetoVector(state, neuralnet.LAYERS[0]); /* Prepare for forwards propagation by turning the state into a vector. */
      forwardspropagation(neuralnet);
      neuralnet.DESIREDOUTPUT->ve[0] = bestValue; /* The 'target value' for backprop. */
      backpropagation(neuralnet);

      getSoftmaxPolicy(neuralnet, learningparams, state, policy, iteration); /* Get a softmax policy. */
      chooseActionFromPolicy(policy, action); /* Choose an action. */
      startcell[0] = 'a' + action[0] % 8;
      startcell[1] = '1' + action[0] / 8;
      endcell[0] = 'a' + action[1] % 8;
      endcell[1] = '1' + action[1] / 8;
      state = makemove(state, startcell, endcell); /* Make the move. */      
    }
  }
}



void learnFromWholeGame(struct neuralNetObjects neuralnet, struct learningParameters learningparams, struct stateInfo stateList[1000], int length, char outcome[20], int iteration) {
  

  int gamepos;
  double bestValue;
  int reward;

  if (iteration % 10000 == 0 && iteration > 0) {
    judgePerformance(neuralnet, learningparams, iteration);
  }
  if (iteration % 10 == 0) {
   printf("\nIteration %d\n", iteration);
  }
  
  if (iteration % 200 == 0) {
    saveWeights(neuralnet, iteration);
  }
  neuralnet.learningrate = learningparams.baselearningrate / (1 + iteration*learningparams.learningratedecay);

  if (strcmp(outcome, "1-0") == 0 || strcmp(outcome, "0-1") == 0) {
    reward = 1;
  }
  else if (strcmp(outcome, "1/2-1/2") == 0) {
    reward = 0;
  }
  else {
    printf("Error: could not interpret outcome of game.");
  }
  
  convertStatetoVector(stateList[length-1], neuralnet.LAYERS[0]);
  forwardspropagation(neuralnet);
  neuralnet.DESIREDOUTPUT->ve[0] = reward;
  backpropagation(neuralnet);

  for (gamepos = length-2; gamepos >= 0; gamepos--) {
    /* displayBoard(state.Board); */
    /* printf("Game number %d, move %d\n", iteration, state.fullmoveclock); */

    bestValue = pow(-1 * learningparams.timediscount, length-1 - gamepos) * reward;
    convertStatetoVector(stateList[gamepos], neuralnet.LAYERS[0]);
    

    neuralnet.DESIREDOUTPUT->ve[0] = bestValue;

    /* printf("Desired output: "); */
    /* v_output(neuralnet.DESIREDOUTPUT); */

    forwardspropagation(neuralnet);

    /* printf("\n\nOld output: "); */
    /* v_output(neuralnet.LAYERS[neuralnet.layernumber-1]); */

    backpropagation(neuralnet);

    /* forwardspropagation(neuralnet); */
    /* printf("New output: "); */
    /* v_output(neuralnet.LAYERS[neuralnet.layernumber-1]); */
  }
}

/* void *multithreadbackprop(void *neuralnet) { */
/*   forwardspropagation((struct neuralNetObjects) *neuralnet); */
/*   backpropagation((struct neuralNetObjects) *neuralnet); */
/* } */





void initialiseLearningParameters(struct learningParameters *learningparams) {
  learningparams->baselearningrate = 0.005;
  learningparams->learningratedecay = 0.0001;

  learningparams->timediscount = 0.9;

  learningparams->searchdepth = 1;
  learningparams->maxgames = 100000;
}



void initialiseNeuralNetObjects(struct neuralNetObjects *neuralnet, int load) {
  int i;
  int layernumber;
 
  if (load == 0) {
    neuralnet->layernumber = 4;

    neuralnet->layersizes = (int *) malloc(neuralnet->layernumber * sizeof(int));
    if (neuralnet->layersizes == NULL) {
      printf("Error in mallocing layersizes.\n");
      exit(EXIT_FAILURE);
    }

    neuralnet->layersizes[0] = 838;
    neuralnet->layersizes[1] = 1;
    neuralnet->layersizes[2] = 1; /* Placeholder for if layernumber = 4 */
    neuralnet->layersizes[neuralnet->layernumber-1] = 1; 
  }
  neuralnet->weightregularisation = 0.001;
  neuralnet->momentum = 0.1;

  layernumber = neuralnet->layernumber;

  neuralnet->LAYERS = (VEC **) malloc(layernumber * sizeof(VEC *));
  if (neuralnet->LAYERS == NULL) {
    printf("Error in mallocing layers vectors.\n");
    exit(EXIT_FAILURE);
  }
  neuralnet->WEIGHTS = (MAT **) malloc((layernumber-1) * sizeof(MAT *));
  if (neuralnet->WEIGHTS == NULL) {
    printf("Error in mallocing weights matrices.\n");
    exit(EXIT_FAILURE);
  }
  neuralnet->LASTWEIGHTCHANGES = (MAT **) malloc((layernumber-1) * sizeof(MAT *));
  if (neuralnet->LASTWEIGHTCHANGES == NULL) {
    printf("Error in mallocing lastweightchanges matrices.\n");
    exit(EXIT_FAILURE);
  }

  neuralnet->DESIREDOUTPUT = v_get(neuralnet->layersizes[layernumber-1]); /* The desired output vector; will be a temporary vector to store the output that the neural net should output for a state. */

  /* The layers that will be used in the neural network. */
  for (i = 0; i < layernumber; i++) {
    neuralnet->LAYERS[i] = v_get(neuralnet->layersizes[i]);
  }

  for (i = 0; i < layernumber-2; i++) {
    neuralnet->WEIGHTS[i] = m_get(neuralnet->layersizes[i], neuralnet->layersizes[i+1]-1); /* Some twiddling to make room for bias units. */
  }
  neuralnet->WEIGHTS[layernumber-2] = m_get(neuralnet->layersizes[layernumber - 2], neuralnet->layersizes[layernumber-1]); /* No bias units in final layer. */
  
   /* Used in momentum for backprop */
  for (i = 0; i < layernumber-1; i++) {
    neuralnet->LASTWEIGHTCHANGES[i] = m_get(neuralnet->WEIGHTS[i]->m, neuralnet->WEIGHTS[i]->n);
  }
}



void saveWeights(struct neuralNetObjects neuralnet, int iteration) {
  /* Saves weights to a file named 'Weights' */
  FILE *f;
  int i;

  f = fopen("SavedWeights/Weights", "w");
  fprintf(f, "%d\n", iteration);
  fprintf(f, "%d\n", neuralnet.layernumber);
  for (i = 0; i < neuralnet.layernumber; i++) {
    fprintf(f, "%d\n", neuralnet.layersizes[i]);
  }
  for (i = 0; i < neuralnet.layernumber-1; i++) {
    m_foutput(f, neuralnet.WEIGHTS[i]);
  }
  fclose(f);

  if (iteration % 1000 == 0) { /* If we are on one of the 1000th games, we make a special save so that progress can be compared. */
    char filelocation[30];
    strcpy(filelocation, "SavedWeights/");
    filelocation[13] = '0' + neuralnet.layernumber;
    filelocation[14] = 'l';
    for (i = 0; i < 3; i++) {
      filelocation[15+i] = '0' + ((int)(iteration / pow(10, 6-i-1)) % 10);
    }
    filelocation[18] = 'k';
    filelocation[19] = '\0';
    f = fopen(filelocation, "w");
    fprintf(f, "%d\n", iteration);
    fprintf(f, "%d\n", neuralnet.layernumber);
    for (i = 0; i < neuralnet.layernumber; i++) {
      fprintf(f, "%d\n", neuralnet.layersizes[i]);
    }
    for (i = 0; i < neuralnet.layernumber-1; i++) {
      m_foutput(f, neuralnet.WEIGHTS[i]);
    }
    fclose(f);
  }
}



void loadWeights(struct neuralNetObjects *neuralnet, int *iteration) {
  /* Loads weights from a saved file. The user chooses which file to load. */

  FILE *f;
  int i;

  char filename[20];
  char filelocation[35];

  printf("Enter the Weights' file name you would like to open: ");
  getinputofsize(filename, 20);
  for (i = 0; i < 20; i++) {
    if (isalnum(filename[i]) == 0) { /* Checks for alphanumeric characters only - or else cuts off. */
      filename[i] = '\0';
      break; /* Cutoff */
    }
  }
  filename[19] = '\0'; /* Ensure the string has an end. */

  printf("Opening %s.\n", filename);
  strcpy(filelocation, "SavedWeights/");
  strcat(filelocation, filename);
  

  f = fopen(filelocation, "r");
  if (f == NULL) {
    printf("ERROR: File does not exist.\n"); /* Error catching. */
    exit(EXIT_FAILURE);
  }

  fscanf(f, "%d\n", iteration); /* Get the iteration it was on. */
  fscanf(f, "%d\n", &(neuralnet->layernumber)); /* Get the number of layers. */
  neuralnet->layersizes = (int *) malloc(neuralnet->layernumber * sizeof(int)); /* Initiate layersizes */
  if (neuralnet->layersizes == NULL) {
    printf("Error in mallocing layersizes.\n");
    exit(EXIT_FAILURE);
  }
  for (i = 0; i < neuralnet->layernumber; i++) { /* Get layer sizes. */
    fscanf(f, "%d\n", &(neuralnet->layersizes[i]));
  }

  initialiseNeuralNetObjects(neuralnet, 1); /* Now we initialise the layers and weights' matrices and vectors. */

  for (i = 0; i < neuralnet->layernumber-1; i++) { /* Now we get the Weights from the file. */
    m_finput(f, neuralnet->WEIGHTS[i]);
  }
  fclose(f);
  printf("Loaded weights from %s.\n", filename);
}



void loadRandomWeights(struct neuralNetObjects *neuralnet) {
  /* Get some random weights. */

  MAT *TEMPMAT;
  int i;

  initialiseNeuralNetObjects(neuralnet, 0);

  for (i = 0; i < neuralnet->layernumber-1; i++) { /* Cycle through all weights. */
    TEMPMAT = m_get(neuralnet->WEIGHTS[i]->m, neuralnet->WEIGHTS[i]->n);
    sm_mlt(-2, m_rand(TEMPMAT), TEMPMAT); /* Get random numbers in the range of -2 to 0. */
    m_add(m_ones(neuralnet->WEIGHTS[i]), TEMPMAT, neuralnet->WEIGHTS[i]); /* Now the random numbers are in range of -1 to 1. This is not necessary. */
    M_FREE(TEMPMAT);
  }
}






void *AIplayRandomMultithread(void *judgeperfvoid) {
  /* A function compatible with pthreads so that we may multithread to judge performance. Makes the process between 3x and 4x as fast (on a quad-core CPU). */

  struct judgePerformance_multithread_struct *judgeperfstruct = (struct judgePerformance_multithread_struct *) judgeperfvoid;
  int winsdrawslosses = 0;
  char colour;
  int game;
  int i;
  struct neuralNetObjects neuralnet;

  neuralnet = copyneuralnetstruct(judgeperfstruct->neuralnet);

  for (i = 0; i < 9; i++) { /* At maximum, 9 iterations per thread. This limitation is only due to the way we have encoded the wins, draws, and losses. */
    if (rolldie(0.5) == 1) {
      colour = 'w';
    }
    else {
      colour = 'b';
    }
    game = AIplayRandom(neuralnet, judgeperfstruct->learningparams, colour);
    if (game == 1) { /* AI wins */
      /* printf("Ai wins (%c), %d\n", colour, i); */
      winsdrawslosses += 100;
    }
    else if (game == 0) { /* Draw */
      winsdrawslosses += 10;
    }
    else { /* AI loses */
      /* printf("Ai loses (%c), %d\n", colour, i); */
      winsdrawslosses += 1;
    }
  }
  
  return (void *) winsdrawslosses;
}



void judgePerformance(struct neuralNetObjects neuralnet, struct learningParameters learningparams, int iteration) {
  /* Judge the performance of the AI by playing it against a random opponent. */

  int wins, draws, losses, total;
  void *winsdrawslosses; /* For receiving wins, losses and draws from multithread function. */
  int i, j;
  pthread_t threadids[4];
  int maxgames = 1000;
  struct judgePerformance_multithread_struct *judgeperfstruct = malloc(sizeof(struct judgePerformance_multithread_struct));
  judgeperfstruct->neuralnet = neuralnet;
  judgeperfstruct->learningparams = learningparams;
  wins = 0;
  draws = 0;
  losses = 0;
  total = 0;
  for (j = 0; j < maxgames/(4*9); j++) { /* Divide maxgames by 4 then 9 because there are 4 threads, each doing 9 games. */
    printf("%d\n", j);
    for (i = 0; i < 4; i++) { 
      /* printf("\nRunning thread %d.\n", i%4); */
      pthread_create(&threadids[i%4], NULL, AIplayRandomMultithread, (void *) judgeperfstruct);
    }

    for (i = 0; i < 4; i++) { /* Join all the threads to unify the results before moving on to another round. */
      pthread_join(threadids[i%4], &winsdrawslosses); /* Get the wins, draws and losses totals from each thread (which were encoded via wins*100 + draws*10 + losses) */
      wins += (int)winsdrawslosses/100;
      draws += ((int)winsdrawslosses%100)/10;
      losses += (int)winsdrawslosses%10;
      total += 9;
    }
  }
  
  free(judgeperfstruct);
  
  printf("Wins: %f, draws: %f, losses: %f\n", (float)wins/total*100, (float)draws/total*100, (float)losses/total*100); 


  /* Save the results of the performance in a file, so that it can be easily compared to other performances. */
  FILE *f;

  f = fopen("Performance", "a");
  if (f == NULL) {
    printf("ERROR: Could not open file.\n");
    exit(EXIT_FAILURE);
  }

  fprintf(f, "Iterations: %d | Layernumber: %d | ", iteration, neuralnet.layernumber);
  for (i = 0; i < neuralnet.layernumber; i++) {
    fprintf(f, "Layer %d: %d  | ", i, neuralnet.layersizes[i]);
  }
  fprintf(f, "Wins: %f | Draws: %f | Losses: %f\n", (float)wins/total*100, (float)draws/total*100, (float)losses/total*100);
  fclose(f);
}



struct neuralNetObjects copyneuralnetstruct(struct neuralNetObjects oldnet) {
  /* Copy the contents of a neuralNetObjects structure to another. 'Deep' copy. */

  struct neuralNetObjects neuralnet;
  int layernumber;
  int i;

  neuralnet.layernumber = oldnet.layernumber;
  neuralnet.layersizes = malloc(neuralnet.layernumber*sizeof(int));
  if (neuralnet.layersizes == NULL) {
    printf("Error in mallocing layersizes.\n");
    exit(EXIT_FAILURE);
  }
  layernumber = neuralnet.layernumber;
  for (i = 0; i < layernumber; i++) {
    neuralnet.layersizes[i] = oldnet.layersizes[i];
  }

  neuralnet.weightregularisation = oldnet.weightregularisation;
  neuralnet.momentum = oldnet.momentum;

  layernumber = neuralnet.layernumber;

  initialiseNeuralNetObjects(&neuralnet, 1); /* Initialise all the vectors and matrices of the layers, weights and desiredoutput. */
  
  /* Copying all the weights. */
  for (i = 0; i < layernumber-1; i++) {
    neuralnet.WEIGHTS[i] = m_copy(oldnet.WEIGHTS[i], MNULL);
  }

  return neuralnet;
}






void readfrompgn(struct neuralNetObjects neuralnet, struct learningParameters learningparams, int startingiteration) {
  FILE *f;
  char string[20];
  int isgame = 0;
  int denied;
  int iteration;
  struct stateInfo state;
  int maxgames = 1000000000;
  char startcell[2], endcell[2];
  char promotion;
  int i;
  struct stateInfo stateList[1000];

  denied = 0;
  f = fopen("pgnfiles/THISONE.pgn", "r");
  for (iteration = 0; iteration < maxgames; iteration++) {
    state = setupBoard();
    isgame = 0;
    for (i = 0; i < 1000; i++) { /* Clear the states. */
      stateList[i] = setupBoard();
    }
    i = 0;
    while (1==1) {
      stateList[i] = copystate(state);
      if (fscanf(f, "%s", string) == EOF) {
	iteration = maxgames;
	break;
      }
      if (strcmp(string, "\"1-0\"]") == 0 || strcmp(string, "\"0-1\"]") == 0 || strcmp(string, "\"1/2-1/2\"]") == 0) {
	isgame = -1;
	continue;
      }
      else if ((strcmp(string, "1.") == 0) && (isgame == -1)) {
	isgame = 1;
      }
      else if (strcmp(string, "1-0") == 0 || strcmp(string, "0-1") == 0 || strcmp(string, "1/2-1/2") == 0) {
	isgame = 0;
	if (iteration < startingiteration) {
	  break;
	}
	learnFromWholeGame(neuralnet, learningparams, stateList, i, string, iteration);
	break;
      }

      if (isgame == 1) {
	if (isdigit(string[0])) {
	  continue;
	}
	convertalgebraictocells(string, state, startcell, endcell, &promotion);
	if (endcell[0] == 'z' || endcell[1] == '9' || startcell[0] == 'z' || startcell[1] == '9') {
	  printf("Error: No startcell or no endcell.\n");
	  denied += 1;
	  break;
	}
	if (checkvalidmove(state, startcell, endcell) == 0) {
	  printf("Error: Move not valid.\n");
	  denied += 1;
	  break;
	}
	else {
	  state = makemovewithPawnPromote(state, startcell, endcell, promotion);
	  i += 1;
	}
        if (iteration < 0) {printf("\n\n%s, %d\n", string, (int)strlen(string)); displayBoard(state.Board); printf("%.2s, %.2s, %.2s\n", state.whitecastle, state.blackcastle, state.enpassant); }
      }
    }
  }  
  fclose(f);

  printf("Ran through %d games, denied %d.\n", iteration, denied);
}
