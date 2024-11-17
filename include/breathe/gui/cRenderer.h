#ifndef CRENDERER_H
#define CRENDERER_H

// libopenglmm headers
#include <libopenglmm/cFont.h>
#include <libopenglmm/cTexture.h>
#include <libopenglmm/cVertexBufferObject.h>

// Breathe headers
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
      explicit cRenderer(const cManager& manager, opengl::cContext& context);
      ~cRenderer();

      bool LoadResources(size_t resolutionWidth, size_t resolutionHeight);
      void DestroyResources();

      void SetWireFrame(bool bWireFrame);

      void Update();
      void Render();

    private:
      enum class ORIENTATION {
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT
      };

      void AddRect(opengl::cGeometryBuilder_v2_t2_c4& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, const spitfire::math::cColour& colour);
      void AddRect(opengl::cGeometryBuilder_v2_t2_c4& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, const spitfire::math::cColour& colour, const render::cTextureCoordinatesRectangle& textureCoordinates);
      void AddArc(opengl::cGeometryBuilder_v2_t2_c4& builder, const spitfire::math::cVec2& position, float fRadius, const spitfire::math::cColour& colour, ORIENTATION orientation);
      void AddArc(opengl::cGeometryBuilder_v2_t2_c4& builder, const spitfire::math::cVec2& position, float fRadius, const spitfire::math::cColour& colour, ORIENTATION orientation, const render::cTextureCoordinatesRectangle& textureCoordinates);
      void AddCapsule(opengl::cGeometryBuilder_v2_t2_c4& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, float fRadius, const spitfire::math::cColour& colour);

      void AddDropShadow(opengl::cGeometryBuilder_v2_t2_c4& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, float fRadius, const spitfire::math::cColour& colour);

      void AddWindow(opengl::cGeometryBuilder_v2_t2_c4& builder, opengl::cGeometryBuilder_v2_t2_c4& builderText, const cWindow& widget);
      void AddStaticText(opengl::cGeometryBuilder_v2_t2_c4& builder, opengl::cGeometryBuilder_v2_t2_c4& builderText, const cStaticText& widget);
      void AddButton(opengl::cGeometryBuilder_v2_t2_c4& builder, opengl::cGeometryBuilder_v2_t2_c4& builderText, const cButton& widget);
      void AddInput(opengl::cGeometryBuilder_v2_t2_c4& builder, opengl::cGeometryBuilder_v2_t2_c4& builderText, const cInput& widget);
      void AddComboBox(opengl::cGeometryBuilder_v2_t2_c4& builder, opengl::cGeometryBuilder_v2_t2_c4& builderText, const cComboBox& widget);
      void AddSlider(opengl::cGeometryBuilder_v2_t2_c4& builder, const cSlider& widget);
      void AddToolbar(opengl::cGeometryBuilder_v2_t2_c4& builder, const cToolbar& widget);
      void AddScrollbar(opengl::cGeometryBuilder_v2_t2_c4& builder, const cScrollbar& widget);

      void AddRetroButton(opengl::cGeometryBuilder_v2_t2_c4& builder, opengl::cGeometryBuilder_v2_t2_c4& builderText, const cRetroButton& widget);
      void AddRetroInput(opengl::cGeometryBuilder_v2_t2_c4& builder, opengl::cGeometryBuilder_v2_t2_c4& builderText, const cRetroInput& widget);
      void AddRetroInputUpDown(opengl::cGeometryBuilder_v2_t2_c4& builder, opengl::cGeometryBuilder_v2_t2_c4& builderText, const cRetroInputUpDown& widget);
      void AddRetroColourPicker(opengl::cGeometryBuilder_v2_t2_c4& builder, opengl::cGeometryBuilder_v2_t2_c4& builderText, const cRetroColourPicker& widget);

      void AddChildrenOfWidget(const cWidget& widget);

      void Visit(const cWidget& widget);

      const cManager& manager;
      opengl::cContext& context;

      bool bWireFrame;

      opengl::cTexture widgetsTexture;
      opengl::cShader widgetsShader;
      opengl::cFont font;

      opengl::cTextureFrameBufferObject textureFrameBufferObject;
      opengl::cShader guiShader;
      opengl::cStaticVertexBufferObject vbo;

      size_t resolutionWidth;
      size_t resolutionHeight;
    };
  }
}

#endif // CRENDERER_H
