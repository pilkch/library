// Standard headers
#include <cassert>
#include <cmath>
#include <cstring>

#include <string>
#include <iostream>
#include <sstream>

#include <algorithm>
#include <map>
#include <vector>
#include <list>

// OpenGL headers
#include <GL/GLee.h>
#include <GL/glu.h>

// SDL headers
#include <SDL/SDL_image.h>

// libopenglmm headers
#include <libopenglmm/libopenglmm.h>
#include <libopenglmm/cContext.h>
#include <libopenglmm/cFont.h>
#include <libopenglmm/cGeometry.h>
#include <libopenglmm/cShader.h>
#include <libopenglmm/cSystem.h>
#include <libopenglmm/cTexture.h>
#include <libopenglmm/cVertexBufferObject.h>
#include <libopenglmm/cWindow.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cColour.h>

#include <spitfire/storage/filesystem.h>

// Breathe headers
#include <breathe/gui/cManager.h>
#include <breathe/gui/cRenderer.h>

namespace breathe
{
  namespace gui
  {
    // ** cRenderer

    cRenderer::cRenderer(const cManager& _manager, opengl::cSystem& _system, opengl::cContext& _context) :
      manager(_manager),
      system(_system),
      context(_context),
      pWidgetsTexture(nullptr),
      pWidgetsShader(nullptr),
      pTextureFrameBufferObject(nullptr),
      pGuiShader(nullptr),
      pVBO(nullptr)
    {
      pWidgetsTexture = context.CreateTexture(TEXT("data/textures/gui.png"));
      pWidgetsShader = context.CreateShader(TEXT("data/shaders/passthroughwithcolour.vert"), TEXT("data/shaders/passthroughwithcolour.frag"));

      pGuiShader = context.CreateShader(TEXT("data/shaders/passthroughalphamask.vert"), TEXT("data/shaders/passthroughalphamask.frag"));

      std::vector<float> vertices;
      //std::vector<float> normals;
      std::vector<float> textureCoordinates;
      std::vector<float> colours;
      //std::vector<uint16_t> indices;

      opengl::cGeometryBuilder_v2_c4_t2 builder(vertices, colours, textureCoordinates);

      // Texture coordinates
      // NOTE: The v coordinates have been swapped, the code looks correct but with normal v coordinates the gui is rednered upside down
      const float fU = 0.0f;
      const float fV = 1.0f;
      const float fU2 = 1.0f;
      const float fV2 = 0.0f;

      const float x = 0.0f;
      const float y = 0.0f;
      const size_t width = 1;
      const size_t height = 1;
      const float fWidth = float(width);
      const float fHeight = float(height);

      const spitfire::math::cColour colour(1.0f, 1.0f, 1.0f);

      // Front facing quad
      builder.PushBack(spitfire::math::cVec2(x, y + fHeight), colour, spitfire::math::cVec2(fU, fV2));
      builder.PushBack(spitfire::math::cVec2(x + fWidth, y + fHeight), colour, spitfire::math::cVec2(fU2, fV2));
      builder.PushBack(spitfire::math::cVec2(x + fWidth, y), colour, spitfire::math::cVec2(fU2, fV));
      builder.PushBack(spitfire::math::cVec2(x, y), colour, spitfire::math::cVec2(fU, fV));

      pVBO = context.CreateStaticVertexBufferObject();
      pVBO->SetVertices(vertices);
      //pVBO->SetNormals(normals);
      pVBO->SetTextureCoordinates(textureCoordinates);
      pVBO->SetColours(colours);
      //pVBO->SetIndices(indices);

      pVBO->Compile2D(system);

      pTextureFrameBufferObject = context.CreateTextureFrameBufferObject(1024, 1024, opengl::PIXELFORMAT::R8G8B8A8);
      ASSERT(pTextureFrameBufferObject != nullptr);
    }

    cRenderer::~cRenderer()
    {
      if (pWidgetsShader != nullptr) context.DestroyShader(pWidgetsShader);
      if (pWidgetsTexture != nullptr) context.DestroyTexture(pWidgetsTexture);

      if (pTextureFrameBufferObject != nullptr) context.DestroyTextureFrameBufferObject(pTextureFrameBufferObject);
      if (pGuiShader != nullptr) context.DestroyShader(pGuiShader);
      if (pVBO != nullptr) context.DestroyStaticVertexBufferObject(pVBO);
    }

    void cRenderer::AddRect(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, const spitfire::math::cColour& colour)
    {
      const float fU = 0.0f;
      const float fV = 0.0f;
      const float fU2 = 1.0f;
      const float fV2 = 1.0f;

      const float x = position.x;
      const float y = position.y;

      // Front facing quad
      builder.PushBack(spitfire::math::cVec2(x, y + fHeight), colour, spitfire::math::cVec2(fU, fV2));
      builder.PushBack(spitfire::math::cVec2(x + fWidth, y + fHeight), colour, spitfire::math::cVec2(fU2, fV2));
      builder.PushBack(spitfire::math::cVec2(x + fWidth, y), colour, spitfire::math::cVec2(fU2, fV));
      builder.PushBack(spitfire::math::cVec2(x, y), colour, spitfire::math::cVec2(fU, fV));
    }

    void cRenderer::AddRect(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, const spitfire::math::cColour& colour, float fU, float fV, float fU2, float fV2)
    {
      const float x = position.x;
      const float y = position.y;

      // Front facing quad
      builder.PushBack(spitfire::math::cVec2(x, y + fHeight), colour, spitfire::math::cVec2(fU, fV2));
      builder.PushBack(spitfire::math::cVec2(x + fWidth, y + fHeight), colour, spitfire::math::cVec2(fU2, fV2));
      builder.PushBack(spitfire::math::cVec2(x + fWidth, y), colour, spitfire::math::cVec2(fU2, fV));
      builder.PushBack(spitfire::math::cVec2(x, y), colour, spitfire::math::cVec2(fU, fV));
    }

    void cRenderer::AddWindow(opengl::cGeometryBuilder_v2_c4_t2& builder, const cWindow& widget)
    {
      const spitfire::math::cVec2 position = widget.GetAbsolutePosition();

      const spitfire::math::cColour colour(1.0f, 0.0f, 0.0f);

      AddRect(builder, position, widget.width, widget.height, colour);
    }

    void cRenderer::AddStaticText(opengl::cGeometryBuilder_v2_c4_t2& builder, const cStaticText& widget)
    {
      const spitfire::math::cVec2 position = widget.GetAbsolutePosition();

      const spitfire::math::cColour colour(1.0f, 0.0f, 1.0f);

      AddRect(builder, position, widget.width, widget.height, colour);
    }

    void cRenderer::AddButton(opengl::cGeometryBuilder_v2_c4_t2& builder, const cButton& widget)
    {
      const spitfire::math::cVec2 position = widget.GetAbsolutePosition();

      const spitfire::math::cColour colour(1.0f, 1.0f, 0.0f);

      AddRect(builder, position, widget.width, widget.height, colour);
    }

    void cRenderer::AddInput(opengl::cGeometryBuilder_v2_c4_t2& builder, const cInput& widget)
    {
      const spitfire::math::cVec2 position = widget.GetAbsolutePosition();

      const spitfire::math::cColour colour(1.0f, 0.0f, 0.0f);

      AddRect(builder, position, widget.width, widget.height, colour);
    }

    //void const cRenderer::AddImage(opengl::cGeometryBuilder_v2_c4_t2& builder, cImage& widget)
    //{
    //  const cImage& image = widget.GetImage();
    //  render image to texture
    //}

    //void const cRenderer::AddGraph(opengl::cGeometryBuilder_v2_c4_t2& builder, cGraph& widget)
    //{
    //  const std::array<float>& values = widget.GetValues();
    //  for each value
    //    render to texture
    //}

    void cRenderer::AddChildrenOfWidget(opengl::cGeometryBuilder_v2_c4_t2& builder, const cWidget& widget)
    {
      ASSERT(!widget.children.empty());

      const size_t n = widget.children.size();
      for (size_t i = 0; i < n; i++) {
        const cWidget& child = *(widget.children[i]);
        switch (child.GetType()) {
          case WIDGET_TYPE::WINDOW:
            AddWindow(builder, static_cast<const cWindow&>(child));
            break;

          case WIDGET_TYPE::STATICTEXT:
            AddStaticText(builder, static_cast<const cStaticText&>(child));
            break;

          case WIDGET_TYPE::BUTTON:
            AddButton(builder, static_cast<const cButton&>(child));
            break;

          case WIDGET_TYPE::INPUT:
            AddInput(builder, static_cast<const cInput&>(child));
            break;

          case WIDGET_TYPE::INVISIBLE_CONTAINER:
            // Purposely do not render this control
            break;

          default:
            std::cout<<"cRenderer::Visit UNKNOWN WIDGET TYPE "<<child.GetType()<<std::endl;
        };
      }
    }

    void cRenderer::Visit(const cWidget& widget)
    {
      ASSERT(!widget.children.empty());

      // For widgets that can group other widgets we need to render their children
      {
        std::vector<float> vertices;
        //std::vector<float> normals;
        std::vector<float> textureCoordinates;
        std::vector<float> colours;
        //std::vector<uint16_t> indices;

        opengl::cGeometryBuilder_v2_c4_t2 builder(vertices, colours, textureCoordinates);

        AddChildrenOfWidget(builder, widget);

        if (!vertices.empty()) {
          opengl::cStaticVertexBufferObject* pVBO = context.CreateStaticVertexBufferObject();
          pVBO->SetVertices(vertices);
          //pVBO->SetNormals(normals);
          pVBO->SetTextureCoordinates(textureCoordinates);
          pVBO->SetColours(colours);
          //pVBO->SetIndices(indices);

          pVBO->Compile2D(system);

          // Render the vbo to the frame buffer texture
          context.BindTexture(0, *pWidgetsTexture);

          context.BindShader(*pWidgetsShader);

          context.BindStaticVertexBufferObject2D(*pVBO);
          context.DrawStaticVertexBufferObjectQuads2D(*pVBO);
          context.UnBindStaticVertexBufferObject2D(*pVBO);

          context.UnBindShader(*pWidgetsShader);

          context.UnBindTexture(0, *pWidgetsTexture);

          context.DestroyStaticVertexBufferObject(pVBO);
        }
      }

      // Visit any children that contain children themselves
      const size_t n = widget.children.size();
      for (size_t i = 0; i < n; i++) {
        const cWidget& child = *(widget.children[i]);
        if (!child.children.empty()) Visit(child);
      }
    }

    void cRenderer::Update()
    {
      // Render the gui into the frame buffer object
      const spitfire::math::cColour clearColour(0.0f, 0.0f, 0.0f, 0.0f);
      context.SetClearColour(clearColour);

      context.BeginRenderToTexture(*pTextureFrameBufferObject);

      context.BeginRenderMode2D(opengl::MODE2D_TYPE::Y_INCREASES_DOWN_SCREEN);

      {
        const cWidget* pRoot = manager.GetRoot();
        assert(pRoot != nullptr);

        Visit(*pRoot);
      }

      context.EndRenderMode2D();

      context.EndRenderToTexture(*pTextureFrameBufferObject);
    }

    void cRenderer::Render()
    {
      // Render the gui to the screen
      context.BeginRenderMode2D(opengl::MODE2D_TYPE::Y_INCREASES_DOWN_SCREEN);

      {
        // Set the position of the layer
        spitfire::math::cMat4 matModelView;
        matModelView.SetTranslation(manager.GetHUDOffset().x, manager.GetHUDOffset().y, 0.0f);
        context.SetModelViewMatrix(matModelView);

        context.BindTexture(0, *pTextureFrameBufferObject);

        context.BindShader(*pGuiShader);

        context.BindStaticVertexBufferObject2D(*pVBO);
        context.DrawStaticVertexBufferObjectQuads2D(*pVBO);
        context.UnBindStaticVertexBufferObject2D(*pVBO);

        context.UnBindShader(*pGuiShader);

        context.UnBindTexture(0, *pTextureFrameBufferObject);
      }

      context.EndRenderMode2D();
    }
  }
}