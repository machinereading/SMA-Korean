#include "../const.h"

#include "gen.h"
#include "seg.h"
#include "tag.h"

#include "../public/PriorityStack.h"
#include "../public/outQ.h"

// for Gen
GEN gen_run;
PGstack genstack;

// for Seg
SEG seg_run;
PSstack segstack;

// for Tag
TAG tag_run;
PTstack tstack;
PSstack tagstack;

// for outQ
OUTQ gen_node(MAXANODE);
OUTQ seg_node(MAXANODE);
OUTQ tag_node(MAXANODE);
