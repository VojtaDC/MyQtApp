cmake_minimum_required(VERSION 3.16) # Need 3.16+ for modern CMake features

# Option for building tests
option(BUILD_TESTING "Build the testing tree" OFF)

# Set compiler to clang for x86_64
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

# Force x86_64 architecture (must be before project declaration)
set(CMAKE_OSX_ARCHITECTURES "x86_64")

# Set minimum macOS deployment target for C++17 and std::filesystem
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS version")

# Enable Qt automoc, autorcc, and autouic features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

project(MyQtApp LANGUAGES C CXX)

# C++17 required for std::filesystem, std::optional, etc.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Add position-independent code flag
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add sanitizers for development builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SANITIZER_FLAGS "-fsanitize=address,undefined -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${SANITIZER_FLAGS}")
endif()

# Specify the Qt 5.12 path - update this to your actual Qt 5.12 installation path
set(Qt5_DIR "/Users/vojtadeconinck/MyQtApp/Qt5.12.0/5.12.0/clang_64/lib/cmake/Qt5")
set(CMAKE_PREFIX_PATH "/Users/vojtadeconinck/Qt5.12.0/5.12.0/clang_64")

# Set RPATH settings for easy development
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_PREFIX_PATH}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Find Qt5 components
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui)

# Path to the x86_64 OpenCV
set(OpenCV_DIR "/usr/local/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)

# Include FetchContent for managing external dependencies
include(FetchContent)

# Fetch Catch2 for unit testing if testing is enabled
if(BUILD_TESTING)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.13.7  # Use an older version compatible with C++17
    )
    FetchContent_MakeAvailable(Catch2)
endif()

# Fetch {fmt} library for modern string formatting
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 7.1.3  # Use an older version for better compatibility
)
FetchContent_MakeAvailable(fmt)

# Add include directories
include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${OpenCV_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/external/include
  ${CMAKE_SOURCE_DIR}/external/include/frugally-deep/include
  ${CMAKE_SOURCE_DIR}/external/include/FunctionalPlus/include_all_in_one/include
  ${CMAKE_SOURCE_DIR}/external/include/json/include
  ${CMAKE_SOURCE_DIR}/external/include/Eigen
)

# Add executable target with source files
add_executable(MyQtApp
  src/main.cpp
  src/mainwindow.cpp
  src/mainwindow.h
  src/model_inference.cpp
  src/model_inference.h
  src/usertypedialog.cpp
  src/usertypedialog.h
  src/logindialog.cpp
  src/logindialog.h
  src/hospitaldatamanager.cpp
  src/hospitaldatamanager.h
  src/patient.cpp
  src/patient.h
  src/doctor.cpp
  src/doctor.h
  src/person.cpp
  src/person.h
  src/xraybuffer.cpp
  src/xraybuffer.h
)

# Link libraries
target_link_libraries(MyQtApp PRIVATE
  Qt5::Core
  Qt5::Widgets
  Qt5::Gui
  ${OpenCV_LIBS}
  fmt::fmt
)

# Compile options
target_compile_options(MyQtApp PRIVATE 
  -Wall
  -Wextra
  -Wpedantic
  -Wno-deprecated-declarations
)

# Configure testing
if(BUILD_TESTING)
    enable_testing()
    
    # Add test executable
    add_executable(unit_tests
        tests/test_main.cpp
        tests/test_person.cpp
        tests/test_patient.cpp
        tests/test_doctor.cpp
        tests/test_xraybuffer.cpp
        tests/test_hospitaldatamanager.cpp
        
        # Include necessary source files to test
        src/person.cpp
        src/patient.cpp
        src/doctor.cpp
        src/xraybuffer.cpp
        src/hospitaldatamanager.cpp
    )
    
    # Link test executable with Qt and Catch2
    target_link_libraries(unit_tests PRIVATE
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
        ${OpenCV_LIBS}
        fmt::fmt
        Catch2::Catch2WithMain
    )
    
    # Add sanitizer flags for tests too
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(unit_tests PRIVATE ${SANITIZER_FLAGS})
        target_link_options(unit_tests PRIVATE ${SANITIZER_FLAGS})
    endif()
    
    # Register tests with CTest
    include(Catch)
    catch_discover_tests(unit_tests)
endif()

# Add Qt deployment rules for macOS
if(APPLE)
  # Comment out the bundle property to create a simple executable
  # set_target_properties(MyQtApp PROPERTIES
  #   MACOSX_BUNDLE TRUE
  # )

  # Comment out macdeployqt which bundles Qt libraries
  # find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS 
  #   "/Users/vojtadeconinck/Qt5.12.0/5.12.0/clang_64/bin"
  #   "/usr/local/bin"
  # )
  
  # if(MACDEPLOYQT_EXECUTABLE)
  #   add_custom_command(TARGET MyQtApp POST_BUILD
  #     COMMAND ${MACDEPLOYQT_EXECUTABLE} 
  #       ${CMAKE_CURRENT_BINARY_DIR}/MyQtApp.app
  #       -verbose=2
  #     COMMENT "Running macdeployqt..."
  #   )
  # endif()
  
  # Instead, just set the RPATH for the executable to find Qt libraries at runtime
  set_target_properties(MyQtApp PROPERTIES
    INSTALL_RPATH "@executable_path/../lib;/Users/vojtadeconinck/Qt5.12.0/5.12.0/clang_64/lib"
    BUILD_WITH_INSTALL_RPATH TRUE
  )
endif()

# Add frugally-deep library for deep learning model inference
find_package(frugally-deep CONFIG QUIET)
if(NOT frugally-deep_FOUND)
  message(STATUS "frugally-deep not found via find_package, will use the include directory")
endif()