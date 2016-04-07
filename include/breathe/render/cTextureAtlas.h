#ifndef CTEXTUREATLAS_H
#define CTEXTUREATLAS_H

struct SDL_Surface;

#include <spitfire/math/geometry.h>

#include <breathe/breathe.h>

namespace breathe
{
  namespace render
  {
    // ** cTextureCoordinatesRectangle
    // A texture mapper for simple scales, rotations and translations
    // NOTE: If your shape is not symmetrical on both axes then rotating it 90 will result in stretched coordinates
    class cTextureCoordinatesRectangle
    {
    public:
      cTextureCoordinatesRectangle();
      cTextureCoordinatesRectangle(float fU, float fV, float fU2, float fV2);

      void Rotate90DegreesClockWise();
      void Rotate90DegreesCounterClockWise();
      void Translate(const spitfire::math::cVec2& distance);
      void Scale(const spitfire::math::cVec2& scale);

      spitfire::math::cVec2 textureCoordinates[4];
    };
    // A custom texture atlas
    // You specify the width and height of the atlas and start adding rectangles anywhere within the atlas (They can be rectangular and they can be overlapping)
    // There is no texture, this is really just a collection of rectangles

    class cTextureAtlasCustom
    {
    public:
      cTextureAtlasCustom();

      void Init(size_t width, size_t height);

      size_t AddRectangle(const spitfire::math::cRectangle& rectangle);
      spitfire::math::cRectangle GetRectangle(size_t index) const;

      // Convert a texture coordinate to "atlas space" so that we can then use the new coordinate with our atlas texture
      spitfire::math::cVec2 TransformTextureCoordinate(size_t index, const spitfire::math::cVec2& textureCoordinate) const;
      spitfire::math::cVec2 TransformTextureCoordinate(const spitfire::math::cRectangle& rectangle, const spitfire::math::cVec2& textureCoordinate) const;
      spitfire::math::cRectangle TransformTextureCoordinate(size_t index, const spitfire::math::cRectangle& textureCoordinates) const;
      spitfire::math::cRectangle TransformTextureCoordinate(const spitfire::math::cRectangle& rectangle, const spitfire::math::cRectangle& textureCoordinates) const;

    private:
      size_t width;
      size_t height;
      std::vector<spitfire::math::cRectangle> textures;
    };

    class cTextureMapper
    {
    public:
      spitfire::math::cVec2 GetTextureCoordinateFromPointInPlaneSpace(const spitfire::math::cVec2& point) const { return spitfire::math::cVec2(point.x, 1.0f - point.y); }
    };

    class cTextureMapperRealWorld
    {
    public:
      void SetTextureRealWorldDimensions(float fHorizontalMeters, float fVerticalMeters) { scale.Set(1.0f / fHorizontalMeters, 1.0f / fVerticalMeters); }

      // TODO: Check the y texture coordinate that this generates, is it right?
      spitfire::math::cVec2 GetTextureCoordinateFromPositionInPlaneSpace(const spitfire::math::cVec2& point) const { return spitfire::math::cVec2(scale.x * point.x, scale.y * (1.0f - point.y)); }

    private:
      spitfire::math::cVec2 scale;
    };


    // A classic texture atlas
    // You specify the width and height of the atlas, the width and height of each segment and start adding textures

    /*class cTextureAtlas
    {
    public:
      cTextureAtlas();

      void Init(size_t width, size_t height, size_t segmentWidth, size_t segmentHeight);

      size_t AddTexture(const voodoo::cImage& image);
      const spitfire::math::cRectangle& GetRectangle(size_t index) const;

      // Convert a texture coordinate to "atlas space" so that we can then use the new coordinate with our generated atlas texture
      spitfire::math::cVec2 TransformTextureCoordinate(size_t index, const spitfire::math::cVec2& textureCoordinate) const;
      spitfire::math::cVec2 TransformTextureCoordinate(const spitfire::math::cRectangle& rectangle, const spitfire::math::cVec2& textureCoordinate) const;

      size_t GetWidth() const { return image.GetWidth(); }
      size_t GetHeight() const { return image.GetHeight(); }
      const voodoo::cImage& GetImage() const { return image; }

    private:
      voodoo::cImage image;
      size_t width;
      size_t height;
      size_t segmentWidth;
      size_t segmentHeight;
      size_t nNextSegment;
      size_t nMaxSegments;
    };*/

    /*class cTextureAtlas
    {
    public:
      cTextureAtlas();

      float GetWidth() const { return fWidth; }
      float GetHeight() const { return fHeight; }

      spitfire::math::cVec2 GetTextureCoordinatesFromPointInSubTexture(const cTextureAtlasSubTexture& subTexture, const spitfire::math::cVec2& point) const;

    private:
      float fWidth;
      float fHeight;
    };

    cTextureAtlas::cTextureAtlas() :
      fWidth(1.0f),
      fHeight(1.0f)
    {
    }

    spitfire::math::cVec2 cTextureAtlas::GetTextureCoordinatesFromPointInSubTexture(const cTextureAtlasSubTexture& subTexture, const spitfire::math::cVec2& point) const
    {
      return spitfire::math::cVec2(subTexture.rectangle.x + (point.x * subTexture.scale.x), subTexture.rectangle.y + ((1.0f - point.y) * subTexture.scale.y));
    }*/

    /*class cTextureAtlasSubTexture
    {
    public:
      friend class cTextureAtlas;

      void SetRectangle(const cTextureAtlas& atlas, const spitfire::math::cRectangle& rectangle);

    protected:
      spitfire::math::cRectangle rectangle;
      spitfire::math::cVec2 scale;
    };

    void cTextureAtlasSubTexture::SetRectangle(const cTextureAtlas& atlas, const spitfire::math::cRectangle& _rectangle)
    {
      rectangle = _rectangle;
      scale.Set(rectangle.width / atlas.GetWidth(), rectangle.height / atlas.GetHeight());
    };*/

    /*// Atlas0 landscape textures: bitumen, dirt, grass, concrete, snow, water rocks, stairs, cliffs, sky
    // Atlas1 building textures: bricks, concrete, windows, roofing,
    // Atlas2 tree trunks, branches, leaves, foliage, grass blades and tuffets, signs, benches, bins, props
    // Atlas3 vehicles, people, tires, guns, weapons, tools, etc.
    // Atlas4 special effects: particles, smoke, lens flare, sun

    enum ATLAS {
      LANDSCAPE,
      BUILDING,
      FOLIAGE,
      VEHICLES,
      PROPS,
      WEAPONS,
      EFFECTS,

      nAtlas,

      NONE = 0xFFFFFF
    };

    class cTextureAtlas :
      public cTexture
    {
    public:
      explicit cTextureAtlas(unsigned int id);
      ~cTextureAtlas();

      void Begin(unsigned int uiSegmentWidthPX, unsigned int uiSegmentSmallPX, unsigned int uiAtlasWidthPX);
      void End();

      cTextureRef AddTexture(const string_t& sFilename);


      unsigned int uiSegmentSmallPX; // An image this width or smaller is considered small

      unsigned int uiSegmentWidthPX; // Width of a segment
      unsigned int uiAtlasWidthNSegments; // Total atlas width in segments
      unsigned int uiAtlasSegmentN; // Total atlas segments

      std::vector<bool> vSegment;

    private:
      unsigned int uiID; // ATLAS_* number so we can tell people who we are

      void BlitSurface(SDL_Surface* src, unsigned int x, unsigned int y);
    };

    typedef std::shared_ptr<cTextureAtlas> cTextureAtlasRef;*/
  }
}

#endif // CTEXTUREATLAS_H
