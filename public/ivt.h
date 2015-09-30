/* 

  Statistical Morphological Analyzer(SMA)
  Copyright (c) 2012 by CBNU LaKE Lab.  Ver. 0.5

*/

#include <stdio.h>
#include "../const.h"


typedef struct {
	int key;		// string of l (3 chars), x (3 chars), r (3 chars) plus null char)
	int	paircnt;
	int	offset;
} IVTdic;

typedef struct {
	int	yindex;
	float	p;
} IVTpost;

typedef struct {
	char word[MAXSTR];
	int paircnt;
	int offset;
} IVTdic_word;

typedef struct {
	char answer[MAXSTR];
	int freq;
} IVTpost_word;

class IVT{
public:
	IVT();
	~IVT();

	void initIVT(char*, char*, int);
	void deleteIVT();

	int get_paircnt();
	void cnt_offset();

	int lookup(int);
	void get_post(char*);
	float get_p(int);

	int lookup(char*);
	void get_answer(char*);

private:

	int maxIVT;
	int maxPOST;
	int bisearch(int, int);
	int bisearch(char*, int);
	int bisearch(int, int, int);
	int bisearch(char*, int, int);

	IVTdic *ivtnode;
	IVTpost *postnode;
	IVTdic_word *ivtnode_w;
	IVTpost_word *postnode_w;

	int index;
	int offset;
	int paircnt;

	int isChar;

};

