//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// VertexBuffer.h: Defines the VertexBuffer and derivations, classes that
// perform graphics API agnostic vertex buffer operations.

#ifndef LIBGLESV2_RENDERER_VERTEXBUFFER_H_
#define LIBGLESV2_RENDERER_VERTEXBUFFER_H_

#include <vector>
#include <cstddef>

#define GL_APICALL
#include <GLES2/gl2.h>

#include "libGLESv2/Context.h"

namespace rx
{

class VertexBuffer
{
  public:
    VertexBuffer(rx::Renderer9 *renderer, std::size_t size, DWORD usageFlags);
    virtual ~VertexBuffer();

    void unmap();

    IDirect3DVertexBuffer9 *getBuffer() const;
    unsigned int getSerial() const;

  protected:
    rx::Renderer9 *const mRenderer;   // D3D9_REPLACE
    IDirect3DVertexBuffer9 *mVertexBuffer;

    unsigned int mSerial;
    static unsigned int issueSerial();
    static unsigned int mCurrentSerial;

private:
    DISALLOW_COPY_AND_ASSIGN(VertexBuffer);
};

class ArrayVertexBuffer : public VertexBuffer
{
  public:
    ArrayVertexBuffer(rx::Renderer9 *renderer, std::size_t size, DWORD usageFlags);
    ~ArrayVertexBuffer();

    std::size_t size() const { return mBufferSize; }
    virtual void *map(const gl::VertexAttribute &attribute, std::size_t requiredSpace, std::size_t *streamOffset) = 0;
    virtual void reserveRequiredSpace() = 0;
    void addRequiredSpace(UINT requiredSpace);

  protected:
    std::size_t mBufferSize;
    std::size_t mWritePosition;
    std::size_t mRequiredSpace;
};

class StreamingVertexBuffer : public ArrayVertexBuffer
{
  public:
    StreamingVertexBuffer(rx::Renderer9 *renderer, std::size_t initialSize);
    ~StreamingVertexBuffer();

    void *map(const gl::VertexAttribute &attribute, std::size_t requiredSpace, std::size_t *streamOffset);
    void reserveRequiredSpace();
};

class StaticVertexBuffer : public ArrayVertexBuffer
{
  public:
    explicit StaticVertexBuffer(rx::Renderer9 *renderer);
    ~StaticVertexBuffer();

    void *map(const gl::VertexAttribute &attribute, std::size_t requiredSpace, std::size_t *streamOffset);
    void reserveRequiredSpace();

    std::size_t lookupAttribute(const gl::VertexAttribute &attribute);   // Returns the offset into the vertex buffer, or -1 if not found

  private:
    struct VertexElement
    {
        GLenum type;
        GLint size;
        GLsizei stride;
        bool normalized;
        int attributeOffset;

        std::size_t streamOffset;
    };

    std::vector<VertexElement> mCache;
};

}

#endif // LIBGLESV2_RENDERER_VERTEXBUFFER_H_