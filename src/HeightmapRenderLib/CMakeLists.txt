make_library()

target_precompile_headers(${PROJECT} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/pch.h)

target_link_libraries(${PROJECT}
    MathLib
    VossGeneratorLib
    )
