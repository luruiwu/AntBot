# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.6

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pepper/usst_ws/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pepper/usst_ws/build

# Include any dependencies generated for this target.
include hello/CMakeFiles/server.dir/depend.make

# Include the progress variables for this target.
include hello/CMakeFiles/server.dir/progress.make

# Include the compile flags for this target's objects.
include hello/CMakeFiles/server.dir/flags.make

hello/CMakeFiles/server.dir/src/server.cpp.o: hello/CMakeFiles/server.dir/flags.make
hello/CMakeFiles/server.dir/src/server.cpp.o: /home/pepper/usst_ws/src/hello/src/server.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pepper/usst_ws/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object hello/CMakeFiles/server.dir/src/server.cpp.o"
	cd /home/pepper/usst_ws/build/hello && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/src/server.cpp.o -c /home/pepper/usst_ws/src/hello/src/server.cpp

hello/CMakeFiles/server.dir/src/server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/src/server.cpp.i"
	cd /home/pepper/usst_ws/build/hello && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/pepper/usst_ws/src/hello/src/server.cpp > CMakeFiles/server.dir/src/server.cpp.i

hello/CMakeFiles/server.dir/src/server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/src/server.cpp.s"
	cd /home/pepper/usst_ws/build/hello && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/pepper/usst_ws/src/hello/src/server.cpp -o CMakeFiles/server.dir/src/server.cpp.s

hello/CMakeFiles/server.dir/src/server.cpp.o.requires:

.PHONY : hello/CMakeFiles/server.dir/src/server.cpp.o.requires

hello/CMakeFiles/server.dir/src/server.cpp.o.provides: hello/CMakeFiles/server.dir/src/server.cpp.o.requires
	$(MAKE) -f hello/CMakeFiles/server.dir/build.make hello/CMakeFiles/server.dir/src/server.cpp.o.provides.build
.PHONY : hello/CMakeFiles/server.dir/src/server.cpp.o.provides

hello/CMakeFiles/server.dir/src/server.cpp.o.provides.build: hello/CMakeFiles/server.dir/src/server.cpp.o


# Object files for target server
server_OBJECTS = \
"CMakeFiles/server.dir/src/server.cpp.o"

# External object files for target server
server_EXTERNAL_OBJECTS =

/home/pepper/usst_ws/devel/lib/hello/server: hello/CMakeFiles/server.dir/src/server.cpp.o
/home/pepper/usst_ws/devel/lib/hello/server: hello/CMakeFiles/server.dir/build.make
/home/pepper/usst_ws/devel/lib/hello/server: /opt/ros/indigo/lib/libroscpp.so
/home/pepper/usst_ws/devel/lib/hello/server: /usr/lib/x86_64-linux-gnu/libboost_signals.so
/home/pepper/usst_ws/devel/lib/hello/server: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
/home/pepper/usst_ws/devel/lib/hello/server: /opt/ros/indigo/lib/librosconsole.so
/home/pepper/usst_ws/devel/lib/hello/server: /opt/ros/indigo/lib/librosconsole_log4cxx.so
/home/pepper/usst_ws/devel/lib/hello/server: /opt/ros/indigo/lib/librosconsole_backend_interface.so
/home/pepper/usst_ws/devel/lib/hello/server: /usr/lib/liblog4cxx.so
/home/pepper/usst_ws/devel/lib/hello/server: /usr/lib/x86_64-linux-gnu/libboost_regex.so
/home/pepper/usst_ws/devel/lib/hello/server: /opt/ros/indigo/lib/libxmlrpcpp.so
/home/pepper/usst_ws/devel/lib/hello/server: /opt/ros/indigo/lib/libroscpp_serialization.so
/home/pepper/usst_ws/devel/lib/hello/server: /opt/ros/indigo/lib/librostime.so
/home/pepper/usst_ws/devel/lib/hello/server: /usr/lib/x86_64-linux-gnu/libboost_date_time.so
/home/pepper/usst_ws/devel/lib/hello/server: /opt/ros/indigo/lib/libcpp_common.so
/home/pepper/usst_ws/devel/lib/hello/server: /usr/lib/x86_64-linux-gnu/libboost_system.so
/home/pepper/usst_ws/devel/lib/hello/server: /usr/lib/x86_64-linux-gnu/libboost_thread.so
/home/pepper/usst_ws/devel/lib/hello/server: /usr/lib/x86_64-linux-gnu/libpthread.so
/home/pepper/usst_ws/devel/lib/hello/server: /usr/lib/x86_64-linux-gnu/libconsole_bridge.so
/home/pepper/usst_ws/devel/lib/hello/server: hello/CMakeFiles/server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pepper/usst_ws/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /home/pepper/usst_ws/devel/lib/hello/server"
	cd /home/pepper/usst_ws/build/hello && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
hello/CMakeFiles/server.dir/build: /home/pepper/usst_ws/devel/lib/hello/server

.PHONY : hello/CMakeFiles/server.dir/build

hello/CMakeFiles/server.dir/requires: hello/CMakeFiles/server.dir/src/server.cpp.o.requires

.PHONY : hello/CMakeFiles/server.dir/requires

hello/CMakeFiles/server.dir/clean:
	cd /home/pepper/usst_ws/build/hello && $(CMAKE_COMMAND) -P CMakeFiles/server.dir/cmake_clean.cmake
.PHONY : hello/CMakeFiles/server.dir/clean

hello/CMakeFiles/server.dir/depend:
	cd /home/pepper/usst_ws/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pepper/usst_ws/src /home/pepper/usst_ws/src/hello /home/pepper/usst_ws/build /home/pepper/usst_ws/build/hello /home/pepper/usst_ws/build/hello/CMakeFiles/server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : hello/CMakeFiles/server.dir/depend

