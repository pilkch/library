#ifndef CRENDERER_H
#define CRENDERER_H

#include <breathe/gui/cManager.h>

// Render each widget to a layer
//  - Each layer has a fbo which we render the widget to
//  - Each layer takes on the properties of the widget
//  - Do not have to rebuild the layer if these properties change

namespace breathe
{
  namespace gui
  {
    class cRenderer
    {
    public:
      explicit cRenderer(const cManager& manager, opengl::cSystem& _system, opengl::cContext& context);
      ~cRenderer();

      void Update();
      void Render();

    private:
      void DeleteRenderables();

      void AddRect(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, const spitfire::math::cColour& colour);
      void AddRect(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, const spitfire::math::cColour& colour, float fU, float fV, float fU2, float fV2);

      void AddWindow(opengl::cGeometryBuilder_v2_c4_t2& builder, const cWindow& widget);
      void AddStaticText(opengl::cGeometryBuilder_v2_c4_t2& builder, const cStaticText& widget);
      void AddButton(opengl::cGeometryBuilder_v2_c4_t2& builder, const cButton& widget);
      void AddInput(opengl::cGeometryBuilder_v2_c4_t2& builder, const cInput& widget);
      void AddSlider(opengl::cGeometryBuilder_v2_c4_t2& builder, const cSlider& widget);

      void AddChildrenOfWidget(opengl::cGeometryBuilder_v2_c4_t2& builder, const cWidget& widget);

      void Visit(const cWidget& widget);

      const cManager& manager;
      opengl::cSystem& system;
      opengl::cContext& context;

      opengl::cTexture* pWidgetsTexture;
      opengl::cShader* pWidgetsShader;
      opengl::cFont* pFont;

      opengl::cTextureFrameBufferObject* pTextureFrameBufferObject;
      opengl::cShader* pGuiShader;
      opengl::cStaticVertexBufferObject* pVBO;
    };
  }
}

#endif // CRENDERER_H
