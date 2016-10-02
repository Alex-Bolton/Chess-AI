/* The AI Brain module. Includes functions for decision making based upon the values of the next possible states. This is done via the neural network and a negamax algorithm. */

#include "main.h"

double negamaxwithpruning(int depth, struct neuralNetObjects neuralnet, struct learningParameters learningparams, struct stateInfo state, double *alpha, double *beta) {
  /* We will do negamax wil alpha-beta pruning. */
  /* Alpha acts as the lower bound, or the best of the worst scores we've found. */
  /* Beta acts as the upper bound, or the worst of the best scores we've found. */
  int i, j;
  double currentValue, bestValue;
  int actions[64][64];
  struct stateInfo nextstate;
  char startcell[2], endcell[2];
  bestValue = -100;

  if (isgameover(state) != 0) { /* printf("Getting reward\n"); */ return reward(state, state.player); }
  if (depth == 1) {
    /* printf("Converting to vector\n"); */
    convertStatetoVector(state, neuralnet.LAYERS[0]);
    /* printf("Forward proping\n"); */
    forwardspropagation(neuralnet);
    /* printf("Forward proped\n"); */
    return neuralnet.LAYERS[neuralnet.layernumber-1]->ve[0]*learningparams.timediscount;
  }
  getallpossiblemoves(state, actions);
  for (i = 0; i < 64; i++) {
    for (j = 0; j < 64; j++) {
      if (actions[i][j] == 0) {
	continue;
      }
      startcell[0] = 'a' + i % 8;
      startcell[1] = '1' + i / 8;
      endcell[0] = 'a' + j % 8;
      endcell[1] = '1' + j / 8;
      nextstate = makemove(state, startcell, endcell);
      *alpha *= -1; /* We feed in -alpha and -beta to the function. */
      *beta *= -1;
      currentValue = -negamaxwithpruning(depth - 1, neuralnet, learningparams, nextstate, beta, alpha); /* Puts the stuff into the point of view of opponent. */
      *alpha *= -1;
      *beta *= -1;
      if (currentValue > bestValue) {
	bestValue = currentValue;
      }
      if (currentValue > *alpha) {
	*alpha = currentValue;
      }
      if (*alpha >= *beta) {
	return *beta;
      }
    }
  }
  
  convertStatetoVector(state, neuralnet.LAYERS[0]); /* Now we try to update the neural network to fit to this value we have found. */
  forwardspropagation(neuralnet);
  neuralnet.DESIREDOUTPUT->ve[0] = bestValue;
  backpropagation(neuralnet);

  return bestValue*learningparams.timediscount;
}



double reward(struct stateInfo state, char player) {
  if (ischeckmate(state) == 1 && player == state.player) {
    return -1;
  }
  else if (ischeckmate(state) == 1 && player != state.player) {
    return 1;
  }
  else {
    return 0;
  }
}








void getSoftmaxPolicy(struct neuralNetObjects neuralnet, struct learningParameters learningparams, struct stateInfo state,  double policy[64][64], int iteration) {
  /* Fills in the policy for a given state - gives probabilities for each action. Softmax. */
  int i, j;
  double QValues[64][64];
  double sumQ;
  double basetemperature = 1.0;
  double temperaturedecay = 0.001;
  double temperature;
  struct stateInfo nextstate;
  char startcell[2], endcell[2];
  double alpha, beta;
  int actions[64][64];

  getallpossiblemoves(state, actions);

  temperature = basetemperature / (1 + temperaturedecay * iteration); /* This is the 'temperature' - it affects how much softmax will choose the actions judged best. */

  sumQ = 0;
  for (i = 0; i < 64; i++) {
    for (j = 0; j < 64; j++) {
      if (actions[i][j] == 0) { continue; }
      alpha = -100;
      beta = 100;
      startcell[0] = 'a' + i % 8;
      startcell[1] = '1' + i / 8;
      endcell[0] = 'a' + j % 8;
      endcell[1] = '1' + j / 8;
      nextstate = makemove(state, startcell, endcell);
      QValues[i][j] = -negamaxwithpruning(learningparams.searchdepth, neuralnet, learningparams, nextstate, &alpha, &beta);
      sumQ += exp(QValues[i][j]/temperature);
    }
  }

  for (i = 0; i < 64; i++) { /* Epsilon greedy policy - will be replaced with softmax soon */
    for (j = 0; j < 64; j++) {
      policy[i][j] = 0;
      if (actions[i][j] == 0) { continue; }
      policy[i][j] = exp(QValues[i][j]/temperature) / sumQ;
    }
  }
}



void getGreedyPolicy(struct neuralNetObjects neuralnet, struct learningParameters learningparams, struct stateInfo state, double policy[64][64]) { 
  /* Simply chooses the action with the best value. */
  struct stateInfo nextstate;
  double bestValue, currentValue, alpha, beta;
  char startcell[2], endcell[2];
  int bestaction[2];
  int actions[64][64];
  int i, j;

  getallpossiblemoves(state, actions);
  bestValue = -100;
  for (i = 0; i < 64; i++) {
    for (j = 0; j < 64; j++) {
      policy[i][j] = 0;
      if (actions[i][j] == 0) {
	continue;
      }
      alpha = -100;
      beta = 100;
      startcell[0] = 'a' + i % 8;
      startcell[1] = '1' + i / 8;
      endcell[0] = 'a' + j % 8;
      endcell[1] = '1' + j / 8;
      nextstate = makemove(state, startcell, endcell);
      /* printf("Considering move %.2s to %.2s.\n", startcell, endcell); */
      currentValue = -negamaxwithpruning(learningparams.searchdepth, neuralnet, learningparams, nextstate, &alpha, &beta);
      /* printf("Considering move %.2s to %.2s. Value = %lf, while bestValue = %lf.\n", startcell, endcell, currentValue, bestValue); */
      if (currentValue > bestValue) {
	/* printf("New bestvalue.\n"); */
	bestValue = currentValue;
	bestaction[0] = i;
	bestaction[1] = j;
      }
    }
  }
  policy[bestaction[0]][bestaction[1]] = 1;
}



void getRandomPolicy(struct stateInfo state, double policy[64][64]) {
  /* Gets a random policy from a state; gives an equal chance of all possible moves from being chosen. */
  int i, j;
  int totalactions;
  int actions[64][64];
  
  getallpossiblemoves(state, actions);

  totalactions = 0;
  for (i = 0; i < 64; i++) {
    for (j = 0; j < 64; j++) {
      if (actions[i][j] == 1) {
	totalactions += 1;
      }
    }
  }

  for (i = 0; i < 64; i++) {
    for (j = 0; j < 64; j++) {
      if (actions[i][j] == 1) {
	policy[i][j] = (double) 1 / totalactions;
      }
      else {
	policy[i][j] = 0;
      }
    }
  }
}



void chooseActionFromPolicy(double policy[64][64], int action[2]) {
  /* Chooses an action based on the values in the behaviour policy. */
  double probability = 1;
  int i, j;

  /* We 'roll a die' to see if we take the action.*/
  /* The probability variable stores the probabilities we have already taken. */
  /* It's basically a probability tree. */
  for (i = 0; i < 64; i++) {
    for (j = 0; j < 64; j++) {
      if (rolldie(policy[i][j] / probability) == 1) {
	action[0] = i;
	action[1] = j;
	return;
      }
      else {
	probability = probability*(1 - (policy[i][j] / probability));
      }
    }
  }
  action[0] = i;
  action[1] = j;
}



int rolldie(double probability) {
  /* Rolls a 100000-sided dice*/
  /* Returns 1 if the dice gives a number within the bounds of 0 and probability*100000. */
  /* Chance should be a probability between 0 and 1, but it will work fine outside of these bounds. */
  int number;

  number = rand() % 100000;
  if (number < probability*100000) {
    return 1;
  }
  else {
    return 0;
  }
}






int AIplayRandom(struct neuralNetObjects neuralnet, struct learningParameters learningparams, char AIcolour) {
  struct stateInfo state;
  double policy[64][64];
  int action[2];
  char startcell[2], endcell[2];

  state = setupBoard();
  while (isgameover(state) == 0) {
    /* displayBoard(state.Board); */
    if (state.player == AIcolour) {
      /* printf("AI's turn.\n"); */
      getGreedyPolicy(neuralnet, learningparams, state, policy); 
      /* printf("Got Greedy.\n"); */
      /* getSoftmaxPolicy(neuralnet, learningparams, state, policy, 0.1); */
      /* getRandomPolicy(state, policy); */
      chooseActionFromPolicy(policy, action);
      startcell[0] = 'a' + action[0] % 8;
      startcell[1] = '1' + action[0] / 8;
      endcell[0] = 'a' + action[1] % 8;
      endcell[1] = '1' + action[1] / 8;
      state = makemove(state, startcell, endcell);
    }
    else {
      getRandomPolicy(state, policy);
      chooseActionFromPolicy(policy, action);
      startcell[0] = 'a' + action[0] % 8;
      startcell[1] = '1' + action[0] / 8;
      endcell[0] = 'a' + action[1] % 8;
      endcell[1] = '1' + action[1] / 8;
      state = makemove(state, startcell, endcell);
    }
  }
  /* displayBoard(state.Board); */
  
  return reward(state, AIcolour);
}


void playHumanAgainstAI(struct neuralNetObjects neuralnet, struct learningParameters learningparams) {
  struct stateInfo state;
  char startcell[2], endcell[2];
  char promotion;
  char input[1];
  char humancolour, AIcolour;
  double policy[64][64];
  int bestaction[2];

  printf("Enter the colour you would like to be: (w or b): ");
  getinputofsize(input, 1);
  if (tolower(input[0]) == 'w') {
    humancolour = 'w';
    AIcolour = 'b';
  }
  else if (tolower(input[0]) == 'b') {
    humancolour = 'b';
    AIcolour = 'w';
  }
  else {
    printf("Invalid colour entered.\n");
    return;
  }

  state = setupBoard();

  while (isgameover(state) == 0) {
    displayBoard(state.Board);
    printf("Whitecastle: %.2s, Blackcastle: %.2s, enpassant: %.2s\n", state.whitecastle, state.blackcastle, state.enpassant);
    if (state.player == humancolour) {
      displayPrompt(state);
      getmove(state, startcell, endcell, &promotion); /* Get move from player; also checks if move is valid. */
      state = makemovewithPawnPromote(state, startcell, endcell, promotion);
    }
    else if (state.player == AIcolour) {
      getGreedyPolicy(neuralnet, learningparams, state, policy);
      chooseActionFromPolicy(policy, bestaction);

      startcell[0] = 'a' + bestaction[0] % 8;
      startcell[1] = '1' + bestaction[0] / 8;
      endcell[0] = 'a' + bestaction[1] % 8;
      endcell[1] = '1' + bestaction[1] / 8;
      /* printf("Startcell: %.2s, endcell: %.2s \n", startcell, endcell); */
      state = makemove(state, startcell, endcell);
    }
    else { printf("Error: Player is neither the human nor the AI.\n"); }
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
