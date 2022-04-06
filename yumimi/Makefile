IDIR := src
SRCDIR := src
ODIR := obj
LDIR :=

BLACKTDIR = ../blackt
LIBMDDIR = ../libmd
DISCASTERDIR = ../discaster

AR=ar
CXX=g++
# Compile only
CDEFINES = -DBLACKT_ENABLE_LIBPNG
#CLIBS = -lpng
CFLAGS = -std=gnu++11 -O2 -Wall -L${LIBMDDIR} -lmd -L${BLACKTDIR} -lblackt -lpng
CINCLUDES = -I${BLACKTDIR}/src -I${LIBMDDIR}/src
CXXFLAGS=$(CFLAGS) $(CDEFINES) $(CINCLUDES) -I$(IDIR)

LIBDEPS := $(LIBMDDIR)/libmd.a $(BLACKTDIR)/libblackt.a

TOOLSRCS := $(wildcard $(SRCDIR)/*.cpp)
TOOLSINDIR := $(patsubst %.cpp,%,$(TOOLSRCS))
TOOLS := $(notdir $(TOOLSINDIR))

.SECONDEXPANSION:

all: blackt libmd discaster $(TOOLS)

blackt:
#	cd ${BLACKTDIR} && $(MAKE) && cd $(CURDIR)
	$(MAKE) -C${BLACKTDIR}

libmd:
#	cd ${LIBMDDIR} && $(MAKE) && cd $(CURDIR)
	$(MAKE) -C${LIBMDDIR}

discaster:
	$(MAKE) -C${DISCASTERDIR}

$(TOOLS): $(SRCDIR)/$$@.cpp $(LIBDEPS)
	make blackt
	make libmd
	make discaster
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $(notdir $@) $(CXXFLAGS)

.PHONY: blackt libmd discaster cleanme clean

cleanme:
	rm -f $(TOOLS)

clean: cleanme
#	rm -f $(LIB)
#	rm -rf $(ODIR)
#	cd ${BLACKTDIR} && $(MAKE) clean && cd $(CURDIR)
#	cd ${LIBMDDIR} && $(MAKE) clean && cd $(CURDIR)
	$(MAKE) -C${BLACKTDIR} clean
	$(MAKE) -C${LIBMDDIR} clean
	$(MAKE) -C${DISCASTERDIR} clean