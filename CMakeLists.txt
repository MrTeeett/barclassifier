cmake_minimum_required(VERSION 3.10)

project(Barclassifier VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(OpenCV_DIR "C:/Lib/OpenCV/opencv/build")
set(OpenCV_DLL_DIR "C:/Lib/OpenCV/opencv/build/x64/vc16/bin")

find_package(OpenCV REQUIRED)

set(SOURCES
    main.cpp
    source/barclasses.cpp
    source/barcodeCreator.cpp
    source/component.cpp
    source/hole.cpp
)

set(HEADERS
    detection.h
    include/barclasses.h
    include/barcodeCreator.h
    include/barline.h
    include/barstrucs.h
    include/component.h
    include/hole.h
    include/include_cv.h
    include/include_py.h
    include/presets.h
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    C:/Lib/OpenCV/include
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gen/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gen/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gen/lib)
set(CMAKE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gen)

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${OpenCV_DLL_DIR}")
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${OpenCV_DLL_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
