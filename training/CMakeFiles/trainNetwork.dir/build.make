# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/martin/Documents/programovani/bzik/training

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/martin/Documents/programovani/bzik/training

# Include any dependencies generated for this target.
include CMakeFiles/trainNetwork.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/trainNetwork.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/trainNetwork.dir/flags.make

CMakeFiles/trainNetwork.dir/trainNetwork.cpp.o: CMakeFiles/trainNetwork.dir/flags.make
CMakeFiles/trainNetwork.dir/trainNetwork.cpp.o: trainNetwork.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/martin/Documents/programovani/bzik/training/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/trainNetwork.dir/trainNetwork.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/trainNetwork.dir/trainNetwork.cpp.o -c /home/martin/Documents/programovani/bzik/training/trainNetwork.cpp

CMakeFiles/trainNetwork.dir/trainNetwork.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/trainNetwork.dir/trainNetwork.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/martin/Documents/programovani/bzik/training/trainNetwork.cpp > CMakeFiles/trainNetwork.dir/trainNetwork.cpp.i

CMakeFiles/trainNetwork.dir/trainNetwork.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/trainNetwork.dir/trainNetwork.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/martin/Documents/programovani/bzik/training/trainNetwork.cpp -o CMakeFiles/trainNetwork.dir/trainNetwork.cpp.s

CMakeFiles/trainNetwork.dir/trainNetwork.cpp.o.requires:
.PHONY : CMakeFiles/trainNetwork.dir/trainNetwork.cpp.o.requires

CMakeFiles/trainNetwork.dir/trainNetwork.cpp.o.provides: CMakeFiles/trainNetwork.dir/trainNetwork.cpp.o.requires
	$(MAKE) -f CMakeFiles/trainNetwork.dir/build.make CMakeFiles/trainNetwork.dir/trainNetwork.cpp.o.provides.build
.PHONY : CMakeFiles/trainNetwork.dir/trainNetwork.cpp.o.provides

CMakeFiles/trainNetwork.dir/trainNetwork.cpp.o.provides.build: CMakeFiles/trainNetwork.dir/trainNetwork.cpp.o

# Object files for target trainNetwork
trainNetwork_OBJECTS = \
"CMakeFiles/trainNetwork.dir/trainNetwork.cpp.o"

# External object files for target trainNetwork
trainNetwork_EXTERNAL_OBJECTS =

trainNetwork: CMakeFiles/trainNetwork.dir/trainNetwork.cpp.o
trainNetwork: CMakeFiles/trainNetwork.dir/build.make
trainNetwork: /usr/local/lib/libopencv_videostab.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_videoio.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_video.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_superres.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_stitching.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_shape.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_photo.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_objdetect.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_ml.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_imgproc.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_imgcodecs.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_highgui.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_hal.a
trainNetwork: /usr/local/lib/libopencv_flann.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_features2d.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_core.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_calib3d.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_features2d.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_ml.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_highgui.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_videoio.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_imgcodecs.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_flann.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_video.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_imgproc.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_core.so.3.0.0
trainNetwork: /usr/local/lib/libopencv_hal.a
trainNetwork: /usr/lib/x86_64-linux-gnu/libGLU.so
trainNetwork: /usr/lib/x86_64-linux-gnu/libGL.so
trainNetwork: /usr/lib/x86_64-linux-gnu/libSM.so
trainNetwork: /usr/lib/x86_64-linux-gnu/libICE.so
trainNetwork: /usr/lib/x86_64-linux-gnu/libX11.so
trainNetwork: /usr/lib/x86_64-linux-gnu/libXext.so
trainNetwork: /usr/local/share/OpenCV/3rdparty/lib/libippicv.a
trainNetwork: CMakeFiles/trainNetwork.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable trainNetwork"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/trainNetwork.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/trainNetwork.dir/build: trainNetwork
.PHONY : CMakeFiles/trainNetwork.dir/build

CMakeFiles/trainNetwork.dir/requires: CMakeFiles/trainNetwork.dir/trainNetwork.cpp.o.requires
.PHONY : CMakeFiles/trainNetwork.dir/requires

CMakeFiles/trainNetwork.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/trainNetwork.dir/cmake_clean.cmake
.PHONY : CMakeFiles/trainNetwork.dir/clean

CMakeFiles/trainNetwork.dir/depend:
	cd /home/martin/Documents/programovani/bzik/training && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/martin/Documents/programovani/bzik/training /home/martin/Documents/programovani/bzik/training /home/martin/Documents/programovani/bzik/training /home/martin/Documents/programovani/bzik/training /home/martin/Documents/programovani/bzik/training/CMakeFiles/trainNetwork.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/trainNetwork.dir/depend

