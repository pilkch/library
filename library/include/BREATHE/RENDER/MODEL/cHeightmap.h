#ifndef CMODELHEIGHTMAP_H
#define CMODELHEIGHTMAP_H

namespace BREATHE
{
	namespace RENDER
	{
		namespace MODEL
		{
			class cHeightmap : public cModel
			{
			public:
				cHeightmap();
				~cHeightmap();

				int Load(std::string sFilename);
					
				float Height(int x, int y);
				float Height(float x, float y);

				unsigned int Render();

				void Update(float fTime);

				std::vector<MATH::cVec3>vVertex;
				std::vector<MATH::cVec3>vNormal;
				std::vector<MATH::cVec2>vTextureCoord;
				
				MATERIAL::cMaterial* pMaterial;

			protected:
				float* pHeight;

				unsigned int uiWidth;
				unsigned int uiHeight;

				float fWidth;
				float fHeight;
				float fScale;

				cVertexBufferObject* pVBO;

				//static BREATHE::MATH::cFrustum *pFrustum;
			};

			
			inline float cHeightmap::Height(int x, int y)
			{
				x += uiWidth>>1;
				y += uiHeight>>1;

				return pHeight[x + (y * uiWidth)];
			}

			inline float cHeightmap::Height(float x, float y)
			{
				x += static_cast<float>(uiWidth>>1) * fWidth;
				y += static_cast<float>(uiHeight>>1) * fHeight;

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
		}
	}
}

#endif //CMODELHEIGHTMAP_H
