/*
	2012. 8. 28. Daniel Lee

	��ū ���о��� ��Ģ�� ����ǵ��� �ֽ� �ҽ��� ����
	- ��ū ���� �� �˻� �Լ� ����

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

