// 
// 0x21 - 0x2d token character definition for normalization
//
#define SLTAG '!'		// 0x21 ALPHA
#define SNTAG '"'		// NUM
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
#define SEPTC '-'		//0x2d

#define JASOTAG	'`'

#define DBSF	"£®£¿£¡"										// ¸¶Ä§Ç¥, ¹°À½Ç¥, ´À³¦Ç¥
#define DBSP	"£¬¤ý¡¤£º£¯"									// ½°Ç¥, °¡¿îµ«Á¡, ÄÝ·Ð, ºø±Ý
#define DBSS	"£¢¡®¡¯¡°¡±¡²¡³¡´¡µ¡¶¡·¡¸¡¹¡º¡»¡¼¡½£¨£©¡©¡ª"	//µû¿ÈÇ¥, °ýÈ£Ç¥, ÁÙÇ¥
#define DBSE	"¡¦"											// ÁÙÀÓÇ¥
#define DBSO	"¡­"											//¹°°á ¼û±è? ºüÁü?

// Macro function definition

#define TCTOKEN(c)		(0x21 <= (unsigned char)c && (unsigned char)c <= 0x2c || (unsigned char)c == 0x60)
#define TCODE(c)	(0x2e <= (unsigned char)c && (unsigned char)c <= 0x7b && (unsigned char)c != 0x60)

//#define HANGUL1ST(x)	((unsigned char)x==0xa4 || (unsigned char)x >= 0xb0 && (unsigned char)x <= 0xc8)
#define HANGUL1ST(x)	((unsigned char)x >= 0xb0 && (unsigned char)x <= 0xc8)
#define HANJA1ST(x)		((unsigned char)x >= 0xca && (unsigned char)x <= 0xfd)
#define HANGUL2ND(x)	((unsigned char)x >= 0xa1 && (unsigned char)x <= 0xfe)
#define HANJA2ND(x)		HANGUL2ND(x)

#define HANGUL(c1, c2) (HANGUL1ST(c1) && HANGUL2ND(c2))
#define HANJA(c1, c2) (HANJA1ST(c1) && HANJA2ND(c2))

#define HANGULCHO(c1, c2) ( (unsigned char)c1 == 0xa4 && HANGUL2ND(c2) )

#define DBLETTER(c1, c2)	((unsigned char)c1==0xa3 && ((unsigned char)c2 >= 0xc1 && (unsigned char)c2 <= 0xda) \
|| (unsigned char)c2 >= 0xe1 && (unsigned char)c2 <= 0xfa))
#define DBNUM(c1, c2)	((unsigned char)c1==0xa3 && ((unsigned char)c2 >= 0xb0 && (unsigned char)c2 <= 0xb9) 

#define LETTER(c)	(c>='a' && c<='z' || c>='A' && c<='Z')
#define NUM(c)		(c>='0' && c<='9')

#define MAXSYMTBL 100
#define MAXMORPTBL 100