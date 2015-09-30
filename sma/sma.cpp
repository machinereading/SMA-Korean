/* 

  Statistical Morphological Analyzer(SMA)
  Copyright (c) 2012 by CBNU LaKE Lab.  Ver. 0.5

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sma.h"
#include "sma_func.h"
#include "../const.h"
#include "../public/TBL.h"

extern float initprob;
extern float segminp;

extern TBL SEG_TBL;
extern TBL TAG_TBL;

SMA::SMA()
{
}

SMA::~SMA()
{
}

void SMA::analyze(char* tcword, SMA_Result* result, int topk)
{
	char orgdata[MAXSTR];
	char data[MAXSTR];
	float p;
	int i;

	maxnode = topk;
	strcpy(data, tcword);

	// GEN
	genstack.emptystack();
	gen_node.EmptyQ();
	gen_node.EnQ(data, initprob);				// echo input
	gen_node.EnQ(data, initprob);				// original output as a variation	
	
	
	gen_run.GenRoot(data, maxnode);

//	printf("----------------------------------------\n");
//	gen_node.PrintQ();
	// SEG
	seg_node.EmptyQ();
	gen_node.DeQ(orgdata, &initprob);
	seg_node.EnQ(orgdata, initprob);
	segstack.emptystack(maxnode);						// answer will be pushed into vstack in CKY routine
	while (gen_node.DeQ(data, &initprob) > 0) {
		if (strcmp(data, "$") == 0) break;
		seg_run.CKY(data);
	}
	dumpvstack2seg_node(orgdata);

//	printf("----------------------------------------\n");
//	seg_node.PrintQ();
	//TAG
	tag_node.EmptyQ();
	seg_node.DeQ(data, &initprob); // insert orgdata
	tag_node.EnQ(data, initprob);  // original output as a variation
	tagstack.emptystack(maxnode);
	while (seg_node.DeQ(data, &initprob) > 0) {
		if (strcmp(data, "$") == 0)	break;
		tag_run.wptag(data, maxnode);
	}
	dumpvstack2tag_node(orgdata);

//	printf("----------------------------------------\n");

	i = 0;
	tag_node.DeQ(data, &p);
	do {
		tag_node.DeQ(data, &p);
//		printf("%s %0.5f\n", data, p);
		strcpy(result[i].str, data);
		result[i].p = p;
	} while( strcmp(data, "$") != 0 && ++i < maxnode);
}

void SMA::dumpvstack2seg_node(char* data)
{
	int i, top;

	top = segstack.top;
	
	for ( i = 0; (i < maxnode ) && (top >= 0); i++) {
		seg_node.EnQ(segstack.stack[top]->str, segstack.stack[top]->p);
		top--;
	}
	
	if (segstack.top < 0) {
		segminp = SEG_TBL.getminp();
		seg_node.EnQ(data, segminp);
	}
	else {
		segminp = segstack.stack[0]->p;
	}
	
	seg_node.EnQ((char*)"$", 0.0);
}

void SMA::dumpvstack2tag_node(char* data)
{
	int i, top;
	float prob;

	top = tagstack.top;
	
	for ( i = 0; (i < maxnode ) && (top >= 0); i++) {
		tag_node.EnQ(tagstack.stack[top]->str, tagstack.stack[top]->p );
		top--;
	}

	// 출력 결과가 없는 경우.. 형태소와 그 품사는 존재하나 품사간 연결관계가 존재 하지 않을 경우..
	if (tagstack.top < 0 ) {
		strcat(data, "-");

		strcat(data, TAG_TBL.getstring(TAG_TBL.getminindex()));
		prob = TAG_TBL.getminp();
		tag_node.EnQ(data, prob);
	}
	tag_node.EnQ((char*)"$", 0.0);

}

