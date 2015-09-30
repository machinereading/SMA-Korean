#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "KSCCVT.h"
#include "KSCCVTtbl.h"

// Hangul Jaso
#define JASO_1STB	((unsigned char) 0xa4)		// Jaso 1st Byte: hangul jaso 0xa4a1 - 0xa4fe
#define JASO_START_2NDB	((unsigned char) 0xa1)
#define JASO_END_2NDB	((unsigned char) 0xd3)
#define SIZE_JASO	51 // 0xd3 - 0xa1 + 1

// Hangul syllable: 0xb0a1 - 0xc8fe (except 0x**a0 & 0x**ff)
#define START_1STB	((unsigned char) 0xb0)		// Han Start 1st Byte
#define END_1STB	((unsigned char) 0xc8)		// Han End 1st Byte
#define START_2NDB	((unsigned char) 0xa1)
#define END_2NDB	((unsigned char) 0xfe)

#define SIZE_2NDB	94		// 0xfe - 0xa1 + 1
#define SIZE_HAN	2350	// Hangul Syllable size: 0xb0a1 - 0xc8fe (except 0x**a0 & 0x**ff)

#define TCODE_IC_BEG	0x2e	// '.'	initial consonant
#define TCODE_IC_END	0x40	// '@'	
#define TCODE_MV_BEG	0x41	// 'A'	middle vowel
#define TCODE_MV_END	0x56	// 'U'
#define TCODE_FC_BEG	0x61	// 'a'	final consonant
#define TCODE_FC_END	0x7B	// '}'

// TCODE type
#define ITYPE		1		// initial consonant
#define MTYPE		2		// middle vowel general: standalone or y, w combined
#define FTYPE		3		// final consonant general

#define CHO_IUNG	'9'		// 段失 し

#define ERROR -1


// K2T: convert Korean code string to T code string
//
void KSCCVT::k2t(char* k, char* t)
{
	unsigned char kcode[3];
	char tcode[6];
	int index;
	
	*t='\0';
	kcode[0] = '\0';
	tcode[0] = '\0';
	
	while (*k != '\0') {
		index = -1;
		kcode[0] = *k++;
		kcode[1] = *k++;
		kcode[2] = '\0';
		
		if ( kcode[0] == JASO_1STB && kcode[1] >= JASO_START_2NDB && kcode[1] <= JASO_END_2NDB )
			index = kcode[1] - START_2NDB;
		
		else if ( kcode[0] >= START_1STB &&  kcode[0] <= END_1STB 
			&& kcode[1] >= START_2NDB && kcode[1] <= END_2NDB )  
			index = (kcode[0] - START_1STB) * SIZE_2NDB + (kcode[1] - START_2NDB) + SIZE_JASO;
		
		if (index >=0 && index <= SIZE_HAN + SIZE_JASO) 
			strcpy(tcode, k2ttbl[index]);
		
		strcat(t, tcode);
	}
	
}

// T2K: convert T code string to Korean code string
//
void KSCCVT::t2k(char* t, char* k)
{
	char tsyl[6];
	int index;
	char kcode[3];
	
	*k='\0';
	while (*t != '\0') {
		
		// cut each syllable from t code string
		t = cut_tsyl(t, tsyl);	// update t and return syllable (or standalone t Jaso) in tsyl
		
		index = bisearch_t2k(MAXT2K, tsyl);
		
		if (index < 0) continue; // skip non Hangul
		
		kcode[0] = '\0';
		
		if (index < SIZE_JASO)
			kcode[0] = JASO_1STB;
		else if (index < SIZE_JASO + SIZE_HAN) {
			index -= SIZE_JASO;
			kcode[0] = (unsigned char)(index/SIZE_2NDB + START_1STB);		// SIZE_2NDB=94, START_1STB=0xb0
		}
		
		kcode[1] = (unsigned char)(index % SIZE_2NDB + START_2NDB);		// SIZE_2NDB=94, START_2NDB=0xa1
		kcode[2] = '\0';
		strcat(k, kcode);
		
	} // while
}


// K2TX: convert Korean code string to T code string with Chosung Iung (null sound) deleted
//
void KSCCVT::k2tx(char* k, char* tx)
{
	char tbuf[MAXSTR*2];		// for fast run. (instead of malloc)
	int len;
	char *t;

	t = tbuf;
	
	len = strlen(k);
	
	if ((len*3+1) >= MAXSTR*2) {
		printf("k2tx: possible code buffer overflow\n");
	}
	
	k2t(k, t);
	
	while( *t != '\0') {
		if ( *t == CHO_IUNG ) {		// delete 段失 'し'
			t++;
			continue;
		};
		*tx++ = *t++;
	};
	*tx = '\0';
}


// TX2K: convert T code string (without Chosung Iung) to Korean code string with automatic Iung insertion
//
void KSCCVT::tx2k(char* tx, char* k)
{
	char tbuf[MAXSTR*2];		// for fast run. (instead of malloc)
	int len;
	int pre_ctype, ctype;
	char *t;
	
	t = tbuf;
	len = strlen(tx);
	
	if ((len*2+1) >= MAXSTR*2) {
		printf("tx2k: possible code buffer overflow\n");
	}
	
	pre_ctype = FTYPE;		// assume previous syllable is finished with Final type
	while( *tx != '\0') {
		ctype = codetype(*tx);
		if ( (pre_ctype >= MTYPE) && // if pre code type is middle or final type
			( ctype == MTYPE ) ) {		// if middle vowel
			*t++ = CHO_IUNG;
		};
		*t++ = *tx++;
		pre_ctype =ctype;
	};
	*t = '\0';
	t2k(tbuf, k);
}


// J2T: convert Jaso (non-syllable format) Korean code string to T code string
//
void KSCCVT::j2t(char* j, char* t)
{
	char key[2];
	char tcode[2];
	int index;
	
	*t = '\0';
	
	while(*j != '\0') {
		key[0] = *j++;
		key[1] = *j++;
		key[2] = '\0';
		
		index = bisearch_j2t(MAXJ2T, key);
		if (index < 0) continue;	// ignore invalid key
		tcode[0] = j2tglyph[index];
		tcode[1] = '\0';
		strcat(t, tcode);

	}
}


// T2J: convert  T code string to Jaso (non-syllable format) Korean code string
//
void KSCCVT::t2j(char* t, char* j)
{
	int index;
	char key[2];
	char jcode[3];
	
	*j = '\0';
	
	while(*t != '\0') {
		key[0] = *t++;
		key[1] = '\0';
		
		index = bisearch_t2j(MAXT2J, key);
		if (index < 0) continue;	// ignore invalid key
		jcode[0] = t2jglyph[index*2];
		jcode[1] = t2jglyph[index*2+1];
		jcode[2] = '\0';
		strcat(j, jcode);
	}
}

void KSCCVT::t2j_cho(char* t, char* j)
{
	int index;
	char key[2];
	char jcode[3];
	
	*j = '\0';
	
	while(*t != '\0') {
		key[0] = *t++;
		key[1] = '\0';
		
		index = bisearch_t2j(MAXT2JCHO, key);
		if (index < 0) continue;	// ignore invalid key
		jcode[0] = t2jglyph_cho[index*2];
		jcode[1] = t2jglyph_cho[index*2+1];
		jcode[2] = '\0';
		strcat(j, jcode);
	}
}

char* KSCCVT::cut_tsyl(char* t, char* tsyl)
{
	int i;
	char tc;
	int ctype;
	
	//state0:
	i = 0;
	tc = t[i];
	ctype = codetype(tc);
	
	if (ctype == ITYPE) 
		goto state1;
	else if (ctype == MTYPE) 
		goto state2;
	else if (ctype == FTYPE) 
		goto state3;
	goto error;
	
state1:
	*tsyl++ = tc;
	i++;
	tc = t[i];
	ctype = codetype(tc);
	
	if (ctype == MTYPE) 
		goto state4;
	else 
		goto finstar;
	
state2:
	goto fin;
	
state3:
	goto fin;
	
state4:	
	*tsyl++ = tc;
	i++;
	tc = t[i];
	ctype = codetype(tc);
	
	if (ctype == FTYPE) // if final
		goto fin;
	goto finstar;
	
fin:
	*tsyl++ = tc;
	i++;				// should go down through
	
finstar:				// token back 
	*tsyl = '\0';
	return(&t[i]);
	
	
error:				// do nothing but return
	printf("t code automata error(syl=%s code=%c)\n", tsyl, tc);
	i++;			// skip error code
	goto finstar;	
}


int KSCCVT::codetype(char c)
{
	
	if ( c >= TCODE_IC_BEG && c <= TCODE_IC_END ) 
		return (ITYPE);				/* initial consonant type */
    if ( c >= TCODE_MV_BEG && c <= TCODE_MV_END ) 
		return (MTYPE);				/* middel vowel type */
    if ( c >= TCODE_FC_BEG && c <= TCODE_FC_END ) 
		return (FTYPE);				/* last consonant type */
	
	//	printf("ERROR: invalid code type\n");
	return(ERROR);
}


int KSCCVT::bisearch_t2k(int max, char* key)
{
    register int lower = 0;
    register int upper = max - 1;
    register int mid;
	register int cmp;
	
    while (lower <= upper) {
        mid = (lower + upper) >> 1;
		cmp = strcmp(key, k2ttbl[t2ktbl[mid]]);
        if ( cmp > 0 )
            lower = mid + 1;
        else if (cmp < 0 )
            upper = mid - 1;
        else // cmp == 0 
            return t2ktbl[mid];
    }
    return ERROR;
}


int KSCCVT::bisearch_j2t(int max, char* key)
{
    register int lower = 0;
    register int upper = max - 1;
    register int mid;
	register int cmp;
	
    while (lower <= upper) {
        mid = (lower + upper) >> 1;
		cmp = dbcmp(key, &j2tindex[mid*2]);
        if ( cmp > 0 )
            lower = mid + 1;
        else if (cmp < 0 )
            upper = mid - 1;
        else // cmp == 0 
            return mid;
    }
    return ERROR;
}

int KSCCVT::bisearch_t2j(int max, char* key)
{
    register int lower = 0;
    register int upper = max - 1;
    register int mid;
	
    while (lower <= upper) {
        mid = (lower + upper) >> 1;
        if ( *key > t2jindex[mid] )
            lower = mid + 1;
        else if (*key < t2jindex[mid] )
            upper = mid - 1;
        else // key == tbl[mid] 
            return mid;
    }
    return ERROR;
}

int KSCCVT::dbcmp(char* s, char* t)
{
	if (*s > *t )
		return 1;
	else if (*s < *t)
		return -1;
	
	s++; t++;
	if (*s > *t) 
		return 1;
	else if (*s < *t) 
		return -1;
	
	return(0);
}