/*
	2012. 8. 28. Daniel Lee

	토큰 구분없이 규칙이 적용되도록 최신 소스로 수정
	- 토큰 정의 및 검사 함수 삭제

*/


#include "../public/PriorityStack.h"
#include "../const.h"

typedef struct {
	char str[MAXWORDLEN];
	float p;
} ANSWERTYPE;

class GEN{

public :
	GEN();
	~GEN();
	void GenRoot(char*, int);
	
private :
	int chkseg(PG);
	void genvar(PG);
	void initanswer();
	void addanswerlist(char* , float );
	void printanswer();
	float pooseg(int *, int);
	int makekey(int, int, int);
	int get_post(int, char*, int*, float*);
	
	PG pvstack;
	PG pv;

	G u;

	char pu_str[MaxTokenLen +1];
	
	char answer[MAXWORDLEN];
	char kanswer[MAXWORDLEN];
	
	ANSWERTYPE answerlist[MAXANS+1];		// save 0 for input data
	char input_data[MAXWORDLEN];

};

