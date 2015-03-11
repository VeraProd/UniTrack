# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Sources and headers
HEADERS = \
	mongo/client.h \
	mongo/client_pool.h \
	mongo/document.h \
	mongo/mongoc_handler.h \
	mongo/uri.h

SOURCES_CPP = \
	mongo/client.cpp \
	mongo/client_pool.cpp \
	mongo/document.cpp \
	mongo/mongoc_handler.cpp \
	mongo/uri.cpp

MAIN_SOURCES_CPP = main.cpp

# Static library
TARGETS = unitrack

# Tests
TEST_SOURCES_CPP = 


# Sources only
SOURCES_DIR	= src

# Result of build only (executable)
BUILD_DIR	= build

# All .o files (including tests)
OBJECTS_DIR	= obj

# All test executables
TEST_DIR	= test


# Compiler and library archiever
GPP					= g++ -pipe -O2 -Wall -std=c++11 $(EXTRA_CPPFLAGS)
GPP_COMPILEFLAGS	= $(shell pkg-config --cflags libmongoc-1.0)
GPP_LINKFLAGS		= $(shell pkg-config --libs libmongoc-1.0)

ifeq ($(shell uname -s),Darwin)
	GPP_COMPILEFLAGS	+= -I/opt/local/include
	GPP_LINKFLAGS		+= -L/opt/local/lib
endif

# Don't need yet
AR = ar cr


# Terminal colors (0 -- reset, 1 -- bold, 31 -- red, 32 -- green, 34 -- blue).
# See the 'check' target.
COLOR_RESET				= \033[0m
COLOR_TEST_RUN			= \033[34m
COLOR_TEST_PASS			= \033[32m
COLOR_TEST_FAIL			= \033[31m


# Files
HEADER_FILES			= $(addprefix $(SOURCES_DIR)/,$(HEADERS))
SOURCES_CPP_FILES		= $(addprefix $(SOURCES_DIR)/,$(SOURCES_CPP))
MAIN_SOURCES_CPP_FILES	= $(addprefix $(SOURCES_DIR)/,$(MAIN_SOURCES_CPP))

# Object files
OBJECTS					= $(SOURCES_CPP:.cpp=.o)
MAIN_OBJECTS			= $(MAIN_SOURCES_CPP:.cpp=.o)

OBJECT_FILES			= $(addprefix $(OBJECTS_DIR)/,$(OBJECTS))
MAIN_OBJECT_FILES		= $(addprefix $(OBJECTS_DIR)/,$(MAIN_OBJECTS))

TEST_OBJECTS			= $(TEST_SOURCES_CPP:.cpp=.o)
TEST_OBJECT_FILES		= $(addprefix $(OBJECTS_DIR)/,$(TEST_OBJECTS))

# TargetS files
TARGET_FILES			= $(addprefix $(BUILD_DIR)/,$(TARGETS))

TEST_TARGETS			= $(basename $(TEST_SOURCES_CPP))
TEST_TARGET_FILES		= $(addprefix $(TEST_DIR)/,$(TEST_TARGETS))


# Targets
.PHONY: all clean check dirs main run-tests
.SILENT: run-tests dirs


all: dirs main

clean:
	rm -rf $(OBJECTS_DIR)/* $(TEST_TARGET_FILES)

# Tests targets
check: dirs run-tests

dirs:
	mkdir -p $(sort $(realpath $(BUILD_DIR) $(OBJECTS_DIR) $(TEST_DIR) \
							   $(dir $(addprefix $(OBJECTS_DIR)/, \
							   					 $(SOURCES_CPP) $(MAIN_SOURCES_CPP)))))


main: $(TARGET_FILES)

run-tests: $(TEST_TARGET_FILES)
	for T in $(TEST_TARGETS); do											\
		echo "$(COLOR_TEST_RUN)Running test: \"$$T\"...$(COLOR_RESET)";		\
		$(realpath $(TEST_DIR))/$$T;										\
		if (( $$? == 0 )); then												\
			echo "$(COLOR_TEST_PASS)Test \"$$T\" passed.$(COLOR_RESET)";	\
		else																\
			echo "$(COLOR_TEST_FAIL)Test \"$$T\" failed.$(COLOR_RESET)";	\
		fi;																	\
	done


# Objects compilation (universal for main program and tests)
$(OBJECTS_DIR)/%.o: $(SOURCES_DIR)/%.cpp $(HEADER_FILES)
	$(GPP) $(GPP_COMPILEFLAGS) -c -o "$@" "$<"


# Main targets
$(TARGET_FILES): $(HEADER_FILES) $(OBJECT_FILES) $(MAIN_OBJECT_FILES)
	$(GPP) $(GPP_LINKFLAGS) -o "$@" $(MAIN_OBJECT_FILES) $(OBJECT_FILES)


# Tests
$(TEST_DIR)/%: $(OBJECTS_DIR)/%.o $(HEADER_FILES) $(OBJECT_FILES)
	$(GPP) $(GPP_LINKFLAGS) -o "$@" "$<" $(OBJECT_FILES)
