#include "../public/PriorityStack.h"

typedef struct {
	int itag;
	float p;
} TAGLST, *PTAGLST;


class TAG{
public :
	TAG();
	~TAG();

	void wptag(char *, int);

private :

	void init_morplist(char**);
	int splitwp(char *);
	void outtagseq();
	void initseg();
	void addseg(char*);

	int mtag(char *, char *);
	int pwt(int);
	int ptt(int);
	
	void init_taglist(PTAGLST);
	
	char** morphlist;
	char* segstring;
	int segstrptr;
	char tagstr[MAXSTR];

	int maxnode;

	float init_prob_tag;

	S v;

	T tt;
	PT pt;

	PTAGLST wtaglist;
	PTAGLST ttaglist;
	PTAGLST taglist;
};

