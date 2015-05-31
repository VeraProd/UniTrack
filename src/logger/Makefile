# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for server part of this program.


# Makefile advanced
include $(MAKEFILE_ADVANCED_ABS)


# Dynamic library
TARGET_LIB = liblogger.so


# Paths
SOURCES_DIR_CURR			= $(SOURCES_DIR_ABS)/$(MODULE_NAME)
BUILD_DIR_CURR				= $(BUILD_DIR_ABS)/$(MODULE_NAME)
LIBS_DIR_CURR				= $(LIBS_DIR_ABS)
OBJECTS_DIR_CURR			= $(OBJECTS_DIR_ABS)/$(MODULE_NAME)
TEST_DIR_CURR				= $(TEST_DIR_ABS)/$(MODULE_NAME)


# Sources and headers (files in modules/ must be _AFTER_ files in current dir!)
HEADERS =																	\
	$(notdir $(shell find $(SOURCES_DIR_CURR)								\
		-depth 1 \( -type file -or -type link \)							\
		-not -name 'test*'													\
		\( -name '*.h' -or -name '*.hpp' \) ))


SOURCES_CPP =																\
	$(notdir $(shell find $(SOURCES_DIR_CURR)								\
		-depth 1 \( -type file -or -type link \)							\
		-not -name 'test*'													\
		-name '*.cpp' ))


# Tests
TEST_SOURCES_CPP =															\
	$(notdir $(shell find $(SOURCES_DIR_CURR)								\
		-depth 1 \( -type file -or -type link \)							\
		-name 'test*.cpp' ))


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
.SILENT:


all: dirs main


clean:
	rm -rf $(OBJECT_FILES) $(TEST_TARGET_FILES) $(TARGET_LIB_FILE)


# Tests targets
check: dirs run-tests


dirs:
	mkdir -p $(sort $(dir $(OBJECT_FILES) $(TEST_OBJECT_FILES) \
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
	@echo "    $(COLOR_RUN)Compiling: $(subst $(OBJECTS_DIR_CURR)/,,$@) ($(MODULE_NAME))...$(COLOR_RESET)"
	$(call gpp_compile) -o '$@' '$<'


$(TARGET_LIB_FILE): $(HEADER_FILES) $(OBJECT_FILES)
	@echo "    $(COLOR_RUN)Linking shared lib: $(subst $(LIBS_DIR_CURR)/,,$@) ($(MODULE_NAME))...$(COLOR_RESET)"
	$(call gpp_shared_lib) -o '$@' $(OBJECT_FILES)


# Tests
$(TEST_DIR_CURR)/%: $(OBJECTS_DIR)/%.o $(HEADER_FILES) $(OBJECT_FILES)
	@echo "    $(COLOR_RUN)Linking test: $(subst $(TEST_DIR_CURR)/,,$@) ($(MODULE_NAME))...$(COLOR_RESET)"
	$(call gpp_link) -o '$@' '$<' $(OBJECT_FILES)
