#include "board.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Node empty_node = {{-1,-1,ACROSS},0};


Board *newBoard()
{
	return (Board*)malloc(sizeof(Board));
}
void freeBoard(Board *board)
{
	for(int i=0;i<2;++i)
		for(int j=0;j<board->size_y;++j)
			for(int k=0;k<board->size_x;++k)
				free(board->nodes[i][j][k]);
	free(board);
}

void initBoard(Board *board,int *data,int size_x,int size_y)
{
	if(size_x > MAX_WORD_LEN || size_y > MAX_WORD_LEN)
	{
		fprintf(stderr,"Error in initBoard(), size exceedes MAX_WORD_LEN "
				"size x: %d size y: %d\n",size_x,size_y);
		exit(1);
	}
	for(int i=0;i<size_y;++i)
	{
		for(int j=0;j<size_x;++j)
		{
			int index = i*size_x+j;
			board->data[i][j] = tolower(data[index]);
		}
	}
	board->size_x = size_x;
	board->size_y = size_y;
	for(int i=0;i<2;++i)
		for(int j=0;j<board->size_y;++j)
			for(int k=0;k<board->size_x;++k)
				board->nodes[i][j][k]=0;
}
char *getNodeString(Board *board,Node *node)
{
	static char buff[MAX_WORD_LEN+1];
	int len=0;
	Cord cord = node->cord;
	while(1)
	{
		if(cord.x<0||cord.x>=board->size_x||cord.y<0||cord.y>=board->size_y)
			break;
		if(board->data[cord.y][cord.x]==SOLID)
			break;
		buff[len] = board->data[cord.y][cord.x];
		++len;
		cord = nextCord(cord);
	}
	buff[len]='\0';
	++len;
	char *str = (char*)malloc(sizeof(char)*len);
	memcpy(str,buff,len);
	return str;
}
void fillNodeString(Board *board,Node *node,const char *str)
{
	Cord cord = node->cord;
	while(1)
	{
		if(cord.x<0||cord.x>=board->size_x||cord.y<0||cord.y>=board->size_y)
			break;
		if(board->data[cord.y][cord.x]==SOLID)
			break;
		if(*str==0)
			break;
		board->data[cord.y][cord.x] = tolower(*str);
		++str;
		cord = nextCord(cord);
	}
}
void clearBoardCharAt(Board *board,int x,int y)
{
	if(x<0||x>=board->size_x||y<0||y>=board->size_y)
		return;
	if(board->data[y][x]!=SOLID)
		board->data[y][x]=BLANK;
}
void updateNodes(Board *board)
{
	for(int i=0;i<board->size_y;++i)
	{
		for(int j=0;j<board->size_x;++j)
		{
			free(board->nodes[DOWN][i][j]);
			free(board->nodes[ACROSS][i][j]);
			board->nodes[DOWN][i][j] = 0;
			board->nodes[ACROSS][i][j] = 0;
		}
	}

	for(int i=0;i<board->size_y;++i)
	{
		for(int j=0;j<board->size_x;++j)
		{
			if(board->data[i][j]!=SOLID)
			{
				/*ACROSS*/
				if((j==0 || board->data[i][j-1]==SOLID) && (j<board->size_x-1 && board->data[i][j+1]!=SOLID))
				{
					Node *n_ptr = (Node*)malloc(sizeof(Node));
					n_ptr->cord.x = j;
					n_ptr->cord.y = i;
					n_ptr->cord.dir = ACROSS;
					n_ptr->used = 0;
					n_ptr->constraint = 0;
					board->nodes[ACROSS][i][j] = n_ptr;
				}
				/*DOWN*/
				if((i==0 || board->data[i-1][j]==SOLID) && (i<board->size_y-1 && board->data[i+1][j]!=SOLID))
				{
					Node *n_ptr = (Node*)malloc(sizeof(Node));
					n_ptr->cord.x = j;
					n_ptr->cord.y = i;
					n_ptr->cord.dir = DOWN;
					n_ptr->used = 0;
					n_ptr->constraint = 0;
					board->nodes[DOWN][i][j] = n_ptr;
				}
			}
		}
	}
}

Cord nextCord(Cord cord)
{
	if(cord.dir == ACROSS)
		++cord.x;
	else
		++cord.y;
	return cord;
}
Cord prevCord(Cord cord)
{
	if(cord.dir == ACROSS)
		--cord.x;
	else
		--cord.y;
	return cord;
}

void fPrintBoard(const Board *board,FILE *fh)
{
	for(int i=0;i<board->size_y;++i)
	{
		for(int j=0;j<board->size_x;++j)
		{
			fputc(board->data[i][j],fh);
		}
		fputc('\n',fh);
	}
}


GPtrArray *getAllNodesArray(Board *board)
{
	GPtrArray *ptr_arr = g_ptr_array_new();
	for(int i=0;i<board->size_y;++i)
	{
		for(int j=0;j<board->size_x;++j)
		{
			Node *ptr_down = board->nodes[DOWN][i][j];
			Node *ptr_across = board->nodes[ACROSS][i][j];
			if(ptr_down != 0)
			{
				g_ptr_array_add(ptr_arr,ptr_down);
			}
			if(ptr_across != 0)
			{
				g_ptr_array_add(ptr_arr,ptr_across);
			}
		}
	}
	return ptr_arr;
}

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
		fprintf(stderr,"data out of memory in readBoardDataFromFile(), size_x was: %d, size_y was: %d\n",size_x,size_y);
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
void saveBoardToFile(const Board *board,const char *filename)
{
    FILE *fd=0;
	fd=fopen(filename,"w");
	if(fd==0)
	{
		fprintf(stderr,"file open error (%s) in saveBoardToFile()",filename);
		exit(1);
	}
	fprintf(fd,"%d %d\n",board->size_x,board->size_y);
	fPrintBoard(board,fd);
	fclose(fd);
}

Node *findNodeAt(Board *board,int x,int y,enum direction dir)
{
	if(x<0||x>=board->size_x||y<0||y>=board->size_y)
		return 0;
	if(board->data[y][x]==SOLID)
		return 0;
	Cord cord;
	cord.x= x;
	cord.y= y;
	cord.dir = dir;
	while(1)
	{
		if(cord.x<0||cord.x>=board->size_x||cord.y<0||cord.y>=board->size_y)
			break;
		if(board->data[cord.y][cord.x]==SOLID)
			break;
		cord = prevCord(cord);
	}
	cord = nextCord(cord);
	Node *ptr = board->nodes[cord.dir][cord.y][cord.x];
	return ptr;
}


GPtrArray *getIntersectingNodesArray(Board *board,Node *node)
{
	GPtrArray *ptr_arr = g_ptr_array_new();
	Cord cord = node->cord;
	enum direction opposite_dir = OPPOSITE_DIR(cord.dir);
	while(1)
	{
		if(cord.x<0||cord.x>=board->size_x||cord.y<0||cord.y>=board->size_y)
			break;
		if(board->data[cord.y][cord.x]==SOLID)
			break;

		Node *intersection_node = findNodeAt(board,cord.x,cord.y,opposite_dir);
		if(intersection_node!=0)
			g_ptr_array_add(ptr_arr,intersection_node);
		cord = nextCord(cord);
	}
	return ptr_arr;
}



