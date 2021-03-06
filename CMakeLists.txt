cmake_minimum_required (VERSION 3.6)
project (CaPS)

option(BUILD_SHARED "Build libcaps as shared library" OFF)

# git is optional
find_package(Git)

# required packages: mpi, blas, and lapack
find_package(MPI REQUIRED)
find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

# generate position independent code for all targets
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# check if we need to link against libm
check_library_exists(m sqrt "" LIBM)

# add necessary include directories
include_directories(${MPI_INCLUDE_PATH})
include_directories("src/include/")
include_directories("src/cquadpack/include/")
include_directories("src/libhodlr/include/")

# set release to default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# set warnings
set(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -Wmissing-prototypes -Wshadow -Wpointer-arith -Wcast-qual -Wwrite-strings -Wno-unused-parameter -fstrict-aliasing")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")

# By default, icc violates strict IEEE floating point behaviour - similar to
# GCC's --fast-math option. The code, however, relies on strict IEEE floating
# point behaviour. The option "-fp-model precise" sets the correct floating
# point behaviour.
# Also, when using the math kernel library (MKL), use the non-threaded library.
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Intel")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fp-model precise -mkl=sequential")
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fp-model precise -mkl=sequential")
endif()

# libhodlr
add_library(hodlr STATIC src/libhodlr/src/hodlr.cpp src/libhodlr/src/HODLR_Matrix.cpp src/libhodlr/src/HODLR_Node.cpp src/libhodlr/src/HODLR_Tree.cpp src/libhodlr/src/HODLR_Tree_NonSPD.cpp src/libhodlr/src/HODLR_Tree_SPD.cpp src/libhodlr/src/KDTree.cpp)
target_compile_definitions(hodlr PRIVATE -DUSE_DOUBLE)
set_target_properties(hodlr PROPERTIES COMPILE_FLAGS "-Wno-unknown-pragmas")

# cquadpack
add_library(cquadpack STATIC src/cquadpack/src/dqage.c src/cquadpack/src/dqagi.c src/cquadpack/src/dqags.c src/cquadpack/src/dqext.c src/cquadpack/src/dqk15.c src/cquadpack/src/dqk15i.c src/cquadpack/src/dqk21.c src/cquadpack/src/dqk31.c src/cquadpack/src/dqk41.c src/cquadpack/src/dqk51.c src/cquadpack/src/dqk61.c src/cquadpack/src/dqsort.c)

# argparse
add_library(argparse STATIC src/argparse.c)

# libcaps
set(caps_src src/bessel.c src/cache.c src/fcqs.c src/integration.c src/libcaps.c src/logfac.c src/material.c src/matrix.c src/misc.c src/plm.c src/psd.c src/utils.c)
if(BUILD_SHARED)
    add_library(caps SHARED ${caps_src})
else()
    add_library(caps STATIC ${caps_src})
endif()

target_compile_definitions(caps PRIVATE LIBCAPS_VERSION="0.5")

# get information about host
cmake_host_system_information(RESULT hostname QUERY HOSTNAME)
cmake_host_system_information(RESULT processor_description QUERY PROCESSOR_DESCRIPTION)
cmake_host_system_information(RESULT os_name QUERY OS_NAME)
cmake_host_system_information(RESULT os_release QUERY OS_RELEASE)
cmake_host_system_information(RESULT os_version QUERY OS_VERSION)
cmake_host_system_information(RESULT os_platform QUERY OS_PLATFORM)
set(host_info_raw "${hostname}, ${processor_description}, ${os_name} ${os_version} (${os_platform})")
string(REPLACE "#" "" host_info ${host_info_raw})
target_compile_definitions(caps PRIVATE -DMACHINE="${host_info}")

# get GIT head and branch
if(EXISTS "${CMAKE_SOURCE_DIR}/.git" AND ${GIT_FOUND})
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  execute_process(
    COMMAND ${GIT_EXECUTABLE} log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  target_compile_definitions(caps PRIVATE -DGIT_HEAD="${GIT_COMMIT_HASH}" -DGIT_BRANCH="${GIT_BRANCH}")
endif()

if(${LIBM})
    target_link_libraries(caps m)
endif()
target_link_libraries(caps ${BLAS_LIBRARIES})
target_link_libraries(caps ${LAPACK_LIBRARIES})
target_link_libraries(caps cquadpack)
target_link_libraries(caps hodlr)


# caps frontend exectuable
add_executable(caps_frontend src/caps.c)
set_target_properties(caps_frontend PROPERTIES OUTPUT_NAME "caps")

if(${LIBM})
    target_link_libraries(caps_frontend m)
endif()
target_link_libraries(caps_frontend ${MPI_LIBRARIES})
target_link_libraries(caps_frontend ${BLAS_LIBRARIES})
target_link_libraries(caps_frontend ${LAPACK_LIBRARIES})
target_link_libraries(caps_frontend caps)
target_link_libraries(caps_frontend argparse)


# caps_logdetD
add_executable(caps_logdetD src/caps_logdetD.c)

if(${LIBM})
    target_link_libraries(caps_logdetD m)
endif()
target_link_libraries(caps_logdetD ${BLAS_LIBRARIES})
target_link_libraries(caps_logdetD ${LAPACK_LIBRARIES})
target_link_libraries(caps_logdetD caps)
target_link_libraries(caps_logdetD argparse)


# capc
add_executable(capc src/capc.cpp)

if(${LIBM})
    target_link_libraries(capc m)
endif()
target_link_libraries(capc ${BLAS_LIBRARIES})
target_link_libraries(capc ${LAPACK_LIBRARIES})
target_link_libraries(capc caps)
target_link_libraries(capc argparse)


# cass
add_executable(cass src/cass.c)

if(${LIBM})
    target_link_libraries(cass m)
endif()
target_link_libraries(cass ${BLAS_LIBRARIES})
target_link_libraries(cass ${LAPACK_LIBRARIES})
target_link_libraries(cass caps)
target_link_libraries(cass argparse)


# tests
add_executable(tests src/tests/test_bessels.c src/tests/test_fresnel.c src/tests/test_lnLambda.c src/tests/test_lfac.c src/tests/test_logdetD.c src/tests/test_logi.c src/tests/test_mie.c src/tests/test_mie_drude.c src/tests/test_lnPlm.c src/tests/tests.c src/tests/unittest.c)
set_target_properties(tests PROPERTIES EXCLUDE_FROM_ALL 1)
set_target_properties(tests PROPERTIES OUTPUT_NAME "caps_tests")

if(${LIBM})
    target_link_libraries(tests m)
endif()
target_link_libraries(tests ${BLAS_LIBRARIES})
target_link_libraries(tests ${LAPACK_LIBRARIES})
target_link_libraries(tests caps)


# examples
# AuAl
add_executable(AuAl examples/AuAl.c)
set_target_properties(AuAl PROPERTIES EXCLUDE_FROM_ALL 1)

if(${LIBM})
    target_link_libraries(AuAl m)
endif()
target_link_libraries(AuAl ${BLAS_LIBRARIES})
target_link_libraries(AuAl ${LAPACK_LIBRARIES})
target_link_libraries(AuAl caps)

# install
install(TARGETS caps_frontend cass capc caps_logdetD DESTINATION "bin")
install(TARGETS caps DESTINATION "lib")

message("")
message("#################")
message("# Build options #")
message("#################")
message("")
message("C compiler:        " ${CMAKE_C_COMPILER})
message("C++ compiler:      " ${CMAKE_CXX_COMPILER})
message("blas libraries:    " ${BLAS_LIBRARIES})
message("lapack libraries:  " ${LAPACK_LIBRARIES})
message("latest git commit: " ${GIT_COMMIT_HASH})
message("git branch:        " ${GIT_BRANCH})
message("machine:           " ${host_info})
message("")
