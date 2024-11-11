CXX       := g++-9
CXXFLAGS  := -Wall -g3 -std=gnu++17

CUR_DIR   := /local/cse582_dynamicswap
SRC_DIR   := $(CUR_DIR)/src
OBJ_DIR   := $(CUR_DIR)/obj

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

INC_DIR := $(CUR_DIR)/inc

AIFM_PATH := /local/AIFM/aifm

SHENANGO_PATH := /local/AIFM/shenango
INC := -I$(SHENANGO_PATH)/bindings/cc -I$(SHENANGO_PATH)/ksched -I$(AIFM_PATH)/inc \
       -I$(AIFM_PATH)/DataFrame/AIFM/include/ -I$(SHENANGO_PATH)/inc

all: $(OBJ_FILES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) $(INC) -fconcepts -fpermissive -c -o  $@ $<

