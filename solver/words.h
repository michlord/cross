#ifndef WORDS_H_
#define WORDS_H_

#include "defines.h"
#include <glib.h>

typedef struct word Word;
struct word
{
	char *str;
	int used;
};

typedef struct words Words;
struct words
{
	char *memory_pool;
	/*words are indexed from 1*/
	GPtrArray *words[MAX_WORD_LEN+1];
};
typedef struct words_data WordsData;
struct words_data
{
	char *data;
	int size;
};

Words *newWords();
void freeWords(Words *words);
/*data is copied so source of data may be freed*/
void initWords(Words *words,const char *data,int size);
WordsData wordsDataFromFile(const char *filename);
/*if strings match it returns 1*/
int wildcardMatch(const char *str1, const char *str2);
int amtMatchingWords(Words *words,const char *str);
int doesHaveMatch(Words *words,const char *str);
GPtrArray *arrayOfMatchingWords(Words *words,const char *str);

#endif
