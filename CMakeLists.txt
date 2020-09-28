###
# This is a CMake file.
# Copyright 2020
###

#####
# Project Properties
#####

# Minimum required CMake Version
cmake_minimum_required(VERSION 3.6.1)

# Project Name
project(hycast LANGUAGES C CXX)
set(PACKAGE "hycast" CACHE STRING "")

###
# Get system configuration, use it to determine os name, release, cpu.
# This information may or may not be used.
###
set(BUILDNAME "build")
find_program(UNAME NAMES uname)
IF(UNAME)
  macro(getuname name flag)
    exec_program("${UNAME}" ARGS "${flag}" OUTPUT_VARIABLE "${name}")
  endmacro(getuname)
  getuname(osname -s)
  getuname(osrel  -r)
  getuname(cpu    -m)
  set(BUILDNAME "${osname}-${osrel}-${cpu}")
ENDIF(UNAME)


###
# End BUILDNAME determination
###

###
# Add relevant CMake functionality via includes.
###

##
# Used by the CMAKE_INSTALL_LIBDIR variable.
##
include(GnuInstallDirs)

include(CheckLibraryExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckCXXSourceCompiles)
include(CheckCSourceCompiles)
include(TestBigEndian)
include(CheckSymbolExists)
include(GetPrerequisites)

include(CheckCCompilerFlag)


###
# End cmake module/include functionalities.
###


###
# Create some utility macros.
###

# A macro to check if a C linker supports a particular flag.
macro(CHECK_C_LINKER_FLAG M_FLAG M_RESULT)
  set(T_REQ_FLAG "${CMAKE_REQUIRED_FLAGS}")
  set(CMAKE_REQUIRED_FLAGS "${M_FLAG}")
  check_c_source_compiles("int main() {return 0;}" ${M_RESULT})
  set(CMAKE_REQUIRED_FLAGS "${T_REQ_FLAG}")
end()

###
# End utility macro definitions.
###


# Enable ctest-based testing
enable_testing()
include(CTest)

# Copy the CTest customization file into binary directory, as required.
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Set variable to define the build type.
include(GenerateExportHeader)

#####
# End Project Properties
#####
