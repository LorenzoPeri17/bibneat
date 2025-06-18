cmake_minimum_required(VERSION 3.20)
project(bibneat-test CXX C)

enable_testing()

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE CXX_TEST_SRC src/*.cpp)
file(GLOB_RECURSE CXX_TEST_H src/*.h)
file(GLOB_RECURSE CXX_ONLINE_TEST_SRC srcOnline/*.cpp)

add_compile_definitions(CMAKE)
set(TESTBIB_PATH ${CMAKE_SOURCE_DIR}/)
configure_file(${CMAKE_SOURCE_DIR}/src/testBibfilesLoc.cmake.h.in ${CMAKE_SOURCE_DIR}/src/testBibfilesLoc.cmake.h @ONLY)

find_package(Catch2 CONFIG REQUIRED)
find_package(ICU REQUIRED COMPONENTS uc i18n io data)
find_package(CURL REQUIRED)

if (UNIX AND (NOT APPLE))
    find_package (PkgConfig REQUIRED)
    pkg_check_modules(ICU_PKG REQUIRED icu-io icu-uc icu-i18n)
else()
    set(ICU_PKG_LIBRARIES "")
endif()


# Offline tests
add_executable(tests ${CXX_TEST_SRC} ${CXX_TEST_H})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests PRIVATE ICU::uc ICU::i18n ICU::io ICU::data)
target_link_libraries(tests PRIVATE ${ICU_PKG_LIBRARIES})
target_link_libraries(tests PRIVATE CURL::libcurl)
set_target_properties(tests PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(tests PRIVATE bibneat)
target_include_directories(tests PRIVATE ../include)
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_directories(tests PRIVATE ../lib)

set_property(TARGET tests PROPERTY OUTPUT_NAME bibneat_tests)
set_target_properties(tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_test(tests COMMAND ${CMAKE_SOURCE_DIR}/bin/bibneat_tests --reporter compact)

# Online tests
add_executable(tests_online ${CXX_ONLINE_TEST_SRC})
target_link_libraries(tests_online PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests_online PRIVATE ICU::uc ICU::i18n ICU::io ICU::data)
target_link_libraries(tests_online PRIVATE ${ICU_PKG_LIBRARIES})
target_link_libraries(tests_online PRIVATE CURL::libcurl)
set_target_properties(tests_online PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(tests_online PRIVATE bibneat)
target_include_directories(tests_online PRIVATE ../include)
target_link_directories(tests_online PRIVATE ../lib)

set_property(TARGET tests_online PROPERTY OUTPUT_NAME bibneat_online_tests)
set_target_properties(tests_online PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

if(DEFINED DO_ONLINE_TESTS)
    add_test(tests_online COMMAND ${CMAKE_SOURCE_DIR}/bin/bibneat_online_tests --reporter compact)
endif()