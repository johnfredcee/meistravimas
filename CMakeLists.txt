cmake_minimum_required (VERSION 2.8)
project (skeleton)

# Name of the executable
set(EXENAME ${CMAKE_PROJECT_NAME})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Set default compile flags for GCC
if (CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "GCC detected, adding compile flags")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c99")
endif()


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
list(APPEND LIBS ${SDL2_LIBRARIES})

find_package(SDL2_net REQUIRED)
include_directories(${SDL2_NET_INCLUDE_DIRS})
list(APPEND LIBS ${SDL2_NET_LIBRARIES})

find_package(PhysFS REQUIRED)
include_directories(${PHYSFS_INCLUDE_DIRS})
list(APPEND LIBS ${PHYSFS_LIBRARY})

# Find all the libs that don't require extra parameters
foreach(lib SOIL OpenGL GLEW)
	find_package(${lib} REQUIRED)
	include_directories(${${lib}_INCLUDE_DIRS})
	list(APPEND LIBS ${${lib}_LIBRARIES})
	add_definitions(${${lib}_DEFINITIONS})
endforeach(lib)


file(GLOB Includes include/*.h)
file(GLOB Sources src/*.c*)

include_directories("include")
add_executable(skeleton ${Sources} ${Includes})
target_link_libraries(skeleton ${LIBS}) 
