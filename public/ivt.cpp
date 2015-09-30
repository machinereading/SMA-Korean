/* 

  Statistical Morphological Analyzer(SMA)
  Copyright (c) 2012 by CBNU LaKE Lab.  Ver. 0.5

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ivt.h"
#include "../const.h"


IVT::IVT(){
}

IVT::~IVT(){
}

void IVT::initIVT(char *ivtname , char *postname, int flag)
{
	int i;
	int maxIVTTBL, maxPOSTTBL;
	char buf[MAXSTR];

	FILE* fp_ivt;
	FILE* fp_post;

	maxIVTTBL = maxPOSTTBL = 0;
	isChar = flag;

	fp_ivt = fopen(ivtname, "r");
	fp_post = fopen(postname, "r");

	if ( fp_ivt == NULL ) {
		printf("ivt input file error :: check %s file\n", ivtname);
		return;
	}
	if ( fp_post == NULL ) {
		printf("post input file error :: check %s file\n", postname);
		return;
	}


	// ivt load
	fgets(buf, MAXSTR, fp_ivt);
	maxIVTTBL = atoi(buf);

	switch(isChar) {
	case 1:
		ivtnode = new IVTdic[maxIVTTBL];
		break;
	case 2:
		ivtnode_w = new IVTdic_word[maxIVTTBL];
		break;
	}

	maxIVT = maxIVTTBL;

	i = 0;
	do {
		if ( i == maxIVTTBL ) break;

		fgets(buf, MAXSTR, fp_ivt);

		switch(isChar) {
		case 1:
			sscanf(buf, "%d %d %d", &ivtnode[i].key, &ivtnode[i].paircnt, &ivtnode[i].offset);
			break;
		case 2:
			sscanf(buf, "%s %d %d", ivtnode_w[i].word, &ivtnode_w[i].paircnt, &ivtnode_w[i].offset);
			break;
		}
		
		i++;
		
		if (i>maxIVTTBL) {
			printf("museg: ivt overflow\n");
			break;
		}
	} while(1);

	// post load
	fgets(buf, MAXSTR, fp_post);	
	maxPOSTTBL = atoi(buf);

	switch(isChar) {
	case 1:
		postnode = new IVTpost[maxPOSTTBL];
		break;
	case 2:
		postnode_w = new IVTpost_word[maxPOSTTBL];
		break;
	}

	maxPOST = maxPOSTTBL;
	
	i = 0;
	do {
		if ( i == maxPOSTTBL ) break;
		fgets(buf, MAXSTR, fp_post);

		switch( isChar ) {
		case 1:
			sscanf( buf, "%d %f", &postnode[i].yindex, &postnode[i].p);
			break;
		case 2:
			sscanf( buf, "%s %d", postnode_w[i].answer, &postnode_w[i].freq);
			break;
		}

		i++;

		if (i>maxPOSTTBL) {
			printf("post table overflow\n");
			break;
		}
	} while(1);

	fclose(fp_ivt);
	fclose(fp_post);

}

void IVT::deleteIVT()
{
	switch(isChar) {
	case (1):
		delete[] ivtnode;
		delete[] postnode;
		break;
	case (2):
		delete[] ivtnode_w;
		delete[] postnode_w;
		break;
	}
}

int IVT::lookup(int key){
	int i;

	i = bisearch(key, maxIVT);
	if ( i == -1) return 0;
	index = i;
	offset = ivtnode[i].offset;
	paircnt = ivtnode[i].paircnt;
	return paircnt;
}

void IVT::cnt_offset()
{
	offset++;
}

void IVT::get_post(char* dest)
{
	sprintf(dest, "%d %f", postnode[offset].yindex, postnode[offset].p);
	strcat(dest, "\n");
}

float IVT::get_p(int yindex)
{
	int i;
	i = bisearch(yindex, offset, offset+paircnt);
	if (i == -1)
		return (float) MAXPLOG;
	return postnode[i].p;
}

int IVT::lookup(char* key) {
	int i;

	i = bisearch(key, maxIVT);
	if ( i == -1) return 0;
	index = i;

	offset = ivtnode_w[i].offset;
	paircnt = ivtnode_w[i].paircnt;

	return paircnt;
}

void IVT::get_answer(char* dest) 
{
	sprintf(dest, "%s %d", postnode_w[offset].answer, postnode_w[offset].freq);
	strcat(dest, "\n");
}

int	IVT::bisearch(int key, int max)
{
    int lower;
    int upper;
    int mid;

	lower = 0;
	upper = max - 1;

    while (lower <= upper) {
        mid = (lower + upper) >> 1;
        if (key == ivtnode[mid].key)
			return(mid);
		else if (key > ivtnode[mid].key)
            lower = mid + 1;
        else // if (key < ivt[mid].key)
            upper = mid - 1;
        }
    return (-1); // ERROR;
}

int	IVT::bisearch(char* key, int max)
{
    int lower;
    int upper;
    int mid;
	int comp;

	lower = 0;
	upper = max - 1;

    while (lower <= upper) {
        mid = (lower + upper) >> 1;
		comp = strcmp(key, ivtnode_w[mid].word);

        if ( comp == 0 )
			return(mid);
		else if ( comp > 0)
            lower = mid + 1;
        else // if (key < ivt[mid].key)
            upper = mid - 1;
        }
    return (-1); // ERROR;
}

int	IVT::bisearch(int key, int min, int max)
{
    int lower;
    int upper;
    int mid;

	lower = min;
	upper = max - 1;

    while (lower <= upper) {
        mid = (lower + upper) >> 1;
        if (key == postnode[mid].yindex)
			return(mid);
		else if (key > postnode[mid].yindex)
            lower = mid + 1;
        else // if (key < ivt[mid].key)
            upper = mid - 1;
        }
    return (-1); // ERROR;
}

int	IVT::bisearch(char* key, int min, int max)
{
    int lower;
    int upper;
    int mid;

	lower = min;
	upper = max - 1;

    while (lower <= upper) {
        mid = (lower + upper) >> 1;
		if ( strcmp(key, postnode_w[mid].answer) == 0)
			return(mid);
		else if ( strcmp(key, postnode_w[mid].answer) > 0 )
            lower = mid + 1;
        else // if (key < ivt[mid].key)
            upper = mid - 1;
        }
    return (-1); // ERROR;
}



