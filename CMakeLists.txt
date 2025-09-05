cmake_minimum_required(VERSION 3.16 FATAL_ERROR) # Or a newer version based on your Qt version

project(AIOAM VERSION 1.0.0 LANGUAGES CXX)

# Find Qt and its required modules
find_package(Qt6 COMPONENTS Core Gui Widgets Multimedia REQUIRED) # Adjust components as needed (e.g., Quick, Network)

# Set up standard Qt project defaults (recommended for Qt 6.5+)
qt_standard_project_setup(REQUIRES 6.5)

# Add your executable
add_executable(AIOAM
    main.cpp
    mainWindow.cpp
    # Add other source files here
    # YourWidget.cpp
    # YourDialog.cpp
)

# Enable automatic MOC, RCC, and UIC processing for Qt files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Link your executable against the necessary Qt libraries
target_link_libraries(AIOAM PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    # Add other Qt modules if used, e.g., Qt6::Quick, Qt6::Network
)

# Optional: Add resource files
# qt_add_resources(YourProjectName_RESOURCES YourProjectName.qrc)
# target_link_libraries(YourProjectName PRIVATE YourProjectName_RESOURCES)

# Optional: Add QML modules for Qt Quick applications
# qt_add_qml_module(YourProjectName
#     URI YourProjectName
#     VERSION 1.0
#     QML_FILES
#         qml/main.qml
#         qml/AnotherComponent.qml
# )
