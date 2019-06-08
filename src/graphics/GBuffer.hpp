#pragma once



namespace grx {
    class GBuffer {
    public:
        enum TextureType {
            TextureType_Position = 0,
            TextureType_Diffuse  = 1,
            TextureType_Normal   = 2,
            TextureType_COUNT    = 3
        };

         GBuffer(unsigned wndWidth, unsigned wndHeight);
        ~GBuffer();

        void start();
        void geometryPass();
        void stencilPass();
        void lightPass();
        void finalize();

    private:
        unsigned fbo;
        unsigned textures[TextureType_COUNT];
        unsigned depth_texture;
        unsigned final_texture;
    };
} // namespace grx