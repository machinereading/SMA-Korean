/* 

  Statistical Morphological Analyzer(SMA)
  Copyright (c) 2012 by CBNU LaKE Lab.  Ver. 0.5

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "UTFCVT.h"
#include "UTFCVTtbl.h"

UTFCVT::UTFCVT() {
	uidx = 0;
	ujidx = 0;
	symbols = new SYMTABLE_UTF;
	symbols_x = symbols_dx = 0;
}

UTFCVT::~UTFCVT() {
}

//---------------------- public functions -------------------------
void UTFCVT::UTF2T(unsigned char* UTF, char* Tcode, int flag) {
	unsigned char UTF_temp[4];
	unsigned char UTF_temp_before[4];

	char Tcode_temp[10];
	char Tcode_temp_before[10];

	int i;
	int cnt_utf, UTFINT;
	int separater;

	init_symbols();
	
	Tcode[0] = Tcode_temp_before[0] = '\0';
	UTF_temp[0] = UTF_temp_before[0] = 0;
	cnt_utf = UTFINT = 0;

	// utf == 47 is '/'

	cnt_utf = 0;

	if ( flag == 0 )
		separater = 0;
	else
		separater = 47;

startloop:
	
	if ( UTF[cnt_utf] == 0 ) goto loopout;
	
	Tcode_temp[0] = '\0';
	if ( UTF[cnt_utf] != separater ) { 
			
		if ( UTF[cnt_utf] <= 223 ) {
			// 1 or 2 byte code
			if ( UTF[cnt_utf] <= 126 ) {
				strncpy(UTF_temp, UTF, cnt_utf, 1);
				cnt_utf++;
			} else {
				strncpy(UTF_temp, UTF, cnt_utf, 2);
				cnt_utf += 2;
			}
			goto state5;

		} else {
			// 3 byte code
			strncpy(UTF_temp, UTF, cnt_utf, 3);
			cnt_utf += 3;
			goto state1;
		}
	}

	// if '/' is special character
	if ( UTF[cnt_utf + 1] == separater || UTF[cnt_utf + 1] < 65 || UTF[cnt_utf + 1] > 90 ) {
		strncpy(UTF_temp, UTF, cnt_utf, 1);
		cnt_utf++;
		goto state5;
	}

	i = 0;

	cnt_utf++;
	Tcode_temp[i++] = '-';

	while( UTF[cnt_utf] != 43 && UTF[cnt_utf] != 0)
		Tcode_temp[i++] = UTF[cnt_utf++];

	if ( UTF[cnt_utf] == 43 ) {
		Tcode_temp[i++] = '-';
		Tcode_temp[i] = '\0';
		cnt_utf++;
	}

	Tcode_temp[i] = '\0';
	goto state6;

state1:
	// compare 3byte code - JASO or SYLLABLE
	u2ui( UTF_temp, &UTFINT );
	if ( UTF_temp[0] == 227 ) {
		if ( (UTFINT + 31439) >= 0 && (UTFINT + 31439) <= 50 ) {
			goto state4;
		}else {
			goto state5;
		}
	}
	else if ( UTF_temp[0] >= 234 && UTF_temp[0] <= 237 && UTFINT >=0 && UTFINT <= 11171)
		goto state3;
	else 
		goto state5;
		
state3:
	// UTF_temp is SYLLABLE
	u2t(UTF_temp, Tcode_temp);
	strcpy(Tcode_temp_before, Tcode_temp);
	goto state6;
		
state4:
	//UTF_temp is JASO
	// flag == 0 -> special character, flag == 1 -> JONGSUNG
	if ( flag == 0 ) {
		// 자소 -> symbol
		goto state5;
	} else {
		
		// 이전이 초성이었으면, 
		if ( Tcode_temp_before[0] == '`' || Tcode_temp_before[0] == '\0' ) {
			// 자소 -> symbol
			goto state5;
		} else {
			// 자소 -> 종성
			uj2t(UTF_temp, Tcode_temp);
			strcpy(Tcode_temp_before, Tcode_temp);
			goto state6;
		}

	}
			
state5:
	// UTF_temp is special charactor
	u2t_symbols(UTF_temp, Tcode_temp);
	recordsymbols(Tcode_temp, Tcode_temp_before, UTF_temp);

	if ( Tcode_temp[0] == '\0' )
			goto startloop;
	
	strcpy(Tcode_temp_before, Tcode_temp);

state6:

	strcat(Tcode, Tcode_temp);
	goto startloop;


loopout:;


}

void UTFCVT::T2UTF(char* Tcode, unsigned char* UTF){
	
	int Tcode_cnt, cnt_temp, utf_cnt;
	
	unsigned char UTF_temp[MAXSTR];
	char Tcode_temp[4];
	
	Tcode_cnt = utf_cnt = 0;
	UTF[0] = 0;

	symbols_dx = 0;
	while( Tcode[Tcode_cnt] != '\0' ) {
		
		cnt_temp = 0;
		Tcode_temp[0] = '\0';
		UTF_temp[0] = 0;
		
		if ( Tcode[Tcode_cnt] == SEPTC )
			goto state6;
		
		// if c is special character
		if ( (unsigned char) Tcode[Tcode_cnt] <= 0x2d || (unsigned char) Tcode[Tcode_cnt] == 0x60 )
			goto state5;

		// if c is ( JungSung || JungSung-JongSung )
		if ( (unsigned char) Tcode[Tcode_cnt] >= 0x41 && (unsigned char) Tcode[Tcode_cnt] <= 0x55 )  {
			Tcode_temp[cnt_temp++] = Tcode[Tcode_cnt++];

			if ( (unsigned char) Tcode[Tcode_cnt] >= 0x61 )
				Tcode_temp[cnt_temp++] = Tcode[Tcode_cnt++];

			Tcode_temp[cnt_temp] = '\0';
			goto state3;
		}

		// if c is JongSung
		if ( ( unsigned char) Tcode[Tcode_cnt] >= 0x61 )
			goto state1;

		// if c is ChoSung
		if ( ( (unsigned char) Tcode[Tcode_cnt] == 0x32 || (unsigned char) Tcode[Tcode_cnt] == 0x36 || (unsigned char) Tcode[Tcode_cnt] == 0x3b ) 
			&& (unsigned char) Tcode[Tcode_cnt+1] <= 0x2e || (unsigned char) Tcode[Tcode_cnt+1] >= 0x60) {
				goto state1;
		}

		// if c is SYLLABLE start
		Tcode_temp[cnt_temp++] = Tcode[Tcode_cnt++];
		Tcode_temp[cnt_temp++] = Tcode[Tcode_cnt++];
		if ( ( unsigned char) Tcode[Tcode_cnt] >= 0x61 )
			Tcode_temp[cnt_temp++] = Tcode[Tcode_cnt++];

		Tcode_temp[cnt_temp] = '\0';
		
		/*
		while( (unsigned char) Tcode[Tcode_cnt] >= 0x41 && (unsigned char) Tcode[Tcode_cnt] != 0x60 && Tcode[Tcode_cnt] != SEPTC && Tcode[Tcode_cnt] != '\0' && cnt_temp < 3)
			Tcode_temp[cnt_temp++] = Tcode[Tcode_cnt++];
		Tcode_temp[cnt_temp] = '\0';
		*/

		goto state3;
		
state1 :
		//  JASO - JONGSUNG processing
		Tcode_temp[cnt_temp++] = Tcode[Tcode_cnt++];
		Tcode_temp[cnt_temp] = '\0';
		goto state4;

state3:
		t2u(Tcode_temp, UTF_temp);
		strncatu(UTF, UTF_temp);
		continue;
		
state4 :
		t2uj(Tcode_temp[0], UTF_temp);
		strncatu(UTF, UTF_temp);
		continue;
		
state5 :
		// symbols extract
		Tcode_cnt++;

		t2u_symbol(UTF_temp);
		strncatu(UTF, UTF_temp);

		continue;
		
state6 :
		// lexical, tag separator
		utf_cnt = strcnt(UTF);
		UTF[utf_cnt++] = (unsigned char) '/';
		Tcode_cnt++;
		while( Tcode[Tcode_cnt] != SEPTC && Tcode[Tcode_cnt] != '\n' && Tcode[Tcode_cnt] != '\0')
			UTF[utf_cnt++] = (unsigned char) Tcode[Tcode_cnt++];
		
		if ( Tcode[Tcode_cnt] == SEPTC ) {
			UTF[utf_cnt++] = (unsigned char) '+';
			Tcode_cnt++;
		}
		
		UTF[utf_cnt] = 0;
		
	}

}


//-------------------- symbol table processing --------------------

// initial table
void UTFCVT::init_symbols() {
	int i;

	symbols_x = 0;
	symbols->symbols[0] = 0;

	for( i = 0; i < MAXSPNUM; i++ ) {
		symbols->len[i] = 0;
		symbols->normchar[i][0] = '\0';
	}
}

// symbols - TAG matching
void UTFCVT::u2t_symbols(unsigned char* UTF, char* tcode) {

	if ( UTF[0] == 33 || UTF[0] == 46 || UTF[0] == 63 ) {
		tcode[0] = SFTAG;
	} else if ( UTF[0] == 44 || UTF[0] == 47 || UTF[0] == 58 || UTF[0] == 59 )  {
		tcode[0] = SPTAG;
	} else if ( UTF[0] == 40 || UTF[0] ==41 || UTF[0] == 60 || UTF[0] == 62 || UTF[0] == 91 || UTF[0] == 93 || UTF[0] == 123 || UTF[0] == 125 ) {
		tcode[0] = SSTAG;
	} else if ( UTF[0] == 126 ) {
		tcode[0] = SOTAG;
	} else if ( UTF[0] >= 48 && UTF[0] <=57 ) {
		tcode[0] = SNTAG;
	} else if ( (UTF[0] >= 65 && UTF[0] <= 90) || (UTF[0] >= 97 && UTF[0] <= 122) ) {
		tcode[0] = SLTAG;
	} else if ( UTF[0] <= 126 ) {
		tcode[0] = SWTAG;
	} else if ( (UTF[0] >= 194 && UTF[0] <= 223) ) {

		if ( ( UTF[0] == 206 && UTF[1] >= 144 ) || ( UTF[0] > 206 && UTF[0] < 222 ) || ( UTF[0] == 222 && UTF[1] <= 177) ) 
			tcode[0] = SLTAG;
		else if ( (UTF[0] == 194 && (UTF[1] == 188 || UTF[1] == 189 || UTF[1] == 190)))
			tcode[0] = SNTAG;
		else if ( ( UTF[0] >= 195 && UTF[0] <= 201 ) || ( UTF[0] == 202 && UTF[1] <= 184 ) )
			tcode[0] = SLTAG;
		else
			tcode[0] = SWTAG;

	} else if ( (UTF[0] == 227 && UTF[1] >= 144) || (UTF[0] >= 228 && UTF[0] <= 232) || (UTF[0] == 233 && UTF[1] <= 190) ) {
		tcode[0] = SHTAG;
	} else if ( UTF[0] == 227 && ( (UTF[1] == 132 && UTF[2] >= 176) || (UTF[1] == 133 && UTF[2] <= 163) ) ) {
		tcode[0] = JASOTAG;
	} else {

		if ( (UTF[0] == 224 && UTF[1] >= 164) || ( UTF[0] == 225 && UTF[1] >= 136 && UTF[1] <= 153) || ( UTF[0] == 225 && UTF[1] >= 184 && UTF[1] <= 191) ) 
			tcode[0] = SLTAG;
		else if ( ( UTF[0] == 227 && UTF[1] >=142 ) || (UTF[0] == 227 && UTF[1] == 143 && UTF[2] <= 158 ))
			tcode[0] = SWTAG;
		else 
			tcode[0] = SWTAG;
	}

	tcode[1] = '\0';	
}

// record symbol to symbol-table
void UTFCVT::recordsymbols(char* Tcode, char* Tcode_before, unsigned char* UTF) {

	int UTF_len;

	UTF_len = strcnt(UTF);

	if ( ( Tcode_before[0] == SHTAG || Tcode_before[0] == SNTAG || Tcode_before[0] == SLTAG || Tcode_before[0] == JASOTAG)
		&& ( Tcode[0] == SHTAG || Tcode[0] == SNTAG || Tcode[0] == SLTAG || Tcode[0] == JASOTAG ) && (Tcode[0] == Tcode_before[0])) {

		symbols->len[symbols_x-1] += UTF_len;
		strncatu(symbols->symbols, UTF);

		Tcode[0] = '\0';
	} else {
		strcpy(symbols->normchar[symbols_x], Tcode);
		symbols->len[symbols_x++] = UTF_len;
		strncatu(symbols->symbols, UTF);
	}

}

// extract symbols from symbol-table
void UTFCVT::t2u_symbol(unsigned char* UTF) {

	int i, symbols_dx_start;
	
	symbols_dx_start = 0;
	for( i = 0; i < symbols_dx; i++ )
		symbols_dx_start += symbols->len[i];

	for( i = 0; i < symbols->len[symbols_dx]; i++ )
		UTF[i] = symbols->symbols[i+symbols_dx_start];
	UTF[i] = 0;

	symbols_dx++;
}


//---------------------- internal functions -----------------------

void UTFCVT::strncatu(unsigned char* target, unsigned char* source) {

	int i, j;

	i = 0;
	while ( target[i++] != 0 );
	i--;

	j = 0;
	while( source[j] != 0 )
		target[i++] = source[j++];

	target[i] = 0;

}

int UTFCVT::strcnt(unsigned char* UTF_temp) {
	int i = 0;
	while( UTF_temp[i++] != 0 );
	return (i-1);
}

void UTFCVT::strncpy(unsigned char* target, unsigned char* source, int startposition, int cnt) {
	int i;

	for( i = 0; i < cnt; i++ )
		target[i] = source[startposition++];
	target[i] = 0;
}

//-------------------- UTF JASO CONVERSION ---------------------------
void UTFCVT::uj2t(unsigned char *u, char *t)
{
	int uji;
	int i, j, k;
	
	i = 0x0f & u[0];
	i = i << 12;
	j = 0x3f & u[1];
	j = j << 6;
	k = 0x3f & u[2];
	uji = i + j + k - 0x3131;
//	printf("uj2t: 0 = %d,%d,%d  u=%s uji=%d\n", u[0], u[1], u[2], u, uji);
	*t = uji2t_tbl[uji];
	t[1] = '\0';
}

void UTFCVT::t2uj(char t, unsigned char *u)
{
	int ti, uji;
	unsigned char i, j, k; // first(1110xxxx), second(10xxxxxx), third(10xxxxxx) bytes
	int tuj;
	

	if (t >= 0x2e && t <= 0x55)
		ti = t - 0x2e;
	else if (t >= 0x61 && t <= 0x7b)
		ti = t - 0x61 + 40;
	else {
		*u = '\0'; return;	// invalid T code
	}

	uji = ti2uji_tbl[ti];
	
	//	printf("t2uj: ti=%d uji=%d\n", ti, uji);
	
	tuj =  uji + 0x003131;	// Jaso base UTF8 = 0x3131
	i = (0xf000 & tuj) >> 12;
	i |= 0xe0;
	
	j = (0x0fc0 & tuj) >> 6;
	j |= 0x80;
	
	k = 0x003f & tuj;
	k |= 0x80;
	
	
	u[0]=i; u[1]=j; u[2]=k; u[3]='\0'; // null for sz type string
	
}


//------------------- UTF SYLABLE CONVERSION -------------------------
void UTFCVT::u2t(unsigned char *u, char *t)
{
	int ui;
	
	u2ui(u, &ui);
	ui2t(ui,(unsigned char*) t);
}

void UTFCVT::t2u(char *t, unsigned char *u)
{
	int ui;
	
	t2ui((unsigned char*)t, &ui);
	ui2u(ui, u);
}

//------------------- UTF INDEX CALCULATION --------------------------
void UTFCVT::u2ui(unsigned char *u, int *ui)
{
	int i, j, k;
	
	i = 0x0f & u[0];
	i = i << 12;
	j = 0x3f & u[1];
	j = j << 6;
	k = 0x3f & u[2];
	
	*ui = i + j + k - 0xac00;
//	printf("u2ui: ui=%d (%x)\n", *ui, *ui);
	
}

void UTFCVT::ui2u(int ui, unsigned char *u)
{
	unsigned char i, j, k; // first(1110xxxx), second(10xxxxxx), third(10xxxxxx) bytes
	int t;
	
	t =  ui + 0x00ac00;	// base UTF8 = 0xac00
	i = (0xf000 & t) >> 12;
	i |= 0xe0;
	
	j = (0x0fc0 & t) >> 6;
	j |= 0x80;
	
	k = 0x003f & t;
	k |= 0x80;
	
	
	u[0]=i; u[1]=j; u[2]=k; u[3]='\0'; // null for sz type string
	
}

void UTFCVT::t2ui(unsigned char *t, int *ui)
{
	unsigned char i, j, k;

#if	NULL_CHO
	unsigned char t1[4];

	if ( t[0] >= TCODE_MV_BEG && t[0] <= TCODE_MV_END ) {
		t1[0] = '9';
		t1[1] = t[0];
		t1[2] = t[1];
		t1[3] = '\0';
	} else {
		t1[0] = t[0];
		t1[1] = t[1];
		t1[2] = t[2];
		t1[3] = t[3];
	}

	i=t1[0]; j=t1[1]; 
	if (strlen((char*)t1) <3) k=0x60;
	else k=t1[2];

#else

	i=t[0]; j=t[1]; 
	if (strlen((char*)t) <3) k=0x60;
	else k=t[2];

#endif
	
	*ui = (i-0x2e)*21*28 + (j-0x41)*28 + k-0x60;
}

void UTFCVT::ui2t(int ui, unsigned char *t)
{
	unsigned char i, j, k;
	int tmp;
	
	i = ui / (21*28);
	tmp = ui % (21*28);
	j = tmp / 28;
	k = tmp % 28; 
	t[0] = i + 0x2e;
	t[1] = j + 0x41;
	if (k > 0)
		t[2] = k + 0x60;
	else
		t[2] = '\0';		// null for sz type string
	t[3] = '\0';

#if NULL_CHO
	if( t[0] == '9' ) {
		t[0] = t[1];
		t[1] = t[2];
		t[2] = '\0';
	}
#endif
}
