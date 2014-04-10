#include <stdio.h>
#include <locale.h>
#include "words.h"
#include <glib.h>
#include "board.h"
#include <string.h>
#include <stdlib.h>
#include "solver.h"
/*
int test1()
{
	Words *words = newWords();
	char str1[] = "Michael\0Six\0Four\0Five\0Word\0SomeOtherWord\0Hello\0"
					"AutomaticNullTermination\0Youlo\0HAHA\0XO\0Moje";
	initWords(words,str1,sizeof(str1)/sizeof(char));
	GPtrArray *arr = words->words[4];
	printf("Words of length four are:\n");
	for(int i=0;i<arr->len;++i)
	{
		Word *ptr = g_ptr_array_index(arr,i);
		printf("%s, ",ptr->str);
	}
	printf("\n");
	freeWords(words);
	return 0;
}
int test2()
{
	WordsData data = wordsDataFromFile("dict/eng.lst");
	Words *words = newWords();
	initWords(words,data.data,data.size);
	GPtrArray *arr = words->words[4];
	printf("Words of length four are:\n");
	for(int i=0;i<arr->len;++i)
	{
		Word *ptr = g_ptr_array_index(arr,i);
		printf("%s, ",ptr->str);
	}
	printf("\n");
	freeWords(words);
	return 0;
}



int test3()
{
	char *str1 = "mi____";
	char *str2 = "michal";
	char *str3 = "adrian";
	char *str4 = "miroslawa";
	char *str5 = "michalina";
	
	if(wildcardMatch(str1,str2) == 0)
		return 1;
	if(wildcardMatch(str1,str3) == 1)
		return 2;
	if(wildcardMatch(str1,str4) == 1)
		return 3;
	if(wildcardMatch(str1,str5) == 1)
		return 4;
	return 0;
	
}

int test4()
{
	Board *board = newBoard();
	static int data[4*3] = {
		'#','_','_','#',
		'#','#','#','#',
		'#','_','_','#'
	};
	initBoard(board,data,4,3);
	fPrintBoard(board,stdout);
	return 0;
}

int test5()
{
	Board *board = newBoard();
	static int data[5*5] = {
		'#','_','_','_','#',
		'#','#','M','A','K',
		'#','_','_','_','O',
		'#','#','_','#','T',
		'#','_','_','_','Y',
	};
	initBoard(board,data,5,5);
	printf("Original board:\n");
	fPrintBoard(board,stdout);
	putchar('\n');
	Node node1 = {{2,1,ACROSS},0};
	Node node2 = {{4,1,DOWN},0};
	char *str1 = getNodeString(board,&node1);
	char *str2 = getNodeString(board,&node2);
	
	char str3[] = "car";
	Node node3 = {{3,0,DOWN},0};
	fillNodeString(board,&node3,str3);
	printf("Board after inserting %s:\n",str3);
	fPrintBoard(board,stdout);
	
	char *str4 = getNodeString(board,&node3);
	
	printf("(2,1,ACROSS) = %s\n(4,1,DOWN) = %s\n"
			"(3,0,DOWN) = %s",str1,str2,str4);
	
	if(strncmp(str3,str4,4)!=0)
		return 1;
	
	return 0;
}
*/
/*
int test6()
{
	Board *board = newBoard();
	static int data[5*5] = {
		'#','_','_','_','#',
		'#','#','M','A','K',
		'#','_','_','_','O',
		'#','#','_','#','T',
		'#','_','_','_','Y',
	};
	initBoard(board,data,5,5);
	printf("Original board:\n");
	fPrintBoard(board,stdout);
	putchar('\n');
	
	printf("Nodes:\n");
	updateNodes(board);
	GPtrArray *nodes = getAllNodesArrayCopy(board);
	for(int i=0;i<nodes->len;++i)
	{
		Node *node_ptr = g_ptr_array_index(nodes,i);
		printf("NODE (%d,%d), dir: %d, used: %d\n",node_ptr->cord.x,node_ptr->cord.y,node_ptr->cord.dir,node_ptr->used);
	}
	g_ptr_array_free(nodes,TRUE);
	return 0;
}

int test7()
{
	Board *board = newBoard();
	static int data[5*5] = {
		'#','_','_','_','#',
		'#','#','M','A','K',
		'#','_','_','_','O',
		'#','#','_','#','T',
		'#','_','_','_','Y',
	};
	initBoard(board,data,5,5);
	printf("Original board:\n");
	fPrintBoard(board,stdout);
	putchar('\n');
	updateNodes(board);
	printf("Simulating mouse click at (4,3)\n");
	Node *node = findNodeAt(board,4,3,DOWN);
	if(node == 0)
		return 1;
	char *str = getNodeString(board,node);
	printf("NODE (%d,%d), dir: %d, used: %d\n",node->cord.x,node->cord.y,node->cord.dir,node->used);
	printf("String: %s",str);
	return 0;
}

int test8()
{
	Board *board = newBoard();
	static int data[5*5] = {
		'#','_','K','L','#',
		'#','#','M','A','K',
		'#','T','O','W','O',
		'#','#','R','#','T',
		'O','S','T','R','Y',
	};
	initBoard(board,data,5,5);
	printf("Original board:\n");
	fPrintBoard(board,stdout);
	putchar('\n');
	updateNodes(board);
	Node *node = findNodeAt(board,4,3,DOWN);
	if(node == 0)
		return 1;
	char * str_orig = getNodeString(board,node);
	GPtrArray *nodes = getIntersectingNodesArray(board,node);
	printf("Nodes intersecting with %s:\n",str_orig);
	for(int i=0;i<nodes->len;++i)
	{
		Node *n = g_ptr_array_index(nodes,i);
		char *str=getNodeString(board,n);
		printf("%s\n",str);
		free(str);
	}
	return 0;
}

int test9()
{
	static char dictionary[] =  "myszyy\0muszyy\0umytyy\0"
		"pyszalkowaty\0dziwny\0pusty\0myszy\0kuszy\0muszy\0reszty\0skarbowy\0skarbyyy\0"
		"bary\0rok\0dziwna\0dziwka\0dziwak\0pycha\0potem\0piotr\0skarbowa\0";
	Words *words = newWords();
	initWords(words,dictionary,sizeof(dictionary)/sizeof(char));
	
	
	static int data[12*12] =
	{
		'#','#','#','#','#','#','#','#','#','#','#','_',
		'#','#','#','#','#','#','#','#','#','#','#','_',
		'#','#','#','#','#','#','#','#','#','#','#','_',
		'#','#','#','#','#','#','#','#','#','#','#','_',
		'#','#','#','_','#','#','_','#','#','#','#','_',
		'p','y','s','z','a','l','k','o','w','a','t','y',
		'_','#','#','_','#','#','_','#','#','#','#','#',
		'_','#','#','_','#','#','_','#','#','#','#','#',
		'_','#','#','_','#','#','b','a','r','y','#','#',
		'_','#','#','_','#','#','_','#','_','#','#','#',
		'#','#','#','#','#','#','_','#','_','#','#','#',
		'#','#','#','#','#','#','_','#','#','#','#','#',
	};
	Board *board = newBoard();
	initBoard(board,data,12,12);
	updateNodes(board);
	printf("Board:\n");
	fPrintBoard(board,stdout);
	
	Node *selected_node = findNodeAt(board,1,5,ACROSS);
	if(selected_node==0)
		return 1;
	char *selected_node_str = getNodeString(board,selected_node);
	printf("Selected node at(%d,%d) that contains: %s\n",selected_node->cord.x,selected_node->cord.y,selected_node_str);
	
	printf("These are the intersections:\n");
	GPtrArray *intersecting_nodes = getIntersectionsArray(board,selected_node);
	for(int i=0;i<intersecting_nodes->len;++i)
	{
		Intersection *intersection = g_ptr_array_index(intersecting_nodes,i);
		char *intersection_str = getNodeString(board,intersection->node);
		intersection->amt_matching = amtMatchingWords(words,intersection_str);
		printf("%s, amt:%d\n",intersection_str,intersection->amt_matching);
	}
	printf("Sorting intersections:\n");
	g_ptr_array_sort(intersecting_nodes,(GCompareFunc)compareIntersections);
	for(int i=0;i<intersecting_nodes->len;++i)
	{
		Intersection *intersection = g_ptr_array_index(intersecting_nodes,i);
		char *intersection_str = getNodeString(board,intersection->node);
		printf("%s, amt:%d\n",intersection_str,intersection->amt_matching);
	}
	
	
	return 0;
}

int test10()
{
	return 0;
}

int (*tests[])() = {test1,test2,test3,test4,test5,test6,test7,test8,test9,test10,0};
char test_names[][256] = {"test words","test words from file","test wildcardMatch()",
	"initBoard()","retrieving and filling strings using board functions",
	"populating board with nodes","finding node from click action",
	"intersecting nodes","returning intersections and calculating amt of matching words",
	"placeholder"
	
};
*/
/*
int (*tests[])() = {test1,test2,test3,test4,test5,0};
char test_names[][256] = {"test words","test words from file","test wildcardMatch()",
	"initBoard()","retrieving and filling strings using board functions",
	"populating board with nodes","finding node from click action",
	"intersecting nodes","returning intersections and calculating amt of matching words",
	"placeholder"
	
};
void runTests()
{
	int res = 0;
	for(int i=0;tests[i]!=0;++i)
	{
		printf("----Running: %s #%d----\n",test_names[i],i);
		res |= tests[i]();
		if(res!=0)
		{
			printf("\nTest failed. Return value is: %d\n",res);
		}
		printf("\n----End of test----\n");
	}
	if(res!=0)
	{
		printf("Some of the tests failed.\n");
	}
	else
	{
		printf("All tests finished successfully\n");
	}
	
}
*/
int * readBoardDataFromFile(const char *filename,int *sx,int *sy)
{
	FILE *fd=0;
	fd=fopen(filename,"r");
	if(fd==0)
	{
		fprintf(stderr,"file open error (%s) in fillBoardFromFile()",filename);
		exit(1);
	}
	
	int size_x,size_y;

	fscanf(fd,"%d%d",&size_x,&size_y);

	
	int *data = (int*)malloc(sizeof(int)*size_x*size_y);
	if(data==0)
	{
		fprintf(stderr,"data out of memory in readBoardDataFromFile()");
		exit(1);
	}

	for(int i=0;i<size_y*size_x;++i)
	{
		int tmp = fgetc(fd);
		while(tmp == '\n'||tmp=='\r')
			tmp = fgetc(fd);
		if(tmp == EOF)
			break;
		data[i]=tmp;
	}
		
	fclose(fd);
	*sx = size_x;
	*sy= size_y;
	return data;	
}
void fPrint2DArray(int *arr,int size_x,int size_y,FILE *fh)
{
	for(int i=0;i<size_y;++i)
	{
		for(int j=0;j<size_x;++j)
		{
			int idx = i*size_x+j;
			fputc(arr[idx],fh);
		}
		fputc('\n',fh);
	}
}
int main()
{
	setlocale(LC_ALL,"pl_PL");
	//runTests();
	
	WordsData words_data = wordsDataFromFile("dict/eng.lst");
	Words *words = newWords();
	initWords(words,words_data.data,words_data.size);
	free(words_data->data);
	/*
	static int board_data[12*12] =
	{
		'#','#','#','#','#','#','#','#','#','#','#','_',
		'#','#','#','#','#','#','#','#','#','#','#','_',
		'#','#','#','#','#','#','#','#','#','#','#','_',
		'#','#','#','#','#','#','#','#','#','#','#','_',
		'#','#','#','_','#','#','_','#','#','#','#','_',
		'_','_','_','_','_','_','_','_','_','_','_','_',
		'_','#','#','_','#','#','_','#','#','#','#','#',
		'_','#','#','_','#','#','_','#','#','#','#','#',
		'_','#','#','_','#','#','_','_','_','_','#','#',
		'_','#','#','_','#','#','_','#','_','#','#','#',
		'#','#','#','#','#','#','_','#','_','#','#','#',
		'#','#','#','#','#','#','_','#','#','#','#','#',
	};
	*/
	/*
	static int board_data[12*12] =
	{
		'#','#','#','#','#','#','#','#','#','#','#','b',
		'#','#','#','#','#','#','#','#','#','#','#','_',
		'#','#','#','#','#','#','#','#','#','#','#','d',
		'#','#','#','#','#','#','#','#','#','#','#','_',
		'#','#','#','_','#','#','_','#','#','#','#','_',
		'p','y','s','z','a',179,'k','o','w','a','t','y',
		'_','#','#','_','#','#','_','#','#','#','#','#',
		'_','#','#','_','#','#','_','#','#','#','#','#',
		'_','#','#','_','#','#','b',185,'d',159,'#','#',
		'_','#','#','_','#','#','_','#','_','#','#','#',
		'#','#','#','#','#','#','_','#','_','#','#','#',
		'#','#','#','#','#','#','_','#','#','#','#','#',
	};
	*/
	int sizex;
	int sizey;
	int *board_data = readBoardDataFromFile("board/15x15.txt",&sizex,&sizey);
	fPrint2DArray(board_data,sizex,sizey,stdout);
	
	Board *board = newBoard();
	initBoard(board,board_data,sizex,sizey);
	updateNodes(board);
	printf("Board size x: %d, y:%d\n",sizex,sizey);
	
	
	initSolver(board,words);
	solve();
	deinitSolver();
	fPrintBoard(board,stdout);
	
	free(board_data);
	freeBoard(board);
	freeWords(words);
	return 0;
}