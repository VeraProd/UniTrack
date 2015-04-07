# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for Templatizer -- [html] template engine module.

# IMPORTANT!!!
# NOTE: This module has own sub-modules (see modules/).
# Sub-modules need to be _AFTER_ module_registrar in HEADERS
# and SOURCES_CPP lists! Otherwise, module will NOT work!


# Makefile advanced
include $(MAKEFILE_ADVANCED_ABS)


# Sources and headers
HEADERS =						\
	chunk.h						\
	page.h page.hpp				\
	model.h						\
	module_registrar.h			\
	module.h module.hpp			\
	modules/var_chunk.h
	


SOURCES_CPP =					\
	chunk.cpp					\
	page.cpp					\
	model.cpp					\
	module_registrar.cpp		\
	modules/var_chunk.cpp
	


# Static library
TARGET_LIB = libtemplatizer.a


# Tests
TEST_SOURCES_CPP =				\
	


# Mongo flags
GPP_COMPILEFLAGS			+= $(shell pkg-config --cflags libmongoc-1.0)
GPP_LINKFLAGS				+= $(shell pkg-config --libs libmongoc-1.0)


# Absolute paths
SOURCES_DIR_CURR			= $(SOURCES_DIR_ABS)/$(MODULE_NAME)
BUILD_DIR_CURR				= $(BUILD_DIR_ABS)/$(MODULE_NAME)
LIBS_DIR_CURR				= $(LIBS_DIR_ABS)
OBJECTS_DIR_CURR			= $(OBJECTS_DIR_ABS)/$(MODULE_NAME)
TEST_DIR_CURR				= $(TEST_DIR_ABS)/$(MODULE_NAME)


# Files
HEADER_FILES				= $(call get_sources_files,$(HEADERS))
SOURCES_CPP_FILES			= $(call get_sources_files,$(SOURCES_CPP))
MAIN_SOURCES_CPP_FILES		= $(call get_sources_files,$(MAIN_SOURCES_CPP))

# Object files
OBJECTS						= $(call get_objects_cpp,$(SOURCES_CPP))
TEST_OBJECTS				= $(call get_objects_cpp,$(TEST_SOURCES_CPP))

OBJECT_FILES				= $(call get_object_files,$(OBJECTS))
TEST_OBJECT_FILES			= $(call get_object_files,$(TEST_OBJECTS))

# Target files
TARGET_LIB_FILE				= $(call get_target_lib_files,$(TARGET_LIB))

TEST_TARGETS				= $(call get_targets,$(TEST_SOURCES_CPP))
TEST_TARGET_FILES			= $(call get_test_files,$(TEST_TARGETS))


.PHONY: all clean check dirs main run-tests
.SILENT: clean dirs modules run-tests


all: dirs main


clean:
	rm -rf $(OBJECT_FILES) $(TEST_TARGET_FILES) $(TARGET_LIB_FILE)


# Tests targets
check: dirs run-tests


dirs:
	mkdir -p $(sort $(dir $(OBJECT_FILES) $(MAIN_OBJECT_FILES) $(TEST_OBJECT_FILES) \
						  $(TARGET_LIB_FILE) $(TEST_TARGET_FILES)))


main: $(TARGET_LIB_FILE)


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
	done


# Objects compilation (universal for main program and tests)
$(OBJECTS_DIR_CURR)/%.o: $(SOURCES_DIR_CURR)/%.cpp $(HEADER_FILES)
	$(GPP) $(GPP_COMPILEFLAGS) -c -o "$@" "$<"


$(TARGET_LIB_FILE): $(HEADER_FILES) $(TARGET_LIB_FILE)($(OBJECTS))


$(TARGET_LIB_FILE)(%.o): $(call get_object_files,%.o)
	$(AR) "$@" "$<"


# Tests
$(TEST_DIR_CURR)/%: $(OBJECTS_DIR)/%.o $(HEADER_FILES) $(OBJECT_FILES)
	$(GPP) $(GPP_LINKFLAGS) -o "$@" "$<" $(OBJECT_FILES)
