cmake_minimum_required(VERSION 2.9)
project(qcustomplot)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Core REQUIRED)

include_directories(${Qt5Widgets_INCLUDES} ${Qt5Core_INCLUDES} ./include/ ../include  ../libdaq/include /usr/local/include)

add_definitions(${Qt5Widgets_DEFINITIONS} -DQCUSTOMPLOT_COMPILE_LIBRARY)

#qt5_wrap_ui(UIS_HDRS
#    src/mainwindow.ui
#)

set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

file (
	GLOB_RECURSE

	source_files
	src/*.c
	src/*.cpp
        
	include/*.h     
	../../include/*.h
)


add_library(
	qcustomplot 
	SHARED
	${source_files}
)

target_link_libraries(qcustomplot ${Qt5Widgets_LIBRARIES} Qt5::PrintSupport)




