/* 

  Statistical Morphological Analyzer(SMA)
  Copyright (c) 2012 by CBNU LaKE Lab.  Ver. 0.5

*/


#define NULL_CHO 1	// 段失戚 蒸澗 Tcode 持失..

/*
ぁ  あ  ぃ  い  ぅ  う  ぇ  え  ぉ  お  
か  が  き  ぎ  く  ぐ  け  げ  こ  ご  
さ  ざ  し  じ  す  ず  せ  ぜ  そ  ぞ  
た  だ  ち  ぢ  っ  つ  づ  て  で  と  
ど  な  に  ぬ  ね  の  は  ば  ぱ  ひ  
び  */

#define TCODE_IC_BEG	0x2E	// '.'	initial consonant
#define TCODE_IC_END	0x40	// '@'	
#define TCODE_MV_BEG	0x41	// 'A'	middle vowel
#define TCODE_MV_END	0x56	// 'U'
#define TCODE_FC_BEG	0x61	// 'a'	final consonant
#define TCODE_FC_END	0x7B	// '{'


#define SLTAG '!'		// 0x21 ALPHA
#define SNTAG '\"'		// NUM
#define SHTAG '#'		// HANJA tag
#define NULLTC '$'
#define SFTAG '%'
#define SPTAG '&'
#define SSTAG '\''
#define STARTTC '('
#define ENDTC ')'
#define SETAG '*'
#define SOTAG '+'
#define SWTAG ','
#define SEPTC '-'
#define JASOTAG '`'	// JASO TAG ( 2012.08.23 )

/*
char uji2t_tbl[51] = {
	'.', '/', 'c', '0', 'e', 'f', '1', '2', '3', 'i', 
	'j', 'k', 'l', 'm', 'n', 'o', '4', '5', '6', 'r', 
	'7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
	'U'};
	*/

/*
ぁ  あ  い  ぇ  え  ぉ  け  げ  こ  さ  
ざ  し  じ  す  ず  せ  ぜ  そ  ぞ  た  
だ  ち  ぢ  っ  つ  づ  て  で  と  ど  
な  に  ぬ  ね  の  は  ば  ぱ  ひ  び  
ぁ  あ  ぃ  い  ぅ  う  ぇ  ぉ  お  か  
が  き  ぎ  く  ぐ  け  げ  ご  さ  ざ  
し  じ  ず  せ  ぜ  そ  ぞ  */

/*
int ti2uji_tbl[67] = {
	0, 1, 3, 6, 7, 8, 16, 17, 18, 20, 
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 
	31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 
	0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 
	11, 12, 13, 14, 15, 16, 17, 19, 20, 21, 
	22, 23, 25, 26, 27, 28, 29};
*/


// 切社研 曽失生稽 坦軒.
char uji2t_tbl[51] = {
	'a', 'b', 'c', 'd', 'e', 'f', 'g', '2', 'h', 'i',
	'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', '6', 'r',
	's', 't', 'u', 'v', ';', 'w', 'x', 'y', 'z', '{',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
	'U'
};
/*
     ぁ あ ぃ い ぅ う ぇ え ぉ お   // え,こ,す -> not in JongSung
     か が き ぎ く ぐ け げ こ ご
     さ ざ し じ す ず せ ぜ そ ぞ
     た だ ち ぢ っ つ づ て で と
     ど な に ぬ ね の は ば ぱ ひ
     び
*/

int ti2uji_tbl[67] = {
	0, 1, 3, 6, 7, 8, 16, 17, 18, 20, 
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 
	31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 
	0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 
	11, 12, 13, 14, 15, 16, 17, 19, 20, 21, 
	22, 23, 25, 26, 27, 28, 29
};

