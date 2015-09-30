

// main
#define DEBUG			0
#define	MAXSTR			500
#define MAX_EJ			500

#define MAXMORPH	 	100	//TAG, PriorityStack
#define MAXK			100	//SMA, PriorityStack

// SMA
#define MAXWORDLEN		200
#define MAXANODE		12

#define MAXPLOG			(float) 9999999.9

// converter
#define MAXSPNUM		200

// Priority Stack
#define	MAXWPTAG		MAXMORPH
#define BOTTOM			0
#define UP				1
#define DOWN			-1

/* log probability comparison */
#define	LE(x, y)		((x)>=(y))
#define LT(x, y)		((x)>(y))
#define GE(x, y)		((x)<=(y))
#define GT(x, y)		((x)<(y))

// GEN
#define MaxTokenLen 	2
#define MAXANS 			50
#define MAXGEN 			50
#define MAXNGRAMTBL 	150000

#define NULLCODE		3
#define LEFTENDCODE		7
#define RIGHTENDCODE 	8
#define LEFTENDCHAR 	'('
#define RIGHTENDCHAR 	')'

// SEG
#define MAXNODESTACK 	300

// TAG
#define MAXMORPSET		100
#define MAXMORPLEN		200
#define MAXTAGLIST		80
#define MAXTAGLEN		8

// Files
#define FN_IVT_DicMA		"./data/DIC-ivt.txt"
#define FN_POST_DicMA		"./data/DIC-post.txt"

#define FN_IVT_GEN			"./data/GEN-tccivt.txt"
#define FN_IVT_POST			"./data/GEN-tccpost.txt"
#define FN_TBL_GENPU		"./data/GEN-tcputbl.txt"
#define FN_TBL_GENNGRAM		"./data/GEN-tcngramcnt.txt"

#define FN_IVT_SEG			"./data/SEG-ivt.txt"
#define FN_POST_SEG			"./data/SEG-post.txt"
#define FN_TBL_SEG			"./data/SEG-mutbl.txt"

#define FN_IVT_TAGWT		"./data/TAG-wtivt.txt"
#define FN_POST_TAGWT		"./data/TAG-wtpost.txt"
#define FN_IVT_TAGTT		"./data/TAG-ttivt.txt"
#define FN_POST_TAGTT		"./data/TAG-ttpost.txt"
#define FN_TBL_TAG			"./data/TAG-tagtbl.txt"

