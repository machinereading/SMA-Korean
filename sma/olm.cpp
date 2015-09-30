/* 

  Statistical Morphological Analyzer(SMA)
  Copyright (c) 2012 by CBNU LaKE Lab.  Ver. 0.5

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../const.h"
#include "../public/TBL.h"

extern TBL GEN_NGRAM;

float poo(char *o2, char *o1)
{
	char x[MAXSTR], y[MAXSTR], y1[MAXSTR];
	// o2 follows o1
	// last two letters of o1 and first two letters of o2 should be checked.

	int n1, n2;
	int i, j;
	float p, p1, p2;

	n1 = strlen(o1);
	n2 = strlen(o2);

	if (n1 < 1 || n2 < 1) return(MAXPLOG);

	if (n1 >= 2) {
		strncpy(x, &o1[n1-2], 2);
		x[2] = '\0';
	}
	else { // n1 == 1
		strncpy(x, o1, 1);
		x[1] = '\0';
	}


	if (n2 >= 2) {
		strncpy(y1, o2, 2);
		y1[2] = '\0';
	}
	else { // n2 == 1
		strncpy(y1, o2, 1);
		y1[1] = '\0';
	}

	strcpy(y, x);
	strcat(y, y1);
	i = GEN_NGRAM.getindex(x);
	j = GEN_NGRAM.getindex(y);

	if (i < 0 || j < 0) return(MAXPLOG);
	p1 = GEN_NGRAM.getpx(i);
	p2 = GEN_NGRAM.getpx(j);

	p=(float)-log((float)p2/(float)p1);
	
//DEBUG
	if (p<0) {
		printf("DEBUG: x=%s, y=%s, p=%f, %d/%d\n", x, y, p, (int)p2, (int)p1);
	}

	return(p);
}

