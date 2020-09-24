make_executable()

target_include_directories(${PROJECT} PUBLIC
    ${OPENGL_INCLUDE_DIR}
    ${PLOG_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    ${IMGUI_INCLUDE_DIR}
    )

target_link_libraries(${PROJECT}
    ${GLFW_LIBRARIES}
    ${GLAD_LIBRARIES}
    ${IMGUI_LIBRARIES}
    MathLib
    UtilsLib
    GraphicsLib
    VossGeneratorLib
    HeightmapRenderLib
    )

