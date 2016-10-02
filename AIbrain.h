#ifndef AIBRAIN_H
#define AIBRAIN_H

double negamaxwithpruning(int depth, struct neuralNetObjects neuralnet, struct learningParameters learningparams, struct stateInfo state, double *alpha, double *beta);
double reward(struct stateInfo state, char player);

void getSoftmaxPolicy(struct neuralNetObjects neuralnet, struct learningParameters learningparams, struct stateInfo state, double policy[64][64], int iteration);
void getGreedyPolicy(struct neuralNetObjects neuralnet, struct learningParameters learningparams, struct stateInfo state, double policy[64][64]);
void getRandomPolicy(struct stateInfo state, double policy[64][64]);
void chooseActionFromPolicy(double policy[64][64], int action[2]);
int rolldie(double probability);

int AIplayRandom(struct neuralNetObjects neuralnet, struct learningParameters learningparams, char AIcolour);
void playHumanAgainstAI(struct neuralNetObjects neuralnet, struct learningParameters learningparams);

#endif
