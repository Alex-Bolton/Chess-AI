#include "main.h"

void backpropagation(struct neuralNetObjects neuralnet) {
  /* Backprop algorithm */
  VEC *TEMPONE=VNULL, *TEMPTWO=VNULL, *TEMPTHREE=VNULL;
  MAT *DELTAMAT=MNULL, *LAYERMAT=MNULL, *WEIGHTMAT=MNULL;
  MAT *TEMPMAT=MNULL;
  int i;
  VEC **DELTAS;
  DELTAS = (VEC **) malloc ((neuralnet.layernumber - 1) * sizeof(VEC *)); /* There is no delta vector for the first layer, because backprop dinnae need it. This causes us to write -1s when getting the index of DELTAS. */
  for (i = 1; i < neuralnet.layernumber-1; i ++) {
    DELTAS[i-1] = v_get(neuralnet.LAYERS[i]->dim - 1); /* Setting up sizes of delta vectors as the size of their respective vectors. [i-1] is due to the fact that there is no delta vector for the first layer, so they all start a bit later. dim - 1 is cutting off bias.*/
  }
  DELTAS[neuralnet.layernumber-1-1] = v_get(neuralnet.LAYERS[neuralnet.layernumber-1]->dim);
  

  /* Calculating deltalast = (a(last) - y). This can be gained with an obscure cost function.*/
  v_sub(neuralnet.LAYERS[neuralnet.layernumber-1], neuralnet.DESIREDOUTPUT, DELTAS[neuralnet.layernumber-1-1]);

  /* Calculating deltal = (Weightsl * delta(l+1)) dot (1 - a(l)^2) */
  for (i = neuralnet.layernumber - 2; i > 0; i--) {      
    TEMPONE = v_get(neuralnet.LAYERS[i]->dim);
    TEMPTWO = v_get(neuralnet.LAYERS[i]->dim);
    mv_mlt(neuralnet.WEIGHTS[i], DELTAS[i+1-1], TEMPONE); /* Because the deltas are all offset by 1 index, the -1 is present for clarity - it is purposely unsimplified. */
    /* TEMPONE now stores Weightsl * delta(l+1) */
    v_star(neuralnet.LAYERS[i], neuralnet.LAYERS[i], TEMPTWO);
    TEMPTHREE = v_get(neuralnet.LAYERS[i]->dim);
    v_ones(TEMPTHREE);
    v_sub(TEMPTHREE, TEMPTWO, TEMPTHREE); /* TEMPTHREE now stores (1 - a(l)^2) */
    /* Finally, we find the Hadamard product between these two things to find the next delta */
    v_star(TEMPONE, TEMPTHREE, TEMPTWO);
    /* Then we snip off the bias unit and put the result into DELTAS[i-1] */
    v_move(TEMPTWO, 0, neuralnet.LAYERS[i]->dim - 1, DELTAS[i-1], 0);

    V_FREE(TEMPONE);
    V_FREE(TEMPTWO);
    V_FREE(TEMPTHREE);
  }


  for (i = 0; i < neuralnet.layernumber - 1; i++) { /* Now calculating the deltaWeights - the actual gradient things. */
    DELTAMAT = m_get(DELTAS[i+1-1]->dim, 1);
    vm_move(DELTAS[i+1-1], 0, DELTAMAT, 0, 0, DELTAMAT->m, 1);   /* Moving the vectors of DELTA and LAYER into a matix of equal size so that we can do some operations on them. */
    LAYERMAT = m_get(neuralnet.LAYERS[i]->dim, 1);
    vm_move(neuralnet.LAYERS[i], 0, LAYERMAT, 0, 0, neuralnet.LAYERS[i]->dim, 1);
    WEIGHTMAT = m_get(neuralnet.WEIGHTS[i]->m, neuralnet.WEIGHTS[i]->n);
    mmtr_mlt(LAYERMAT, DELTAMAT, WEIGHTMAT);   /* deltaWl = delta(l+1) * (a(l))^T  (This also covers the bias unit deltaBl = delta(l+1) ) */
    
    TEMPMAT = m_get(neuralnet.WEIGHTS[i]->m, neuralnet.WEIGHTS[i]->n);
    m_move(neuralnet.WEIGHTS[i], 0, 0, neuralnet.WEIGHTS[i]->m - 1, neuralnet.WEIGHTS[i]->n, TEMPMAT, 0, 0); /* We do m-1 so that this is not applied to bias. */
    sm_mlt(neuralnet.weightregularisation, TEMPMAT, TEMPMAT); /* Applying weight regularisation */
    m_add(WEIGHTMAT, TEMPMAT, WEIGHTMAT); /* deltaWl + regularise*Wl. */
    sm_mlt(-neuralnet.learningrate, WEIGHTMAT, WEIGHTMAT);  /* Applying learning rate. */
    sm_mlt(neuralnet.momentum, neuralnet.LASTWEIGHTCHANGES[i], neuralnet.LASTWEIGHTCHANGES[i]); /* Applying momentum */
    m_add(WEIGHTMAT, neuralnet.LASTWEIGHTCHANGES[i], WEIGHTMAT);

    m_add(neuralnet.WEIGHTS[i], WEIGHTMAT, neuralnet.WEIGHTS[i]); /* Applying the change */

    neuralnet.LASTWEIGHTCHANGES[i] = m_copy(WEIGHTMAT, neuralnet.LASTWEIGHTCHANGES[i]);

    M_FREE(DELTAMAT);
    M_FREE(LAYERMAT);
    M_FREE(WEIGHTMAT);
    M_FREE(TEMPMAT);
  }
  

  for (i = 0; i < neuralnet.layernumber - 1; i++) {
    V_FREE(DELTAS[i]);
  }
  free(DELTAS);
}



void forwardspropagation(struct neuralNetObjects neuralnet){
  int i;
  /* Forwards Propagation Algorithm */
  /* ACTLAYERTWO is LAYERTWO after the 'activation function' has been applied. Tanh is the activation function. */
  /* LAYERTHREE is OUTPUT before tanh */
 
  for (i = 1; i < neuralnet.layernumber; i++) {
    vm_mlt(neuralnet.WEIGHTS[i-1], neuralnet.LAYERS[i-1], neuralnet.LAYERS[i]); /* Doing Weight^T x Layer to get next layer numbers before activation */
    if (i != neuralnet.layernumber-1) {
      v_map(tanh, neuralnet.LAYERS[i], neuralnet.LAYERS[i]); /* Applying activation function tanh to all numbers in next layer */
      neuralnet.LAYERS[i] = v_resize(neuralnet.LAYERS[i], neuralnet.WEIGHTS[i]->m); /* Adding bias unit */
      neuralnet.LAYERS[i]->ve[neuralnet.LAYERS[i]->dim - 1] = 1;
    }
    else {
      v_map(tanh, neuralnet.LAYERS[i], neuralnet.LAYERS[i]); /* Can be changed to sigmoid if desired */
    }
  }
}







void convertFENtoVector(char *FEN, VEC *INPUT) {
  struct stateInfo state;

  state = convertFENtoState(FEN); 
  convertStatetoVector(state, INPUT);
}



void convertStatetoVector(struct stateInfo state, VEC *INPUT) {
  char piece;
  int pieceowner, piecetype;
  int x, y;

  for (x = 0; x < 8; x++) {
    for (y = 0; y < 8; y++) {
      piece = state.Board[x][y];
      if (piece == toupper(piece)) {
	pieceowner = 0;
      }
      else if (piece == tolower(piece)) {
	pieceowner = 1;
      }
      switch (toupper(piece)) {
      case 'P':
	piecetype = 0;
	break;
      case 'R':
	piecetype = 1;
	break;
      case 'N':
	piecetype = 2;
	break;
      case 'B':
	piecetype = 3;
	break;
      case 'K':
	piecetype = 4;
	break;
      case 'Q':
	piecetype = 5;
	break;
      default:
	continue;
      }
      INPUT->ve[384*pieceowner + 64*piecetype + 8*y + x] = 1;
    }
  }

  if (state.player == 'w') {
    INPUT->ve[768] = 1;
  }
  else if (state.player == 'b') {
    INPUT->ve[769] = 1;
  }
  
  if (state.whitecastle[0] == 'K') {
    INPUT->ve[770] = 1;
  }
  if (state.whitecastle[0] == 'Q' || state.whitecastle[1] == 'Q') {
    INPUT->ve[771] = 1;
  }
  if (state.blackcastle[0] == 'k') {
    INPUT->ve[772] = 1;
  }
  if (state.blackcastle[0] == 'q' || state.blackcastle[1] == 'q') {
    INPUT->ve[773] = 1;
  }

  if (state.enpassant[0] != '-') {
    INPUT->ve[774 + (state.enpassant[0] - 'a') + 8*(state.enpassant[1] - '1')] = 1;
  }

  /* Vector size 838 */
  /* The half move clock can only cause a draw; it is not relevant. */
  /* The full move clock has no effect on the game, so it is not turned into the state. */

  /* v_output(INPUT); */
}
