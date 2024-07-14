set(PROJECT_NAME mines)
set(SFML_DIR /usr/local/lib/SFML-2.6.1/lib/cmake/SFML)

project(${PROJECT_NAME})

find_package(SFML COMPONENTS graphics window system REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)


target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)