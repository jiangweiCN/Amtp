#makefile

ZMQLIB              = /home/libdev/zmq/lib
HOME                = ..
SRC_CODE            = $(HOME)/source
INCLUDE             = $(SRC_CODE)/include
OUTPUT_PATH         = $(HOME)/bin
OUTPUT_PATH_I368    = $(HOME)/bin_i386
MAIN                = $(SRC_CODE)/libjwumq
JWUMQ               = $(SRC_CODE)/jwumq

CXX = g++
CXX_OPTIONS = -Wall -DOS_LINUX -O2 -std=c++14 -fPIC -rdynamic -Wl,-rpath,./
CXX_LDOPTIONS = -shared -Wl,-lz -lm -lpthread
LIBS = -lzmq -L$(ZMQLIB) -lglog -lprotobuf


SRCS = $(wildcard $(MAIN)/libjwumq.cpp \
					$(JWUMQ)/jwumq_base.cpp  \
					$(JWUMQ)/jwumq_dealer.cpp\
					$(JWUMQ)/jwumq_message.cpp \
					$(JWUMQ)/jwumq_message.pb.cpp )

OBJS= $(SRCS:%.cpp=%.o)

EXES= $(OUTPUT_PATH)/libjwumq.so.1.0.3
EXES_I368= $(OUTPUT_PATH_I368)/libjwumq.so.1.0.3


all_i386:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES_I368) $(OBJS) $(LIBS) $(CXX_LDOPTIONS)

all:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES) $(OBJS) $(LIBS) $(CXX_LDOPTIONS)

$(OBJS):%.o:%.cpp
	$(CXX) $(CXX_OPTIONS) -o $@ -c $<

clean:
	rm -f ./$(EXES) $(MAIN)/*.o $(JWUMQ)/*.o
	
clean_i386:
	rm -f ./$(EXES_I368) $(MAIN)/*.o $(JWUMQ)/*.o


