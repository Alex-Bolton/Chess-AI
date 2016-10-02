#ifndef NEURALNETWORK_H /* This stops the functions being defined twice by defining the NEURALNETWORK_H - if it already exists, we skip this. */
#define NEURALNETWORK_H

void backpropagation(struct neuralNetObjects neuralnet);
void forwardspropagation(struct neuralNetObjects neuralnet);

void convertFENtoVector(char *FEN, VEC *INPUT);
void convertStatetoVector(struct stateInfo state, VEC *INPUT);

#endif /* NEURALNETWORK_H */
