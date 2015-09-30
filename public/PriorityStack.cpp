/* 

  Statistical Morphological Analyzer(SMA)
  Copyright (c) 2012 by CBNU LaKE Lab.  Ver. 0.5

*/

#include <stdio.h>
#include <string.h>
#if !defined(__APPLE__)
#include <malloc.h>
#endif
#include "PriorityStack.h"

void PGstack::emptystack()
{
	top = -1;
}

PGstack::PGstack()
{
	int i;
	
	top = -1; // empty

	pg = new G;
	pv = new G;
	
	for (i=0; i<MAXK; i++) {
		stack[i] = new G;
		if (stack[i] == NULL) {
			printf("init stack mem alloc error\n");
			return;
		}
	}
}

PGstack::~PGstack()
{
	int i;
	
	for (i=0; i<MAXK; i++)
		delete stack[i];

	delete pg;
}

// for gen
void PGstack::initstack()
{
//	PG pv;
	pv = stack[0];
	top = 0;
	
	pv->si = 0;
	pv->di = -1;
	pv->pu = -1;
	pv->li = pv->ri = -1;
	pv->nilgen = 0;
	pv->seg[0] = -1;
	pv->p = pv->pPre = 0.0;
}

PG PGstack::popstack()
{
//	PG pg = new G;
	if (top >= 0)
	{
		copy(pg,stack[top]);
		top--;
		return(pg);
	}
	else
		return NULL;
}

void PGstack::pushstack(PG pv)
{
	int i, j;
	
	if (top < 0) {	// stack is empty 
		top = 0;
		copy(stack[0], pv);
		return ;
	}
	
	if (top < (MAXK-1)) {	// stack is not full
		i = top;
		top++;
		while(i >= 0 && pv->p >= stack[i]->p)	{
			copy(stack[i+1], stack[i]); 
			i--;
		}
		copy(stack[i+1], pv);
		return;
	}
	
	if (pv->p < stack[0]->p) { // if stack full and p is higher than p value of the stack bottom 
		i = top;
		while ( pv->p >= stack[i]->p && i > 0 ) {
			i--;
		}
		for(j=0; j < i; j++)
			copy(stack[j], stack[j+1]);
		
		copy(stack[i], pv);
	} // if stack full and pv is inserted into stack 
	// Ignore pv in other cases( stack full and p is lower than bottom p of the stack
	return ;
}

void PGstack::copy(PG pt, PG ps)
{
	memcpy(pt,ps,sizeof(G));
}



// for seg
void PSstack::emptystack(int topk)
{
	top = -1;
	maxnode = topk;
}

PSstack::PSstack()
{
	int i;
	
	ps = new S;

	top = -1; // empty
	
	for (i=0; i<MAXK; i++) {
		stack[i] = new S;
		if (stack[i] == NULL) {
			printf("init stack mem alloc error\n");
			return;
		}
	}
}

PSstack::~PSstack()
{
	int i;
	
	for (i=0; i<MAXK; i++)
		delete stack[i];

	delete ps;
}

PS PSstack::popstack()
{
//	PS ps = new S;
	if (top >= 0)
	{
		copy(ps,stack[top]);
		top--;
		return(ps);
	}
	else
		return NULL;
}

void PSstack::copy(PS pt, PS ps)
{
	memcpy(pt,ps,sizeof(S));
}

// for seg, tag
void PSstack::dumpstack(FILE *fp){
	int i;
	
	for (i=top; i>=0; i--) 
		fprintf(fp, "%s %f ", stack[i]->str, stack[i]->p);
}


// for seg
// LT, LE, GT, GE macros are used for probability comparison
//   they are same when origin probability is used.
//   they are differently(opposite) defined when log probability is used.
//	 But we can read them consistently for the probability...	
void PSstack::pushstack(PS pv)
{
	int i;
	// -------------------
	// empty stack  
	// -------------------
	if (top < BOTTOM) {	
		top = BOTTOM;
		copy(stack[BOTTOM], pv);
		return;
	}

	// -------------------
	// stack full
	// -------------------

	if (top >= (maxnode-1)) {
		if (LE(pv->p, stack[BOTTOM]->p))		// lower probability is not pushed
			return;

		// check duplication
		for (i=BOTTOM; i<=top; i++) {
			if (strcmp(stack[i]->str, pv->str) == 0) {
				if (LE(pv->p, stack[i]->p)) 
					return;	// do not push the duplicated string with lower probability
				stack[i]->p = pv->p;
				stackadjust(i, top, UP);
				return;
			}
		} // for loop			
		
		// no duplication
		copy(stack[BOTTOM], pv);		// delete item with lowest prob.
		stackadjust(BOTTOM, top, UP);
		return;
	} 
	
	// -------------------
	// stack is not full
	// -------------------
	// check duplication
	for (i=BOTTOM; i<=top; i++) {
		if (strcmp(stack[i]->str, pv->str) == 0) {
			if (LE(pv->p, stack[i]->p)) 
				return;	// do not push the duplicated string with lower probability
			stack[i]->p = pv->p;
			stackadjust(i, top, UP);
			return;
		}
	} // for loop
	
	// no duplication
	top++;
	copy(stack[top], pv);		// delete item with lowest prob.
	stackadjust(BOTTOM, top, DOWN);
	return;
}


void PSstack::stackadjust(int low, int high, int direction)
{
	PS pv;
	int i;

	pv = new S;

	if (low >= high) { // there is nothing to do
		delete pv;
		return;	
	}


	if (direction == UP) {
		copy(pv, stack[low]);
		i = low;
		while ( i < high && GT(pv->p, stack[i+1]->p) ) {
			copy(stack[i], stack[i+1]);
			i++;
		}
		copy(stack[i], pv);
	}
	else {	// direction == DOWN
		copy(pv, stack[high]);
		i = high;
		while ( i > low && LT(pv->p, stack[i-1]->p) ) {
			copy(stack[i], stack[i-1]);
			i--;
		}
		copy(stack[i], pv);
	}

	delete pv;

}



// for tag

PTstack::PTstack()
{
	int i;
	
	top = -1; // empty
	pt = new T;
	
	for (i=0; i<MAXK; i++) {
		stack[i] = new T;
		if (stack[i] == NULL) {
			printf("init stack mem alloc error\n");
			return;
		}
	}
}

PTstack::~PTstack()
{
	int i;
	
	for (i=0; i<MAXK; i++)
		delete stack[i];
	delete pt;

}

void PTstack::initstack(float initprob)
{
	PT temp;
	temp = stack[0];
	top = 0;	// push 1st t defined as follows:	

	temp->mu = 0;		// current mu number: 0 is null mu, start mu
	temp->itag[0] = 0;	// keep tag history: 0 is a special tag, meaning start '$'
	temp->p = initprob;	// propagate init probability from SEG program
	
}

void PTstack::copy(PT pt, PT ps)
{
	memcpy(pt,ps,sizeof(T));
}

PT PTstack::popstack()
{
//	PT pt = new T;
	if (top >= 0)
	{
		copy(pt,stack[top]);
		top--;
		return(pt);
	}
	else
		return NULL;
}

// for tag
void PTstack::pushstack(PT pt)
{
	int i, j;
	
	if (top < 0) {	// stack is empty 
		top = 0;
		copy(stack[0], pt);
		return ;
	}
	
	if (top < (MAXK-1)) {	// stack is not full
		i = top;
		top++;
		while(i >= 0 && pt->p >= stack[i]->p)	{
			copy(stack[i+1], stack[i]); 
			i--;
		}
		copy(stack[i+1], pt);
		return ;
	}
	
	if (pt->p < stack[0]->p) { // if stack full and p is higher than p value of the stack bottom 
		i = top;
		while ( pt->p >= stack[i]->p && i > 0 ) {
			i--;
		}
		for(j=0; j < i; j++)
			copy(stack[j], stack[j+1]);
		
		copy(stack[i], pt);
	} // if stack full and pv is inserted into stack 
	// Ignore pv in other cases( stack full and p is lower than bottom p of the stack
	return ;
}

