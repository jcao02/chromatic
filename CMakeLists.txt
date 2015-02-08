cmake_minimum_required( VERSION 2.8 )

project( CHROMATIC )

# Different builts, different flags
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Werror -pedantic -g")

add_subdirectory(gtest-1.7.0) 

include_directories (
    "${PROJECT_BINARY_DIR}"
    "${gtest_SOURCE_DIR}/include" 
    "${gtest_SOURCE_DIR}"
)


#add_library( common SHARED  ) 

#add_executable ( chromatic chromatic.cpp ) 
#target_link_libraries ( chromatic common ) 

add_executable(test test.cpp)
target_link_libraries(test gtest)
add_custom_target(run_tests test --gtest_output=xml ) 
