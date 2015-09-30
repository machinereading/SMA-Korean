#include <iostream>
#include <cstring>

#include "../const.h"
#include "KSCCVT.h"
#include "KSCCVTlib.h"

KSCCVT::KSCCVT()
{
	symbols = new SYMTABLE;
	symbol_x = symbol_dx = 0;
}

KSCCVT::~KSCCVT()
{

}

// converter library fuction
//
/*
void KSCCVT::init_symbolindex() {
	symbol_x = 0;
}
*/

void KSCCVT::init_spchar()
{
	int i;

	symbol_x = 0;
	symbols->sym[0] = '\0';

	for( i = 0; i < MAXSPNUM; i++) {
		symbols->len[i] = 0;	
		symbols->normchar[i][0] = '\0';
	}
}

void KSCCVT::initStrbuf()
{
	strbuf[0] = '\0';
	str_index = 0;
}

void KSCCVT::initInword(char* in)
{
	strcpy(input, in);
	input_len = strlen(input);
	input[input_len] = '\0';
	input_ptr = 0;
}

void KSCCVT::initOutword()
{
	output_ptr = 0;
	output[0] = '\0';
}

char KSCCVT::nextchar()
{
	if (input_ptr < input_len)
		return(input[input_ptr++]);
	else
		return(input[input_ptr]);
}

void KSCCVT::addStrbuf(char c)
{
	strbuf[str_index++] = c;
	strbuf[str_index] = '\0';
}

void KSCCVT::addOutword(char c)
{
	output[output_ptr++] = c;
	output[output_ptr] = '\0';
}

void KSCCVT::recordBook(char c)
{
	symbols->len[symbol_x] = strlen(strbuf);
	symbols->normchar[symbol_x][0] = c;
	symbols->normchar[symbol_x][1] = '\0';
	strcatz(symbols->sym, strbuf);
	symbols->sym[strlen(symbols->sym)] = '\0';

	symbol_x++;
	addOutword(c);
}

int KSCCVT::dbtoken(char c1, char c2, char* dbstr)
{
	char c;
	
	while( (c = *dbstr++) != '\0' ) {
		if(c1 == c && c2 == *dbstr) return (1);
		dbstr++;
	}
	return (0);
}

void KSCCVT::markbackchar()
{
	mark_index = input_ptr;
}

void KSCCVT::backchar()
{
	input_ptr = mark_index;
}

void KSCCVT::tcodeout()
{
	char tcstr[MAXSTR*2];
	
	k2tx(strbuf, tcstr);
	addOutwordStr(tcstr);
}

void KSCCVT::addOutwordStr(char* s)
{
	strcpy(&output[output_ptr], s);
	output_ptr = strlen(output);
}

void KSCCVT::strcatz(char* t, char* s)
{
	int i;
	
	i = strlen(t);
	while (*s != '\0')
		t[i++] = *s++;
	t[i] = '\0';
}

void KSCCVT::addchar(char* p, char c)
{
	char temp[2];
	
	temp[0] = c;
	temp[1] = '\0';
	strcatz(p, temp);
}


// analyzed tcode(lexical/tag) -> analyzed korean(lexical/tag)
//void KSCCVT::denorm(char* in, char* out)
void KSCCVT::denorm(char* in, char* out)
{
	int i;
	char lex[MAXSTR*2], tag[MAXSTR*2], denormlex[MAXSTR*2];
	char c;
	
	initInword(in);
	initOutword();

	i = 0;
	symbol_dx = 0;

state0:
	c = nextchar();
	
	if ( c == '\0' ) goto state3;
	if ( c == SEPTC) goto state1;

	lex[i++] = c;
	lex[i] = '\0';
	goto state0;

state1:
	i = 0;
	denorm_lex(lex, denormlex);
	strcatz(output, denormlex);
	addchar(output, '/');

state2:
	c = nextchar();

	if ( c == SEPTC ) {
		tag[i++] = '+';
		tag[i] = '\0';
		i = 0;
		strcatz(output, tag);
		goto state0;
	} else if ( c == '\0' ) {
		i = 0;
		strcatz(output, tag);
		goto state4;
	}

	tag[i++] = c;
	tag[i] = '\0';
	goto state2;

state3:
	denorm_lex(lex, denormlex);
	strcatz(output, denormlex);

state4:
	strcpy(out, output);
	symbol_dx = 0;
}


// tcode -> korean
void KSCCVT::denorm_lex(char* in, char* out) 
{
	char c;
	char temp[MAXSTR*2], temp2[MAXSTR*2];
	int i, starti, len;
	int j, str_len;
	int k, sym_locate;
	
	// input: TCODE or TC TOKEN only
	
	*out = '\0';
	i = 0;
	while ((c = in[i++]) != '\0') {
		
		if (TCODE(c)) {
			starti = i - 1;
			while (TCODE(c)) 
				c = in[i++];
			i--;							// back to non TCODE index when exit while loop
			len = i - starti;
			strncpy(temp, &in[starti], len );
			temp[len]='\0';

			if ( len == 1 && temp[0] >= (unsigned char) 0x2e && temp[0] <= (unsigned char) 0x40 ) {
				t2j_cho(temp, temp2);
			} else { 
				tx2k(temp, temp2);
			}

			strcatz(out, temp2);
			continue;
		}
		
		if (TCTOKEN(c)) {

			if ( c == symbols->normchar[symbol_dx][0]) {

				sym_locate = 0;
				for ( k = 0; k < symbol_dx; k++ ) {
					sym_locate += symbols->len[k];
				}

				str_len = strlen(out);
				for( j = 0; j < symbols->len[symbol_dx]; j++) {
					out[str_len+j] = symbols->sym[sym_locate+j];
				}

				out[str_len+j] = '\0';

			} else {
				is_err = 1;
				strcpy(out, "X");
				//printf("TOKEN code mismatch\n");
			}

			symbol_dx++;

		}
	}
}

// korean(lexical) -> tcode(lexical)
void KSCCVT::tcnorm(char* in, char* out, int flag)
{
	char c, c1;
	char temp[MAXTAGLEN+3];
	int temp_len;
	int output_len;
	
	initInword(in);
	initOutword();
	init_spchar();

	temp[0] ='\0';
	out[0] = '\0';
	temp_len =  output_len = 0;
	

state0:

	c = nextchar();

state1:
	if( c == '/' && flag == 1) {
		markbackchar();
		c = nextchar();

		if ( c == '/' ) {
			backchar();
		} else {
			strcpy(temp, "-");
			temp_len = strlen(temp);
			goto state2;
		}
	}

	if (c == '\0') {
		symbol_x++;
		strcpy(out, output);
		return;
	}
	
	initStrbuf();		// lexical buf (for special tokens like alpha, num....)
	
	if ((unsigned char)c>=0x80) goto DBCS;
	
	//SBCS:
	if (LETTER(c)) {
		do {
			addStrbuf(c);
			c=nextchar();
		} while(LETTER(c));
		recordBook(SLTAG);			// Alphabet
		goto state1;
	}
	else if (NUM(c)) {
		do {
			addStrbuf(c);
			c=nextchar();
		} while(NUM(c));
		recordBook(SNTAG);			// Number
		goto state1;
	}
	
	switch(c) {
	case '.':		//finish mark
	case '?':
	case '!':
		addStrbuf(c);
		recordBook(SFTAG);
		break;
	case ',':
	case ':':
	case '/':
		addStrbuf(c);
		recordBook(SPTAG);
		break;
	case '\'':
	case '"':
	case '(':
	case ')':
	case '-':
		addStrbuf(c);
		recordBook(SSTAG);
		break;
	case '~':
		addStrbuf(c);
		recordBook(SOTAG);
		break;
	case '$':
	case '\\':
		addStrbuf(c);
		recordBook(SWTAG);
		break;
	default:
		addStrbuf(c);
		recordBook(SWTAG);
		break;
	}
	goto state0;
	
	
DBCS:
	c1 = nextchar();
	
	if (dbtoken(c, c1, (char*)DBSF)) {
		addStrbuf(c);
		addStrbuf(c1);
		recordBook(SFTAG);
		goto state0;
	}
	else if (dbtoken(c, c1, (char*)DBSP)) {
		addStrbuf(c);
		addStrbuf(c1);
		recordBook(SPTAG);
		goto state0;
	}
	else if (dbtoken(c, c1, (char*)DBSS)) {
		addStrbuf(c);
		addStrbuf(c1);
		recordBook(SSTAG);
		goto state0;
	}
	else if (dbtoken(c, c1, (char*)DBSE)) {
		addStrbuf(c);
		addStrbuf(c1);
		recordBook(SETAG);
		goto state0;
	}
	else if (dbtoken(c, c1, (char*)DBSO)) {
		addStrbuf(c);
		addStrbuf(c1);
		recordBook(SOTAG);
		goto state0;
	}

	if (HANJA(c, c1)) {
		do {
			addStrbuf(c);
			addStrbuf(c1);
			markbackchar();
			c=nextchar();
			c1=nextchar();
		} while(HANJA(c, c1));
		backchar();
		recordBook(SHTAG);
		goto state0;
	}
	
	if ( flag == 0 || ( flag == 1 && ( output_ptr - (temp_len + output_len) == 0 ) ) ) {
		if (HANGULCHO(c, c1)) {

			/*
			addStrbuf(c);
			addStrbuf(c1);
			recordBook(JASOTAG);
			output_len = output_ptr;
			goto state0;
			*/
			//초성을 특수문자로 처리하여 여러개가 동시에 나올 경우 하나의 기호로 처리
			
			do {
				addStrbuf(c);
				addStrbuf(c1);
				markbackchar();
				c=nextchar();
				c1=nextchar();
			} while(HANGULCHO(c, c1));
			backchar();
			recordBook(JASOTAG);
			output_len = output_ptr;
			goto state0;

		}

		if (HANGUL(c, c1)) {
			do {
				addStrbuf(c);
				addStrbuf(c1);
				markbackchar();
				c=nextchar();
				c1=nextchar();
			} while(HANGUL(c, c1));
			backchar();
			tcodeout();		// convert strbuf to tcode  and save it in outstr
			goto state0;
		}
	} else {

		if (HANGUL(c, c1) || HANGULCHO(c, c1)) {
			do {
				addStrbuf(c);
				addStrbuf(c1);
				markbackchar();
				c=nextchar();
				c1=nextchar();
			} while(HANGUL(c, c1));
			backchar();
			tcodeout();		// convert strbuf to tcode  and save it in outstr
			goto state0;
		}
	}

	addStrbuf(c);
	addStrbuf(c1);
	recordBook(SWTAG);
	goto state0;


state2:

	while( c != '+' && c != '\0') {
		temp[temp_len++] = c;
		temp[temp_len] = '\0';
		c = nextchar();
	}

	if ( c == '+' ) {
		temp[temp_len++] = '-';
		temp[temp_len] = '\0';
		strcatz(output, temp);
		output_ptr += temp_len;
		goto state0;
	}

	symbol_x++;
	strcatz(output, temp);
	strcpy(out, output);

}

char* KSCCVT::norm_symbols_list(int index)
{
	return symbols[index].sym;
}