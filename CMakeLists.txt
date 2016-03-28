project(WarhammerBattleSimulator)

cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 14)

if(MSVC)
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -WExtra -pedantic -WError")
endif()

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)

set(SRC_LIST
    simulator.cpp
    diceroller.cpp
    character.cpp 
)

qt5_wrap_cpp(MOC_LIST ${PROJECT_SOURCE_DIR}/mainwindow.h)
qt5_wrap_ui(UI_LIST ${PROJECT_SOURCE_DIR}/mainwindow.ui)
qt5_add_resources(RSC_LIST ${PROJECT_SOURCE_DIR}/resources.qrc)

include_directories(${PROJECT_BINARY_DIR})

add_executable(${PROJECT_NAME}
        main.cpp
        mainwindow.cpp
        ${SRC_LIST}
        ${MOC_LIST}
        ${UI_LIST}
        ${RSC_LIST}
)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Concurrent)

if(APPLE)
    find_path(MACDEPLOYQT_PATH macdeployqt PATH_SUFFIXES bin)
    if(NOT MACDEPLOYQT_PATH)
        message(FATAL_ERROR "Could not find macdeployqt for OSX bundling. You can point MACDEPLOYQT_PATH to it's path.")
    endif()

    # If macdeployqt is found just build and install.
    # With xcode it is:
    # $ xcodebuild -project project.xcodeproj
    # $ xcodebuild -project project.xcodeproj -target install
    # OSX budle is ready.
    install(CODE "
            execute_process(COMMAND ${MACDEPLOYQT_PATH}/macdeployqt \"${BUNDLE_PATH}\" -verbose=2)
            message(STATUS \"Creating dmg of app\")
            execute_process(COMMAND hdiutil create -volname ${PROJECT_NAME} -srcfolder \"${BUNDLE_PATH}\" -ov -format UDZO \"${PROJECT_NAME}.dmg\")
            " COMPONENT Runtime
    )
elseif(UNIX)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "/usr/bin")
endif()

### Tests
find_package(Threads REQUIRED)

add_subdirectory(googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_executable(runUnitTests
    tests.cpp
    character_tests.cpp
    simulator_tests.cpp
    ${SRC_LIST}
)
target_link_libraries(runUnitTests gtest gtest_main Qt5::Core ${CMAKE_THREAD_LIBS_INIT})
add_test(runUnitTests runUnitTests)

