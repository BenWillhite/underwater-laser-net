# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 4.0

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

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/ben/ns-3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/ben/ns-3/build

# Include any dependencies generated for this target.
include src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/compiler_depend.make

# Include the progress variables for this target.
include src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/progress.make

# Include the compile flags for this target's objects.
include src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/flags.make

src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/codegen:
.PHONY : src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/codegen

src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.o: src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/flags.make
src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.o: /Users/ben/ns-3/src/point-to-point-layout/model/point-to-point-dumbbell.cc
src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.o: CMakeFiles/stdlib_pch.dir/cmake_pch.hxx
src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.o: CMakeFiles/stdlib_pch.dir/cmake_pch.hxx.pch
src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.o: src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/ben/ns-3/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.o"
	cd /Users/ben/ns-3/build/src/point-to-point-layout && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -Xclang -include-pch -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx.pch -Xclang -include -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx -MD -MT src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.o -MF CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.o.d -o CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.o -c /Users/ben/ns-3/src/point-to-point-layout/model/point-to-point-dumbbell.cc

src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.i"
	cd /Users/ben/ns-3/build/src/point-to-point-layout && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -Xclang -include-pch -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx.pch -Xclang -include -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx -E /Users/ben/ns-3/src/point-to-point-layout/model/point-to-point-dumbbell.cc > CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.i

src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.s"
	cd /Users/ben/ns-3/build/src/point-to-point-layout && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -Xclang -include-pch -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx.pch -Xclang -include -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx -S /Users/ben/ns-3/src/point-to-point-layout/model/point-to-point-dumbbell.cc -o CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.s

src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.o: src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/flags.make
src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.o: /Users/ben/ns-3/src/point-to-point-layout/model/point-to-point-grid.cc
src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.o: CMakeFiles/stdlib_pch.dir/cmake_pch.hxx
src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.o: CMakeFiles/stdlib_pch.dir/cmake_pch.hxx.pch
src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.o: src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/ben/ns-3/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.o"
	cd /Users/ben/ns-3/build/src/point-to-point-layout && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -Xclang -include-pch -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx.pch -Xclang -include -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx -MD -MT src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.o -MF CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.o.d -o CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.o -c /Users/ben/ns-3/src/point-to-point-layout/model/point-to-point-grid.cc

src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.i"
	cd /Users/ben/ns-3/build/src/point-to-point-layout && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -Xclang -include-pch -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx.pch -Xclang -include -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx -E /Users/ben/ns-3/src/point-to-point-layout/model/point-to-point-grid.cc > CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.i

src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.s"
	cd /Users/ben/ns-3/build/src/point-to-point-layout && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -Xclang -include-pch -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx.pch -Xclang -include -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx -S /Users/ben/ns-3/src/point-to-point-layout/model/point-to-point-grid.cc -o CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.s

src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.o: src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/flags.make
src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.o: /Users/ben/ns-3/src/point-to-point-layout/model/point-to-point-star.cc
src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.o: CMakeFiles/stdlib_pch.dir/cmake_pch.hxx
src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.o: CMakeFiles/stdlib_pch.dir/cmake_pch.hxx.pch
src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.o: src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/ben/ns-3/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.o"
	cd /Users/ben/ns-3/build/src/point-to-point-layout && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -Xclang -include-pch -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx.pch -Xclang -include -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx -MD -MT src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.o -MF CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.o.d -o CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.o -c /Users/ben/ns-3/src/point-to-point-layout/model/point-to-point-star.cc

src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.i"
	cd /Users/ben/ns-3/build/src/point-to-point-layout && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -Xclang -include-pch -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx.pch -Xclang -include -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx -E /Users/ben/ns-3/src/point-to-point-layout/model/point-to-point-star.cc > CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.i

src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.s"
	cd /Users/ben/ns-3/build/src/point-to-point-layout && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -Xclang -include-pch -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx.pch -Xclang -include -Xclang /Users/ben/ns-3/build/CMakeFiles/stdlib_pch.dir/cmake_pch.hxx -S /Users/ben/ns-3/src/point-to-point-layout/model/point-to-point-star.cc -o CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.s

# Object files for target point-to-point-layout
point__to__point__layout_OBJECTS = \
"CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.o" \
"CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.o" \
"CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.o"

# External object files for target point-to-point-layout
point__to__point__layout_EXTERNAL_OBJECTS =

lib/libns3-dev-point-to-point-layout.dylib: src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-dumbbell.cc.o
lib/libns3-dev-point-to-point-layout.dylib: src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-grid.cc.o
lib/libns3-dev-point-to-point-layout.dylib: src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/model/point-to-point-star.cc.o
lib/libns3-dev-point-to-point-layout.dylib: src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/build.make
lib/libns3-dev-point-to-point-layout.dylib: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libsqlite3.tbd
lib/libns3-dev-point-to-point-layout.dylib: /opt/homebrew/Cellar/gsl/2.8/lib/libgsl.dylib
lib/libns3-dev-point-to-point-layout.dylib: /opt/homebrew/Cellar/gsl/2.8/lib/libgslcblas.dylib
lib/libns3-dev-point-to-point-layout.dylib: src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/ben/ns-3/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX shared library ../../lib/libns3-dev-point-to-point-layout.dylib"
	cd /Users/ben/ns-3/build/src/point-to-point-layout && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/point-to-point-layout.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/build: lib/libns3-dev-point-to-point-layout.dylib
.PHONY : src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/build

src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/clean:
	cd /Users/ben/ns-3/build/src/point-to-point-layout && $(CMAKE_COMMAND) -P CMakeFiles/point-to-point-layout.dir/cmake_clean.cmake
.PHONY : src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/clean

src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/depend:
	cd /Users/ben/ns-3/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/ben/ns-3 /Users/ben/ns-3/src/point-to-point-layout /Users/ben/ns-3/build /Users/ben/ns-3/build/src/point-to-point-layout /Users/ben/ns-3/build/src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : src/point-to-point-layout/CMakeFiles/point-to-point-layout.dir/depend

