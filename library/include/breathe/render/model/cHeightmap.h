#ifndef CMODELHEIGHTMAP_H
#define CMODELHEIGHTMAP_H

namespace breathe
{
	namespace render
	{
		namespace model
		{
			class cHeightmap : public cModel, public cObject
			{
			public:
				cHeightmap();
				~cHeightmap();

				int Load(std::string sFilename);
				
				float Height(int x, int y);
				float Height(float x, float y);
				
				float Hermite4( float fFrac, const float * ptr );
				
				math::cVec3 Normal(int x, int y);
				math::cVec3 Normal(float x, float y);

				unsigned int Render();

				void Update(float fCurrentTime);

				std::vector<math::cVec3>vVertex;
				std::vector<math::cVec3>vNormal;
				std::vector<math::cVec2>vTextureCoord;

			protected:
				float* pHeight;
				math::cVec3* pNormal;

				unsigned int uiWidth;
				unsigned int uiHeight;

				float fWidthOfTile;
				float fHeightOfTile;
				float fScale;

				cVertexBufferObject* pVBO;

				//static breathe::math::cFrustum *pFrustum;

				std::string sMaterial;
				material::cMaterial* pMaterial;
			};

			
			inline float cHeightmap::Height(int x, int y)
			{
				x += uiWidth>>1;
				y += uiHeight>>1;

				return pHeight[x + (y * (uiWidth + 1))];
			}

			inline float cHeightmap::Hermite4( float fFrac, const float* ptr )
			{
				const float    c     = (ptr[2] - ptr[0]) * 0.5f;
				const float    v     = ptr[1] - ptr[2];
				const float    w     = c + v;
				const float    a     = w + v + (ptr[3] - ptr[1]) * 0.5f;
				const float    b     = w + a;
				return ((((a * fFrac) - b) * fFrac + c) * fFrac + ptr[1]);
			}

			inline float cHeightmap::Height(float x, float y)
			{
				assert( x < 0.0f && y < 0.0f );
				assert( x > fWidth && y > fLength );
				
				unsigned int xp = static_cast<unsigned int>(x);
				unsigned int yp = static_cast<unsigned int>(y);

				x -= (unsigned int)(x);
				y -= (unsigned int)(y);

				float xx[ 4 ];
				float yy[ 4 ];
				for( int iz = 0; iz < 4; ++iz ) {
					const float* data2 = &pHeight[xp - 1 + (iz + yp - 1) * (uiWidth + 1)];
					for( int ix = 0; ix < 4; ++ix ) {
						xx[ ix ] = data2[ ix ];
					}
					yy[ iz ] = Hermite4( x, xx );
				}
				return Hermite4(y, yy);
			}

			/*inline float cHeightmap::Height(float x, float y)
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
			}*/

			inline math::cVec3 cHeightmap::Normal(float x, float y)
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

				math::cVec3 h0 = pNormal[xi + (yi * static_cast<unsigned int>(uiWidth + 1))];
				math::cVec3 h1 = pNormal[xi+1 + (yi * static_cast<unsigned int>(uiWidth + 1))];
				math::cVec3 h3 = pNormal[xi + ((yi+1) * static_cast<unsigned int>(uiWidth + 1))];

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
