cmake_minimum_required(VERSION 3.16)

project(Jupiter_Drugstore VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Crucial for Qt: Enables automatic compilation of UI, MOC, and Resource files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Adjust '6' to '5' if you are using Qt 5
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

#file directories
include_directories(include)

set(SOURCES #source files
    src/main.cpp

    src/helpers.cpp
)

set(HEADERS #header files
    include/mainwindow.h
    include/helpers.h
    include/models.h
)

#set(UIS #ui files
#    ui/mainwindow.ui
#)

set(RESOURCES #resource files
    assets/resources.qrc
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    #${UIS}
    ${RESOURCES}

    "src/mainwindow.cpp"
    src/mainwindow.ui
)

# Link Qt libraries to your executable
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Sql)