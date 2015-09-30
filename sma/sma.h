#include "../const.h"

typedef struct SMA_OUT
{
	char str[MAXSTR*2];
	float p;
} SMA_Result;

class SMA {

public:
	SMA();
	~SMA();
	void analyze(char*, SMA_Result*, int);

private:
	int maxnode;
	void dumpvstack2seg_node(char*);
	void dumpvstack2tag_node(char*);
	float SMA_initprob(char*);
};
