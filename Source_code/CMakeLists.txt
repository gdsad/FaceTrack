cmake_minimum_required(VERSION 3.16.3)

project(FaceTrack)

# TODO: fix warnings and remove -fpermissive.
set(CMAKE_CXX_FLAGS "-fpermissive -g")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Qt5
find_package(
    Qt5
    COMPONENTS
        Charts
        Core
        Gui
        Widgets
    REQUIRED
)

# OpenCV
find_package(OpenCV REQUIRED)

# ALGLIB
find_package(ALGLIB REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${ALGLIB_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
)

add_executable(FaceTrack
    main.cpp
)

target_link_libraries(FaceTrack
    Qt5::Charts
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    ${OpenCV_LIBS}
    ${ALGLIB_LIB}
)

# Set AUTO* tools.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# robo-scope-library
set_target_properties(
    FaceTrack
    PROPERTIES
        AUTOMOC
        ON # for moc_
        AUTOUIC
        ON # for ui_
        AUTORCC
        ON # for resource files like resources.qrc
)

target_sources(
    FaceTrack
    PRIVATE
        facialmesh.cpp
        facetracking.cpp
        logfile.cpp
        main.cpp
        mainwindow.cpp
        keyframeselection.cpp
        modelfitting.cpp
        onlymesh.cpp
        results.cpp
        trackingvideo.cpp
        PointState.cpp
)

file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY candide-fit DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(resources)
add_subdirectory(utils)