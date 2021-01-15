#makefile


HOME                = ..
SRC_CODE            = $(HOME)/source
OUTPUT_PATH         = $(HOME)/bin
MAIN                = $(SRC_CODE)/license

CXX = g++
CXX_OPTIONS = -Wall -DOS_LINUX -O2 -std=c++14 -fPIC -rdynamic -Wl,-rpath,./
CXX_LDOPTIONS = -shared -Wl,-lz -lm -lpthread
LIBS = -lprotobuf


SRCS = $(wildcard $(MAIN)/libamtpa_license.cpp \
					$(MAIN)/license_msg.pb.cpp )

OBJS= $(SRCS:%.cpp=%.o)

EXES= $(OUTPUT_PATH)/libamtpa_license.so.1.0.0

all:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES) $(OBJS) $(CXX_LDOPTIONS) $(LIBS)

$(OBJS):%.o:%.cpp
	$(CXX) $(CXX_OPTIONS) -o $@ -c $<

clean:
	rm -f ./$(EXES) $(MAIN)/*.o




