#ifndef SOLVER_H_
#define SOLVER_H_

#include "board.h"
#include "words.h"
#include <glib.h>


/*initSolver() has to be called before any other solver functions*/
void initSolver(Board* board_, Words *words_);
void deinitSolver();
int solve();
int putWord(Node *n,const char *str);
/*array should be cleared by the user using g_ptr_array_free(name,TRUE)*/
GPtrArray *getClues(Node *n);


#endif
