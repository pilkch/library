// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cmath>

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

// libopenglmm headers
#include <libopenglmm/cGeometry.h>

namespace opengl
{
  // Templated functions to take any builder
  template <class T>
  void CreatePlaneTemplated(T& builder, float fWidth, float fDepth)
  {
    const spitfire::math::cVec2 vMin(-fWidth * 0.5f, -fDepth * 0.5f);
    const spitfire::math::cVec2 vMax(fWidth * 0.5f, fDepth * 0.5f);

    // Upper Square
    builder.PushBack(spitfire::math::cVec3(vMin.x, vMin.y, 0.0f), spitfire::math::cVec3(0.0f, 0.0f, 1.0f), spitfire::math::cVec2(0.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec3(vMax.x, vMin.y, 0.0f), spitfire::math::cVec3(0.0f, 0.0f, 1.0f), spitfire::math::cVec2(1.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec3(vMax.x, vMax.y, 0.0f), spitfire::math::cVec3(0.0f, 0.0f, 1.0f), spitfire::math::cVec2(1.0f, 1.0f));
    builder.PushBack(spitfire::math::cVec3(vMin.x, vMax.y, 0.0f), spitfire::math::cVec3(0.0f, 0.0f, 1.0f), spitfire::math::cVec2(0.0f, 1.0f));
  }

  template <class T>
  void CreateBoxTemplated(T& builder, float fWidth, float fDepth, float fHeight)
  {
    const spitfire::math::cVec3 vMin(-fWidth * 0.5f, -fDepth * 0.5f, -fHeight * 0.5f);
    const spitfire::math::cVec3 vMax(fWidth * 0.5f, fDepth * 0.5f, fHeight * 0.5f);

    // Upper Square
    builder.PushBack(spitfire::math::cVec3(vMin.x, vMin.y, vMax.z), spitfire::math::cVec3(0.0f, 0.0f, 1.0f), spitfire::math::cVec2(0.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec3(vMax.x, vMin.y, vMax.z), spitfire::math::cVec3(0.0f, 0.0f, 1.0f), spitfire::math::cVec2(1.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec3(vMax.x, vMax.y, vMax.z), spitfire::math::cVec3(0.0f, 0.0f, 1.0f), spitfire::math::cVec2(1.0f, 1.0f));
    builder.PushBack(spitfire::math::cVec3(vMin.x, vMax.y, vMax.z), spitfire::math::cVec3(0.0f, 0.0f, 1.0f), spitfire::math::cVec2(0.0f, 1.0f));

    // Bottom Square
    builder.PushBack(spitfire::math::cVec3(vMin.x, vMin.y, vMin.z), spitfire::math::cVec3(0.0f, 0.0f, -1.0f), spitfire::math::cVec2(0.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec3(vMin.x, vMax.y, vMin.z), spitfire::math::cVec3(0.0f, 0.0f, -1.0f), spitfire::math::cVec2(1.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec3(vMax.x, vMax.y, vMin.z), spitfire::math::cVec3(0.0f, 0.0f, -1.0f), spitfire::math::cVec2(1.0f, 1.0f));
    builder.PushBack(spitfire::math::cVec3(vMax.x, vMin.y, vMin.z), spitfire::math::cVec3(0.0f, 0.0f, -1.0f), spitfire::math::cVec2(0.0f, 1.0f));

    // Side Squares
    builder.PushBack(spitfire::math::cVec3(vMin.x, vMax.y, vMin.z), spitfire::math::cVec3(0.0f, 1.0f, 0.0f), spitfire::math::cVec2(0.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec3(vMin.x, vMax.y, vMax.z), spitfire::math::cVec3(0.0f, 1.0f, 0.0f), spitfire::math::cVec2(1.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec3(vMax.x, vMax.y, vMax.z), spitfire::math::cVec3(0.0f, 1.0f, 0.0f), spitfire::math::cVec2(1.0f, 1.0f));
    builder.PushBack(spitfire::math::cVec3(vMax.x, vMax.y, vMin.z), spitfire::math::cVec3(0.0f, 1.0f, 0.0f), spitfire::math::cVec2(0.0f, 1.0f));

    builder.PushBack(spitfire::math::cVec3(vMax.x, vMax.y, vMin.z), spitfire::math::cVec3(1.0f, 0.0f, 0.0f), spitfire::math::cVec2(0.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec3(vMax.x, vMax.y, vMax.z), spitfire::math::cVec3(1.0f, 0.0f, 0.0f), spitfire::math::cVec2(1.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec3(vMax.x, vMin.y, vMax.z), spitfire::math::cVec3(1.0f, 0.0f, 0.0f), spitfire::math::cVec2(1.0f, 1.0f));
    builder.PushBack(spitfire::math::cVec3(vMax.x, vMin.y, vMin.z), spitfire::math::cVec3(1.0f, 0.0f, 0.0f), spitfire::math::cVec2(0.0f, 1.0f));

    builder.PushBack(spitfire::math::cVec3(vMax.x, vMin.y, vMin.z), spitfire::math::cVec3(0.0f, -1.0f, 0.0f), spitfire::math::cVec2(0.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec3(vMax.x, vMin.y, vMax.z), spitfire::math::cVec3(0.0f, -1.0f, 0.0f), spitfire::math::cVec2(1.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec3(vMin.x, vMin.y, vMax.z), spitfire::math::cVec3(0.0f, -1.0f, 0.0f), spitfire::math::cVec2(1.0f, 1.0f));
    builder.PushBack(spitfire::math::cVec3(vMin.x, vMin.y, vMin.z), spitfire::math::cVec3(0.0f, -1.0f, 0.0f), spitfire::math::cVec2(0.0f, 1.0f));

    builder.PushBack(spitfire::math::cVec3(vMin.x, vMin.y, vMin.z), spitfire::math::cVec3(-1.0f, 0.0f, 1.0f), spitfire::math::cVec2(0.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec3(vMin.x, vMin.y, vMax.z), spitfire::math::cVec3(-1.0f, 0.0f, 1.0f), spitfire::math::cVec2(1.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec3(vMin.x, vMax.y, vMax.z), spitfire::math::cVec3(-1.0f, 0.0f, 1.0f), spitfire::math::cVec2(1.0f, 1.0f));
    builder.PushBack(spitfire::math::cVec3(vMin.x, vMax.y, vMin.z), spitfire::math::cVec3(-1.0f, 0.0f, 1.0f), spitfire::math::cVec2(0.0f, 1.0f));
  }



  void cGeometryBuilder::CreatePlane(float fWidth, float fDepth, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& textureCoords, size_t nTextureUnits)
  {
    if (nTextureUnits == 0) {
      cGeometryBuilder_v3_n3 builder(vertices, normals);
      CreatePlaneTemplated(builder, fWidth, fDepth);
    } else if (nTextureUnits == 1) {
      cGeometryBuilder_v3_n3_t2 builder(vertices, normals, textureCoords);
      CreatePlaneTemplated(builder, fWidth, fDepth);
    } else if (nTextureUnits == 2) {
      cGeometryBuilder_v3_n3_t2_t2 builder(vertices, normals, textureCoords);
      CreatePlaneTemplated(builder, fWidth, fDepth);
    } else if (nTextureUnits == 3) {
      cGeometryBuilder_v3_n3_t2_t2_t2 builder(vertices, normals, textureCoords);
      CreatePlaneTemplated(builder, fWidth, fDepth);
    } else {
      std::cout<<"cGeometryBuilder::CreatePlane Invalid nTextureUnits "<<nTextureUnits<<std::endl;
      assert(false);
    }
  }

  void cGeometryBuilder::CreateCube(float fWidthAndDepthAndHeight, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& textureCoords, size_t nTextureUnits)
  {
    CreateBox(fWidthAndDepthAndHeight, fWidthAndDepthAndHeight, fWidthAndDepthAndHeight, vertices, normals, textureCoords, nTextureUnits);
  }

  void cGeometryBuilder::CreateBox(float fWidth, float fDepth, float fHeight, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& textureCoords, size_t nTextureUnits)
  {
    if (nTextureUnits == 0) {
      cGeometryBuilder_v3_n3 builder(vertices, normals);
      CreateBoxTemplated(builder, fWidth, fDepth, fHeight);
    } else if (nTextureUnits == 1) {
      cGeometryBuilder_v3_n3_t2 builder(vertices, normals, textureCoords);
      CreateBoxTemplated(builder, fWidth, fDepth, fHeight);
    } else if (nTextureUnits == 2) {
      cGeometryBuilder_v3_n3_t2_t2 builder(vertices, normals, textureCoords);
      CreateBoxTemplated(builder, fWidth, fDepth, fHeight);
    } else if (nTextureUnits == 3) {
      cGeometryBuilder_v3_n3_t2_t2_t2 builder(vertices, normals, textureCoords);
      CreateBoxTemplated(builder, fWidth, fDepth, fHeight);
    } else {
      std::cout<<"cGeometryBuilder::CreateBox Invalid nTextureUnits "<<nTextureUnits<<std::endl;
      assert(false);
    }
  }

  void cGeometryBuilder::CreateSphere(float fRadius, size_t nSegments, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& textureCoords, size_t nTextureUnits)
  {
    std::cout<<"cGeometryBuilder::CreateSphere Invalid nTextureUnits "<<nTextureUnits<<std::endl;
    assert(false);
  }

  void cGeometryBuilder::CreateTeapot(float fRadius, size_t nSegments, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& textureCoords, size_t nTextureUnits)
  {
    std::cout<<"cGeometryBuilder::CreateTeapot Invalid nTextureUnits "<<nTextureUnits<<std::endl;
    assert(false);
  }

  void cGeometryBuilder::CreateGear(float fInnerRadius, float fOuterRadius, float fWidth, size_t nTeeth, float fToothDepth, std::vector<float>& vertices, std::vector<float>& normals)
  {
    const float r0 = fInnerRadius;
    const float r1 = fOuterRadius - fToothDepth / 2.0f;
    const float r2 = fOuterRadius + fToothDepth / 2.0f;

    const float da = 2.0f * spitfire::math::cPI / float(nTeeth) / 4.0f;

    opengl::cGeometryBuilder_v3_n3 builder(vertices, normals);

    {
      const spitfire::math::cVec3 normal(0.0f, 0.0f, 1.0f);

      // Draw front face
      for (size_t i = 0; i < nTeeth + 1; i++) {
        const float angle = float(i) * 2.0f * spitfire::math::cPI / float(nTeeth);

        // Wide quad
        builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle), r0 * sinf(angle), fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle), r1 * sinf(angle), fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle + 3.0f * da), r1 * sinf(angle + 3.0f * da), fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle + 3.0f * da), r0 * sinf(angle + 3.0f * da), fWidth * 0.5f), normal);

        // Thin quad
        builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle + 3.0f * da), r0 * sinf(angle + 3.0f * da), fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle + 3.0f * da), r1 * sinf(angle + 3.0f * da), fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle + 4.0f * da), r1 * sinf(angle + 4.0f * da), fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle + 4.0f * da), r0 * sinf(angle + 4.0f * da), fWidth * 0.5f), normal);
      }

      // Draw front sides of teeth
      for (size_t i = 0; i < nTeeth; i++) {
        const float angle = float(i) * 2.0f * spitfire::math::cPI / float(nTeeth);

        builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle), r1 * sinf(angle), fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r2 * cosf(angle + da), r2 * sinf(angle + da), fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r2 * cosf(angle + 2.0f * da), r2 * sinf(angle + 2.0f * da), fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle + 3.0f * da), r1 * sinf(angle + 3.0f * da), fWidth * 0.5f), normal);
      }
    }

    {
      const spitfire::math::cVec3 normal(0.0, 0.0, -1.0f);

      // Draw front face
      for (size_t i = 0; i < nTeeth + 1; i++) {
        const float angle = float(i) * 2.0f * spitfire::math::cPI / float(nTeeth);

        // Wide quad
        builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle), r1 * sinf(angle), -fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle), r0 * sinf(angle), -fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle + 3.0f * da), r0 * sinf(angle + 3.0f * da), -fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle + 3.0f * da), r1 * sinf(angle + 3.0f * da), -fWidth * 0.5f), normal);

        // Thin quad
        builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle + 3.0f * da), r1 * sinf(angle + 3.0f * da), -fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle + 3.0f * da), r0 * sinf(angle + 3.0f * da), -fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle + 4.0f * da), r0 * sinf(angle + 4.0f * da), -fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle + 4.0f * da), r1 * sinf(angle + 4.0f * da), -fWidth * 0.5f), normal);
      }

      // Draw front sides of teeth
      for (size_t i = 0; i < nTeeth; i++) {
        const float angle = float(i) * 2.0f * spitfire::math::cPI / float(nTeeth);

        builder.PushBack(spitfire::math::cVec3(r2 * cosf(angle + da), r2 * sinf(angle + da), fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle), r1 * sinf(angle), fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle + 3.0f * da), r1 * sinf(angle + 3.0f * da), fWidth * 0.5f), normal);
        builder.PushBack(spitfire::math::cVec3(r2 * cosf(angle + 2.0f * da), r2 * sinf(angle + 2.0f * da), fWidth * 0.5f), normal);
      }
    }


    // Draw inside radius cylinder
    for (size_t i = 0; i < nTeeth + 1; i++) {
      float angle = float(i) * 2.0f * spitfire::math::cPI / float(nTeeth);
      spitfire::math::cVec3 normal(-cosf(angle + 1.5f * da), -sinf(angle + 1.5f * da), 0.0f);

      builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle), r0 * sinf(angle), -fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle), r0 * sinf(angle), fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle + 3.0f * da), r0 * sinf(angle + 3.0f * da), fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle + 3.0f * da), r0 * sinf(angle + 3.0f * da), -fWidth * 0.5f), normal);

      normal.Set(-cosf(angle + 3.5f * da), -sinf(angle + 3.5f * da), 0.0f);

      builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle + 3.0f * da), r0 * sinf(angle + 3.0f * da), -fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle + 3.0f * da), r0 * sinf(angle + 3.0f * da), fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle + 4.0f * da), r0 * sinf(angle + 4.0f * da), fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r0 * cosf(angle + 4.0f * da), r0 * sinf(angle + 4.0f * da), -fWidth * 0.5f), normal);
    }



    spitfire::math::cVec3 normal;
    float u = 0.0f;
    float v = 0.0f;
    float len = 0.0f;

    // draw outward faces of teeth
    for (size_t i = 0; i < nTeeth + 1; i++) {
      const float angle = float(i) * 2.0f * spitfire::math::cPI / nTeeth;

      normal.Set(cosf(angle - 0.5f * da), sinf(angle - 0.5f * da), 0.0f);
      builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle - da), r1 * sinf(angle - da), fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle - da), r1 * sinf(angle - da), -fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle), r1 * sinf(angle), -fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle), r1 * sinf(angle), fWidth * 0.5f), normal);

      // This is for the "wall" quad
      u = r2 * cosf(angle + da) - r1 * cosf(angle);
      v = r2 * sinf(angle + da) - r1 * sinf(angle);
      len = sqrt(u * u + v * v);
      u /= len;
      v /= len;
      normal.Set(v, -u, 0.0f);
      builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle), r1 * sinf(angle), fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle), r1 * sinf(angle), -fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r2 * cosf(angle + da), r2 * sinf(angle + da), -fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r2 * cosf(angle + da), r2 * sinf(angle + da), fWidth * 0.5f), normal);


      normal.Set(cosf(angle + 1.5f * da), sinf(angle + 1.5f * da), 0.0f);
      builder.PushBack(spitfire::math::cVec3(r2 * cosf(angle + da), r2 * sinf(angle + da), fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r2 * cosf(angle + da), r2 * sinf(angle + da), -fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r2 * cosf(angle + 2.0f * da), r2 * sinf(angle + 2.0f * da), -fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r2 * cosf(angle + 2.0f * da), r2 * sinf(angle + 2.0f * da), fWidth * 0.5f), normal);

      // This is for the "wall" quad
      u = r1 * cosf(angle + 3.0f * da) - r2 * cosf(angle + 2.0f * da);
      v = r1 * sinf(angle + 3.0f * da) - r2 * sinf(angle + 2.0f * da);
      normal.Set(v, -u, 0.0f);
      builder.PushBack(spitfire::math::cVec3(r2 * cosf(angle + 2.0f * da), r2 * sinf(angle + 2.0f * da), fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r2 * cosf(angle + 2.0f * da), r2 * sinf(angle + 2.0f * da), -fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle + 3.0f * da), r1 * sinf(angle + 3.0f * da), -fWidth * 0.5f), normal);
      builder.PushBack(spitfire::math::cVec3(r1 * cosf(angle + 3.0f * da), r1 * sinf(angle + 3.0f * da), fWidth * 0.5f), normal);
    }
  }
}
