CXX       := g++-9
CXXFLAGS  := -Wall -g3 -std=gnu++17
CXXTESTFLAGS := -g3

AIFM_PATH := /local/AIFM/aifm
SHENANGO_PATH := /local/AIFM/shenango

RUNTIME_LIBS = $(SHENANGO_PATH)/libruntime.a $(SHENANGO_PATH)/libnet.a \
	       $(SHENANGO_PATH)/libbase.a -lpthread
librt_libs = $(SHENANGO_PATH)/bindings/cc/librt++.a
lib_src = $(wildcard $(AIFM_PATH)/src/*.cpp)
lib_src := $(filter-out $(AIFM_PATH)/src/tcp_device_server.cpp,$(lib_src))
lib_obj = $(lib_src:.cpp=.o)

CUR_DIR   := /local/cse582_dynamicswap
SRC_DIR   := $(CUR_DIR)/src
OBJ_DIR   := $(CUR_DIR)/obj

TEST_DIR := $(CUR_DIR)/unit_tests
TEST_SRC := $(TEST_DIR)/src
TEST_OBJ := $(TEST_DIR)/obj
TEST_BIN := $(TEST_DIR)/bin

TEST_SRC_FILES := $(wildcard $(TEST_SRC)/*.cpp)
TEST_OBJ_FILES := $(patsubst $(TEST_SRC)/%.cpp,$(TEST_OBJ)/%.o,$(TEST_SRC_FILES))

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

INC_DIR := $(CUR_DIR)/inc


INC := -I$(SHENANGO_PATH)/bindings/cc -I$(SHENANGO_PATH)/ksched -I$(AIFM_PATH)/inc \
       -I$(AIFM_PATH)/DataFrame/AIFM/include/ -I$(SHENANGO_PATH)/inc

all: test_tcp_01

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) $(INC) -fconcepts -fpermissive -c -o $@ $<

$(TEST_OBJ)/%.o: $(TEST_SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -Wno-subobject-linkage -g -Wall -D_GNU_SOURCE $(INC) -I$(SHENANGO_PATH)/inc -I$(SHENANGO_PATH)/rdma-core/build/include -I$(SHENANGO_PATH)/bindings/cc -I$(SHENANGO_PATH)/ksched -Iinc -I$(AIFM_PATH)/DataFrame/AIFM/include/ -I/local/cse582_dynamicswap/inc -DMLX_CX4 -DNDEBUG -O3 -march=native -DMLX5 -DDIRECTPATH -std=gnu++2a -fconcepts -Wno-unused-function -c $< -o $@

test_tcp_01: $(TEST_OBJ)/test_tcp_01.o $(OBJ_FILES)
	$(CXX) -o $(AIFM_PATH)/bin/$@ $(TEST_OBJ)/$@.o $(lib_obj) $(OBJ_FILES) $(librt_libs) $(RUNTIME_LIBS) $(LDFLAGS) -L$(SHENANGO_PATH)/rdma-core/build/lib/statics/ -lmlx5 -libverbs -lnl-3 -lnl-route-3 -T $(SHENANGO_PATH)/base/base.ld -lnuma

test_tcp_02: $(TEST_OBJ)/test_tcp_02.o
	$(CXX) -o $(AIFM_PATH)/bin/$@ $(TEST_OBJ)/$@.o $(lib_obj) $(OBJ_FILES) $(librt_libs) $(RUNTIME_LIBS) $(LDFLAGS) -L$(SHENANGO_PATH)/rdma-core/build/lib/statics/ -lmlx5 -libverbs -lnl-3 -lnl-route-3 -T $(SHENANGO_PATH)/base/base.ld -lnuma