make_executable()

include_directories(
    ${PNG_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    )

target_link_libraries(${PROJECT}
    ${PNG_LIBRARIES}
    ${ZLIB_LIBRARIES}
    MathLib
    VossGeneratorLib
    HeightmapRenderLib
    )

