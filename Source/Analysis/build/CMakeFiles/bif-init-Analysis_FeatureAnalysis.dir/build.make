# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/hendra/CCD/Source/Analysis

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hendra/CCD/Source/Analysis/build

# Utility rule file for bif-init-Analysis_FeatureAnalysis.

# Include the progress variables for this target.
include CMakeFiles/bif-init-Analysis_FeatureAnalysis.dir/progress.make

CMakeFiles/bif-init-Analysis_FeatureAnalysis:
	cd /home/hendra/CCD/Source/Analysis/build/lib/bif && sh -c "rm -f /home/hendra/CCD/Source/Analysis/build/lib/bif/__load__.bro"
	cd /home/hendra/CCD/Source/Analysis/build/lib/bif && sh -c "for i in  analysis.bif.bro; do echo @load ./\$$i >> /home/hendra/CCD/Source/Analysis/build/lib/bif/__load__.bro; done"

bif-init-Analysis_FeatureAnalysis: CMakeFiles/bif-init-Analysis_FeatureAnalysis
bif-init-Analysis_FeatureAnalysis: CMakeFiles/bif-init-Analysis_FeatureAnalysis.dir/build.make

.PHONY : bif-init-Analysis_FeatureAnalysis

# Rule to build all files generated by this target.
CMakeFiles/bif-init-Analysis_FeatureAnalysis.dir/build: bif-init-Analysis_FeatureAnalysis

.PHONY : CMakeFiles/bif-init-Analysis_FeatureAnalysis.dir/build

CMakeFiles/bif-init-Analysis_FeatureAnalysis.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/bif-init-Analysis_FeatureAnalysis.dir/cmake_clean.cmake
.PHONY : CMakeFiles/bif-init-Analysis_FeatureAnalysis.dir/clean

CMakeFiles/bif-init-Analysis_FeatureAnalysis.dir/depend:
	cd /home/hendra/CCD/Source/Analysis/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hendra/CCD/Source/Analysis /home/hendra/CCD/Source/Analysis /home/hendra/CCD/Source/Analysis/build /home/hendra/CCD/Source/Analysis/build /home/hendra/CCD/Source/Analysis/build/CMakeFiles/bif-init-Analysis_FeatureAnalysis.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/bif-init-Analysis_FeatureAnalysis.dir/depend
