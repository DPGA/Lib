cmake_minimum_required(VERSION 2.8)
#Déclaration du projet

project(ringshm)

set (O_FLAG "-O2")
set (CFLAGS "${O_FLAG} -Wall -D ENABLE_BPF")
set (CMAKE_CXX_FLAGS "-std=c++11 ${CFLAGS} -Wextra -Wno-unused-parameter -ggdb -g -pthread")



#set (USERLIB tinyxml)

include_directories(./include ../include /usr/local/include)

#link_directories(/usr/local/lib )

#Configuration de l'édition de liens

#add_library(libpthread STATIC IMPORTED)
#add_library(libpfring STATIC IMPORTED)
#add_library(libbsd STATIC IMPORTED)
#add_library(libz STATIC IMPORTED)
#add_library(librt STATIC IMPORTED)

file (
		GLOB_RECURSE

	source_files
	src/*.c
	src/*.cpp
        
	inc/*.h     
	../include/*.h
)

#Déclaration de l'exécutable

add_library(

        ringshm
        SHARED
        ${source_files}

)


#target_link_libraries(daqdpga PUBLIC pthread pfring pcap z bsd rt)


