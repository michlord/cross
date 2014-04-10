#ifndef BOARD_H_
#define BOARD_H_

#include "defines.h"
#include <stdio.h>
#include <glib.h>

enum direction {ACROSS=0, DOWN=1};
#define OPPOSITE_DIR(x) ((x)==ACROSS ? DOWN : ACROSS)


typedef struct cord Cord;
struct cord
{
	int x;
	int y;
	enum direction dir;
};

typedef struct node Node;
struct node
{
	Cord cord;
	int used;
	int constraint;
};

typedef struct board Board;
struct board
{
	int size_x;
	int size_y;
	int data[MAX_WORD_LEN][MAX_WORD_LEN];
	Node *nodes[2][MAX_WORD_LEN][MAX_WORD_LEN]; /*2 nodes for DOWN and ACROSS */
};

/*===Initialisation and deinistialisation===*/
Board *newBoard();
void freeBoard(Board *board);
/*After calling initBoard() int* data can be freed*/
void initBoard(Board *board,int *data,int size_x,int size_y);
/*Has to be called each time board structure is changed*/
void updateNodes(Board *board);
/*User should free returned memory manually, sx and sy are modiffied within function*/
int *readBoardDataFromFile(const char *filename,int *sx,int *sy);
void saveBoardToFile(const Board *board,const char *filename);

/*===Functions for retrieving nodes strings and filling nodes strings===*/
/*user has to free the string*/
char *getNodeString(Board *board,Node *node);
void fillNodeString(Board *board,Node *node,const char *str);

void clearBoardCharAt(Board *board,int x,int y);


/*===Functions for retrieving nodes===*/
GPtrArray *getAllNodesArray(Board *board);
Node *findNodeAt(Board *board,int x,int y,enum direction dir);
GPtrArray *getIntersectingNodesArray(Board *board,Node *node);

/*===Misc. functions===*/
Cord nextCord(Cord cord);
Cord prevCord(Cord cord);
void fPrintBoard(const Board *board,FILE *fh);



#endif
