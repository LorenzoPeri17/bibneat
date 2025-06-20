cmake_minimum_required(VERSION 3.20)

project(bibneat-wasm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find ICU and CURL
find_package(ICU REQUIRED COMPONENTS uc i18n io data)
find_package(CURL REQUIRED)

# Gather CXX
set(CXX_SUBMODULES database recipes utils cli uni)

set(BIBNEAT_CXX_SOURCES "")
set(BIBNEAT_CXX_HEADERS "")
FOREACH(mod ${CXX_SUBMODULES})
    file(GLOB_RECURSE sources ../src/${mod}/*.cpp)
    file(GLOB_RECURSE headers ../src/${mod}/*.hpp)
    list(APPEND BIBNEAT_CXX_SOURCES ${sources})
    list(APPEND BIBNEAT_CXX_HEADERS ${headers})
ENDFOREACH()

# Gather C
set(C_SUBMODULES requests)

set(BIBNEAT_C_SOURCES "")
set(BIBNEAT_C_HEADERS "")
FOREACH(mod ${C_SUBMODULES})
    file(GLOB_RECURSE sources ../src/${mod}/*.c)
    file(GLOB_RECURSE headers ../src/${mod}/*.h)
    list(APPEND BIBNEAT_C_SOURCES ${sources})
    list(APPEND BIBNEAT_C_HEADERS ${headers})
ENDFOREACH()

# Gather JS handlers
set(CXX_JSGLUE_SUBMODULES handlers)
set(CXX_JSGLUE_INCLUDE handlers)

FOREACH(mod ${CXX_JSGLUE_SUBMODULES})
    file(GLOB_RECURSE sources ${mod}/*.cpp)
    file(GLOB_RECURSE headers ${mod}/*.hpp)
    list(APPEND BIBNEAT_CXX_SOURCES ${sources})
    list(APPEND BIBNEAT_CXX_HEADERS ${headers})
    list(APPEND CXX_JSGLUE_INCLUDE ${mod})
ENDFOREACH()

# setup output dirs
make_directory(${CMAKE_SOURCE_DIR}/lib)
make_directory(${CMAKE_SOURCE_DIR}/lib/node)
make_directory(${CMAKE_SOURCE_DIR}/lib/web)

# Build requests as a C static library
add_library(requests STATIC ${BIBNEAT_C_SOURCES} ${BIBNEAT_C_HEADERS})
set_target_properties(requests PROPERTIES LINKER_LANGUAGE C)
target_include_directories(requests PUBLIC ../src/requests)
target_link_libraries(requests PUBLIC CURL::libcurl)

# Static library
add_library(bibneat_static STATIC ${BIBNEAT_CXX_SOURCES} ${BIBNEAT_CXX_HEADERS})
set_target_properties(requests PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(bibneat_static PUBLIC ../src)
target_link_libraries(bibneat_static PUBLIC ICU::uc ICU::i18n ICU::io ICU::data)
target_link_libraries(bibneat_static PUBLIC requests)
set_property(TARGET bibneat_static PROPERTY OUTPUT_NAME bibneat)
target_link_libraries(bibneat_static PUBLIC ${ICU_PKG_LIBRARIES})

# Add embindings source
set(EMBINDINGS_SRC ${CMAKE_SOURCE_DIR}/bindings/embindings.cpp)
set(EMBINDINGS_HEADERS ${CMAKE_SOURCE_DIR}/bindings/embindings.hpp)

set(EMCXX_LINK_FLAGS "-lembind -sWASM=1 -sNO_EXIT_RUNTIME=1 -sALLOW_MEMORY_GROWTH=1 -sMODULARIZE=1 -sEXPORT_NAME=createBibneatModule -sEXPORTED_RUNTIME_METHODS=ccall,cwrap")

# WASM library target for embind
add_executable(bibneat_wasm ${EMBINDINGS_SRC} ${EMBINDINGS_HEADERS})
target_include_directories(bibneat_wasm PRIVATE ../src ${CMAKE_SOURCE_DIR}/bindings ${CXX_JSGLUE_INCLUDE})
target_link_libraries(bibneat_wasm PRIVATE ${ICU_PKG_LIBRARIES})
target_link_libraries(bibneat_wasm PRIVATE "-Wl,--whole-archive" bibneat_static)
set_target_properties(bibneat_wasm 
    PROPERTIES 
    OUTPUT_NAME "bibneat"
    SUFFIX ".js"
    LINK_FLAGS "${EMCXX_LINK_FLAGS} -sENVIRONMENT=node"
)

set_target_properties(bibneat_wasm PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/node)

# WASM library target for embind
add_executable(bibneat_web ${EMBINDINGS_SRC} ${EMBINDINGS_HEADERS})
target_include_directories(bibneat_web PRIVATE ../src ${CMAKE_SOURCE_DIR}/bindings ${CXX_JSGLUE_INCLUDE})
target_link_libraries(bibneat_web PRIVATE ${ICU_PKG_LIBRARIES})
target_link_libraries(bibneat_web PRIVATE "-Wl,--whole-archive" bibneat_static)
set_target_properties(bibneat_web 
    PROPERTIES 
    OUTPUT_NAME "bibneat"
    SUFFIX ".js"
    LINK_FLAGS "${EMCXX_LINK_FLAGS} -sENVIRONMENT=web"
)

set_target_properties(bibneat_web PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/web)