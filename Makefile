# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This is the root Makefile of project.

# Helper Makefile with advanced feautures (include it into all subprojects' Makefiles)
MAKEFILE_ADVANCED = Makefile.advanced
export MAKEFILE_ADVANCED_ABS = $(abspath $(MAKEFILE_ADVANCED))
include $(MAKEFILE_ADVANCED_ABS)


# Project name
PROJECT_NAME	= UniTrack


# Project directories
# Sources only
SOURCES_DIR		= src

# Result of build only (executable)
BUILD_DIR		= build

# Static libraries (for modules)
LIBS_DIR		= libs

# All .o files (including tests)
OBJECTS_DIR		= obj

# All test executables
TEST_DIR		= test

# Config
CONFIG			= config

# WWW data
WWW				= www


# Installation directories
# Shared libraries
PREFIX_LIBS		= /usr/lib

# Executables
PREFIX_TARGET	= /usr/bin

# Config
PREFIX_CONFIG	= /etc

# WWW data
PREFIX_WWW		= /var


# Modules should be linked as dynamic libraries (not third-party!)
MODULES =						\
	base						\
	logger						\
	server						\
	templatizer					\
	# mongo


# Executables
MAIN_TARGETS = unitrack


# Tests (extra tests, that must do some specific. For usual tests
# see Makefiles of main part and modules).
TEST_SOURCES_CPP =				\
	


# Target system
export SYSTEM				= $(shell uname -s)

# Compiler settings
ifeq ($(SYSTEM),Darwin)
	GPP						= g++
	
	# MacPorts installs boost and others into /opt/local
	GPP_HEADER_PATHS		+= -I/opt/local/include
	GPP_LIB_PATHS			+= -L/opt/local/lib
	
	# Strange, but this need for server
	GPP_LIBS				+= -lboost_system-mt -lboost_filesystem-mt
else
	# Use g++-5 because of c++14 features
	GPP						= g++-5
	
	# Strange, but this need for server
	GPP_LIBS				+= -lboost_system -lpthread -lboost_filesystem
endif


# Current project settings
GPP_HEADER_PATHS			+= -I"$(abspath $(SOURCES_DIR))"
GPP_LIB_PATHS				+= -L"$(abspath $(LIBS_DIR))"

# Installation prefixes
# Config
PREFIX_CONFIG_FULL			= $(PREFIX_CONFIG)/unitrack

# WWW data
PREFIX_WWW_FULL				= $(PREFIX_WWW)/unitrack

GPP_PROJECT_DATA			+= -DPATH_CONFIG="\"$(PREFIX_CONFIG_FULL)/$(CONFIG)\""	\
							   -DPATH_WWW="\"$(PREFIX_WWW_FULL)/$(WWW)\""

# Compiler flags
GPP_COMPILE_FLAGS			+= -pipe -fPIC -O2 -Wall -std=c++14 -c $(GPP_PROJECT_DATA) $(EXTRA_CPP_FLAGS)
GPP_LINK_FLAGS				+= -pipe -fPIC -O2 -Wall $(EXTRA_LINK_FLEGS)
GPP_SHARED_LIB_FLAGS		+= -pipe -fPIC -O2 -Wall --shared $(EXTRA_SHARED_LIB_FLAGS)


# Export commands
export GPP
export GPP_HEADER_PATHS
export GPP_LIB_PATHS
export GPP_LIBS
export GPP_COMPILE_FLAGS
export GPP_LINK_FLAGS
export GPP_SHARED_LIB_FLAGS


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


# Files
OBJECT_FILES				= $(OBJECTS_DIR_CURR)/*.o
TEST_OBJECT_FILES			= $(call get_object_files,$(TEST_OBJECTS))

# Target files
TARGET_FILES				= $(call get_target_files,$(MAIN_TARGETS))

TEST_TARGETS				= $(call get_targets,$(TEST_SOURCES_CPP))
TEST_TARGET_FILES			= $(call get_test_files,$(TEST_TARGETS))

# Module dinamic libraries
MODULE_FILES_ABS			= $(call get_target_lib_files,$(MODULES))
MODULE_FILES				= $(notdir $(MODULE_FILES_ABS))


GPP_LIBS_CURR				= $(addprefix -lut_,$(MODULES))


# Targets
.PHONY:														\
	all clean												\
	install-bin install-config install-www install			\
	uninstall-bin uninstall-config uninstall-www uninstall	\
	upgrade happy git-pull									\
	check dirs modules main objects run run-tests


.SILENT:


all: dirs main


# Cleaning project submodules (not third-party!) too
clean:
	rm -rf $(TEST_OBJECT_FILES) $(TEST_TARGET_FILES)
	$(MAKE) -C $(SOURCES_DIR_CURR) clean;
	for T in $(MODULES); do																	\
		$(MAKE) -C "$(call get_sources_files,$$T)" MODULE_NAME="$$T" clean;					\
	done


install-bin:
	@echo "$(COLOR_RUN)Installing files to \"$(PREFIX_LIBS)\"...$(COLOR_RESET)"
	install $(MODULE_FILES_ABS) $(PREFIX_LIBS)
	
	@echo "$(COLOR_RUN)Installing files to \"$(PREFIX_TARGET)\"...$(COLOR_RESET)"
	install $(TARGET_FILES) $(PREFIX_TARGET)
	
	@echo "$(COLOR_PASS)==> Binaries installed.$(COLOR_RESET)"


install-config:
	@echo "$(COLOR_RUN)Creating directories in \"$(PREFIX_CONFIG_FULL)\"...$(COLOR_RESET)"
	find $(CONFIG) -type d -not -name '.*' | while read DIR; do install -d "$(PREFIX_CONFIG_FULL)/$$DIR"; done
	
	@echo "$(COLOR_RUN)Installing files to \"$(PREFIX_CONFIG_FULL)\"...$(COLOR_RESET)"
	find $(CONFIG) -type f -not -name '.*' | while read FILE; do install "$$FILE" "$(PREFIX_CONFIG_FULL)/$$FILE"; done
	
	@echo "$(COLOR_PASS)==> Config installed.$(COLOR_RESET)"


install-www:
	@echo "$(COLOR_RUN)Creating directories in \"$(PREFIX_WWW_FULL)\"...$(COLOR_RESET)"
	find $(WWW) -type d -not -name '.*' | while read DIR; do install -d "$(PREFIX_WWW_FULL)/$$DIR"; done
	
	@echo "$(COLOR_RUN)Installing files to \"$(PREFIX_WWW_FULL)\"...$(COLOR_RESET)"
	find $(WWW) -type f -not -name '.*' | while read FILE; do install "$$FILE" "$(PREFIX_WWW_FULL)/$$FILE"; done
	
	@echo "$(COLOR_PASS)==> WWW data installed.$(COLOR_RESET)"


install: install-bin install-config install-www


uninstall-bin:
	rm $(addprefix $(PREFIX_TARGET)/,$(MAIN_TARGETS))
	rm $(addprefix $(PREFIX_LIBS)/,$(MODULE_FILES))
	@echo "$(COLOR_PASS)==> Binaries removed.$(COLOR_RESET)"


uninstall-config:
	rm -r $(addprefix $(PREFIX_CONFIG_FULL)/,$(CONFIG))
	@echo "$(COLOR_PASS)==> Config removed.$(COLOR_RESET)"


uninstall-www:
	rm -r $(addprefix $(PREFIX_WWW_FULL)/,$(WWW))
	@echo "$(COLOR_PASS)==> WWW data removed.$(COLOR_RESET)"


uninstall: uninstall-bin uninstall-config uninstall-www


upgrade:
	@echo "$(COLOR_PASS)NOTE:$(COLOR_RESET) \`upgrade' command will occur an error, if program was not installed."
	sudo $(MAKE) uninstall-bin uninstall-www
	
	$(MAKE) git-pull
	$(MAKE)
	
	@echo "$(COLOR_RUN)Please, enter the password (if need) for installation or press Ctrl+C...$(COLOR_RESET)"
	sudo make install-bin install-www
	@echo "$(COLOR_PASS)==> Successfully upgraded.$(COLOR_RESET)"


happy: git-pull all
	@echo "$(COLOR_RUN)Please, enter the password (if need) for installation or press Ctrl+C...$(COLOR_RESET)"
	sudo make install
	@echo "$(COLOR_PASS)==> Successfully built and installed.$(COLOR_RESET)"
	
	@echo "$(COLOR_PASS)NOTE:$(COLOR_RESET) To work with $(PROJECT_NAME) try following commands:" \
		  "$(addprefix \n    ,$(MAIN_TARGETS))"
	
	@echo "$(COLOR_PASS)NOTE:$(COLOR_RESET) Next times you can simply do:"
	@echo '    make upgrade'


git-pull:
	echo '$(COLOR_RUN)Downloading new version...$(COLOR_RESET)';						\
	git pull --recurse-submodules=yes;													\
	STATUS=$$?;																			\
	if [ "X$$STATUS" = 'X0' ]; then														\
		echo '$(COLOR_PASS)==> New version downloaded.$(COLOR_RESET)';					\
	else																				\
		echo "$(COLOR_FAIL)==> Download failed with status: $$STATUS.$(COLOR_RESET)";	\
	fi


# Tests targets
check: run-tests


dirs:
	mkdir -p $(sort $(BUILD_DIR_CURR) $(LIBS_DIR_CURR) \
					$(OBJECTS_DIR_CURR) $(TEST_DIR_CURR) \
					$(dir $(OBJECT_FILES) $(MAIN_OBJECT_FILES)))


main: $(TARGET_FILES)


objects:
	echo '$(COLOR_RUN)Building objects...$(COLOR_RESET)';										\
	$(MAKE) -C $(SOURCES_DIR_CURR);																\
	STATUS=$$?;																					\
	if [ "X$$STATUS" = 'X0' ]; then																\
		echo '$(COLOR_PASS)==> Objects built successfully.$(COLOR_RESET)';						\
	else																						\
		echo '$(COLOR_FAIL)==> Objects building failed.$(COLOR_RESET)';							\
	fi;


modules: dirs
	for T in $(MODULES); do																		\
		echo "$(COLOR_RUN)Building module: $$T...$(COLOR_RESET)";								\
		$(MAKE) -C "$(call get_sources_files,$$T)" MODULE_NAME="$$T";							\
		STATUS=$$?;																				\
		if [ "X$$STATUS" = 'X0' ]; then															\
			echo "$(COLOR_PASS)==> Module $$T built successfully.$(COLOR_RESET)";				\
		else																					\
			echo "$(COLOR_FAIL)==> Module $$T building failed.$(COLOR_RESET)";					\
		fi;																						\
	done


run: all
	for T in $(MAIN_TARGETS); do																\
		echo "$(COLOR_RUN)Running program: $$T...$(COLOR_RESET)";								\
		$(call get_target_files,$$T);															\
		STATUS=$$?;																				\
		if [ "X$$STATUS" = 'X0' ]; then															\
			echo "$(COLOR_PASS)==> Program $$T completed successfully.$(COLOR_RESET)";			\
		else																					\
			echo "$(COLOR_FAIL)==> Program $$T failed with code: $$STATUS.$(COLOR_RESET)";		\
		fi;																						\
	done


# Running tests for submodules too
run-tests: dirs $(TEST_TARGET_FILES)
	for T in $(TEST_TARGETS); do																\
		echo "$(COLOR_RUN)Running test: $$T...$(COLOR_RESET)";									\
		$(call get_test_files,$$T);																\
		STATUS=$$?;																				\
		if [ "X$$STATUS" = 'X0' ]; then															\
			echo "$(COLOR_PASS)==> Test $$T passed.$(COLOR_RESET)";								\
		else																					\
			echo "$(COLOR_FAIL)==> Test $$T failed with code: $$STATUS.$(COLOR_RESET)";			\
		fi;																						\
	done																						\
	for T in $(MODULES); do																		\
		$(MAKE) -C "$(call get_sources_files,$$T)" MODULE_NAME="$$T" run-tests;					\
	done


# Building of all object files of the main part
$(OBJECT_FILES) $(MAIN_OBJECT_FILES): objects


# Building of all module files
$(MODULE_FILES_ABS): modules


# Main targets
$(TARGET_FILES): $(HEADER_FILES) modules objects
	TARGET=$(subst $(BUILD_DIR_CURR)/,,$@);														\
	echo "$(COLOR_RUN)Linking target: $$TARGET (global)...$(COLOR_RESET)";						\
	$(call gpp_link) $(GPP_LIBS_CURR) -o "$@" $(MAIN_OBJECT_FILES) $(OBJECT_FILES);				\
	STATUS=$$?;																					\
	if [ "X$$STATUS" = 'X0' ]; then																\
		echo "$(COLOR_PASS)==> Target $$TARGET linked successfully.$(COLOR_RESET)";				\
	else																						\
		echo "$(COLOR_FAIL)==> Target $$TARGET linking failed.$(COLOR_RESET)";					\
	fi;


# Tests
$(TEST_DIR_CURR)/%: $(OBJECTS_DIR)/%.o $(HEADER_FILES) $(OBJECT_FILES)
	@echo "    $(COLOR_RUN)Linking test: $(subst $(TEST_DIR_CURR)/,,$@) (global)...$(COLOR_RESET)"
	$(call gpp_link) -o '$@' '$<' $(OBJECT_FILES)
