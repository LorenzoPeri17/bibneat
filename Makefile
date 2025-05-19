CXX=clang++
CXXFLAGS=-std=c++20 -O3 -fPIC -Wall -Wextra -Werror -fno-exceptions -DNDEBUG

CC=clang
CFLAGS=-O3 -fPIC -Wall -Wextra -Werror -DNDEBUG

LD_FLAGS= 

BASEDIR = $(shell pwd)
BIBNEAT_SRC_DIR=$(BASEDIR)/src
BIBNEAT_BUILD_DIR=$(BASEDIR)/build
BIBNEAT_BIN_DIR=$(BASEDIR)/bin
BIBNEAT_LIB_DIR=$(BASEDIR)/lib

CURL_LDFLAGS=-lcurl
CURL_CFLAGS= 
# CURL_CFLAGS=`pkg-config --cflags libcurl`
# CURL_LDFLAGS=`pkg-config --libs libcurl`

ICU_CFLAGS=$(shell pkg-config --cflags icu-uc icu-io icu-i18n)
ICU_LDFLAGS=$(shell pkg-config --libs icu-uc icu-io icu-i18n)

CXX_SUBMODULES:=database recipes utils cli
C_CURL_SUBMODULES:=requests
CXX_ICU_SUBMODULES:=uni

BIBNEAT_CXX_INCLUDE = $(foreach mod, $(CXX_SUBMODULES), -I$(BIBNEAT_SRC_DIR)/$(mod))
BIBNEAT_CXX_INCLUDE+= $(foreach mod, $(C_CURL_SUBMODULES), -I$(BIBNEAT_SRC_DIR)/$(mod))
BIBNEAT_CXX_INCLUDE+= $(foreach mod, $(CXX_ICU_SUBMODULES), -I$(BIBNEAT_SRC_DIR)/$(mod))

# define standard colors
ifneq (,$(findstring xterm,${TERM}))
	BLACK        := $(shell tput -Txterm setaf 0)
	RED          := $(shell tput -Txterm setaf 1)
	GREEN        := $(shell tput -Txterm setaf 2)
	YELLOW       := $(shell tput -Txterm setaf 3)
	LIGHTPURPLE  := $(shell tput -Txterm setaf 4)
	PURPLE       := $(shell tput -Txterm setaf 5)
	BLUE         := $(shell tput -Txterm setaf 6)
	WHITE        := $(shell tput -Txterm setaf 7)
	RESET 		 := $(shell tput -Txterm sgr0)
else
	BLACK        := ""
	RED          := ""
	GREEN        := ""
	YELLOW       := ""
	LIGHTPURPLE  := ""
	PURPLE       := ""
	BLUE         := ""
	WHITE        := ""
	RESET        := ""
endif

.PHONY: all
all: bin lib clean

.PHONY: bin
bin: bibneat bibgrab

.PHONY: lib
lib: libshared

.PHONY: dirs
dirs:
	@mkdir -p $(BIBNEAT_BUILD_DIR)
	@mkdir -p $(BIBNEAT_LIB_DIR)
	@mkdir -p $(BIBNEAT_BIN_DIR)

.PHONY: clean
clean:
	@echo "$(YELLOW)Cleaning up$(RED)"
	rm -rf $(BIBNEAT_BUILD_DIR)
	@echo "$(GREEN)All Done$(RESET)"

.PHONY: bibneat
bibneat: $(CXX_SUBMODULES) $(C_CURL_SUBMODULES) $(CXX_ICU_SUBMODULES)
	@echo "$(YELLOW)Linking binary$(RESET)"
	$(CXX) $(CXXFLAGS) $(CURL_CFLAGS) $(ICU_CFLAGS) $(BIBNEAT_CXX_INCLUDE) $(LD_FLAGS) $(CURL_LDFLAGS) $(ICU_LDFLAGS) $(wildcard $(BIBNEAT_BUILD_DIR)/*.o) $(BIBNEAT_SRC_DIR)/bibneat.cpp  -o $(BIBNEAT_BIN_DIR)/bibneat
	@echo "$(GREEN)Bibneat built$(RESET)"

.PHONY: bibgrab
bibgrab: $(CXX_SUBMODULES) $(C_CURL_SUBMODULES) $(CXX_ICU_SUBMODULES)
	@echo "$(YELLOW)Linking binary$(RESET)"
	$(CXX) $(CXXFLAGS) $(CURL_CFLAGS) $(ICU_CFLAGS) $(BIBNEAT_CXX_INCLUDE) $(LD_FLAGS) $(CURL_LDFLAGS) $(ICU_LDFLAGS) $(wildcard $(BIBNEAT_BUILD_DIR)/*.o) $(BIBNEAT_SRC_DIR)/bibgrab.cpp  -o $(BIBNEAT_BIN_DIR)/bibgrab
	@echo "$(GREEN)Bibgrab built$(RESET)"

.PHONY: libshared
libshared: libinclude $(CXX_SUBMODULES) $(C_CURL_SUBMODULES) $(CXX_ICU_SUBMODULES)
	@echo "$(YELLOW)Making shared library$(RESET)"
	$(CXX) $(CXXFLAGS) $(CURL_CFLAGS) $(ICU_CFLAGS) $(BIBNEAT_CXX_INCLUDE) $(LD_FLAGS) $(CURL_LDFLAGS) $(ICU_LDFLAGS) $(wildcard $(BIBNEAT_BUILD_DIR)/*.o) -shared -o $(BIBNEAT_LIB_DIR)/libbibneat.so
	@echo "$(GREEN)Shared library built$(RESET)"

.PHONY: libincludedirs
libincludedirs: dirs
	@mkdir -p $(foreach mod, $(CXX_SUBMODULES), $(BIBNEAT_LIB_DIR)/bibneat/$(mod))
	@mkdir -p $(foreach mod, $(C_CURL_SUBMODULES), $(BIBNEAT_LIB_DIR)/bibneat/$(mod))
	@mkdir -p $(foreach mod, $(CXX_ICU_SUBMODULES), $(BIBNEAT_LIB_DIR)/bibneat/$(mod))

.PHONY: libinclude
libinclude: libincludedirs
	@echo "$(YELLOW)Writing library headers$(RESET)"
	$(foreach mod, $(CXX_SUBMODULES), $(shell cp $(wildcard $(BIBNEAT_SRC_DIR)/$(mod)/*.hpp) $(BIBNEAT_LIB_DIR)/bibneat/$(mod)))
	$(foreach mod, $(C_CURL_SUBMODULES), $(shell cp $(wildcard $(BIBNEAT_SRC_DIR)/$(mod)/*.h) $(BIBNEAT_LIB_DIR)/bibneat/$(mod)))
	$(foreach mod, $(CXX_ICU_SUBMODULES), $(shell cp $(wildcard $(BIBNEAT_SRC_DIR)/$(mod)/*.hpp) $(BIBNEAT_LIB_DIR)/bibneat/$(mod)))
	@echo "$(GREEN)Library headers done$(RESET)"

$(CXX_SUBMODULES): dirs
	@echo "$(YELLOW)compiling module $@$(RESET)"
	(cd $(BIBNEAT_BUILD_DIR) && $(CXX) $(CXXFLAGS) -c $(wildcard $(BIBNEAT_SRC_DIR)/$@/*.cpp))

$(C_CURL_SUBMODULES): dirs
	@echo "$(YELLOW)compiling module $@$(RESET)"
	(cd $(BIBNEAT_BUILD_DIR) && $(CC) $(CFLAGS) $(CURL_CFLAGS) -c $(wildcard $(BIBNEAT_SRC_DIR)/$@/*.c))

$(CXX_ICU_SUBMODULES): dirs
	@echo "$(YELLOW)compiling module $@$(RESET)"
	(cd $(BIBNEAT_BUILD_DIR) && $(CXX) $(CXXFLAGS) $(ICU_CFLAGS) -c $(wildcard $(BIBNEAT_SRC_DIR)/$@/*.cpp))