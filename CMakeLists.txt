cmake_minimum_required(VERSION 3.16)

project(Jupiter_Drugstore VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# crucial for qt: enables automatic compilation of ui, moc, and resource files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Tell CMake to look for .ui files in the 'ui' directory
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

# adjust '6' to '5' if you are using qt 5
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

#file directories
include_directories(include)

set(SOURCES #source files
    src/main.cpp

    src/mainwindow.cpp
    src/helpers.cpp
    src/databasemanager.cpp
    src/paymentdialog.cpp
    src/logindialog.cpp
)

set(HEADERS #header files
    include/mainwindow.h
    include/helpers.h
    include/databasemanager.h
    include/models.h
    include/paymentdialog.h
    include/logindialog.h
)

set(UIS #ui files
    ui/mainwindow.ui
    ui/paymentdialog.ui
    ui/logindialog.ui
)

set(RESOURCES #resource files
    assets/resources.qrc
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UIS}
    ${RESOURCES}
)

# Link Qt libraries to your executable
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Sql)