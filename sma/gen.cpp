/* 

  Statistical Morphological Analyzer(SMA)
  Copyright (c) 2012 by CBNU LaKE Lab.  Ver. 0.5

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gen.h"
#include "../const.h"
#include "../public/outQ.h"
#include "../public/PriorityStack.h"
#include "../public/ivt.h"
#include "../public/TBL.h"

#if DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#include <crtdbg.h>
#endif

float poo(char *, char *);

extern PGstack genstack; 
extern OUTQ gen_node;

extern IVT GEN_IVT;
extern TBL GEN_PU;

GEN::GEN(){	
	PG pvstack = new G;
	PG pv = new G;
}

GEN::~GEN(){
	delete pvstack;
	delete pv;
}

// 메모리 문제로 판단됨... 2011.11.11 daniel Lee
int ans_max;
PG	pvTemp = new G;

void GEN::GenRoot(char* data1, int topk)
{
	int si_last, i, len, paircnt, key;
	int left, right;
	char lc[MaxTokenLen+1], rc[MaxTokenLen+1];
	char c;
	int pu;
	float p, pCurrent;
	char data[MAXWORDLEN];
	char temp[MAXSTR];
	
	int chk_bool; // Daniel 의 chk_seg 위한 변수
	chk_bool = 0;

	genstack.initstack();
	
#if DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	strcpy(input_data, data1);	// backup for answerlist
	len = strlen(data1);
	data[0]=LEFTENDCHAR;
	strcpy(&data[1], data1);
	data[len+1]=RIGHTENDCHAR;
	data[len+2]='\0';
	
	si_last = len + 2;
	
	initanswer();
	
	// segment

	while ((pv=genstack.popstack()) != NULL)
	{
		genstack.copy(pvTemp, pv);					// copy stack V element
		
		if (pv->si == si_last) {
			genvar(pv);
			continue;
		}
		
		// try for new pu from a new character
		
		genstack.copy(&u, pvTemp);					// u for working V variable
		u.li = u.si;
		lc[0] = data[u.li];					// left context
		lc[1] = '\0';
		left = GEN_PU.getindex(lc);


		u.si += 1;
		c = data[u.si];
		pu_str[0]=c;						// one character pu
		pu_str[1]='\0';
		
		u.ri = u.si+1;						// right context
		rc[0] = data[u.ri];
		rc[1] = '\0';
		right = GEN_PU.getindex(rc);

		if ( ( u.pu = GEN_PU.getindex(pu_str)) != -1 )
		{
			u.di += 1;
			u.seg[u.di]=u.pu;
			u.nilgen = 0;		// reset (false) null-to-pu gen flag
			genstack.pushstack(&u);		// add original
			pu = u.pu;
			pCurrent = u.p;
			u.pPre = u.p;
			key = makekey(left, pu, right);

			paircnt = GEN_IVT.lookup(key);
			for( i = 0; i < paircnt; i++ ) {
				GEN_IVT.get_post(temp);
				GEN_IVT.cnt_offset();
				sscanf(temp, "%d %f", &u.pu, &p);

				u.seg[u.di] = u.pu;
				u.p = pCurrent + p + pooseg(u.seg, u.di);
				genstack.pushstack(&u);
			}

		}
		
		// try for null pu
		
		genstack.copy(&u, pvTemp);
		
		if (u.nilgen == 0) {		// avoid duplicated null-to-pu gen
			
			u.di += 1;
			
			u.li = u.si;
			lc[0] = data[u.li];		// left context
			lc[1] = '\0';
			left = GEN_PU.getindex(lc);

			pu = NULLCODE;			// null pu
			u.ri = u.li+1;
			rc[0] = data[u.ri];		// right context
			rc[1] = '\0';
			right = GEN_PU.getindex(rc);

			pCurrent = u.p;
			u.pPre = u.p;
			key = makekey(left, pu, right);

			paircnt = GEN_IVT.lookup(key);
			for (i = 0 ; i < paircnt ; i++) { 
				GEN_IVT.get_post(temp);
				GEN_IVT.cnt_offset();
				sscanf(temp, "%d %f", &u.pu, &p);

				u.seg[u.di] = u.pu;
				u.p=pCurrent + p + pooseg(u.seg, u.di);
				u.nilgen = 1;		// set (true) null-to-pu gen flag
				genstack.pushstack(&u);
			}
			
			
		} // end if (u.nilgen == 0)
		
		// try for concated pu (old-pu + new character)
		
		genstack.copy(&u, pvTemp);
		
		if (u.pu >= 0) { // only try for the valid pu
			
			u.si += 1;
			c = data[u.si];
			strcpy(pu_str, GEN_PU.getstring(u.pu));

			len=strlen(pu_str);
			pu_str[len]=c;					// new concated pu
			pu_str[len+1]='\0';
			
			lc[0] = data[u.li];				// left context
			lc[1] = '\0';
			left = GEN_PU.getindex(lc);

			u.ri = u.si+1;
			rc[0] = data[u.ri];				// right context
			rc[1] = '\0';
			right = GEN_PU.getindex(rc);

			if ( (u.pu = GEN_PU.getindex(pu_str)) != -1)
			{				
				u.seg[u.di]=u.pu;
				u.nilgen = 0;			// reset (false) null-to-pu gen flag
				
				// Daniel 의 chk_Seg 삽입
				chk_bool = chkseg(&u);
				if(chk_bool == 0)
					genstack.pushstack(&u);
				
				pu = u.pu;
				
				key = makekey(left, pu, right);

				paircnt = GEN_IVT.lookup(key);
				for (i = 0 ; i < paircnt ; i++) { 
					GEN_IVT.get_post(temp);
					GEN_IVT.cnt_offset();
					sscanf(temp, "%d %f", &u.pu, &p);

					u.seg[u.di]=u.pu;
					u.p=pCurrent + p + pooseg(u.seg, u.di);
					genstack.pushstack(&u);
				}
			}
		}	// if (u.pu >= 0) 
		
	} // while
	
	if( ans_max > MAXGEN ) ans_max = MAXGEN;
	for (i=1; i<=ans_max && i < topk ; i++) { // 2012. 8. 28. Daniel lee
		gen_node.EnQ(answerlist[i].str, answerlist[i].p);	
	}
	genstack.emptystack();
} 

// 같은 seg의 검사
int GEN::chkseg(PG pv)
{
	int pi, vi, i, j;
	char push_seg[MAXWORDLEN], vstack_seg[MAXWORDLEN];
	char temp[MAXSTR];

	if(pv == NULL) return 0;
	
	// push 될 값을 저장하는 부분
	pi = 0; 
	push_seg[pi] ='\0';
	
	for (i=0; i<=pv->di; i++) {
		if (pv->seg[i] == NULLCODE || pv->seg[i] == LEFTENDCODE 
			|| pv->seg[i] == RIGHTENDCODE) continue; // deletes NULLCHAR ($), LEFTENDCHAR (<), RIGHTENDCHAR (>)

		strcpy(temp, GEN_PU.getstring(pv->seg[i]));
		strcat(&push_seg[pi], temp);
		pi += strlen(temp);
	}
	push_seg[pi] = '\0';
	
	// vstack의 값을 저장후 push값과 비교하는 부분
	for(j = genstack.top ; j>=0 ; j--){
		
		vi = 0; 
		vstack_seg[vi] ='\0';
		
		for (i=0; i<=genstack.stack[j]->di; i++) {
			if (genstack.stack[j]->seg[i] == NULLCODE || genstack.stack[j]->seg[i] == LEFTENDCODE 
				|| genstack.stack[j]->seg[i] == RIGHTENDCODE) continue; // deletes NULLCHAR ($), LEFTENDCHAR (<), RIGHTENDCHAR (>)

			strcpy(temp, GEN_PU.getstring(genstack.stack[j]->seg[i]));
			strcat(&vstack_seg[vi], temp);
			vi += strlen(temp);
		}
		vstack_seg[vi] = '\0';
		
		// 같은 seg 존재
		if(strcmp(push_seg, vstack_seg) == 0 &&  pv->p == genstack.stack[j]->p)
			return 1;
	}
	// 같은 seg 존재 하지 않음
	return 0;
}


// generate variations
void GEN::genvar(PG pv)
{
	int i, ans_i;
	char temp[MAXSTR];

	if (pv == NULL) return;
	
	ans_i=0;				// answer string buf index
	answer[ans_i] = '\0';
	for (i=0; i<=pv->di; i++) {
		if (pv->seg[i] == NULLCODE || pv->seg[i] == LEFTENDCODE 
			|| pv->seg[i] == RIGHTENDCODE) continue; // deletes NULLCHAR ($), LEFTENDCHAR (<), RIGHTENDCHAR (>)
		
		strcpy(temp, GEN_PU.getstring(pv->seg[i]));
		strcat(&answer[ans_i], temp);
		ans_i += strlen(temp);
		
	}
	answer[ans_i] = '\0';	
	addanswerlist(answer, pv->p);
}

void GEN::initanswer()
{
	strcpy(answerlist[0].str, input_data);
	answerlist[0].p=1.0;
	ans_max=0;
}

void GEN::addanswerlist(char* s, float p)
{
	int i;
	for (i=0; i<=ans_max; i++) 
		if (strcmp(s, answerlist[i].str) == 0) return; // duplicated in answers
		
		if (i < MAXANS){
			strcpy(answerlist[i].str, s);
			answerlist[i].p=p;
			ans_max=i;
		}
		
}

void GEN::printanswer()
{
	int i;
	for (i=0; i<ans_max; i++)
		printf("%s %f\n", answerlist[i].str, answerlist[i].p);
}

float GEN::pooseg(int *seg, int di) 
{
	char x[MAXSTR], y[MAXSTR];
	char temp[MAXSTR];

	*x = '\0';

	if (di >=2 ) {
		strcpy(temp, GEN_PU.getstring(seg[di-2]));
		strcat(x, temp);
	}

	if (di >=1 ) {
		strcpy(temp, GEN_PU.getstring(seg[di-1]));
		strcat(x, temp);
	}

	if (di >=0) {
		strcpy(y, GEN_PU.getstring(seg[di]));
	}

	return(poo(y, x));
	
}

int GEN::makekey(int l, int x, int y)
{
	return l*1000000 + x *1000 + y;
}

