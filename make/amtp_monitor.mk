#makefile

HOME                = ..
SRC_CODE            = $(HOME)/source
INCLUDE             = $(SRC_CODE)/include
OUTPUT_PATH         = $(HOME)/bin
MAIN                = $(SRC_CODE)/amtp_monitor
LOG                 = $(SRC_CODE)/jlog
CJSON               = $(INCLUDE)/cjson
CONFIG              = $(SRC_CODE)/config

CXX = g++
CXX_OPTIONS = -Wall -O2 -std=c++14 -ldl -rdynamic -Wl,-rpath,./
CXX_LDOPTIONS = -Wl,-lz -lm -lpthread
LIBS = -L. -lglog


SRCS = $(wildcard $(MAIN)/main.cpp \
				$(MAIN)/monitor.cpp \
				$(LOG)/jLog.cpp \
				$(CONFIG)/config.cpp\
				$(CONFIG)/amtp_monitor_config.cpp\
				$(CJSON)/cJSON.cpp)

OBJS= $(SRCS:%.cpp=%.o)

EXES= $(OUTPUT_PATH)/amtp_monitor

all:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES) $(OBJS) $(CXX_LDOPTIONS) $(LIBS)

$(OBJS):%.o:%.cpp
	$(CXX) $(CXX_OPTIONS) -o $@ -c $<

clean:
	rm -f ./$(EXES) $(MAIN)/*.o $(LOG)/*.o $(CONFIG)/*.o $(CJSON)/*.o

