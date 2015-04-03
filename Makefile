# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This is the main Makefile of project.

# Helper Makefile with advanced feautures (include it into all subprojects' Makefiles)
MAKEFILE_ADVANCED = Makefile.advanced
export MAKEFILE_ADVANCED_ABS = $(abspath $(MAKEFILE_ADVANCED))
include $(MAKEFILE_ADVANCED_ABS)


# Sources and headers
HEADERS =						\
	


SOURCES_CPP =					\
	


# Sources needed for only main targets
MAIN_SOURCES_CPP =				\
	main.cpp


# Modules should be linked as static libraries
MODULES =						\
	mongo						\
	templatizer


# Executables
MAIN_TARGETS = unitrack


# Tests
TEST_SOURCES_CPP = 


# Sources only
SOURCES_DIR	= src

# Result of build only (executable)
BUILD_DIR	= build

# Static libraries (for modules)
LIBS_DIR	= libs

# All .o files (including tests)
OBJECTS_DIR	= obj

# All test executables
TEST_DIR	= test


# Compiler and library archiever
export GPP			= g++ -pipe -O2 -Wall -std=c++11 $(EXTRA_CPPFLAGS)

# Need for modules
export AR			= ar cr


# Other flags
ifeq ($(shell uname -s),Darwin)
	# MacPorts installs boost and others into /opt/local
	export GPP_COMPILEFLAGS	+= -I/opt/local/include
	export GPP_LINKFLAGS	+= -L/opt/local/lib
endif


# Terminal colors (0 -- reset, 1 -- bold, 31 -- red, 32 -- green, 34 -- blue).
# See 'run' and 'run-tests' targets.
export COLOR_RESET			= \033[0m
export COLOR_RUN			= \033[34m
export COLOR_PASS			= \033[32m
export COLOR_FAIL			= \033[31m


# Absolute paths
export SOURCES_DIR_ABS		= $(abspath $(SOURCES_DIR))
export BUILD_DIR_ABS		= $(abspath $(BUILD_DIR))
export LIBS_DIR_ABS			= $(abspath $(LIBS_DIR))
export OBJECTS_DIR_ABS		= $(abspath $(OBJECTS_DIR))
export TEST_DIR_ABS			= $(abspath $(TEST_DIR))


# Current module pathes
SOURCES_DIR_CURR			= $(SOURCES_DIR_ABS)
BUILD_DIR_CURR				= $(BUILD_DIR_ABS)
LIBS_DIR_CURR				= $(LIBS_DIR_ABS)
OBJECTS_DIR_CURR			= $(OBJECTS_DIR_ABS)
TEST_DIR_CURR				= $(TEST_DIR_ABS)


# Current project
export GPP_COMPILEFLAGS		= -I"$(SOURCES_DIR_CURR)"
export GPP_LINKFLAGS		=


# Local data
# Files
HEADER_FILES				= $(call get_sources_files,$(HEADERS))
SOURCES_CPP_FILES			= $(call get_sources_files,$(SOURCES_CPP))
MAIN_SOURCES_CPP_FILES		= $(call get_sources_files,$(MAIN_SOURCES_CPP))

# Object files
OBJECTS						= $(call get_objects_cpp,$(SOURCES_CPP))
MAIN_OBJECTS				= $(call get_objects_cpp,$(MAIN_SOURCES_CPP))
TEST_OBJECTS				= $(call get_objects_cpp,$(TEST_SOURCES_CPP))

OBJECT_FILES				= $(call get_object_files,$(OBJECTS))
MAIN_OBJECT_FILES			= $(call get_object_files,$(MAIN_OBJECTS))
TEST_OBJECT_FILES			= $(call get_object_files,$(TEST_OBJECTS))

# Target files
TARGET_FILES				= $(call get_target_files,$(MAIN_TARGETS))

TEST_TARGETS				= $(call get_targets,$(TEST_SOURCES_CPP))
TEST_TARGET_FILES			= $(call get_test_files,$(TEST_TARGETS))

# Module static libraries
MODULE_FILES				= $(call get_target_lib_files,$(addprefix lib,$(addsuffix .a,$(MODULES))))


# Targets
.PHONY: all clean check dirs modules main run run-tests python-server
.SILENT: clean dirs modules run run-tests


all: dirs modules main


# Cleaning submodules too
clean:
	rm -rf $(OBJECT_FILES) $(MAIN_OBJECT_FILES) $(TEST_OBJECT_FILES) $(TEST_TARGET_FILES)
	for T in $(MODULES); do																	\
		$(MAKE) -C "$(call get_sources_files,$$T)" MODULE_NAME="$$T" clean;					\
	done


# Tests targets
check: dirs run-tests


dirs:
	mkdir -p $(sort $(BUILD_DIR_CURR) $(LIBS_DIR_CURR) \
					$(OBJECTS_DIR_CURR) $(TEST_DIR_CURR) \
					$(dir $(OBJECT_FILES) $(MAIN_OBJECT_FILES)))


main: $(TARGET_FILES)


modules:
	for T in $(MODULES); do																	\
		echo "$(COLOR_RUN)Building module: \"$$T\"...$(COLOR_RESET)";						\
		$(MAKE) -C "$(call get_sources_files,$$T)" MODULE_NAME="$$T";						\
		STATUS=$$?;																			\
		if [ "X$$STATUS" == 'X0' ]; then													\
			echo "$(COLOR_PASS)Module \"$$T\" built successfully.$(COLOR_RESET)";			\
		else																				\
			echo "$(COLOR_FAIL)Module \"$$T\" building failed.$(COLOR_RESET)";				\
		fi;																					\
	done


run: dirs main
	for T in $(MAIN_TARGETS); do															\
		echo "$(COLOR_RUN)Running program: \"$$T\"...$(COLOR_RESET)";						\
		$(call get_target_files,$$T);														\
		STATUS=$$?;																			\
		if [ "X$$STATUS" == 'X0' ]; then													\
			echo "$(COLOR_PASS)Program \"$$T\" completed successfully.$(COLOR_RESET)";		\
		else																				\
			echo "$(COLOR_FAIL)Program \"$$T\" failed with code: $$STATUS.$(COLOR_RESET)";	\
		fi;																					\
	done


# Running tests for submodules too
run-tests: $(TEST_TARGET_FILES)
	for T in $(TEST_TARGETS); do															\
		echo "$(COLOR_RUN)Running test: \"$$T\"...$(COLOR_RESET)";							\
		$(call get_test_files,$$T);															\
		STATUS=$$?;																			\
		if [ "X$$STATUS" == 'X0' ]; then													\
			echo "$(COLOR_PASS)Test \"$$T\" passed.$(COLOR_RESET)";							\
		else																				\
			echo "$(COLOR_FAIL)Test \"$$T\" failed with code: $$STATUS.$(COLOR_RESET)";		\
		fi;																					\
	done																					\
	for T in $(MODULES); do																	\
		$(MAKE) -C "$(call get_sources_files,$$T)" MODULE_NAME="$$T" run-tests;				\
	done


# Objects compilation (universal for main program and tests)
$(OBJECTS_DIR_CURR)/%.o: $(SOURCES_DIR_CURR)/%.cpp $(HEADER_FILES)
	$(GPP) $(GPP_COMPILEFLAGS) -c -o "$@" "$<"


# Main targets
$(TARGET_FILES): $(HEADER_FILES) $(OBJECT_FILES) $(MAIN_OBJECT_FILES)
	$(GPP) $(GPP_LINKFLAGS) -o "$@" $(MAIN_OBJECT_FILES) $(OBJECT_FILES) $(MODULE_FILES)


# Tests
$(TEST_DIR_CURR)/%: $(OBJECTS_DIR)/%.o $(HEADER_FILES) $(OBJECT_FILES)
	$(GPP) $(GPP_LINKFLAGS) -o "$@" "$<" $(OBJECT_FILES)


python-server:
	cd ./www && python3 -m http.server 8080; cd ../
