cmake_minimum_required(VERSION 3.16) # Need 3.16+ for Qt6

# Force x86_64 architecture (must be before project declaration)
set(CMAKE_OSX_ARCHITECTURES "x86_64")

# Set minimum macOS deployment target for C++17 and std::filesystem
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS version")

# Enable Qt automoc, autorcc, and autouic features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

project(MyQtApp LANGUAGES C CXX)

# Set compiler to clang for x86_64
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the x86_64 paths for Qt 6 - don't use the ARM version from Homebrew
set(Qt6_DIR "/usr/local/Cellar/qt/6.9.0/lib/cmake/Qt6")
set(CMAKE_PREFIX_PATH "/usr/local/Cellar/qt/6.9.0")

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

# Path to the x86_64 OpenCV
set(OpenCV_DIR "/usr/local/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)

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

# Add executable target
add_executable(MyQtApp
  src/main.cpp
  src/mainwindow.cpp
  src/model_inference.cpp
  src/mainwindow.h
  src/model_inference.h
)

# Link libraries
target_link_libraries(MyQtApp PRIVATE
  Qt6::Core
  Qt6::Widgets
  Qt6::Gui
  ${OpenCV_LIBS}
)

# Compile options
target_compile_options(MyQtApp PRIVATE -Wno-deprecated-declarations)

# # Add Qt deployment rules for macOS
# if(APPLE)
#   set_target_properties(MyQtApp PROPERTIES
#     MACOSX_BUNDLE TRUE
#   )

#   # Use the qt6 macdeployqt tool instead of relying on rpath
#   find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS 
#     "/usr/local/opt/qt6/bin"
#     "/usr/local/bin"
#     "/opt/homebrew/opt/qt6/bin"
#   )
  
#   if(MACDEPLOYQT_EXECUTABLE)
#     add_custom_command(TARGET MyQtApp POST_BUILD
#       COMMAND ${MACDEPLOYQT_EXECUTABLE} 
#         ${CMAKE_CURRENT_BINARY_DIR}/MyQtApp.app
#         -verbose=2
#       COMMENT "Running macdeployqt..."
#     )
#   endif()
# endif()