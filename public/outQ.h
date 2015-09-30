#include "../const.h"

struct QNODE {
	char str[MAXSTR];
	float p;
};

class OUTQ{
public:
	OUTQ(int);
	int DeQ(char *, float *);
	void EnQ(char *, float);
	void EmptyQ();
	void PrintQ();
	void PrintQ_file(FILE*);
	int sizeof_Q();
	~OUTQ();

private:
	int head;
	int tail;
	int maxQ;
	QNODE *Q;
};
