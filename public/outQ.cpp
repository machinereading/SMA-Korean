/* 

  Statistical Morphological Analyzer(SMA)
  Copyright (c) 2012 by CBNU LaKE Lab.  Ver. 0.5

*/

#include <stdio.h>
#include <string.h>
#include "outQ.h"
#include "../const.h"

int cnt=0;

OUTQ::OUTQ(int size)
{
	Q = new QNODE[MAXSTR];
	maxQ = size;
	EmptyQ();
}


OUTQ::~OUTQ()
{
/*	int i;
	for(i=head; i < tail; i++) {
		delete[] Q[i].str;
	}*/
	delete Q;
}

void OUTQ::EmptyQ()
{
	head = tail = 0;
}

int OUTQ::sizeof_Q()
{
	return tail - head;
}

void OUTQ::EnQ(char* str, float p)
{
	if ((tail != 0) && (tail >= maxQ)){
		printf("OUTQ overflow\n");
	}
	else{
		Q[tail].p = p;
//		Q[tail].str = new char[strlen(str)+1];
		strcpy(Q[tail].str, str);
		tail++;	
	}
}

int OUTQ::DeQ(char *s, float *p)
{
	if (head < tail) {
		strcpy(s, Q[head].str);
		*p = Q[head].p;
		head++;
		return(1);
	}
	else
		return(-1);
}


void OUTQ::PrintQ()
{
	int i;
	for ( i=head; i<tail; i++ ){
#if MODULETEST
		printf("Q[%d] = ", i);
#endif
		printf("%s %f\n", Q[i].str, Q[i].p);
	}
}

void OUTQ::PrintQ_file(FILE* fout)
{
	int i;
	for ( i=head; i<tail; i++ )
		fprintf(fout, "%s %f ", Q[i].str, Q[i].p);
	fprintf(fout,"\n");
}

#if MODULETEST
void main(){
	OUTQ x(5);
	int i, j;
	char s[10];
	float p;
	
	for (i=0; i<5; i++) {
		x.EnQ("Test", i*0.1);
	}
	
	x.PrintQ();
	
	for (i=0; i<5; i++) {
		j = x.DeQ(s, &p);
		if (j>0)
			printf("DeQ: %s %f\n", s, p);
	}
}
#endif
