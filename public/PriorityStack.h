
#include "../const.h"

#ifndef __PRIORITYSTACK_H__
#define __PRIORITYSTACK_H__

// Gen Structure
typedef struct
{
	int li;
	int ri;
	int	pu;
	int	si;
	int	di;
	float p;
	float pPre;
	int nilgen;
	int	seg[MAXWORDLEN];
} G, *PG;

// Seg structure
typedef struct
{
	char str[MAXSTR];
	float p;
} S, *PS;

// Tag structure
typedef struct  
{
	int	mu;
	float p;
	int	itag[MAXWPTAG];
} T, *PT;


// for Gen
class PGstack
{
private:
//	PV stack[MAXK];

public:
	int top;
	PG stack[MAXK];

	PGstack();
	~PGstack();
	
	void initstack(); // for gen
	
	void emptystack(); // topstack = -1
	
	void pushstack(PG); // Gen 
	PG popstack();
	void copy(PG, PG);
	PG pg;
	PG pv;
};


// for Seg
class PSstack
{
private:
//	PV stack[MAXK];

public:
	int maxnode;
	int top;
	PS stack[MAXK];

	PSstack();
	~PSstack();

	void initstack(); // for seg
	
	void emptystack(int); // topstack = -1
	
	void pushstack(PS); // seg
	void stackadjust(int, int, int);
	PS popstack();
	void copy(PS, PS);
	void dumpstack(FILE *);
	PS ps;
};


// for Tag
class PTstack
{
private:
//	PV stack[MAXK];

public:
	int top;
	PT stack[MAXK];

	PTstack();
	~PTstack();

	void initstack(float); // for tag
	
	void emptystack(); // topstack = -1
	
	void pushstack(PT); // Tag 
	PT popstack();
	void copy(PT, PT);

	PT pt;
};
#endif