#ifndef CRENDERER_H
#define CRENDERER_H

#include <breathe/gui/cManager.h>
#include <breathe/render/cTextureAtlas.h>

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

      void SetWireFrame(bool bWireFrame);

      void Update();
      void Render();

    private:
      void DeleteRenderables();

      enum class ORIENTATION {
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT
      };

      void AddRect(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, const spitfire::math::cColour& colour);
      void AddRect(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, const spitfire::math::cColour& colour, const render::cTextureCoordinatesRectangle& textureCoordinates);
      void AddArc(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& position, float fRadius, const spitfire::math::cColour& colour, ORIENTATION orientation);
      void AddArc(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& position, float fRadius, const spitfire::math::cColour& colour, ORIENTATION orientation, const render::cTextureCoordinatesRectangle& textureCoordinates);
      void AddCapsule(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, float fRadius, const spitfire::math::cColour& colour);

      void AddDropShadow(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, float fRadius, const spitfire::math::cColour& colour);

      void AddWindow(opengl::cGeometryBuilder_v2_c4_t2& builder, opengl::cGeometryBuilder_v2_c4_t2& builderText, const cWindow& widget);
      void AddStaticText(opengl::cGeometryBuilder_v2_c4_t2& builder, opengl::cGeometryBuilder_v2_c4_t2& builderText, const cStaticText& widget);
      void AddButton(opengl::cGeometryBuilder_v2_c4_t2& builder, opengl::cGeometryBuilder_v2_c4_t2& builderText, const cButton& widget);
      void AddInput(opengl::cGeometryBuilder_v2_c4_t2& builder, opengl::cGeometryBuilder_v2_c4_t2& builderText, const cInput& widget);
      void AddSlider(opengl::cGeometryBuilder_v2_c4_t2& builder, const cSlider& widget);

      void AddRetroButton(opengl::cGeometryBuilder_v2_c4_t2& builder, opengl::cGeometryBuilder_v2_c4_t2& builderText, const cRetroButton& widget);
      void AddRetroInput(opengl::cGeometryBuilder_v2_c4_t2& builder, opengl::cGeometryBuilder_v2_c4_t2& builderText, const cRetroInput& widget);

      void AddChildrenOfWidget(const cWidget& widget);

      void Visit(const cWidget& widget);

      const cManager& manager;
      opengl::cSystem& system;
      opengl::cContext& context;

      bool bWireFrame;

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
