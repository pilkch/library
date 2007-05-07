#ifndef CTEXTUREATLAS_H
#define CTEXTUREATLAS_H

struct SDL_Surface;

namespace BREATHE
{
	namespace RENDER
	{
		/*	
			Atlas0	landscape textures: bitumen, dirt, grass, concrete, snow, water rocks, stairs, cliffs, sky
			Atlas1	building textures: bricks, concrete, windows, roofing, 
			Atlas2	tree trunks, branches, leaves, foliage, grass blades and tuffets, signs, benches, bins, props
			Atlas3	vehicles, people, tires, guns, weapons, tools, etc.
			Atlas4	special effects: particles, smoke, lens flare, sun
		*/
		const unsigned int ATLAS_NONE=0xFFFFFF;

		const unsigned int ATLAS_LANDSCAPE=0;
		const unsigned int ATLAS_BUILDING=1;
		const unsigned int ATLAS_FOLIAGE=2;
		const unsigned int ATLAS_VEHICLES=3;
		const unsigned int ATLAS_PROPS=4;
		const unsigned int ATLAS_WEAPONS=5;
		const unsigned int ATLAS_EFFECTS=6;

		const unsigned int nAtlas=7;

		class cTextureAtlas
			: public cTexture
		{
			unsigned int uiID; //ATLAS_* number so we can tell people who we are

			void BlitSurface(SDL_Surface *src, unsigned int x, unsigned int y);

		public:
			unsigned int uiSegmentSmallPX; //An image this width or smaller is considered small

			unsigned int uiSegmentWidthPX; //Width of a segment
			unsigned int uiAtlasWidthNSegments; //Total atlas width in segments
			unsigned int uiAtlasSegmentN; //Total atlas segments

			std::vector<bool> vSegment;

			cTextureAtlas(unsigned int id);
			~cTextureAtlas();

			void Begin(unsigned int uiSegmentWidthPX, unsigned int uiSegmentSmallPX, unsigned int uiAtlasWidthPX);
			void End();

			cTexture *AddTexture(std::string sFilename);
		};
	}
}

#endif //CTEXTUREATLAS_H