#pragma once

#include "Renderer.hpp"

class FrameBuffer
{
public:
    FrameBuffer();
    FrameBuffer(unsigned int m_RendererID);
    ~FrameBuffer();

    void Bind() const;
    void Unbind() const;
    void Delete() const;

    void AttachTexture();
    unsigned int GetTextureID() const { return m_TextureID; }

private:
    unsigned int m_RendererID = 0;
    unsigned int m_TextureID = 0;
};