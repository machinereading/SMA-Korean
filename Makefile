CROSS_COMPILE=
CC=$(CROSS_COMPILE)gcc
CXX=$(CROSS_COMPILE)g++
AR=$(CROSS_COMPILE)ar
LD=$(CROSS_COMPILE)ld
NM=$(CROSS_COMPILE)nm
STRIP=$(CROSS_COMPILE)strip
RM=rm
RMDIR=rmdir

BINDIR=.

CONFIG_MODE= -g

CFLAGS  = ${CONFIG_MODE}
CXXFLAGS        = ${CONFIG_MODE}
LFLAGS  = ${CONFIG_MODE}

INC             =
LIB             =

OBJS = SmaMain.o \
	converter/KSCCVT1.o converter/KSCCVT2.o converter/UTFCVT.o \
	public/PriorityStack.o public/TBL.o public/ivt.o public/outQ.o \
	sma/gen.o sma/olm.o sma/pyx.o sma/seg.o sma/sma.o sma/tag.o

TARGET = SMA.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(@:%=${BINDIR}/%) $(LFLAGS) $(OBJS) $(LIB)

.SUFFIXES : .cpp .c .o
#.c.o:
%.o:%.c
	$(CC) -o $@ $(CFLAGS) $(INC) -c $<
#.cpp.o:
%.o:%.cpp
	$(CXX) -o $@ $(CXXFLAGS) $(INC) -c $<

clean:
	$(RM) -f $(OBJS) $(TARGET:%=${BINDIR}/%)

$(BINDIR):
	test -z $(BINDIR) || mkdir -p $(BINDIR)

