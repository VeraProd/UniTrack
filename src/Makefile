# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This is Makefile for building the main part of program. This part
# of program has object files as targets, so there is no rules for
# linking executeble or static/dynamic library. See root Makefile
# and modules' Makefiles, they do that.

# NOTE: This Makefile is NOT for modules building, for that purpose
# see the root Makefile (../Makefile).


# Makefile advanced
include $(MAKEFILE_ADVANCED_ABS)


# Sources and headers
HEADERS =												\
	project_data.h										\
														\
	page_model.h										\
														\
	file_host_template_pages_only.h						\
	file_host_files_and_template_pages.h				\
	file_host_files_and_template_pages_parameters.h		\
														\
	interface_manager.h									\
														\
	json.hpp											\
	json_utils.h										\
	


SOURCES_CPP =											\
	project_data.cpp									\
														\
	page_model.cpp										\
														\
	file_host_template_pages_only.cpp					\
	file_host_files_and_template_pages.cpp				\
	file_host_files_and_template_pages_parameters.cpp	\
														\
	interface_manager.cpp								\
														\
	json_utils.cpp										\
	


# Sources needed for only main targets
MAIN_SOURCES_CPP =				\
	main.cpp


# Tests
TEST_SOURCES_CPP =				\
	


# Absolute paths
SOURCES_DIR_CURR			= $(SOURCES_DIR_ABS)
BUILD_DIR_CURR				= $(BUILD_DIR_ABS)
LIBS_DIR_CURR				= $(LIBS_DIR_ABS)
OBJECTS_DIR_CURR			= $(OBJECTS_DIR_ABS)
TEST_DIR_CURR				= $(TEST_DIR_ABS)


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
TEST_TARGETS				= $(call get_targets,$(TEST_SOURCES_CPP))
TEST_TARGET_FILES			= $(call get_test_files,$(TEST_TARGETS))


# Targets
.PHONY: all clean check dirs main run-tests
.SILENT: clean dirs run-tests


all: dirs main


clean:
	rm -rf $(OBJECT_FILES) $(MAIN_OBJECT_FILES) $(TEST_OBJECT_FILES) $(TEST_TARGET_FILES)


# Tests targets
check: dirs run-tests


main: $(OBJECT_FILES) $(MAIN_OBJECT_FILES)


run-tests: $(TEST_TARGET_FILES)
	for T in $(TEST_TARGETS); do															\
		echo "$(COLOR_RUN)Running test: \"$$T\"...$(COLOR_RESET)";							\
		$(call get_test_files,$$T);															\
		STATUS=$$?;																			\
		if [ "X$$STATUS" = 'X0' ]; then														\
			echo "$(COLOR_PASS)Test \"$$T\" passed.$(COLOR_RESET)";							\
		else																				\
			echo "$(COLOR_FAIL)Test \"$$T\" failed with code: $$STATUS.$(COLOR_RESET)";		\
		fi;																					\
	done


# Objects compilation (universal for main program and tests)
$(OBJECTS_DIR_CURR)/%.o: $(SOURCES_DIR_CURR)/%.cpp $(HEADER_FILES)
	$(call gpp_compile) -o '$@' '$<'


# Tests
$(TEST_DIR_CURR)/%: $(OBJECTS_DIR)/%.o $(HEADER_FILES) $(OBJECT_FILES)
	$(call gpp_link) -o '$@' '$<' $(OBJECT_FILES)
