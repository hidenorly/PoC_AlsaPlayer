# compiler env.
UNAME := $(shell uname -s)
ifeq ($(UNAME),Linux)
	CXX=ccache clang++
	LDLIBS=-ldl -lasound
	CXXFLAGS=-std=c++2a -MMD -MP -Wall -fPIC
endif
ifeq ($(UNAME),Darwin)
	CXX=ccache clang++
	LDLIBS=-stdlib=libc++
	CXXFLAGS=-std=c++2a -MMD -MP -Wall
endif

LDFLAGS=-pthread

# project config
EXEC_DIR ?= ./src
AFW_DIR=../audioframework
INC_DIR=$(AFW_DIR)/include
LIB_DIR=$(AFW_DIR)/lib
OBJ_DIR=./out

# --- source code config --------------
EXEC_SRCS = $(wildcard $(EXEC_DIR)/*.cpp)

# --- the object files config --------------
EXEC_OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(EXEC_SRCS:.cpp=.o)))

# --- Build for sink example(shared) ------------
UNAME := $(shell uname -s)
ifeq ($(UNAME),Linux)
	AFW_SO_TARGET = $(LIB_DIR)/libafw.so
endif
ifeq ($(UNAME),Darwin)
	AFW_SO_TARGET = $(LIB_DIR)/libafw.dylib
endif
EXEC_TARGET = alsaplayer
EXEC_DEPS = $(EXEC_OBJS:.o=.d)

default: $(EXEC_TARGET)
.PHONY: default

$(EXEC_TARGET): $(EXEC_OBJS)
	$(CXX) $(LDFLAGS) $(EXEC_OBJS) -o $@ $(LDLIBS) $(AFW_SO_TARGET)

$(EXEC_OBJS): $(EXEC_SRCS)
	@[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I $(INC_DIR) -c $(EXEC_DIR)/$(notdir $(@:.o=.cpp)) -o $@

-include $(EXEC_DEPS)

# --- clean up ------------------------
clean:
	rm -f $(EXEC_OBJS) $(EXEC_TARGET)
