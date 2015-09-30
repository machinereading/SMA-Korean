#include "../const.h"
#define MAXSYMTBL 100

typedef struct {
	int len[MAXSPNUM];
	char normchar[MAXSPNUM][2];
	char sym[MAXSTR];
} SYMTABLE;

class KSCCVT {

public:	
	KSCCVT();
	~KSCCVT();

	void tcnorm(char*, char*, int); // ( src, dest, switch(word(0)/lextag(1)))
	void denorm(char*, char*); // ( src, dest)

//	void init_symbolindex();
	char* norm_symbols_list(int);
	
private:
	
	//-----------------------------------------//

	void denorm_lex(char*, char*);

	//-----------------------------------------//

	void initStrbuf();
	void initInword(char*);
	void initOutword();
	
	void markbackchar();
	void backchar();
	int dbtoken(char, char, char*);
	
	void addStrbuf(char);
	void addOutword(char);
	void addOutwordStr(char*);
	
	void recordBook(char);
	void tcodeout();
	void strcatz(char* , char*);
	void addchar(char* , char);

	char nextchar();

	//----------------------------------------//

	int is_err;

	int str_index;
	char strbuf[MAXSTR*2];
	
	char input[MAXSTR*2];
	int input_len;
	int input_ptr;
	
	char output[MAXSTR*2];
	int output_ptr;
	
	int mark_index;

	int symbol_x;
	int symbol_dx;

	SYMTABLE* symbols;

	//---------------------------------------//
	// 특수문자 처리를 위한 루틴
	void init_spchar();

	//---------------------------------------//
	// Tcode 변환 프로그램을 위한 루틴		
	int codetype(char);
	void k2tx(char* , char*);
	void k2t(char*, char*);
	void t2k(char*, char*);
	void j2t(char*, char*);
	void t2j(char*, char*);
	void t2j_cho(char*, char*);
	void t2k(char*, char);
	void tx2k(char*, char*);
	char* cut_tsyl(char*, char*);
	int bisearch_t2k(int, char*);
	int bisearch_t2j(int, char*);
	int bisearch_j2t(int, char*);	
	int dbcmp(char*, char*);	
};