#ifndef CMODELHEIGHTMAP_H
#define CMODELHEIGHTMAP_H

namespace BREATHE
{
	namespace RENDER
	{
		namespace MODEL
		{
			class cHeightmap : public cModel, public cObject
			{
			public:
				cHeightmap();
				~cHeightmap();

				int Load(std::string sFilename);
					
				float Height(int x, int y);
				float Height(float x, float y);
				
				MATH::cVec3 Normal(int x, int y);
				MATH::cVec3 Normal(float x, float y);

				unsigned int Render();

				void Update(float fTime);

				std::vector<MATH::cVec3>vVertex;
				std::vector<MATH::cVec3>vNormal;
				std::vector<MATH::cVec2>vTextureCoord;

			protected:
				float* pHeight;
				MATH::cVec3* pNormal;

				unsigned int uiWidth;
				unsigned int uiHeight;

				float fWidthOfTile;
				float fHeightOfTile;
				float fScale;

				cVertexBufferObject* pVBO;

				//static BREATHE::MATH::cFrustum *pFrustum;

				std::string sMaterial;
				MATERIAL::cMaterial* pMaterial;
			};

			
			inline float cHeightmap::Height(int x, int y)
			{
				x += uiWidth>>1;
				y += uiHeight>>1;

				return pHeight[x + (y * (uiWidth + 1))];
			}

			inline float cHeightmap::Height(float x, float y)
			{
				x += static_cast<float>(uiWidth>>1) * fWidthOfTile * 8;
				y += static_cast<float>(uiHeight>>1) * fHeightOfTile * 8;

				if(x < 0.0f) x = 0.0f;
				if(y < 0.0f) y = 0.0f;
				if(x >= static_cast<float>(uiWidth)) x = static_cast<float>(uiWidth-1);
				if(y >= static_cast<float>(uiHeight)) y = static_cast<float>(uiHeight-1);

				unsigned int xi = static_cast<unsigned int>(x);
				unsigned int yi = static_cast<unsigned int>(y);
				
				//   0---1
				//   |   |
				//   3---2

				float h0 = pHeight[xi + (yi * static_cast<unsigned int>(uiWidth + 1))];
				float h1 = pHeight[xi+1 + (yi * static_cast<unsigned int>(uiWidth + 1))];
				float h3 = pHeight[xi + ((yi+1) * static_cast<unsigned int>(uiWidth + 1))];

				float	xfrac = x - static_cast<float>(xi);
				float yfrac = y - static_cast<float>(yi);

				// calculate interpolated ground height
				return 4.0f + (h0 + xfrac*(h1-h0) + yfrac*(h3-h0));
			}

			inline MATH::cVec3 cHeightmap::Normal(float x, float y)
			{
				x += static_cast<float>(uiWidth>>1) * fWidthOfTile * 8;
				y += static_cast<float>(uiHeight>>1) * fHeightOfTile * 8;

				if(x < 0.0f) x = 0.0f;
				if(y < 0.0f) y = 0.0f;
				if(x >= static_cast<float>(uiWidth)) x = static_cast<float>(uiWidth-1);
				if(y >= static_cast<float>(uiHeight)) y = static_cast<float>(uiHeight-1);

				unsigned int xi = static_cast<unsigned int>(x);
				unsigned int yi = static_cast<unsigned int>(y);
				
				//   0---1
				//   |   |
				//   3---2

				MATH::cVec3 h0 = pNormal[xi + (yi * static_cast<unsigned int>(uiWidth + 1))];
				MATH::cVec3 h1 = pNormal[xi+1 + (yi * static_cast<unsigned int>(uiWidth + 1))];
				MATH::cVec3 h3 = pNormal[xi + ((yi+1) * static_cast<unsigned int>(uiWidth + 1))];

				float	xfrac = x - static_cast<float>(xi);
				float yfrac = y - static_cast<float>(yi);

				// calculate interpolated ground height
				//return 4.0f + (h0 + xfrac*(h1-h0) + yfrac*(h3-h0));
				return (h0 + xfrac*(h1-h0) + yfrac*(h3-h0)).GetNormalized();
			}
		}
	}
}

#endif //CMODELHEIGHTMAP_H
