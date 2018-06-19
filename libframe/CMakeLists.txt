cmake_minimum_required(VERSION 2.8)
#Déclaration du projet

project(frame)

set (O_FLAG "-O2 -DHAVE_PF_RING")
set (CFLAGS "${O_FLAG} -Wall -D ENABLE_BPF  -D HAVE_PF_RING_ZC")
set (CMAKE_CXX_FLAGS "-std=c++11 ${CFLAGS} -Wextra -Wno-unused-parameter -ggdb -g -pthread")
message(INFO ${EXECUTABLE_OUTPUT_PATH})
if(DEFINED LIBRARY_OUTPUT_PATH)
else()
	set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)
	set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
endif()
#set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)
#set (Libraries "pfring pthread")



#set (USERLIB tinyxml)

include_directories(./include/ ../include  ${LIB_SHM_PATH}/include /usr/local/include)

#link_directories(/usr/local/lib ../../lib/build)

#Configuration de l'édition de liens

#add_library(libpthread STATIC IMPORTED)
#add_library(libpfring STATIC IMPORTED)
#add_library(libbsd STATIC IMPORTED)
#add_library(libz STATIC IMPORTED)
#add_library(librt STATIC IMPORTED)
#add_library(libringshm SHARED IMPORTED)

file (
		GLOB_RECURSE

	source_files
	src/decodeframe.cpp
        
	include/decodeframe.h    
	../include/*.h
)

#Déclaration de l'exécutable

add_library(
	frame
	${source_files}
)

add_executable(
	testframe
	src/test.cpp
)

#
add_executable(
	ReadEventBuild
	src/ReadEventBuild.cpp
)
#
# mettre .cpp pour cmake
add_executable(
	BuildEvent
	src/BuildEvent.cpp
)
#
add_executable(
	BuildEvent_Multi
	src/BuildEvent_Multi.cpp
)

target_link_libraries(frame PUBLIC pthread)
target_link_libraries(testframe PUBLIC frame)

#
target_link_libraries(ReadEventBuild PUBLIC frame)
#
#
target_link_libraries(BuildEvent PUBLIC frame)

target_link_libraries(BuildEvent_Multi PUBLIC frame)
#


option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message(INFO "Doxygen build started" ${CMAKE_CURRENT_BINARY_DIR})

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT} > /dev/null
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

