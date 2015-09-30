/* 

  Statistical Morphological Analyzer(SMA)
  Copyright (c) 2012 by CBNU LaKE Lab.  Ver. 0.5

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tag.h"
#include "../const.h"
#include "../public/PriorityStack.h"
#include "../public/ivt.h"
#include "../public/TBL.h"

extern float initprob;
extern float segminp;
extern PTstack tstack;
extern PSstack tagstack;

extern IVT TAG_WIVT;
extern IVT TAG_TIVT;
extern TBL SEG_TBL;
extern TBL TAG_TBL;

TAG::TAG()
{
	int i;
	PT pt = new T;
	segstrptr = 0;
	segstring = new char[MAXSTR*3];

	morphlist = new char*[MAXMORPSET];
	for ( i = 0; i < MAXMORPSET; i++ )
		morphlist[i] = new char[MAXMORPLEN];

	wtaglist = new TAGLST[MAXTAGLIST];
	ttaglist = new TAGLST[MAXTAGLIST];
	taglist = new TAGLST[MAXTAGLIST];
}

TAG::~TAG()
{
	int i;

	for ( i = 0; i < MAXMORPSET; i++ ) {
		if ( morphlist[i] == NULL ) break;
		delete[] morphlist[i];
	}

	delete morphlist;
	delete wtaglist;
	delete ttaglist;
	delete taglist;
	delete segstring;
}

void TAG::init_morplist(char** morp)
{
	int i;
	for ( i = 0; i < MAXMORPSET; i++ )
		morp[i][0] = '\0';
}

void TAG::wptag(char *wp, int topk)
{
	int i, c, cmorph;
	int index;
	char pretag[MAXWORDLEN], morph[MAXWORDLEN];

	index = 0;
	maxnode = topk;

	init_morplist(morphlist);

	tstack.initstack(initprob); // init prob
	cmorph = splitwp(wp); // set global variable morphlist

	init_prob_tag = initprob;
	while ((pt=tstack.popstack()) != NULL)
	{
		if (pt->mu >= cmorph) {
			outtagseq();
			continue;
		}
		index = pt->itag[pt->mu];
		strcpy(pretag, TAG_TBL.getstring(index));

		strcpy(morph, morphlist[pt->mu]);
		pt->mu += 1;

		c = mtag(pretag, morph); // fill taglist

		for (i=0; i<c; i++) {
			tstack.copy(&tt, pt);
			tt.itag[tt.mu] = taglist[i].itag;
			tt.p += taglist[i].p;
			tstack.pushstack(&tt);
		}
	} // while
}

int TAG::splitwp(char *wp)
{
	int i, j, k;
	char str[MAXWORDLEN];

	i=k=0;

	while(wp[i] != 0) {
		j=0;
		while(wp[i] != '-' && wp[i] != '\0') 
			str[j++] = wp[i++];
		if (wp[i] == '-') i++;
		str[j]='\0';
		strcpy(morphlist[k++], str);
	}

	return(k);
}

void TAG::outtagseq()
{
	int i, paircnt;
	float end_prob;

	if (pt == NULL) return;

	initseg();
	for (i=0; i<pt->mu; i++) {
		tagstr[0] ='\0';

		strcpy(tagstr, morphlist[i]);
		strcat(tagstr, "-");
		strcat(tagstr, TAG_TBL.getstring(pt->itag[i+1]));

		addseg(tagstr);
		if (i != pt->mu-1) 
			addseg((char*)"-");
	}

	paircnt = TAG_TIVT.lookup(pt->itag[i]);
	end_prob = TAG_TIVT.get_p(TAG_TBL.getindex((char*)"$"));
	
	if ( ( pt->p + end_prob - init_prob_tag) < MAXPLOG ){
		strcpy(v.str, segstring);
		v.p = pt->p + end_prob;
		tagstack.pushstack(&v);
	}

}

void TAG::initseg()
{
	segstrptr = 0;
	segstring[0] = '\0';
}


void TAG::addseg(char *s)
{
	if (strlen(s)+segstrptr >= MAXSTR*3) {
		printf("Error: segstring overflow\n");
		return;
	}

	while (*s != '\0') 
		segstring[segstrptr++] = *s++;

	segstring[segstrptr] = '\0';
}


int TAG::mtag(char *t, char *w)
{
	int i, j, k, cw, ct, cmin;
	int iw, it;

	iw = SEG_TBL.getindex(w);
	it = TAG_TBL.getindex(t);
	
	cw = pwt(iw);
	ct = ptt(it);
	
	if (cw < ct) 
		cmin = cw;
	else 
		cmin = ct;

	init_taglist(taglist);
	i=j=k=0;
	while(i<cw && j<ct) {
		if (wtaglist[i].itag == ttaglist[j].itag) {
			taglist[k].itag = wtaglist[i].itag;
			taglist[k].p = wtaglist[i].p + ttaglist[j].p;
			k++;	
			i++;
			j++; }
		else if (wtaglist[i].itag < ttaglist[j].itag)
			i++;
		else
			j++;
	}

	return(k);
}

int TAG::pwt(int windex)
{
	int i, paircnt;
	char temp[MAXSTR];

	paircnt = TAG_WIVT.lookup(windex);

	if (paircnt == 0 ) {

//		default tag -> max freq( itag = tag, p = 1 / freq * 2 )
		init_taglist(wtaglist);
		wtaglist[0].p = TAG_TBL.getminp() + segminp;
		wtaglist[0].itag = TAG_TBL.getminindex();

		return(1);
	}
	else{	

		init_taglist(wtaglist);
		
		for( i=0; i < paircnt; i++){
			TAG_WIVT.get_post(temp);
			TAG_WIVT.cnt_offset();
			sscanf(temp, "%d %f", &wtaglist[i].itag, &wtaglist[i].p);
		}

		return(i);
	}
	
}


int TAG::ptt(int tindex)
{
	int i, paircnt;
	char temp[MAXSTR];
	
	paircnt = TAG_TIVT.lookup(tindex);
	init_taglist(ttaglist);

	for( i=0; i < paircnt; i++){
		TAG_TIVT.get_post(temp);
		TAG_TIVT.cnt_offset();
		sscanf(temp, "%d %f", &ttaglist[i].itag, &ttaglist[i].p);
	}

	return(i);
}

void TAG::init_taglist(PTAGLST list)
{
	int i;
	for( i = 0; i < MAXTAGLIST; i++) {
		list[i].itag = -1;
		list[i].p = 0;
	}
}


