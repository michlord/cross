#include "solver.h"
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "words.h"

static Board *board = 0;
static Words *words = 0;
static GPtrArray *nodes = 0;
static int finished = 0;

/*prototypes of private functions*/
static void updateAllNodesConstraint();
static void updateNodeConstraint(Node *n);
static void updateIntersectingNodesConstraint(Node *n);
static Node *getMostConstrainedNode();
static void fill(int depth);


/*===public functions====*/

void initSolver(Board* board_, Words *words_)
{
    finished = 0;
	board = board_;
	words = words_;
	nodes = getAllNodesArray(board);
	updateAllNodesConstraint();
}
void deinitSolver()
{
    if(nodes != 0){
        g_ptr_array_free(nodes,TRUE);
    }
}

int solve()
{
	fill(0);
	return finished;
}

int putWord(Node *n,const char *str)
{
    finished = 0;
    n->used = 0;

    /*set original word as unused*/
    char *node_str = getNodeString(board,n);
    if(strchr(node_str,BLANK)==0)
    {
        int len = strlen(node_str);
        GPtrArray *dict = words->words[len];
        for(int i=0;i<dict->len;++i)
        {
            Word *word = g_ptr_array_index(dict,i);
            if(word->used==1 && strcmp(word->str,node_str)==0)
            {
                word->used=0;
            }
        }
    }
    free(node_str);
    /*set all nodes as unused*/
    for(int i=0;i<nodes->len;++i)
    {
        Node *tmp = g_ptr_array_index(nodes,i);
        tmp->used = 0;
    }

    fillNodeString(board,n,str);
    updateAllNodesConstraint();
	return 1;
}

GPtrArray *getClues(Node *n)
{
	//g_ptr_array_add
	GPtrArray *ptr_arr = g_ptr_array_new();
	char *str = getNodeString(board,n);
	int len = strlen(str);
	GPtrArray *dict = words->words[len];
	for(int i=0;i<dict->len;++i)
	{
		Word *word = g_ptr_array_index(dict,i);
		if(word->used==1 || wildcardMatch(word->str,str)==0)
			continue;
		g_ptr_array_add(ptr_arr,word->str);
	}
	free(str);
	return ptr_arr;
}

/*===Private functions===*/

static void updateAllNodesConstraint()
{
	for(int i=0;i<nodes->len;++i)
	{
		Node *n = g_ptr_array_index(nodes,i);
		updateNodeConstraint(n);
	}

}
static void updateNodeConstraint(Node *n)
{
	char *str = getNodeString(board,n);
	int amt_matching = amtMatchingWords(words,str);
	free(str);
	n->constraint = amt_matching;
}
static void updateIntersectingNodesConstraint(Node *n)
{
	GPtrArray *intersecting = getIntersectingNodesArray(board,n);
	for(int i=0;i<intersecting->len;++i)
	{
		Node *n = g_ptr_array_index(intersecting,i);
		updateNodeConstraint(n);
	}
	g_ptr_array_free(intersecting,TRUE);
}
static Node *getMostConstrainedNode()
{
	int min_idx = 0;
	for(int i=0;i<nodes->len;++i)
	{
		Node *n_new = g_ptr_array_index(nodes,i);
		Node *n_min = g_ptr_array_index(nodes,min_idx);
		if(n_new->used == 0 && n_min->used == 1)
			min_idx = i;
		if(n_new->used == 0 && n_new->constraint < n_min->constraint)
			min_idx = i;
	}
	Node *n = g_ptr_array_index(nodes,min_idx);
	if(n->used==1)
		return 0;
	return n;
}
static void fill(int depth)
{
	if(depth >=  nodes->len)
	{
		finished = 1;
		return;
	}
	Node *n = getMostConstrainedNode();
	if(n==0)
	{
		return;
	}
	n->used = 1;
	char *str = getNodeString(board,n);

	/*If word is completely filled by the user leave it as it is*/
	//if(strchr(str,BLANK)!=0) //THIS SHOULD ALSO CHECK THAT THIS WORD IS NOT IN THE DICTIONARY!!!
    //{
    //    fill(depth+1);
    //    return;
    //}


	int len = strlen(str);
	GPtrArray *dict = words->words[len];
	for(int i=0;i<dict->len;++i)
	{
		Word *word = g_ptr_array_index(dict,i);
		if(word->used==1 || wildcardMatch(word->str,str)==0)
			continue;
		fillNodeString(board,n,word->str);
		word->used=1;
		updateIntersectingNodesConstraint(n);
		/*updateAllNodesConstraint();*/
		fill(depth+1);
		word->used=0;
		if(finished==1)
		{
			free(str);
			return;
		}
	}
	fillNodeString(board,n,str);
	/*does it improve performance?*/
	updateIntersectingNodesConstraint(n);
	/*updateAllNodesConstraint();*/
	free(str);
	n->used = 0;
}
