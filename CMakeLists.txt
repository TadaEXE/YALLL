cmake_minimum_required(VERSION 3.20)
project(YALLL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Antlr ---------------------------------------------------------
# add_definitions(-DANTLR4CPP_STATIC)
# set(ANTLR_TAG 4.13.2)
#
# include(ExternalAntlr4Cpp)
#
# message("Including antlr dirs: ${ANTLR4_INCLUDE_DIRS}")
# include_directories(${ANTLR4_INCLUDE_DIRS})
#
# set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/antlr/antlr-4.13.2-complete.jar)
#
# find_package(ANTLR REQUIRED)
#
# antlr_target(YALLL ${CMAKE_CURRENT_SOURCE_DIR}/antlr/YALLL.g4 VISITOR)
#
# include_directories(${ANTLR_YALLL_OUTPUT_DIR})
set(G4_FILE ${CMAKE_CURRENT_SOURCE_DIR}/antlr/YALLL.g4)
set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/antlr/antlr-4.13.2-complete.jar)
set(ANTLR_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/antlr4cpp_generated_src)
set(ANTLR_RUNTIME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/antlr/cpp_runtime_4_13_2)

include_directories(${ANTLR_RUNTIME_DIR})

file(GLOB_RECURSE antlr_runtime_src
  ${ANTLR_RUNTIME_DIR}/**.h
  ${ANTLR_RUNTIME_DIR}/**.cpp
)

message("Generating antlr files from ${G4_FILE} into ${ANTLR_OUTPUT_DIR} using ${ANTLR_EXECUTABLE}")

execute_process(COMMAND java -jar ${ANTLR_EXECUTABLE} -Dlanguage=Cpp -visitor -o ${ANTLR_OUTPUT_DIR} ${G4_FILE})
include_directories(${ANTLR_OUTPUT_DIR})

file(GLOB_RECURSE antlr_generated_src
  ${ANTLR_OUTPUT_DIR}/**.h
  ${ANTLR_OUTPUT_DIR}/**.cpp
)
message("Found generated antlr files: ${antlr_generated_src}")
add_library(antlr_lib ${antlr_generated_src} ${antlr_runtime_src})
message("Made antlr lib: ${antlr_lib}")
# Antlr ---------------------------------------------------------


# LLVM ----------------------------------------------------------
message("Searching for LLVM.")

find_package(LLVM REQUIRED CONFIG)

message("Found LLVM ${LLVM_PACKAGE_VERSION}")
message("Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(llvm_libs core support)
message("Adding LLVM-Libs: ${llvm_libs}")
# LLVM ----------------------------------------------------------

file(GLOB_RECURSE src_files
    ${CMAKE_CURRENT_SOURCE_DIR}/src/**.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${src_files} ${antlr_generated_src} ${antlr_runtime_src})

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${antlr_lib} ${llvm_libs})
