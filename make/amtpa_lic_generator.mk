
#makefile


HOME                = ..
SRC_CODE            = $(HOME)/source
OUTPUT_PATH         = $(HOME)/bin
MAIN                = $(SRC_CODE)/license

CXX = g++
CXX_OPTIONS = -Wall -DOS_LINUX -O2 -std=c++14 -fPIC -rdynamic -Wl,-rpath,./
CXX_LDOPTIONS =  -Wl,-lz -lm -lpthread -ldl
LIBS = -lprotobuf -L../bin -lamtpa_license


SRCS = $(wildcard $(MAIN)/amtpa_lic_generator.cpp  )

OBJS= $(SRCS:%.cpp=%.o)

EXES= $(OUTPUT_PATH)/amtpa_lic_generator

all:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES) $(OBJS) $(LIBS) $(CXX_LDOPTIONS)

$(OBJS):%.o:%.cpp
	$(CXX) $(CXX_OPTIONS) -o $@ -c $<

clean:
	rm -f ./$(EXES) $(MAIN)/*.o 


