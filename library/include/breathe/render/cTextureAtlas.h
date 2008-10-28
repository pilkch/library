#ifndef CTEXTUREATLAS_H
#define CTEXTUREATLAS_H

struct SDL_Surface;

namespace breathe
{
  namespace render
  {
    /*
      Atlas0 landscape textures: bitumen, dirt, grass, concrete, snow, water rocks, stairs, cliffs, sky
      Atlas1 building textures: bricks, concrete, windows, roofing,
      Atlas2 tree trunks, branches, leaves, foliage, grass blades and tuffets, signs, benches, bins, props
      Atlas3 vehicles, people, tires, guns, weapons, tools, etc.
      Atlas4 special effects: particles, smoke, lens flare, sun
    */

    enum ATLAS
    {
      ATLAS_LANDSCAPE=0,
      ATLAS_BUILDING,
      ATLAS_FOLIAGE,
      ATLAS_VEHICLES,
      ATLAS_PROPS,
      ATLAS_WEAPONS,
      ATLAS_EFFECTS,

      nAtlas,

      ATLAS_NONE = 0xFFFFFF
    };

    class cTextureAtlas :
      public cTexture
    {
    public:
      explicit cTextureAtlas(unsigned int id);
      ~cTextureAtlas();

      void Begin(unsigned int uiSegmentWidthPX, unsigned int uiSegmentSmallPX, unsigned int uiAtlasWidthPX);
      void End();

      cTextureRef AddTexture(const std::string& sFilename);


      unsigned int uiSegmentSmallPX; // An image this width or smaller is considered small

      unsigned int uiSegmentWidthPX; // Width of a segment
      unsigned int uiAtlasWidthNSegments; // Total atlas width in segments
      unsigned int uiAtlasSegmentN; // Total atlas segments

      std::vector<bool> vSegment;

    private:
      unsigned int uiID; // ATLAS_* number so we can tell people who we are

      void BlitSurface(SDL_Surface* src, unsigned int x, unsigned int y);
    };

    typedef cSmartPtr<cTextureAtlas> cTextureAtlasRef;
  }
}

#endif // CTEXTUREATLAS_H
