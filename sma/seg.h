#include "../public/PriorityStack.h"

typedef struct STNODE *PNODE;
typedef struct STNODE {
	int row;				// row
	int col;				// column
	PNODE nextnode;
} NODE;

typedef struct STSUB *PSUB;
typedef struct STSUB {
	float prob;
	PNODE startnode;
	PSUB nextsub;
} SUB;


typedef struct STCELL  *PCELL;
typedef struct STCELL {
	PSUB sub;
} CELL;

class SEG{
public :
	SEG();
	~SEG();
	void CKY(char *);
	
private :
	
	void substring(char *, char *, int, int);
	PSUB makesub(PNODE);
	PNODE makenode(int, int);
	
	void initnodestack();
	void pushnode(PNODE);
	PNODE popnode();
	
	void subdump(PSUB);
	void subprint(PSUB);
	void nodeprint(PNODE);
	void subfree(PSUB);
	void initseg();
	void addseg(char *);
	void delseg();
	S v;

	int	segstrptr;
	char* segstring;
	
	int nodestacktop;
	PNODE nodestack[MAXNODESTACK];
	char input[MAXSTR];

};















