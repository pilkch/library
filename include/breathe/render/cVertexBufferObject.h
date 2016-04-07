#ifndef CVERTEXBUFFEROBJECT_H
#define CVERTEXBUFFEROBJECT_H

// libopenglmm headers
#include <libopenglmm/cVertexBufferObject.h>

namespace breathe
{
  namespace render
  {
    using opengl::cStaticVertexBufferObject;

    typedef cStaticVertexBufferObject cVertexBufferObject;
    typedef std::shared_ptr<cVertexBufferObject> cVertexBufferObjectRef;
    typedef std::shared_ptr<cVertexBufferObject> cStaticVertexBufferObjectRef;

    typedef cStaticVertexBufferObject cDynamicVertexBufferObject;
    typedef std::shared_ptr<cDynamicVertexBufferObject> cDynamicVertexBufferObjectRef;
  }
}

#endif // CVERTEXBUFFEROBJECT_H
