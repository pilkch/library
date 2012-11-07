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
    typedef cSmartPtr<cVertexBufferObject> cVertexBufferObjectRef;
    typedef cSmartPtr<cVertexBufferObject> cStaticVertexBufferObjectRef;

    typedef cStaticVertexBufferObject cDynamicVertexBufferObject;
    typedef cSmartPtr<cDynamicVertexBufferObject> cDynamicVertexBufferObjectRef;
  }
}

#endif // CVERTEXBUFFEROBJECT_H
