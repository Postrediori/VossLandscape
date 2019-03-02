make_executable()

include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${PLOG_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    ${IMGUI_INCLUDE_DIR}
    )
    
target_link_libraries(${PROJECT}
    ${SDL2_LIBRARIES}
    ${GLAD_LIBRARIES}
    ${IMGUI_LIBRARIES}
    MathLib
    VossGeneratorLib
    HeightmapRenderLib
    )

