# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Sources and headers
HEADERS = \
	mongo/client.h \
	mongo/client_pool.h \
	mongo/collection.h \
	mongo/cursor.h \
	mongo/document.h \
	mongo/mongoc_handler.h \
	mongo/oid.h \
	mongo/uri.h \
	\
	application.h \
	userprofile.h \
	userheader.h


SOURCES_CPP = \
	mongo/client.cpp \
	mongo/client_pool.cpp \
	mongo/collection.cpp \
	mongo/cursor.cpp \
	mongo/document.cpp \
	mongo/mongoc_handler.cpp \
	mongo/oid.cpp \
	mongo/uri.cpp \
	\
	application.cpp \
	mainwindow.cpp \
	userprofile.cpp \
	userheader.cpp


MAIN_SOURCES_CPP = main.cpp

# Static library
MAIN_TARGETS = unitrack

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
GPP					 = g++ -pipe -O2 -Wall -std=c++11 $(EXTRA_CPPFLAGS)

# Don't need yet
AR = ar cr


# Mongo flags
GPP_COMPILEFLAGS		+= $(shell pkg-config --cflags libmongoc-1.0)
GPP_LINKFLAGS			+= $(shell pkg-config --libs libmongoc-1.0)


# Other flags
ifeq ($(shell uname -s),Darwin)
	# MacPorts installs boost and others into /opt/local
	GPP_COMPILEFLAGS	+= -I/opt/local/include
	GPP_LINKFLAGS		+= -L/opt/local/lib
endif


# Terminal colors (0 -- reset, 1 -- bold, 31 -- red, 32 -- green, 34 -- blue).
# See 'run' and 'run-tests' targets.
COLOR_RESET				= \033[0m
COLOR_RUN				= \033[34m
COLOR_PASS				= \033[32m
COLOR_FAIL				= \033[31m


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

# Target files
TARGET_FILES			= $(addprefix $(BUILD_DIR)/,$(MAIN_TARGETS))

TEST_TARGETS			= $(basename $(TEST_SOURCES_CPP))
TEST_TARGET_FILES		= $(addprefix $(TEST_DIR)/,$(TEST_TARGETS))


# Targets
.PHONY: all clean check dirs main run run-tests python-server
.SILENT: dirs run run-tests


python-server:
	cd ./www && python3 -m http.server 8080; cd ../


all: dirs main

clean:
	rm -rf $(OBJECTS_DIR)/* $(TEST_TARGET_FILES)

# Tests targets
check: dirs run-tests

dirs:
	mkdir -p $(sort $(BUILD_DIR) $(OBJECTS_DIR) $(TEST_DIR) \
					$(dir $(addprefix $(OBJECTS_DIR)/, \
						  $(SOURCES_CPP) $(MAIN_SOURCES_CPP))))


main: $(TARGET_FILES)


run: dirs main
	for T in $(MAIN_TARGETS); do															\
		echo "$(COLOR_RUN)Running program: \"$$T\"...$(COLOR_RESET)";						\
		$(BUILD_DIR)/$$T --docroot test/docroot/ --http-address 127.0.0.1 --http-port 8080;	\
		STATUS=$$?;																			\
		if (( $$STATUS == 0 )); then														\
			echo "$(COLOR_PASS)Program \"$$T\" completed successfully.$(COLOR_RESET)";		\
		else																				\
			echo "$(COLOR_FAIL)Program \"$$T\" failed with code: $$STATUS.$(COLOR_RESET)";	\
		fi;																					\
	done


run-tests: $(TEST_TARGET_FILES)
	for T in $(TEST_TARGETS); do														\
		echo "$(COLOR_RUN)Running test: \"$$T\"...$(COLOR_RESET)";						\
		$(TEST_DIR)/$$T;																\
		STATUS=$$?;																		\
		if (( $$? == 0 )); then															\
			echo "$(COLOR_PASS)Test \"$$T\" passed.$(COLOR_RESET)";						\
		else																			\
			echo "$(COLOR_FAIL)Test \"$$T\" failed with code: $$STATUS.$(COLOR_RESET)";	\
		fi;																				\
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
