set(GraphicSources
        Camera.cpp
        CameraManipulator.cpp
        GBuffer.cpp
        GraphicsContext.cpp
        Mesh.cpp
        ShaderManager.cpp
        TextureManager.cpp
        Window.cpp
        algorithms/FrustumCulling.cpp
        algorithms/asm/x86_64sv_sse_frustum_culling.asm
        algorithms/asm/x86_64sv_avx_frustum_culling.asm
)

set(GraphicHeaders
        Camera.hpp
        CameraManipulator.hpp
        GBuffer.hpp
        forward_declarations.hpp
        GraphicsContext.hpp
        Mesh.hpp
        ShaderManager.hpp
        TextureManager.hpp
        Window.hpp
        algorithms/FrustumCulling.hpp
        algorithms/frustum_culling_asm.hpp
)

add_library(DeGraphics       SHARED ${GraphicSources})
add_library(DeGraphicsStatic STATIC ${GraphicSources})

target_link_libraries(DeGraphics       GL IL GLEW glfw glfx assimp -pthread)
target_link_libraries(DeGraphicsStatic GL IL GLEW glfw glfx assimp -pthread)

target_include_directories(DeGraphics PRIVATE ../base)
target_include_directories(DeGraphics PRIVATE ../input)

target_include_directories(DeGraphicsStatic PRIVATE ../base)
target_include_directories(DeGraphicsStatic PRIVATE ../input)