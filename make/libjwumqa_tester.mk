
#makefile


HOME                = ..
SRC_CODE            = $(HOME)/source
OUTPUT_PATH         = $(HOME)/bin
MAIN                = $(SRC_CODE)/libjwumqa/tester_cpp
JWUMQ               = $(SRC_CODE)/jwumq

CXX = g++
CXX_OPTIONS = -Wall -DOS_LINUX -O2 -std=c++14 -fPIC -ldl -rdynamic -Wl,-rpath,./
CXX_LDOPTIONS =  -Wl,-lz -lm -lpthread -ldl
LIBS = -lprotobuf -L../bin -ljwumqa


SRCS = $(wildcard $(MAIN)/libjwumqa_tester.cpp \
		$(JWUMQ)/jwumq_message.cpp \
		$(JWUMQ)/jwumq_message.pb.cpp )

OBJS= $(SRCS:%.cpp=%.o)

EXES= $(OUTPUT_PATH)/libjwumqa_tester

all:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES) $(OBJS) $(LIBS) $(CXX_LDOPTIONS)

$(OBJS):%.o:%.cpp
	$(CXX) $(CXX_OPTIONS) -o $@ -c $<

clean:
	rm -f ./$(EXES) $(MAIN)/*.o $(JWUMQ)/*.o


