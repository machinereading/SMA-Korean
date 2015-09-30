

class TBL{

public :
	TBL();
	~TBL();

	void initTBL(char*, int);
	void deleteTBL();

	int getindex(char*);
	int gettblsize();
	float getpx(int);
	char* getstring(int);
	float getminp();
	int getminindex();
	
private :

	char** tbl;
	float* tbl_prob;
	float min_prob;
	float max_prob;
	int maxtbl;
	int min_index;
	int bisearch(char*);

	int flag_state;
};
