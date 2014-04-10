#include "words.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
Words *newWords()
{
	Words *words = (Words*)malloc(sizeof(Words));
	words->memory_pool = 0;
	for(int i=0;i<MAX_WORD_LEN;++i)
	{
		words->words[i] = g_ptr_array_new();
	}
	return words;
}

void freeWords(Words *words)
{
	for(int i=0;i<MAX_WORD_LEN;++i)
	{
		/*
		if(words->words[i]==0)
			continue;
		for(int j=0;j<words->words[j]->len;++j)
		{
			Word *word = g_ptr_array_index(words->words[i],j);
			free(word);
		}*/
		g_ptr_array_free(words->words[i], TRUE); /*we have to free the whole table of g_ptr_array one by one*/
	}
	free(words->memory_pool);
}

void initWords(Words *words,const char *data,int size)
{
	words->memory_pool = (char*)malloc(sizeof(char)*size);
	if(words->memory_pool == 0)
	{
		fprintf(stderr,"Out of memory in initWords() trying to allocate %d bytes.\n",(int)sizeof(char)*size);
		exit(1);
	}
	memcpy(words->memory_pool,data,sizeof(char)*size);
	for(int i=0;i<size;++i)
	{
		if(i==0 || words->memory_pool[i-1]=='\0')
		{
			int len = strlen(words->memory_pool+i);
			if(len>MAX_WORD_LEN)
			{
				fprintf(stderr,"Too long word in initWords. Length was: %d characters.\n",len);
				exit(2);
			}
			Word *word = (Word*)malloc(sizeof(Word));
			word->used = 0;
			word->str = words->memory_pool+i;
			g_ptr_array_add(words->words[len], (gpointer)word);
		}
	}
}

WordsData wordsDataFromFile(const char *filename)
{
	WordsData data;
	FILE *fd = 0;
	fd = fopen(filename,"rb");
	if(fd==0)
	{
		fprintf(stderr,"Error opening file: %s in readWordsFromFile()\n",filename);
		exit(1);
	}
	fseek (fd, 0, SEEK_END);   // non-portable
    int size=ftell (fd) + 1;

	rewind (fd);
	char *buff = (char*)calloc(size,sizeof(char));
	if(buff==0)
	{
		fprintf(stderr,"Out of memory for buff in readWordsFromFile()");
		exit(2);
	}
	if(fread(buff,sizeof(char),size-1,fd)!=size-1)
	{
		fprintf(stderr,"Error reading file: %s",filename);
		exit(3);
	}
	for(int i=0;i<size;++i)
	{
		if(buff[i]=='\n')
			buff[i]='\0';
		else
		{
			int ch = tolower(buff[i]);
			buff[i]=ch;
		}
	}
	buff[size-1]=0;
	fclose(fd);
	data.data = buff;
	data.size = size;
	return data;
}

int wildcardMatch(const char *str1, const char *str2)
{
	int i;
	for(i=0;str1[i]!=0 && str2[i]!=0;++i)
	{
		if(str1[i]!=BLANK && str2[i]!=BLANK)
		{
			if(str1[i]!=str2[i])
				return 0;
		}
	}
	return str1[i] == str2[i];
}

int amtMatchingWords(Words *words,const char *str)
{
	int amt=0;
	int len = strlen(str);
	if(len>MAX_WORD_LEN)
	{
		fprintf(stderr,"Too long word in amtMatchingWords. Length was: %d characters.\n",len);
		exit(1);
	}
	GPtrArray *dict = words->words[len];
	for(int i=0;i<dict->len;++i)
	{
		Word *word = g_ptr_array_index(dict,i);
		if((word->used==0) && (wildcardMatch(str,word->str)==1))
		{
			++amt;
		}
	}
	return amt;
}

GPtrArray *arrayOfMatchingWords(Words *words,const char *str)
{
    GPtrArray *ptr_arr = g_ptr_array_new();
    int len = strlen(str);
	if(len>MAX_WORD_LEN)
	{
		fprintf(stderr,"Too long word in amtMatchingWords. Length was: %d characters.\n",len);
		exit(1);
	}
	GPtrArray *dict = words->words[len];
	for(int i=0;i<dict->len;++i)
	{
		Word *word = g_ptr_array_index(dict,i);
		if((word->used==0) && (wildcardMatch(str,word->str)==1))
		{
			g_ptr_array_add(ptr_arr,word->str);
		}
	}
	return ptr_arr;
}

int doesHaveMatch(Words *words,const char *str)
{
    int len = strlen(str);
	if(len>MAX_WORD_LEN)
	{
		fprintf(stderr,"Too long word in amtMatchingWords. Length was: %d characters.\n",len);
		exit(1);
	}
	GPtrArray *dict = words->words[len];
	for(int i=0;i<dict->len;++i)
	{
		Word *word = g_ptr_array_index(dict,i);
		if((word->used==0) && (wildcardMatch(str,word->str)==1))
		{
			return 1;
		}
	}
	return 0;
}
