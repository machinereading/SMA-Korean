/* 

  Statistical Morphological Analyzer(SMA)
  Copyright (c) 2012 by CBNU LaKE Lab.  Ver. 0.5

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "./const.h"
#include "./sma/sma.h"
#include "./public/ivt.h"
#include "./public/TBL.h"
#include "./converter/KSCCVT.h"
#include "./converter/UTFCVT.h"

KSCCVT Kconverter;
UTFCVT Uconverter;

SMA word;

IVT Dictionary;
IVT GEN_IVT;
TBL GEN_PU;
TBL GEN_NGRAM;
IVT SEG_IVT;
TBL SEG_TBL;
IVT TAG_WIVT;
IVT TAG_TIVT;
TBL TAG_TBL;

bool process_option(int, char**);
void init_result(SMA_Result*);
void copy_result(SMA_Result*, int);
void print_result(FILE*, SMA_Result*, float);
int split_word(char*, char*, int);

bool isFirstUTF(unsigned char*);
void deletethreebyte(unsigned char*);

int isUTF;
int topk, DicMA_flag;
int result_bound;
float ratio_of_log_different;

float initprob;
float segminp;

char copyright[]=  {"(C)opyright 2012 by CBNU LaKE Lab.  Ver. 0.5\n"};

FILE *fp_in;
FILE *fp_out;

int main(int argc, char* argv[])
{
	char buf_line[MAXSTR*2];
	char ejeol[MAXSTR];
	char tcdata[MAXSTR];

	SMA_Result* SMA_analyzed;
	
	int i, Dic_cnt, cntline;

	int progress, beforeprogress;
	int total_byte, cnt_byte;

	/////////////////////////////////////////////////////////////////
	/////////////////default 실험용 변수 선언부 //////////////////////
	topk = 10;
	DicMA_flag = 1;
	ratio_of_log_different = (float) 1.2;
	isUTF = 1;
	/////////////////////////////////////////////////////////////////
	///////////////////// argument setting //////////////////////////
	
	if ( !process_option(argc, argv) ) 
		return 0;

	fprintf(stderr, copyright);
	/////////////////////////////////////////////////////////////////
	////////////////////// 학습용 데이터 업로드 //////////////////////
	Dictionary.initIVT((char*)FN_IVT_DicMA, (char*)FN_POST_DicMA, 2);
	
	GEN_IVT.initIVT((char*)FN_IVT_GEN, (char*)FN_IVT_POST, 1);
	GEN_PU.initTBL((char*)FN_TBL_GENPU, 1);
	GEN_NGRAM.initTBL((char*)FN_TBL_GENNGRAM, 2);
	
	SEG_IVT.initIVT((char*)FN_IVT_SEG, (char*)FN_POST_SEG, 1);
	SEG_TBL.initTBL((char*)FN_TBL_SEG, 2);

	TAG_WIVT.initIVT((char*)FN_IVT_TAGWT, (char*)FN_POST_TAGWT, 1);
	TAG_TIVT.initIVT((char*)FN_IVT_TAGTT, (char*)FN_POST_TAGTT, 1);
	TAG_TBL.initTBL((char*)FN_TBL_TAG, 2);
	/////////////////////////////////////////////////////////////////
	/////////////////////// 포인터 공간 할당 /////////////////////////
	SMA_analyzed = new SMA_Result[topk+1];

	/////////////////////////////////////////////////////////////////
	///////////////////// variation initialize //////////////////////
	cntline = 0;
	progress = beforeprogress = total_byte = cnt_byte = 0;

	fseek(fp_in, 0, SEEK_END);
	total_byte = ftell(fp_in);
	rewind(fp_in);
	
	////////////////////////////////////////////////////////////////
	//////////////////// main function /////////////////////////////
	while(!feof(fp_in)){
		
		buf_line[0] = '\0';
		fgets(buf_line, MAXSTR, fp_in);
		if ( buf_line[0] == '\0' || buf_line[0] == '\n') break;

		cnt_byte += strlen(buf_line) + 1;
		cntline++;

		i = 0;
		while( buf_line[i] != '\n' && buf_line[i] != '\0' ) {

			//////////////// 입력 문장을 어절로 구분 /////////////////////
			ejeol[0] = '\0';
			i = split_word(buf_line, ejeol, i);
	
			if ( ejeol[0] == '\0' ) {
			//	i++;
				continue;
			}

			switch(isUTF) {
			case 0:
				Kconverter.tcnorm(ejeol, tcdata, 0);
				break;
			case 1:
				Uconverter.UTF2T((unsigned char*)ejeol, tcdata, 0);
				break;
			}

			init_result(SMA_analyzed);
			
			/////////////////// 형태소 분석 처리 /////////////////////////
		
			// DicMA_search
			Dic_cnt = Dictionary.lookup(tcdata);

			if ( Dic_cnt == 0 || DicMA_flag == 0 ) {
				initprob = 0.0;
				fprintf(fp_out, "%s 1\n", ejeol);
				word.analyze(tcdata, SMA_analyzed, topk);
				print_result(fp_out, SMA_analyzed, ratio_of_log_different);
			} else {
				fprintf(fp_out, "%s 0\n", ejeol);
				copy_result(SMA_analyzed, Dic_cnt);
				print_result(fp_out, SMA_analyzed, 100);
			}

			progress = (int) ( (float) cnt_byte / (float) total_byte * 100 );
			if ( (progress != beforeprogress) && ( ( progress - beforeprogress ) >= 1 ) ) {
				if ( progress > 100 ) progress = 100; 
				printf("%3d%% analyzed completed :: %8d line\n", progress, cntline);
			}
			beforeprogress = progress;

		}

		fprintf(fp_out, "\n");
	}

	//////////////////////////////////////////////////////////////////
	////////////////////////// 메모리 해제 ///////////////////////////
	delete SMA_analyzed;

	Dictionary.deleteIVT();
	GEN_IVT.deleteIVT();
	GEN_PU.deleteTBL();
	GEN_NGRAM.deleteTBL();
	SEG_IVT.deleteIVT();
	SEG_TBL.deleteTBL();
	TAG_WIVT.deleteIVT();
	TAG_TIVT.deleteIVT();
	TAG_TBL.deleteTBL();
	//////////////////////////////////////////////////////////////////
	
	fclose(fp_in);
	fclose(fp_out);	

	return 0;
}

bool process_option(int argc, char** argv)
{
	int i;
	result_bound = 10;

	for( i = 1; i < argc; i++ ) {

		if ( argv[i][0] == '-' ) {

			if ( argv[i][1] == 'i' || argv[i][1] == 'I') {
				i++;
				fp_in = fopen(argv[i], "r");
			} else if( argv[i][1] == 'o' || argv[i][1] == 'O') {
				i++;
				fp_out = fopen(argv[i], "w");
			} else if( argv[i][1] == 'd' || argv[i][1] == 'D' ) {
				i++;
				DicMA_flag = atoi(argv[i]);
			} else if( argv[i][1] == 'p' || argv[i][1] == 'P') {
				i++;
				ratio_of_log_different = (float) atof(argv[i]);
			} else if( argv[i][1] == 't' || argv[i][1] == 'T') {
				i++;
				topk = atoi(argv[i]);
			} else if( argv[i][1] == 'e' || argv[i][1] == 'E') {
				isUTF = 0 ;
			} else if( argv[i][1] == 'n' || argv[i][1] == 'N' ) { 
				i++;
				result_bound = atoi(argv[i]);			
			} else if ( argv[i][1] == 'h' || argv[i][1] == 'H' ) {
				printf("SMA options :: -i | -I  filename  - inputfilename\n");
				printf("               -o | -O  filename  - outputfilename\n");
				printf("               -e                 - input encoding set using EUC-KR               :: default : UTF-8\n");
				printf("               -n | -N  number    - maximum result number\n");
				printf("               -d | -D  ( 0 | 1 ) - Dictionary switch( 0(off) | 1(on) )           :: default : 1\n");
				printf("               -p | -P  number    - probability cut                               :: default : 1.2\n");
				printf("               -t | -T  number    - analyze result number(top-k)                  :: default : 10\n");
				return false;

			} else{
				printf("check options  ::  SMA -h\n");
				return false;
			}

		} else {
			return false;
		}

	}

	if ( fp_in == NULL || fp_out == NULL ) {
		printf("input && output file error\n");
		return false;
	}

	return true;
}

int split_word(char* src, char* dest, int index) {

	int j;

	j = 0;
	switch(isUTF) {
	///////////////////////////////////////////////////////////////////////////
	//////////////////////////// EUC-KR code split ////////////////////////////
	case 0 :
		while( src[index] != ' ' && src[index] != '\0' && src[index] != '\n' )
			dest[j++] = src[index++];
		dest[j] = '\0';

		if (src[index] == ' ' ) index++;
		return index;

	///////////////////////////////////////////////////////////////////////////
	//////////////////////////// UTF-8 code split /////////////////////////////
	case 1:
		while( (unsigned char)src[index] != 0 && (unsigned char)src[index] != 10 && (unsigned char)src[index] != 32 && (unsigned char)src[index] != 13) {
			if (  (unsigned char) src[index] == 0xc2 && (unsigned char) src[index+1] == 0xa0 ) {
				index+=2;
				break;
			}
			dest[j++] = (unsigned char) src[index++];
		}
		dest[j] = 0;

		if( isFirstUTF((unsigned char*)dest) ) {
			deletethreebyte((unsigned char*)dest);
		}

		if ( (unsigned char)src[index] == 32 ) index++;
		return index;
	}

	return index;
}

bool isFirstUTF(unsigned char* data) {
	if ( data[0] == 239 && data[1] == 187 && data[2] == 191 )
		return true;
	return false;
}

void deletethreebyte(unsigned char* data) {

	int i, j;
	unsigned char temp[MAXSTR];

	memset(temp, 0, MAXSTR);

	i = 0;
	j = 3;

	while( data[j] != 0 )
		temp[i++] = data[j++];
	temp[i] = 0;

	memcpy(data, temp, MAXSTR); 

}

void init_result(SMA_Result* result)
{
	int i = 0;
	while( result[i].str[0] != '\0' && result[i].str[0] != '$' && i < topk+1){
		result[i].str[0] = '\0';
		result[i++].p = 0.0;
	}
}

void copy_result(SMA_Result* result, int count)
{
	int i;
	float total;
	char buf[MAXSTR];

	total = 0.0;
	for( i = 0; i < count; i++ ){
		Dictionary.get_answer(buf);
		Dictionary.cnt_offset();
		sscanf(buf, "%s %f", result[i].str, &result[i].p);
		total += result[i].p;
	}

	for( i = 0; i < count; i++ )
		result[i].p /= total;
}

void print_result(FILE* fp_out, SMA_Result* result_SMA, float ratio) 
{
	int i;
	i = 0;	
	char buf[MAXSTR];
	
	while( result_SMA[i].str[0] != '\0' && result_SMA[i].str[0] != '$' && i < result_bound) {
		
		switch ( isUTF ) {
		case 0:
			Kconverter.denorm(result_SMA[i].str, buf);
			break;
		case 1:
			Uconverter.T2UTF(result_SMA[i].str, (unsigned char*)buf);
			break;
		}

		if ( i > 0 && ( result_SMA[i].p / result_SMA[i-1].p ) > ratio ) break;
		fprintf(fp_out, "%s %0.5f\n", buf, result_SMA[i].p);
		i++;
	}
	
	if (i == 0)
		fprintf(fp_out, "analyze_fail 0.0\n");
	
	fprintf(fp_out, "\n");
}
