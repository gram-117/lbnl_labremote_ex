# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake3

# The command to remove a file.
RM = /usr/bin/cmake3 -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gdanderson/ldrd_28nm_testing

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gdanderson/ldrd_28nm_testing/build

# Include any dependencies generated for this target.
include CMakeFiles/testTDC.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/testTDC.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/testTDC.dir/flags.make

CMakeFiles/testTDC.dir/src/testTDC.cpp.o: CMakeFiles/testTDC.dir/flags.make
CMakeFiles/testTDC.dir/src/testTDC.cpp.o: ../src/testTDC.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gdanderson/ldrd_28nm_testing/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/testTDC.dir/src/testTDC.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testTDC.dir/src/testTDC.cpp.o -c /home/gdanderson/ldrd_28nm_testing/src/testTDC.cpp

CMakeFiles/testTDC.dir/src/testTDC.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testTDC.dir/src/testTDC.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gdanderson/ldrd_28nm_testing/src/testTDC.cpp > CMakeFiles/testTDC.dir/src/testTDC.cpp.i

CMakeFiles/testTDC.dir/src/testTDC.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testTDC.dir/src/testTDC.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gdanderson/ldrd_28nm_testing/src/testTDC.cpp -o CMakeFiles/testTDC.dir/src/testTDC.cpp.s

# Object files for target testTDC
testTDC_OBJECTS = \
"CMakeFiles/testTDC.dir/src/testTDC.cpp.o"

# External object files for target testTDC
testTDC_EXTERNAL_OBJECTS =

testTDC: CMakeFiles/testTDC.dir/src/testTDC.cpp.o
testTDC: CMakeFiles/testTDC.dir/build.make
testTDC: lib/libDevCom.so.1.1
testTDC: lib/libEquipConf.so.1.1
testTDC: lib/libPS.so.1.1
testTDC: lib/libDataSink.so.1.1
testTDC: lib/libChiller.so.1.1
testTDC: lib/libMeter.so.1.1
testTDC: lib/libCom.so.1.1
testTDC: lib/libUtils.so.1.1
testTDC: lib/libnlohmann_json_schema_validator.so.2.1.1
testTDC: CMakeFiles/testTDC.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gdanderson/ldrd_28nm_testing/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable testTDC"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testTDC.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/testTDC.dir/build: testTDC

.PHONY : CMakeFiles/testTDC.dir/build

CMakeFiles/testTDC.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/testTDC.dir/cmake_clean.cmake
.PHONY : CMakeFiles/testTDC.dir/clean

CMakeFiles/testTDC.dir/depend:
	cd /home/gdanderson/ldrd_28nm_testing/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gdanderson/ldrd_28nm_testing /home/gdanderson/ldrd_28nm_testing /home/gdanderson/ldrd_28nm_testing/build /home/gdanderson/ldrd_28nm_testing/build /home/gdanderson/ldrd_28nm_testing/build/CMakeFiles/testTDC.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/testTDC.dir/depend

