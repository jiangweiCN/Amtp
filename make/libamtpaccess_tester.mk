
#makefile

ZMQLIB              = /home/libdev/zmq/lib
ZMQINCLUDE          = /home/libdev/zmq/include
HOME                = ..
SRC_CODE            = $(HOME)/source
OUTPUT_PATH         = $(HOME)/bin
OUTPUT_PATH_I368    = $(HOME)/bin_i386
MAIN                = $(SRC_CODE)/libamtpa
MSG                 = $(SRC_CODE)/amtpa_protocol
JWUMQ               = $(SRC_CODE)/jwumq

CXX = g++
CXX_OPTIONS = -Wall -DOS_LINUX -O2 -std=c++14 -fPIC -ldl -rdynamic -Wl,-rpath,./
CXX_LDOPTIONS =  -Wl,-lz -lm -lpthread
LIBS = -lzmq -L$(ZMQLIB) -lprotobuf 


SRCS = $(wildcard $(MAIN)/test_main.cpp \
					$(JWUMQ)/jwumq_message.cpp \
					$(JWUMQ)/jwumq_message.pb.cpp \
					$(MSG)/amtpap_msg.pb.cpp )

OBJS= $(SRCS:%.cpp=%.o)

EXES= $(OUTPUT_PATH)/libamtpaccess_tester

EXES_I368= $(OUTPUT_PATH_I368)/libamtpaccess_tester

all:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES) $(OBJS) $(LIBS) $(CXX_LDOPTIONS)

all_i386:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES_I368) $(OBJS) $(LIBS) $(CXX_LDOPTIONS)
	
$(OBJS):%.o:%.cpp
	$(CXX) $(CXX_OPTIONS) -o $@ -c $<

clean:
	rm -f ./$(EXES) $(MAIN)/*.o $(MSG)/*.o $(JWUMQ)/*.o

clean_i386:
	rm -f ./$(EXES_I368) $(MAIN)/*.o $(MSG)/*.o $(JWUMQ)/*.o

