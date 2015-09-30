/* 

  Statistical Morphological Analyzer(SMA)
  Copyright (c) 2012 by CBNU LaKE Lab.  Ver. 0.5

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "seg.h"
#include "../const.h"
#include "../public/TBL.h"
#include "../public/ivt.h"

float pyx(char *, char *);
float px(char *);
void dumpxycnt();

extern IVT SEG_IVT;
extern TBL SEG_TBL;

float pyx(char *y, char *x)
{
	int xindex, yindex, paircnt;
	float p1, p2;
	float prob = MAXPLOG;

	xindex = SEG_TBL.getindex(x);
	yindex = SEG_TBL.getindex(y);
	
	if ( yindex < 0 ) 
		return(MAXPLOG);

	if ( xindex < 0 ) {
		if (y[strlen(y)-1] == '$')		// if y is ending 
			return SEG_TBL.getpx(yindex) * 2;
		else
			return(MAXPLOG);
	}

	paircnt = SEG_IVT.lookup(xindex);
	if( paircnt >= 0 ) {
		prob = SEG_IVT.get_p(yindex);

		// x->y가 없는 경우는 각각의 확률로 추정
		if( prob >= MAXPLOG ) {
			p1 = SEG_TBL.getpx(xindex);
			p2 = SEG_TBL.getpx(yindex);
			return (p1+p2);
		}
		else
			return(prob);
	
	} else
		return ( MAXPLOG ) ;

}

float px(char *x)
{
	int i;
	
	i = SEG_TBL.getindex(x);
	if ( i < 0 ) return (MAXPLOG);

	return SEG_TBL.getpx(i);
}

