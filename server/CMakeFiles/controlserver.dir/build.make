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
CMAKE_SOURCE_DIR = /data/game/server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /data/game/server

# Include any dependencies generated for this target.
include CMakeFiles/controlserver.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/controlserver.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/controlserver.dir/flags.make

CMakeFiles/controlserver.dir/controlserver.cpp.o: CMakeFiles/controlserver.dir/flags.make
CMakeFiles/controlserver.dir/controlserver.cpp.o: controlserver.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/game/server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/controlserver.dir/controlserver.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/controlserver.dir/controlserver.cpp.o -c /data/game/server/controlserver.cpp

CMakeFiles/controlserver.dir/controlserver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/controlserver.dir/controlserver.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/game/server/controlserver.cpp > CMakeFiles/controlserver.dir/controlserver.cpp.i

CMakeFiles/controlserver.dir/controlserver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/controlserver.dir/controlserver.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/game/server/controlserver.cpp -o CMakeFiles/controlserver.dir/controlserver.cpp.s

CMakeFiles/controlserver.dir/mysocket.cc.o: CMakeFiles/controlserver.dir/flags.make
CMakeFiles/controlserver.dir/mysocket.cc.o: mysocket.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/game/server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/controlserver.dir/mysocket.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/controlserver.dir/mysocket.cc.o -c /data/game/server/mysocket.cc

CMakeFiles/controlserver.dir/mysocket.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/controlserver.dir/mysocket.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/game/server/mysocket.cc > CMakeFiles/controlserver.dir/mysocket.cc.i

CMakeFiles/controlserver.dir/mysocket.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/controlserver.dir/mysocket.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/game/server/mysocket.cc -o CMakeFiles/controlserver.dir/mysocket.cc.s

CMakeFiles/controlserver.dir/proto/project.pb.cc.o: CMakeFiles/controlserver.dir/flags.make
CMakeFiles/controlserver.dir/proto/project.pb.cc.o: proto/project.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/game/server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/controlserver.dir/proto/project.pb.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/controlserver.dir/proto/project.pb.cc.o -c /data/game/server/proto/project.pb.cc

CMakeFiles/controlserver.dir/proto/project.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/controlserver.dir/proto/project.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/game/server/proto/project.pb.cc > CMakeFiles/controlserver.dir/proto/project.pb.cc.i

CMakeFiles/controlserver.dir/proto/project.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/controlserver.dir/proto/project.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/game/server/proto/project.pb.cc -o CMakeFiles/controlserver.dir/proto/project.pb.cc.s

CMakeFiles/controlserver.dir/proto/server.pb.cc.o: CMakeFiles/controlserver.dir/flags.make
CMakeFiles/controlserver.dir/proto/server.pb.cc.o: proto/server.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/game/server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/controlserver.dir/proto/server.pb.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/controlserver.dir/proto/server.pb.cc.o -c /data/game/server/proto/server.pb.cc

CMakeFiles/controlserver.dir/proto/server.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/controlserver.dir/proto/server.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/game/server/proto/server.pb.cc > CMakeFiles/controlserver.dir/proto/server.pb.cc.i

CMakeFiles/controlserver.dir/proto/server.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/controlserver.dir/proto/server.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/game/server/proto/server.pb.cc -o CMakeFiles/controlserver.dir/proto/server.pb.cc.s

CMakeFiles/controlserver.dir/tool/circularbuffer.cc.o: CMakeFiles/controlserver.dir/flags.make
CMakeFiles/controlserver.dir/tool/circularbuffer.cc.o: tool/circularbuffer.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/game/server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/controlserver.dir/tool/circularbuffer.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/controlserver.dir/tool/circularbuffer.cc.o -c /data/game/server/tool/circularbuffer.cc

CMakeFiles/controlserver.dir/tool/circularbuffer.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/controlserver.dir/tool/circularbuffer.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/game/server/tool/circularbuffer.cc > CMakeFiles/controlserver.dir/tool/circularbuffer.cc.i

CMakeFiles/controlserver.dir/tool/circularbuffer.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/controlserver.dir/tool/circularbuffer.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/game/server/tool/circularbuffer.cc -o CMakeFiles/controlserver.dir/tool/circularbuffer.cc.s

CMakeFiles/controlserver.dir/tool/pbhelper.cc.o: CMakeFiles/controlserver.dir/flags.make
CMakeFiles/controlserver.dir/tool/pbhelper.cc.o: tool/pbhelper.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/game/server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/controlserver.dir/tool/pbhelper.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/controlserver.dir/tool/pbhelper.cc.o -c /data/game/server/tool/pbhelper.cc

CMakeFiles/controlserver.dir/tool/pbhelper.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/controlserver.dir/tool/pbhelper.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/game/server/tool/pbhelper.cc > CMakeFiles/controlserver.dir/tool/pbhelper.cc.i

CMakeFiles/controlserver.dir/tool/pbhelper.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/controlserver.dir/tool/pbhelper.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/game/server/tool/pbhelper.cc -o CMakeFiles/controlserver.dir/tool/pbhelper.cc.s

CMakeFiles/controlserver.dir/serverbase.cc.o: CMakeFiles/controlserver.dir/flags.make
CMakeFiles/controlserver.dir/serverbase.cc.o: serverbase.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/game/server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/controlserver.dir/serverbase.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/controlserver.dir/serverbase.cc.o -c /data/game/server/serverbase.cc

CMakeFiles/controlserver.dir/serverbase.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/controlserver.dir/serverbase.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/game/server/serverbase.cc > CMakeFiles/controlserver.dir/serverbase.cc.i

CMakeFiles/controlserver.dir/serverbase.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/controlserver.dir/serverbase.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/game/server/serverbase.cc -o CMakeFiles/controlserver.dir/serverbase.cc.s

# Object files for target controlserver
controlserver_OBJECTS = \
"CMakeFiles/controlserver.dir/controlserver.cpp.o" \
"CMakeFiles/controlserver.dir/mysocket.cc.o" \
"CMakeFiles/controlserver.dir/proto/project.pb.cc.o" \
"CMakeFiles/controlserver.dir/proto/server.pb.cc.o" \
"CMakeFiles/controlserver.dir/tool/circularbuffer.cc.o" \
"CMakeFiles/controlserver.dir/tool/pbhelper.cc.o" \
"CMakeFiles/controlserver.dir/serverbase.cc.o"

# External object files for target controlserver
controlserver_EXTERNAL_OBJECTS =

controlserver: CMakeFiles/controlserver.dir/controlserver.cpp.o
controlserver: CMakeFiles/controlserver.dir/mysocket.cc.o
controlserver: CMakeFiles/controlserver.dir/proto/project.pb.cc.o
controlserver: CMakeFiles/controlserver.dir/proto/server.pb.cc.o
controlserver: CMakeFiles/controlserver.dir/tool/circularbuffer.cc.o
controlserver: CMakeFiles/controlserver.dir/tool/pbhelper.cc.o
controlserver: CMakeFiles/controlserver.dir/serverbase.cc.o
controlserver: CMakeFiles/controlserver.dir/build.make
controlserver: /usr/local/lib/libprotobuf.so
controlserver: CMakeFiles/controlserver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/data/game/server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking CXX executable controlserver"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/controlserver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/controlserver.dir/build: controlserver

.PHONY : CMakeFiles/controlserver.dir/build

CMakeFiles/controlserver.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/controlserver.dir/cmake_clean.cmake
.PHONY : CMakeFiles/controlserver.dir/clean

CMakeFiles/controlserver.dir/depend:
	cd /data/game/server && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/game/server /data/game/server /data/game/server /data/game/server /data/game/server/CMakeFiles/controlserver.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/controlserver.dir/depend
