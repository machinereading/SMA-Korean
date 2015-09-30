/* 

  Statistical Morphological Analyzer(SMA)
  Copyright (c) 2012 by CBNU LaKE Lab.  Ver. 0.5

*/

/*
  UTF2T - code conversion modules between Korean UTF8 and T code (Three byte code for Hangul)
  
	Written by Jae Sung Lee
	Date: Aug 5, 2012
	
	For syllable: 
	 U2T(utf8code, tcode): convert a UTF8 code to the corresponding T code (valid code only) 
	 T2U(tcode, utf8code): convert a T code to the corresponding UTF8 code (valid code only) 
	  
	For Jaso:
	 UJ2T(utf8code, tcode): convert a UTF8 JASO code to the corresponding T code (valid code only) 
	 T2UJ(tcode, utf8code): convert a T code to the corresponding UTF8 JASO code (valid code only) 
		
		  
	* UTF index is a sequence number from start UTF8(=0) to end UTF8(=11171)
	and it is used as conversion pivot.
			
	 internal modules:
	  u2ui: UTF8 to UTF index
	  ui2u: UTF index to UTF8
	  t2ui: T code to UTF index
	  ui2t: UTF index to T code
			  
*/

#include "../const.h"


typedef struct{
	int len[MAXSPNUM];
	char normchar[MAXSPNUM][2];
	unsigned char symbols[MAXSTR];
} SYMTABLE_UTF;

class UTFCVT {
	
public:
	UTFCVT();
	~UTFCVT();

	void UTF2T(unsigned char*, char*, int);
	void T2UTF(char*, unsigned char*);
	
private:
	
	int uidx;
	int ujidx;

	int symbols_x;
	int symbols_dx;

	void uj2t(unsigned char *, char *);
	void t2uj(char, unsigned char *);
	void u2t(unsigned char *, char *);
	void t2u(char *, unsigned char *);
	void u2ui(unsigned char *, int *);
	void ui2u(int, unsigned char *);
	void t2ui(unsigned char *, int *);
	void ui2t(int, unsigned char *);

	void strncpy(unsigned char*, unsigned char*, int, int);	
	void strncatu(unsigned char*, unsigned char*);
//	void stritcpy(unsigned char*);
	int strcnt(unsigned char*);

	// 특수문자 처리
	SYMTABLE_UTF* symbols;

	void init_symbols();
	void u2t_symbols(unsigned char*, char*); 
	void recordsymbols(char*, char*, unsigned char*);
	void t2u_symbol(unsigned char*);


};

