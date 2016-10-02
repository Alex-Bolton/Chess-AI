chessAI: main.c AIbrain.c chess.c neuralnetwork.c training.c
	gcc -o chessAI main.c AIbrain.c chess.c neuralnetwork.c training.c -lm -lmeschach -lpthread -Wall -g