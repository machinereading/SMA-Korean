/* 

  Statistical Morphological Analyzer(SMA)
  Copyright (c) 2012 by CBNU LaKE Lab.  Ver. 0.5

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TBL.h"
#include "../const.h"

TBL::TBL()
{
}

TBL::~TBL()
{
}

void TBL::initTBL(char* fin, int type)
{
	// file open and construct table
	int i = 0;
	char buf[MAXSTR];
	char temp[MAXSTR];

	FILE* fp_in;

	maxtbl = 0;
	min_prob = 1;
	max_prob = MAXPLOG;
	buf[0] = temp[0] = '\0';

	fp_in = fopen(fin, "r");
	if( fp_in == NULL ) {
		printf("input file error :: check %s file\n", fin);
		return;
	}

	fgets(buf, MAXSTR, fp_in);
	maxtbl = atoi(buf);
	flag_state = type;
	
	switch(type) {
	case 1 :
		tbl = new char*[maxtbl];
		for(i = 0; i < maxtbl; i++)
		{
			fgets(buf, MAXSTR, fp_in);
			buf[strlen(buf)-1] = '\0';
			tbl[i] = new char[strlen(buf)+1];
			strcpy(tbl[i], buf);
		}
		break;
		
	case 2 :
		tbl = new char*[maxtbl];
		tbl_prob = new float[maxtbl];
		for(i = 0; i < maxtbl; i++)
		{
			fgets(buf, MAXSTR, fp_in);
			sscanf(buf, "%s %f", temp, &tbl_prob[i]);

			if ( min_prob < tbl_prob[i] && strcmp(temp, "$") != 0 ) { 
				min_prob = tbl_prob[i];
				min_index = i;
			}

			tbl[i] = new char[strlen(temp)+1];
			strcpy(tbl[i], temp);
		}
		break;
		
	default :
		printf("check tbl file\n");
	}

	fclose(fp_in);

}

void TBL::deleteTBL()
{
	int i;
	for(i=0; i < maxtbl; i++) {
		//		printf("delete table[%d] = %s\n", i, tbl[i]);
		delete[] tbl[i];
	}
	delete tbl;	

	if ( flag_state == 2 ) delete[] tbl_prob;
}


float TBL::getminp()
{
	return min_prob;
}

int TBL::getminindex()
{
	return min_index;
}

int TBL::getindex(char* word)
{
	//  char -> index
	return bisearch(word);
}

float TBL::getpx(int index)
{
	return tbl_prob[index];
}

int TBL::bisearch(char* word)
{
    register int lower = 0;
    register int upper = maxtbl - 1;
    register int mid;
	register int dif;

    while (lower <= upper) {
        mid = (lower + upper) >> 1;
		dif = strcmp(word, tbl[mid]);

        if (dif == 0)
			return(mid);
		else if ( dif > 0)
            lower = mid + 1;
        else // if (dif <0)
            upper = mid - 1;
	}
    return (-1); // ERROR;
}

char* TBL::getstring(int index) 
{
	// index -> char
	return tbl[index];
}

int TBL::gettblsize()
{
	return maxtbl+1;
}


