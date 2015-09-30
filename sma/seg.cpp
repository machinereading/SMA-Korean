/* 

  Statistical Morphological Analyzer(SMA)
  Copyright (c) 2012 by CBNU LaKE Lab.  Ver. 0.5

*/

#if DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "seg.h"
#include "../const.h"
#include "../public/PriorityStack.h"

extern PSstack segstack;
extern float initprob;

float pyx(char *, char *);
float px(char *);

#define pcell(x, i, j) (&x[(n*(i) + (j-1))])


/*

  cell structure example for "abcd"
  row: 0-based index
  col: 1-based index
  
	T(row, col)
	
	  1       2       3       4
	  0	a      ab     abc    abcd
	  (0,1)  (0,2)   (0,3)   (0,4)
	  
		1	        b      bc	  bcd
		(1,2)	 (1,3)   (1,4)
		
		  2	                c      cd
		  (2,3)   (2,4)
		  
			3                           d
			(3,4)
			
			  
				coordinate: (row, col)
				
				  substring (row, col) = data[row, col-1] // substring from 'row'th char to 'col-1'th char
				  
					fill first row(0, *) with initial values
					start from substring length 1 to n
					
					  // initialize
					  T(0, 1)
					  T(0, 2)
					  T(0, 3)
					  T(0, 4)
					  
						// CKY loop
						T(1, 2) <- T(0, 1)
						T(1, 3) <- T(0, 1)
						T(2, 3) <- T(0, 2)
						T(2, 3) <- T(1, 2)
						T(1, 4) <- T(0, 1)
						T(2, 4) <- T(0, 2)
						T(2, 4) <- T(1, 2)
						T(3, 4) <- T(0, 3)
						T(3, 4) <- T(1, 3)
						T(3, 4) <- T(2, 3)
						
						  // output result
						  print T(0, 4)
						  print T(1, 4)
						  print T(2, 4)
						  print T(3, 4)
						  
*/

SEG::SEG(){
	segstrptr = 0;
	segstring = new char[MAXSTR*3];
};

SEG::~SEG(){
	delete segstring;
};

void SEG::CKY(char *s)
{
	char sx[MAXSTR];
	char sy[MAXSTR];

	int n, row, col, pre_row, pre_col;
	PCELL p;
	float newprob;
	PNODE pnode;
	PSUB psub, psubPre; 

	int memerrorflag;


	strcpy(input, s);
	//strcat_s(s, strlen(s)+strlen("$")+1, "$");			// add last null mark

	n = strlen(s);
	
	if (n > MAXSTR) { 
		printf("Too long string(%d)\n", n);
		return;
	}
	
	p = (PCELL) malloc(sizeof(CELL)*n*n);
	
	if (p==NULL) return;
	
	for (row = 0; row < n; row++) {				// row: 0 based index
		for (col = 1; col <= n; col++) {		// col: 1 based index
			pcell(p, row, col)->sub = NULL;
		}
	}

	
	// init start cells T(0, *)
	for (col = 1; col <= n; col++) {
		pnode = makenode(0, col);
		if (pnode == NULL) break;
		
		psub = makesub(pnode);
		if (psub == NULL) break;
		substring(sx, s, 0, col);
		psub->prob = initprob;		// propagate previous prob from GEN module
		psub->prob += px(sx);
		
		pcell(p, 0, col)->sub = psub;
	}
	
	
	// CKY loop
	for (col = 1; col <= n; col++) {
		for (row = 1; row < col; row++) {
			pre_col = row;
			for (pre_row = 0; pre_row < row; pre_row++ ) {
				substring(sx, s, pre_row, pre_col);
				substring(sy, s, row, col);

				newprob = pyx(sy, sx);

				if ( newprob >= MAXPLOG ) continue;

				psubPre = pcell(p, pre_row, pre_col)->sub;		// source cell to be linked from
				
				while (psubPre != NULL) {
					pnode = makenode(row, col);
					if (pnode == NULL) {
						memerrorflag = 1;
						goto outCKYloop;
						//	break;
					}
					pnode->nextnode = psubPre->startnode;
					
					psub = makesub(pnode);
					if (psub == NULL) {
						memerrorflag = 1;
						goto outCKYloop;
						//	break;
					}

					psub->prob = psubPre->prob + newprob;

					psub->nextsub = pcell(p, row, col)->sub;
					pcell(p, row, col)->sub = psub;
					
					psubPre = psubPre->nextsub;
				}
			} // pre_row
		} // row
	} // col


	for (row = 0; row < n; row++) {
		subprint(pcell(p, row, n)->sub);
	}


outCKYloop:	
	if (memerrorflag == 1) printf("Skip error word: %s\n", input);
	 
	for (col = 1; col <= n; col++) {
		for (row = 0; row < col; row++)
			subfree(pcell(p, row, col)->sub);
	}

	free(p);

}

PSUB SEG::makesub(PNODE pnode)
{
	PSUB psub;
	psub = (PSUB)malloc(sizeof(SUB));

	if (psub == NULL) {
		printf("memory alloc error in copysub\n");
		free(psub);
		return(NULL);
	}

	psub->prob = 0;			// init prob
	psub->startnode = pnode;
	psub->nextsub = NULL;
	
	return(psub);
}


PNODE SEG::makenode(int r, int c)
{
	PNODE pnodenew;
	pnodenew = (PNODE) malloc(sizeof(NODE));

	if (pnodenew == NULL) {
		printf("memory alloc error in makenode\n");
		free(pnodenew);
		return(NULL);
	}

	pnodenew->row = r;
	pnodenew->col = c;
	pnodenew->nextnode = NULL;
	
	return(pnodenew);
}


void SEG::substring(char *t, char *s, int i, int j)
{
	int k;
	
	for (k=i; k<j; k++) 
		*t++=*(s+k);
	*t = '\0';
}


void SEG::subprint(PSUB psub)
{
	while (psub != NULL) {
		if (psub->prob < MAXPLOG ) {
			nodeprint(psub->startnode);
			strcpy(v.str, segstring);
			v.p = psub->prob;
			segstack.pushstack(&v);
		}
		psub = psub->nextsub;
	}
}

void SEG::nodeprint(PNODE pn)
{
	char sub[MAXSTR];
	
	initnodestack();
	while(pn != NULL) {
		pushnode(pn);
		pn = pn->nextnode;
	}
	
	initseg();
	while ((pn=popnode()) != NULL) {
		substring(sub, input, pn->row, pn->col);	// refer to global var data
		addseg(sub);
		addseg((char*)"-");
	}
	delseg();

}

void SEG::initnodestack()
{
	nodestacktop = -1;
}

void SEG::pushnode(PNODE pn)
{
	nodestack[++nodestacktop] = pn;
	if (nodestacktop >= (MAXNODESTACK-1)) {
		printf("Node stack overflow (%d)\n", nodestacktop);
		nodestacktop--;
	}
}

PNODE SEG::popnode()
{
	if (nodestacktop < 0) return(NULL);
	return(nodestack[nodestacktop--]);
}


void SEG::subfree(PSUB p)
{
	PSUB p1, p2;

	p1 = p;
	
		while (p1 != NULL) {
			p2 = p1->nextsub;
			free(p1->startnode);
			free(p1);
			p1 = p2;
		}
	
}

void SEG::initseg()
{
	segstrptr = 0;
	segstring[0] = '\0';
}

void SEG::addseg(char *s)
{
	if (strlen(s)+segstrptr >= MAXSTR*3) {
		printf("Error: segstring overflow\n");
		return;
	}
	
	while (*s != '\0') 
		segstring[segstrptr++] = *s++;
	
	segstring[segstrptr] = '\0';
}

void SEG::delseg()
{
	int len;
	
	len=strlen(segstring);

	len--;
	while (len >= 0 && (segstring[len] == '-' ||segstring[len] == '$') ) len--;
	
	segstring[len+1] = '\0';
	
}