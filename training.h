#ifndef TRAINING_H
#define TRAINING_H

void trainAI(int menuoption);

void learnFromAgainstSelf(struct neuralNetObjects neuralnetobjs, struct learningParameters learningparams, int iteration);
void learnFromWholeGame(struct neuralNetObjects neuralnet, struct learningParameters learningparams, struct stateInfo stateList[1000], int length, char outcome[20], int iteration);

void initialiseLearningParameters(struct learningParameters *learningparams);
void initialiseNeuralNetObjects(struct neuralNetObjects *neuralnet, int load);
void saveWeights(struct neuralNetObjects neuralnet, int iteration);
void loadWeights(struct neuralNetObjects *neuralnet, int *iteration);
void loadRandomWeights(struct neuralNetObjects *neuralnet);


void *AIplayRandomMultithread(void *judgeperfvoid);
void judgePerformance(struct neuralNetObjects neuralnet, struct learningParameters learningparams, int iteration);
struct neuralNetObjects copyneuralnetstruct(struct neuralNetObjects oldnet);

void readfrompgn(struct neuralNetObjects neuralnet, struct learningParameters learningparams, int iteration);

#endif
