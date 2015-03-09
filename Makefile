# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Sources and headers
HEADERS = \
	mongo/client_pool.h \
	mongo/document.h \
	mongo/mongoc_handler.h \
	mongo/uri.h

SOURCES_CPP = \
	main.cpp \
	mongo/client_pool.cpp \
	mongo/document.cpp \
	mongo/mongoc_handler.cpp \
	mongo/uri.cpp

# Static library
TARGET = unitrack

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
GPP = g++ -pipe -O2 -Wall -std=c++11 $(EXTRA_CPPFLAGS)
GPP_COMPILEFLAGS = $(shell pkg-config --cflags libmongoc-1.0)
GPP_LINKFLAGS = $(shell pkg-config --libs libmongoc-1.0)

ifeq ($(shell uname -s),Darwin)
	GPP_COMPILEFLAGS += -I/opt/local/include
	GPP_LINKFLAGS += -L/opt/local/lib
endif

# Don't need yet
AR = ar cr


# Files
HEADER_FILES = $(addprefix $(SOURCES_DIR)/,$(HEADERS))
SOURCES_CPP_FILES = $(addprefix $(SOURCES_DIR)/,$(SOURCES_CPP))

TARGET_FILE = $(addprefix $(BUILD_DIR)/,$(TARGET))

OBJECTS = $(SOURCES_CPP:.cpp=.o)
OBJECT_FILES = $(addprefix $(OBJECTS_DIR)/,$(OBJECTS))

TEST_OBJECTS = $(TEST_SOURCES_CPP:.cpp=.o)
TEST_OBJECT_FILES = $(addprefix $(OBJECTS_DIR)/,$(TEST_OBJECTS))

TEST_TARGETS = $(basename $(TEST_SOURCES_CPP))
TEST_TARGET_FILES = $(addprefix $(TEST_DIR)/,$(TEST_TARGETS))


# Targets
.PHONY: all clean check

all: $(TARGET_FILE)

clean:
	rm -rf $(OBJECTS_DIR)/* $(TEST_TARGET_FILES)


# Objects compilation (universal for main program and tests)
$(OBJECTS_DIR)/%.o: $(SOURCES_DIR)/%.cpp $(HEADER_FILES)
	mkdir -p "$(dir $@)"
	$(GPP) $(GPP_COMPILEFLAGS) -c -o "$@" "$<"


# Main target
$(TARGET_FILE): $(HEADER_FILES) $(OBJECT_FILES)
	$(GPP) $(GPP_LINKFLAGS) -o "$@" $(OBJECT_FILES)

# Tests targets
check: $(TEST_TARGET_FILES)

$(TEST_DIR)/%: $(OBJECTS_DIR)/%.o $(HEADER_FILES) $(OBJECT_FILES)
	$(GPP) -o "$@" "$<" $(OBJECT_FILES)
