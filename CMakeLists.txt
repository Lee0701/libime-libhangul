cmake_minimum_required( VERSION 3.10 )

include( C:/vcpkg/scripts/buildsystems/vcpkg.cmake )

project( libime-libhangul )

add_subdirectory(${PROJECT_SOURCE_DIR}/libIME2)
add_subdirectory(${PROJECT_SOURCE_DIR}/libhangul)

include_directories(
    ${CMAKE_SOURCE_DIR}/libIME2/src
    ${CMAKE_SOURCE_DIR}/libhangul/hangul
    ${CMAKE_SOURCE_DIR}/ini-cpp/ini
)

add_subdirectory( src bin )
