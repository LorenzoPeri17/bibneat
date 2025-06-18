cmake_minimum_required(VERSION 3.20)
project(bibneat CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find ICU and CURL
find_package(ICU REQUIRED COMPONENTS uc i18n io data)
find_package(CURL REQUIRED)

if (UNIX AND (NOT APPLE))
    find_package (PkgConfig REQUIRED)
    pkg_check_modules(ICU_PKG REQUIRED icu-io icu-uc icu-i18n)
else()
    set(ICU_PKG_LIBRARIES "")
endif()


# Gather CXX
set(CXX_SUBMODULES database recipes utils cli uni)

set(BIBNEAT_CXX_SOURCES "")
set(BIBNEAT_CXX_HEADERS "")
FOREACH(mod ${CXX_SUBMODULES})
    file(GLOB_RECURSE sources src/${mod}/*.cpp)
    file(GLOB_RECURSE headers src/${mod}/*.hpp)
    list(APPEND BIBNEAT_CXX_SOURCES ${sources})
    list(APPEND BIBNEAT_CXX_HEADERS ${headers})
ENDFOREACH()

# Gather C
set(C_SUBMODULES requests)
set(BIBNEAT_C_SOURCES "")
set(BIBNEAT_C_HEADERS "")
FOREACH(mod ${C_SUBMODULES})
    file(GLOB_RECURSE sources src/${mod}/*.c)
    file(GLOB_RECURSE headers src/${mod}/*.h)
    list(APPEND BIBNEAT_C_SOURCES ${sources})
    list(APPEND BIBNEAT_C_HEADERS ${headers})
ENDFOREACH()

# setup output dirs
make_directory(${CMAKE_SOURCE_DIR}/bin)
make_directory(${CMAKE_SOURCE_DIR}/lib)
make_directory(${CMAKE_SOURCE_DIR}/include/bibneat)

# Library headers
FOREACH(mod ${CXX_SUBMODULES})
    make_directory(${CMAKE_SOURCE_DIR}/include/bibneat/${mod})
    file(GLOB_RECURSE headers src/${mod}/*.hpp)
    FOREACH(header ${headers})
        configure_file(${header} ${CMAKE_SOURCE_DIR}/include/bibneat/${mod} COPYONLY)
    ENDFOREACH()
ENDFOREACH()

FOREACH(mod ${C_SUBMODULES})
    make_directory(${CMAKE_SOURCE_DIR}/include/bibneat/${mod})
    file(GLOB_RECURSE headers src/${mod}/*.h)
    FOREACH(header ${headers})
        configure_file(${header} ${CMAKE_SOURCE_DIR}/include/bibneat/${mod} COPYONLY)
    ENDFOREACH()
ENDFOREACH()

# Build requests as a C static library
add_library(requests STATIC ${BIBNEAT_C_SOURCES} ${BIBNEAT_C_HEADERS})
set_target_properties(requests PROPERTIES LINKER_LANGUAGE C)
target_include_directories(requests PUBLIC src/requests)
target_link_libraries(requests PUBLIC CURL::libcurl)

# Shared library
add_library(bibneat_shared SHARED ${BIBNEAT_CXX_SOURCES} ${BIBNEAT_CXX_HEADERS})
target_include_directories(bibneat_shared PUBLIC src)
set_target_properties(bibneat_shared PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(bibneat_shared PRIVATE ICU::uc ICU::i18n ICU::io ICU::data)
target_link_libraries(bibneat_shared PRIVATE requests)
set_property(TARGET bibneat_shared PROPERTY OUTPUT_NAME bibneat)
target_link_libraries(bibneat_shared PRIVATE ${ICU_PKG_LIBRARIES})

# Static library
add_library(bibneat_static STATIC ${BIBNEAT_CXX_SOURCES} ${BIBNEAT_CXX_HEADERS})
target_include_directories(bibneat_static PUBLIC src)
set_target_properties(bibneat_static PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(bibneat_static PUBLIC ICU::uc ICU::i18n ICU::io ICU::data)
target_link_libraries(bibneat_static PRIVATE requests)
set_property(TARGET bibneat_static PROPERTY OUTPUT_NAME bibneat)
target_link_libraries(bibneat_static PUBLIC ${ICU_PKG_LIBRARIES})

# Binaries
add_executable(bibneat_bin src/bibneat.cpp)
target_link_libraries(bibneat_bin PRIVATE bibneat_static)
target_link_libraries(bibneat_bin PRIVATE requests)
set_property(TARGET bibneat_bin PROPERTY OUTPUT_NAME bibneat)

add_executable(bibgrab_bin src/bibgrab.cpp)
target_link_libraries(bibgrab_bin PRIVATE bibneat_static)
target_link_libraries(bibgrab_bin PRIVATE requests)
set_property(TARGET bibgrab_bin PROPERTY OUTPUT_NAME bibgrab)

# Output directories
set_target_properties(bibneat_bin PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(bibgrab_bin PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(bibneat_shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(bibneat_static PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
