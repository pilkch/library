#ifndef CRENDER_H
#define CRENDER_H

extern PFNGLACTIVETEXTUREPROC glActiveTexture;
extern PFNGLMULTITEXCOORD2FPROC glMultiTexCoord2f;
extern PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;

extern PFNGLGENBUFFERSARBPROC glGenBuffersARB;
extern PFNGLBINDBUFFERARBPROC glBindBufferARB;
extern PFNGLBUFFERDATAARBPROC glBufferDataARB;
extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;

extern PFNGLSHADERSOURCEARBPROC glShaderSource;
extern PFNGLCOMPILESHADERARBPROC glCompileShader;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORM3FPROC glUniform3f;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;

namespace BREATHE
{	
	namespace RENDER
	{
		class cRender
		{
		private:
			unsigned int uiSegmentWidthPX;
			unsigned int uiSegmentSmallPX;
			unsigned int uiAtlasWidthPX;

			std::map<std::string, cTexture *> mTexture; //Map that contains filename, texture pairs
			std::map<std::string, cTexture *> mCubeMap; //Map that contains filename, cubemap texture pairs
			

			bool FindExtension(std::string sExt);

			
			bool bActiveColour;
			bool bActiveShader;

			MATH::cColour colour;
			std::vector<MATERIAL::cLayer>vLayer;

		public:
			cLevel *pLevel;


			bool bRenderWireframe;
			bool bLight;
			bool bCubemap;
			bool bShader;

			bool bCanShader;

			bool bFullscreen;
			unsigned int uiWidth;
			unsigned int uiHeight;
			unsigned int uiDepth;

			unsigned int uiFlags;

      int iMaxTextureSize;

			unsigned int uiTextureChanges;
			unsigned int uiTextureModeChanges;

			MATH::cVec4 v3SunPosition;

			std::vector<RENDER::cTextureAtlas *> vTextureAtlas; //Vector that contains texture atlases

			cTexture *pTextureNotFoundTexture;
			cTexture *pMaterialNotFoundTexture;
			
			unsigned int uiActiveUnits;

			MATERIAL::cMaterial *pMaterialNotFoundMaterial;

			std::map<std::string, MATERIAL::cMaterial *> mMaterial;




			MATH::cFrustum *pFrustum;

			cRender();
			~cRender();

			bool Init();

			void BeginFrame(float fCurrentTime);
			void BeginHUD(float fCurrentTime);
			void EndFrame();

			/*//Font
		protected:
			int font_line;

			int font_m_iTexture;
			int font_charWidth;					// Desired width of font
			int font_charHeight;					// Desired height of font
			int font_charSpacing;				// Spacing between each character
			int font_textureFilter;			// What type of texture filtering to render the text with.
															// 0 = point AKA no filtering
															// 1 = Linear

			void *font_pVertices;				// Temp memory to work with when modify vertex buffers

			unsigned int font_base; //which display list
			int font_texture;

		public:
			virtual bool Font_Create(int newTexture, void *pDevice, void *pRenderer)=0;

			virtual void Font_Begin(bool reset)=0;
			virtual void Font_Print(int x, int y, int set, char * string, ...)=0;
			virtual void Font_Println(int set, char * string, ...)=0;
			virtual void Font_End(void)=0;*/


			

			void SetAtlasWidth(unsigned int uiNewSegmentWidthPX, unsigned int uiNewSegmentSmallPX, unsigned int uiNewAtlasWidthPX);

			void BeginLoadingTextures();
			void EndLoadingTextures();

			bool AddTextureNotFoundTexture(std::string sNewFilename);
			bool AddMaterialNotFoundTexture(std::string sNewFilename);

			cTexture *AddCubeMap(std::string sFilename);
			cTexture *AddTexture(std::string sNewFilename);
			cTexture *AddTextureToAtlas(std::string sNewFilename, unsigned int uiAtlas);
			
			cTexture *GetTexture(std::string sFilename);
			cTexture *GetCubeMap(std::string sFilename);


			
			MATERIAL::cMaterial* pCurrentMaterial;


			MATERIAL::cMaterial *AddMaterial(std::string sFilename);
			MATERIAL::cMaterial *AddMaterialNotFoundMaterial(std::string sFilename);
			bool ClearMaterial();
			bool SetMaterial(MATERIAL::cMaterial* pMaterial, MATH::cVec3& pos);

			void ClearColour();
			void SetColour(MATH::cColour inColour);

			MATERIAL::cMaterial *GetMaterial(std::string sFilename);

			void ReloadMaterials();
		};

		
		inline void cRender::ClearColour()
		{
			bActiveColour=false;

			colour.SetBlack();
			colour.a=1.0f;

			glColor4f(colour.r, colour.g, colour.b, colour.a);
		}
		
		inline void cRender::SetColour(MATH::cColour inColour)
		{
			bActiveColour=true;

			colour=inColour;
			colour.a=1.0f;

			glColor4f(colour.r, colour.g, colour.b, colour.a);
		}
	}
}

#endif //CRENDER_H