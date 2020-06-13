cmake_minimum_required (VERSION 2.8)
project (meistravimas)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Name of the executable
set(EXENAME ${CMAKE_PROJECT_NAME})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Set default compile flags for GCC
if (CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "GCC detected, adding compile flags")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wextra -std=c++11 -DSTBI_NO_STDIO")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -Wall -Wextra -std=c99 -DSTBI_NO_STDIO")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
endif()

find_package(Threads)

find_package(ZLIB REQUIRED)
list(APPEND INCS ${ZLIB_INCLUDE_DIRS})
list(APPEND LIBS ${ZLIB_LIBRARIES})

find_package(SDL2 REQUIRED)
list(APPEND INCS ${SDL2_INCLUDE_DIRS})
if (MINGW)
	list(APPEND LIBS mingw32)
endif()
list(APPEND LIBS ${SDL2_LIBRARIES})
#if (MINGW)
#	list(APPEND LIBS mwindows)
#endif()
find_package(SDL2_net REQUIRED)
list(APPEND INCS ${SDL2_NET_INCLUDE_DIRS})
list(APPEND LIBS ${SDL2_NET_LIBRARIES})

find_package(PhysFS REQUIRED)
list(APPEND INCS ${PHYSFS_INCLUDE_DIR})
list(APPEND LIBS ${PHYSFS_LIBRARY})

# Find all the libs that don't require extra parameters
foreach(lib OpenGL ZLIB)
	find_package(${lib} REQUIRED)
	list(APPEND INCS ${${lib}_INCLUDE_DIRS})
	list(APPEND LIBS ${${lib}_LIBRARIES})
	add_definitions(${${lib}_DEFINITIONS})
endforeach(lib)

#if (WIN32)
#   list(APPEND LIBS Iphlpapi.lib ws2_32.lib winmm.lib imm32.lib kernel32.lib user32.lib gdi32.lib version.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib opengl32.lib)
#endif() 

if (MINGW)
   list(APPEND LIBS Iphlpapi.lib imm32.lib version.lib ws2_32.lib winmm.lib opengl32.lib)
endif() 

#add_subdirectory(oui-blendish)
file(GLOB Includes include/*.h GL/include/glad/*.h)
file(GLOB Sources src/*.c* GL/src/*.c)

include_directories(${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/GL/include)

file(GLOB PROJECT_SOURCES app/src/*.cpp app/src/*.c glad/src/*.c)
file(GLOB PROJECT_HEADERS app/include/*.hpp app/include/*.h glad/include/glad/*.h glad/include/KHR/*h)
					
add_library(meistravimas ${Sources} ${Includes})
target_link_libraries(meistravimas ${LIBS} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
target_include_directories(meistravimas PUBLIC ${INCS})

add_executable(skeleton examples/skeleton.cpp)
target_link_libraries(skeleton meistravimas)
