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
include using_makers/CMakeFiles/basic_shapes.dir/depend.make

# Include the progress variables for this target.
include using_makers/CMakeFiles/basic_shapes.dir/progress.make

# Include the compile flags for this target's objects.
include using_makers/CMakeFiles/basic_shapes.dir/flags.make

using_makers/CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.o: using_makers/CMakeFiles/basic_shapes.dir/flags.make
using_makers/CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.o: /home/pepper/usst_ws/src/using_makers/src/basic_shapes.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pepper/usst_ws/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object using_makers/CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.o"
	cd /home/pepper/usst_ws/build/using_makers && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.o -c /home/pepper/usst_ws/src/using_makers/src/basic_shapes.cpp

using_makers/CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.i"
	cd /home/pepper/usst_ws/build/using_makers && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/pepper/usst_ws/src/using_makers/src/basic_shapes.cpp > CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.i

using_makers/CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.s"
	cd /home/pepper/usst_ws/build/using_makers && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/pepper/usst_ws/src/using_makers/src/basic_shapes.cpp -o CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.s

using_makers/CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.o.requires:

.PHONY : using_makers/CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.o.requires

using_makers/CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.o.provides: using_makers/CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.o.requires
	$(MAKE) -f using_makers/CMakeFiles/basic_shapes.dir/build.make using_makers/CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.o.provides.build
.PHONY : using_makers/CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.o.provides

using_makers/CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.o.provides.build: using_makers/CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.o


# Object files for target basic_shapes
basic_shapes_OBJECTS = \
"CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.o"

# External object files for target basic_shapes
basic_shapes_EXTERNAL_OBJECTS =

/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: using_makers/CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.o
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: using_makers/CMakeFiles/basic_shapes.dir/build.make
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /opt/ros/indigo/lib/libroscpp.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libboost_signals.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /opt/ros/indigo/lib/librosconsole.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /opt/ros/indigo/lib/librosconsole_log4cxx.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /opt/ros/indigo/lib/librosconsole_backend_interface.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/liblog4cxx.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libboost_regex.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /opt/ros/indigo/lib/libxmlrpcpp.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_videostab.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_superres.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_stitching.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_ocl.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_gpu.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_contrib.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /opt/ros/indigo/lib/libroscpp_serialization.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /opt/ros/indigo/lib/librostime.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libboost_date_time.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /opt/ros/indigo/lib/libcpp_common.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libboost_system.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libboost_thread.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libpthread.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libconsole_bridge.so
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_photo.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_legacy.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_video.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_objdetect.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_ml.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_calib3d.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_features2d.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_flann.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: /usr/lib/x86_64-linux-gnu/libopencv_core.so.2.4.8
/home/pepper/usst_ws/devel/lib/using_makers/basic_shapes: using_makers/CMakeFiles/basic_shapes.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pepper/usst_ws/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /home/pepper/usst_ws/devel/lib/using_makers/basic_shapes"
	cd /home/pepper/usst_ws/build/using_makers && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/basic_shapes.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
using_makers/CMakeFiles/basic_shapes.dir/build: /home/pepper/usst_ws/devel/lib/using_makers/basic_shapes

.PHONY : using_makers/CMakeFiles/basic_shapes.dir/build

using_makers/CMakeFiles/basic_shapes.dir/requires: using_makers/CMakeFiles/basic_shapes.dir/src/basic_shapes.cpp.o.requires

.PHONY : using_makers/CMakeFiles/basic_shapes.dir/requires

using_makers/CMakeFiles/basic_shapes.dir/clean:
	cd /home/pepper/usst_ws/build/using_makers && $(CMAKE_COMMAND) -P CMakeFiles/basic_shapes.dir/cmake_clean.cmake
.PHONY : using_makers/CMakeFiles/basic_shapes.dir/clean

using_makers/CMakeFiles/basic_shapes.dir/depend:
	cd /home/pepper/usst_ws/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pepper/usst_ws/src /home/pepper/usst_ws/src/using_makers /home/pepper/usst_ws/build /home/pepper/usst_ws/build/using_makers /home/pepper/usst_ws/build/using_makers/CMakeFiles/basic_shapes.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : using_makers/CMakeFiles/basic_shapes.dir/depend

