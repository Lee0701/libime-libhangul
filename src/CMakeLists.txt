include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)
file( GLOB_RECURSE SOURCES *.cpp )
set( DEF_FILE ${CMAKE_PROJECT_NAME}.def )
set(CMAKE_CXX_STANDARD 17)

set( MAIN_LIB_NAME ${CMAKE_PROJECT_NAME} )

add_library( ${MAIN_LIB_NAME} SHARED ${SOURCES} ${DEF_FILE} )

target_link_libraries( ${MAIN_LIB_NAME} PUBLIC
    libIME2_static
    hangul
)
