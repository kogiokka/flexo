add_library(gfx OBJECT)

set_target_properties(gfx PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(gfx PRIVATE -Wall -Wextra --pedantic-errors -ggdb)
    target_compile_definitions(gfx PRIVATE $<$<CONFIG:Debug>:_GLIBCXX_DEBUG>)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(gfx PRIVATE -Wall -Wextra --pedantic-errors)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(gfx PRIVATE /wd4996)
endif()

target_sources(gfx
    PRIVATE
        "VertexArray.cpp"
        "VertexLayout.cpp"
        "UniformBlock.cpp"
        "Graphics.cpp"
        "Renderer.cpp"
        "BindStep.cpp"
        "DrawTask.cpp"
        "Camera.cpp"
        "glwr/IGLWRState.cpp"
        "glwr/IGLWRBuffer.cpp"
        "glwr/IGLWRInputLayout.cpp"
        "glwr/IGLWRVertexShader.cpp"
        "glwr/IGLWRFragmentShader.cpp"
        "glwr/IGLWRSampler.cpp"
        "glwr/IGLWRResource.cpp"
        "glwr/IGLWRTexture2D.cpp"
        "glwr/IGLWRRenderBuffer.cpp"
        "glwr/IGLWRRenderTargetView.cpp"
        "glwr/IGLWRDepthStencilView.cpp"
        "glwr/IGLWRShaderResourceView.cpp"
        "bindable/Primitive.cpp"
        "bindable/InputLayout.cpp"
        "bindable/IndexBuffer.cpp"
        "bindable/UniformBuffer.cpp"
        "bindable/VertexBuffer.cpp"
        "bindable/Texture2D.cpp"
        "bindable/Sampler.cpp"
        "bindable/RasterizerState.cpp"
        "bindable/program/VertexShaderProgram.cpp"
        "bindable/program/FragmentShaderProgram.cpp"
)

target_include_directories(gfx PUBLIC include)
target_link_libraries(gfx PUBLIC glad::glad)
target_link_libraries(gfx PRIVATE stb::image glm::glm flexo::log)

add_library(flexo::gfx ALIAS gfx)
