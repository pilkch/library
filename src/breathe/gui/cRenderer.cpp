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

      pGuiShader = context.CreateShader(TEXT("data/shaders/passthroughwithcolour.vert"), TEXT("data/shaders/passthroughwithcolour.frag"));

      std::vector<float> vertices;
      //std::vector<float> normals;
      std::vector<float> textureCoordinates;
      std::vector<float> colours;
      //std::vector<uint16_t> indices;

      opengl::cGeometryBuilder_v2_c4_t2 builder(vertices, colours, textureCoordinates);

      // Texture coordinates
      // NOTE: The v coordinates have been swapped, the code looks correct but with normal v coordinates the gui is rendered upside down
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

      const spitfire::math::cColour colour(1.0f, 1.0f, 1.0f, 1.0f);

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

      pTextureFrameBufferObject = context.CreateTextureFrameBufferObjectNoMipMaps(1024, 1024, opengl::PIXELFORMAT::R8G8B8A8);
      ASSERT(pTextureFrameBufferObject != nullptr);

      // Create the font for rendering text
      pFont = context.CreateFont(TEXT("data/fonts/pricedown.ttf"), 32, TEXT("data/shaders/font.vert"), TEXT("data/shaders/font.frag"));
    }

    cRenderer::~cRenderer()
    {
      if (pFont != nullptr) context.DestroyFont(pFont);

      if (pWidgetsShader != nullptr) context.DestroyShader(pWidgetsShader);
      if (pWidgetsTexture != nullptr) context.DestroyTexture(pWidgetsTexture);

      if (pTextureFrameBufferObject != nullptr) context.DestroyTextureFrameBufferObject(pTextureFrameBufferObject);
      if (pGuiShader != nullptr) context.DestroyShader(pGuiShader);
      if (pVBO != nullptr) context.DestroyStaticVertexBufferObject(pVBO);
    }

    void cRenderer::AddRect(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, const spitfire::math::cColour& colour)
    {
      const render::cTextureCoordinatesRectangle textureCoordinates(0.0f, 0.0f, 0.0f, 0.0f);

      const float x = position.x;
      const float y = position.y;

      // Front facing quad
      builder.PushBack(spitfire::math::cVec2(x, y + fHeight), colour, textureCoordinates.textureCoordinates[0]);
      builder.PushBack(spitfire::math::cVec2(x + fWidth, y + fHeight), colour, textureCoordinates.textureCoordinates[1]);
      builder.PushBack(spitfire::math::cVec2(x + fWidth, y), colour, textureCoordinates.textureCoordinates[2]);
      builder.PushBack(spitfire::math::cVec2(x, y), colour, textureCoordinates.textureCoordinates[3]);
    }

    void cRenderer::AddRect(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, const spitfire::math::cColour& colour, const render::cTextureCoordinatesRectangle& textureCoordinates)
    {
      const float x = position.x;
      const float y = position.y;

      // Front facing quad
      builder.PushBack(spitfire::math::cVec2(x, y + fHeight), colour, textureCoordinates.textureCoordinates[0]);
      builder.PushBack(spitfire::math::cVec2(x + fWidth, y + fHeight), colour, textureCoordinates.textureCoordinates[1]);
      builder.PushBack(spitfire::math::cVec2(x + fWidth, y), colour, textureCoordinates.textureCoordinates[2]);
      builder.PushBack(spitfire::math::cVec2(x, y), colour, textureCoordinates.textureCoordinates[3]);
    }

    void cRenderer::AddArc(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& _position, float fRadius, const spitfire::math::cColour& colour, ORIENTATION orientation, const render::cTextureCoordinatesRectangle& textureCoordinates)
    {
      // Get the rotation for this orientation
      float fAngle = -90.0f;
      spitfire::math::cVec2 position = _position;
      if (orientation == ORIENTATION::TOP_LEFT) position += spitfire::math::cVec2(fRadius, fRadius);
      else if (orientation == ORIENTATION::TOP_RIGHT) {
        fAngle += 90.0f;
        position += spitfire::math::cVec2(0.0f, fRadius);
      } else if (orientation == ORIENTATION::BOTTOM_RIGHT) fAngle += 180.0f;
      else if (orientation == ORIENTATION::BOTTOM_LEFT) {
        fAngle += 270.0f;
        position += spitfire::math::cVec2(fRadius, 0.0f);
      }

      // Add the points for the arc
      size_t nDots = spitfire::math::clamp<size_t>((100.0f * fRadius), 4, 20);
      if (!spitfire::math::IsDivisibleByTwo(nDots)) nDots++;
      float fAngleBetweenDots = 90.0f / float(nDots);
      for (size_t iDots = 0; iDots < nDots; iDots += 2) {
        // Start with a point that points straight up
        const spitfire::math::cVec2 point(0.0f, -fRadius);

        // Rotate the point
        spitfire::math::cVec2 point0;
        point0.x = (point.x * cosf(spitfire::math::DegreesToRadians(fAngle))) - (point.y * sinf(spitfire::math::DegreesToRadians(fAngle)));
        point0.y = (point.y * cosf(spitfire::math::DegreesToRadians(fAngle))) + (point.x * sinf(spitfire::math::DegreesToRadians(fAngle)));

        // Increment our angle
        fAngle += fAngleBetweenDots;

        // Rotate the point
        spitfire::math::cVec2 point1;
        point1.x = (point.x * cosf(spitfire::math::DegreesToRadians(fAngle))) - (point.y * sinf(spitfire::math::DegreesToRadians(fAngle)));
        point1.y = (point.y * cosf(spitfire::math::DegreesToRadians(fAngle))) + (point.x * sinf(spitfire::math::DegreesToRadians(fAngle)));

        // Increment our angle
        fAngle += fAngleBetweenDots;

        // Rotate the point
        spitfire::math::cVec2 point2;
        point2.x = (point.x * cosf(spitfire::math::DegreesToRadians(fAngle))) - (point.y * sinf(spitfire::math::DegreesToRadians(fAngle)));
        point2.y = (point.y * cosf(spitfire::math::DegreesToRadians(fAngle))) + (point.x * sinf(spitfire::math::DegreesToRadians(fAngle)));

        // Add the points
        // Each segment is two triangles next to each other, so each iteration of the loop we add 4 points, 1 at the origin of the arc and the other 3 are along the outside of the arc
        // The first and last points along the arc in each iteration will be touching a point from the previous iteration and a point from the next iteration
        builder.PushBack(position + point2, colour, textureCoordinates.textureCoordinates[2]);
        builder.PushBack(position + point1, colour, textureCoordinates.textureCoordinates[2]);
        builder.PushBack(position + point0, colour, textureCoordinates.textureCoordinates[2]);
        builder.PushBack(position, colour, textureCoordinates.textureCoordinates[0]);
      }
    }

    void cRenderer::AddArc(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& position, float fRadius, const spitfire::math::cColour& colour, ORIENTATION orientation)
    {
      const render::cTextureCoordinatesRectangle textureCoordinates(0.0f, 0.0f, 0.0f, 0.0f);
      AddArc(builder, position, fRadius, colour, orientation, textureCoordinates);
    }

    void cRenderer::AddCapsule(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, float fRadius, const spitfire::math::cColour& colour)
    {
      const float x = position.x;
      const float y = position.y;

      // Add a cross for the rectangular parts of the window
      AddRect(builder, spitfire::math::cVec2(x + fRadius, y), fWidth - (2.0f * fRadius), fRadius, colour);
      AddRect(builder, spitfire::math::cVec2(x, y + fRadius), fWidth, fHeight - (2.0f * fRadius), colour);
      AddRect(builder, spitfire::math::cVec2(x + fRadius, y + fHeight - fRadius), fWidth - (2.0f * fRadius), fRadius, colour);

      // Add an arc in each corner
      AddArc(builder, spitfire::math::cVec2(x, y), fRadius, colour, ORIENTATION::TOP_LEFT);
      AddArc(builder, spitfire::math::cVec2(x + fWidth - fRadius, y), fRadius, colour, ORIENTATION::TOP_RIGHT);
      AddArc(builder, spitfire::math::cVec2(x, y + fHeight - fRadius), fRadius, colour, ORIENTATION::BOTTOM_LEFT);
      AddArc(builder, spitfire::math::cVec2(x + fWidth - fRadius, y + fHeight - fRadius), fRadius, colour, ORIENTATION::BOTTOM_RIGHT);
    }

    void cRenderer::AddDropShadow(opengl::cGeometryBuilder_v2_c4_t2& builder, const spitfire::math::cVec2& position, float fWidth, float fHeight, float fRadius, const spitfire::math::cColour& colour)
    {
      {
        // Add the rectangles for the sides, top and bottom
        render::cTextureCoordinatesRectangle textureCoordinates(0.5f, 0.5f, 0.5f, 1.0f);
        AddRect(builder, position - spitfire::math::cVec2(0.0f, fRadius), fWidth, fRadius, colour, textureCoordinates);
        textureCoordinates.Rotate90DegreesClockWise();
        AddRect(builder, position + spitfire::math::cVec2(fWidth, 0.0f), fRadius, fHeight, colour, textureCoordinates);
        textureCoordinates.Rotate90DegreesClockWise();
        AddRect(builder, position + spitfire::math::cVec2(0.0f, fHeight), fWidth, fRadius, colour, textureCoordinates);
        textureCoordinates.Rotate90DegreesClockWise();
        AddRect(builder, position - spitfire::math::cVec2(fRadius, 0.0f), fRadius, fHeight, colour, textureCoordinates);
      }

      {
        // Add an arc in each corner
        const render::cTextureCoordinatesRectangle textureCoordinates(0.5f, 0.51f, 0.5f, 1.0f); // NOTE: Our U coordinate is pulled slightly into the shadow part of the texture to prevent bleeding from the solid part of the texture
        AddArc(builder, position + spitfire::math::cVec2(- fRadius, -fRadius), fRadius, colour, ORIENTATION::TOP_LEFT, textureCoordinates);
        AddArc(builder, position + spitfire::math::cVec2(fWidth, -fRadius), fRadius, colour, ORIENTATION::TOP_RIGHT, textureCoordinates);
        AddArc(builder, position + spitfire::math::cVec2(-fRadius, fHeight), fRadius, colour, ORIENTATION::BOTTOM_LEFT, textureCoordinates);
        AddArc(builder, position + spitfire::math::cVec2(fWidth, fHeight), fRadius, colour, ORIENTATION::BOTTOM_RIGHT, textureCoordinates);
      }
    }

    void cRenderer::AddWindow(opengl::cGeometryBuilder_v2_c4_t2& builder, opengl::cGeometryBuilder_v2_c4_t2& builderText, const cWindow& window)
    {
      const spitfire::math::cVec2 position = window.GetAbsolutePosition();

      const spitfire::math::cColour colour(manager.GetColourWindow());

      const float x = position.x;
      const float y = position.y;
      const float fRadius = 0.02f;

      // Create the drop shadow for this window
      //AddDropShadow(builder, position + spitfire::math::cVec2(fRadius, fRadius), window.width - (2.0f * fRadius), window.height - (2.0f * fRadius), 2.0f * fRadius, spitfire::math::cColour(0.0f, 0.0f, 0.0f));

      // Create the geometry for this window
      AddCapsule(builder, position, window.width, window.height, fRadius, colour);


      // Create the text for this window
      pFont->PushBack(builderText, window.sCaption, window.GetTextColour(), spitfire::math::cVec2(x + ((window.width - manager.GetTextWidth(window.sCaption)) * 0.5f), y + manager.GetTextHeight() + 0.005f));
    }

    void cRenderer::AddStaticText(opengl::cGeometryBuilder_v2_c4_t2& builder, opengl::cGeometryBuilder_v2_c4_t2& builderText, const cStaticText& widget)
    {
      const spitfire::math::cVec2 position = widget.GetAbsolutePosition();

      // Create the text for this widget
      pFont->PushBack(builderText, widget.sCaption, widget.GetTextColour(), spitfire::math::cVec2(position.x + 0.01f, position.y + manager.GetTextHeight() + 0.005f));
    }

    void cRenderer::AddButton(opengl::cGeometryBuilder_v2_c4_t2& builder, opengl::cGeometryBuilder_v2_c4_t2& builderText, const cButton& widget)
    {
      const spitfire::math::cVec2 position = widget.GetAbsolutePosition();

      const spitfire::math::cColour colour(manager.GetColourWidget());

      const float fRadius = 0.02f;

      AddCapsule(builder, position, widget.width, widget.height, fRadius, colour);


      // Create the text for this widget
      pFont->PushBack(builderText, widget.sCaption, widget.GetTextColour(), spitfire::math::cVec2(position.x + ((widget.width - manager.GetTextWidth(widget.sCaption)) * 0.5f), position.y + manager.GetTextHeight() + 0.005f));
    }

    void cRenderer::AddInput(opengl::cGeometryBuilder_v2_c4_t2& builder, opengl::cGeometryBuilder_v2_c4_t2& builderText, const cInput& widget)
    {
      const spitfire::math::cVec2 position = widget.GetAbsolutePosition();

      const spitfire::math::cColour colour(manager.GetColourWidget());

      AddRect(builder, position, widget.width, widget.height, colour);


      // Create the text for this widget
      pFont->PushBack(builderText, widget.sCaption, widget.GetTextColour(), spitfire::math::cVec2(position.x + 0.01f, position.y + manager.GetTextHeight() + 0.005f));
    }

    void cRenderer::AddSlider(opengl::cGeometryBuilder_v2_c4_t2& builder, const cSlider& widget)
    {
      const spitfire::math::cVec2 position = widget.GetAbsolutePosition();

      const spitfire::math::cColour colour(manager.GetColourWidget());

      AddRect(builder, position, widget.width, widget.height, colour);
    }

    void cRenderer::AddRetroButton(opengl::cGeometryBuilder_v2_c4_t2& builder, opengl::cGeometryBuilder_v2_c4_t2& builderText, const cRetroButton& widget)
    {
      const spitfire::math::cVec2 position = widget.GetAbsolutePosition();

      // Create the text for this widget
      pFont->PushBack(builderText, widget.sCaption, widget.GetTextColour(), spitfire::math::cVec2(position.x + 0.01f, position.y + manager.GetTextHeight() + 0.005f));
    }

    void cRenderer::AddRetroInput(opengl::cGeometryBuilder_v2_c4_t2& builder, opengl::cGeometryBuilder_v2_c4_t2& builderText, const cRetroInput& widget)
    {
      const spitfire::math::cVec2 position = widget.GetAbsolutePosition();

      // Create the text for this widget
      pFont->PushBack(builderText, widget.sCaption, widget.GetTextColour(), spitfire::math::cVec2(position.x + 0.01f, position.y + manager.GetTextHeight() + 0.005f));
    }

    //void const cRenderer::AddImage(opengl::cGeometryBuilder_v2_c4_t2& builder, voodoo::cImage& widget)
    //{
    //  const voodoo::cImage& image = widget.GetImage();
    //  render image to texture
    //}

    //void const cRenderer::AddGraph(opengl::cGeometryBuilder_v2_c4_t2& builder, cGraph& widget)
    //{
    //  const std::array<float>& values = widget.GetValues();
    //  for each value
    //    render to texture
    //}

    void cRenderer::AddChildrenOfWidget(const cWidget& widget)
    {
      ASSERT(!widget.children.empty());

      const size_t n = widget.children.size();
      for (size_t i = 0; i < n; i++) {
        const cWidget& child = *(widget.children[i]);

        // If this child is not visible then we don't need to render it
        if (!child.IsVisible()) continue;

        std::vector<float> vertices;
        //std::vector<float> normals;
        std::vector<float> textureCoordinates;
        std::vector<float> colours;
        //std::vector<uint16_t> indices;

        opengl::cGeometryBuilder_v2_c4_t2 builder(vertices, colours, textureCoordinates);

        // Text
        assert(pFont != nullptr);
        assert(pFont->IsValid());

        std::vector<float> verticesText;
        std::vector<float> coloursText;
        std::vector<float> textureCoordinatesText;

        opengl::cGeometryBuilder_v2_c4_t2 builderText(verticesText, coloursText, textureCoordinatesText);

        switch (child.GetType()) {
          case WIDGET_TYPE::WINDOW:
            AddWindow(builder, builderText, static_cast<const cWindow&>(child));
            break;

          case WIDGET_TYPE::STATICTEXT:
            AddStaticText(builder, builderText, static_cast<const cStaticText&>(child));
            break;

          case WIDGET_TYPE::BUTTON:
            AddButton(builder, builderText, static_cast<const cButton&>(child));
            break;

          case WIDGET_TYPE::INPUT:
            AddInput(builder, builderText, static_cast<const cInput&>(child));
            break;

          case WIDGET_TYPE::SLIDER:
            AddSlider(builder, static_cast<const cSlider&>(child));
            break;

          case WIDGET_TYPE::RETRO_BUTTON:
            AddRetroButton(builder, builderText, static_cast<const cRetroButton&>(child));
            break;

          case WIDGET_TYPE::RETRO_INPUT:
            AddRetroInput(builder, builderText, static_cast<const cRetroInput&>(child));
            break;

          case WIDGET_TYPE::INVISIBLE_CONTAINER:
            // Purposely do not render this control
            break;

          default:
            std::cout<<"cRenderer::Visit UNKNOWN WIDGET TYPE "<<child.GetType()<<std::endl;
        };

        if (!vertices.empty()) {
          opengl::cStaticVertexBufferObject* pVBO = context.CreateStaticVertexBufferObject();
          pVBO->SetVertices(vertices);
          //pVBO->SetNormals(normals);
          pVBO->SetTextureCoordinates(textureCoordinates);
          pVBO->SetColours(colours);
          //pVBO->SetIndices(indices);

          pVBO->Compile2D(system);

          context.EnableBlending();

          // Render the vbo to the frame buffer texture
          context.BindTexture(0, *pWidgetsTexture);

          context.BindShader(*pWidgetsShader);

          context.BindStaticVertexBufferObject2D(*pVBO);
          context.DrawStaticVertexBufferObjectQuads2D(*pVBO);
          context.UnBindStaticVertexBufferObject2D(*pVBO);

          context.UnBindShader(*pWidgetsShader);

          context.UnBindTexture(0, *pWidgetsTexture);

          context.DisableBlending();

          context.DestroyStaticVertexBufferObject(pVBO);
        }

        if (!verticesText.empty()) {
          opengl::cStaticVertexBufferObject* pVBOText = context.CreateStaticVertexBufferObject();
          pVBOText->SetVertices(verticesText);
          pVBOText->SetColours(coloursText);
          pVBOText->SetTextureCoordinates(textureCoordinatesText);

          pVBOText->Compile2D(system);

          context.BindFont(*pFont);

          context.BindStaticVertexBufferObject2D(*pVBOText);
          context.DrawStaticVertexBufferObjectQuads2D(*pVBOText);
          context.UnBindStaticVertexBufferObject2D(*pVBOText);

          context.UnBindFont(*pFont);

          context.DestroyStaticVertexBufferObject(pVBOText);
        }
      }
    }

    void cRenderer::Visit(const cWidget& widget)
    {
      ASSERT(!widget.children.empty());

      // If this widget is not visible then we don't need to render it
      if (!widget.IsVisible()) return;

      // For widgets that can group other widgets we need to render their children
      AddChildrenOfWidget(widget);

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

        context.EnableBlending();

        context.BindTexture(0, *pTextureFrameBufferObject);

        context.BindShader(*pGuiShader);

        context.BindStaticVertexBufferObject2D(*pVBO);
        context.DrawStaticVertexBufferObjectQuads2D(*pVBO);
        context.UnBindStaticVertexBufferObject2D(*pVBO);

        context.UnBindShader(*pGuiShader);

        context.UnBindTexture(0, *pTextureFrameBufferObject);

        context.DisableBlending();
      }

      context.EndRenderMode2D();
    }
  }
}