// Standard headers
#include <cinttypes>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <vector>

// libvoodoomm headers
#include <libvoodoomm/cImage.h>
#include <libvoodoomm/dds_reader.h>

namespace {

// TODO: Replace these
typedef uint32_t DWORD;
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef int32_t BOOL;

// TODO: Replace these
#define FALSE 0
#define TRUE 1
#define SEEK_SET  0
#define SEEK_CUR  1
#define SEEK_END  2

#define DLL_CALLCONV

// TODO: Replace this rubbish
typedef void* fi_handle;
typedef unsigned (DLL_CALLCONV *FI_ReadProc) (void *buffer, unsigned size, unsigned count, fi_handle handle);
typedef unsigned (DLL_CALLCONV *FI_WriteProc) (void *buffer, unsigned size, unsigned count, fi_handle handle);
typedef int (DLL_CALLCONV *FI_SeekProc) (fi_handle handle, long offset, int origin);
typedef long (DLL_CALLCONV *FI_TellProc) (fi_handle handle);
#define FI_STRUCT(x)	struct x
FI_STRUCT (FIBITMAP) { void *data; };
FI_STRUCT (FIMULTIBITMAP) { void *data; };
FI_STRUCT(FreeImageIO) {
  FI_ReadProc  read_proc;     // pointer to the function used to read data
    FI_WriteProc write_proc;    // pointer to the function used to write data
    FI_SeekProc  seek_proc;     // pointer to the function used to seek
    FI_TellProc  tell_proc;     // pointer to the function used to aquire the current position
};

#if (defined(BYTE_ORDER) && BYTE_ORDER==BIG_ENDIAN) || \
  (defined(__BYTE_ORDER) && __BYTE_ORDER==__BIG_ENDIAN) || \
  defined(__BIG_ENDIAN__)
#define FREEIMAGE_BIGENDIAN
#endif // BYTE_ORDER

// This really only affects 24 and 32 bit formats, the rest are always RGB order.
#define FREEIMAGE_COLORORDER_BGR	0
#define FREEIMAGE_COLORORDER_RGB	1
#if defined(FREEIMAGE_BIGENDIAN)
#define FREEIMAGE_COLORORDER FREEIMAGE_COLORORDER_RGB
#else
#define FREEIMAGE_COLORORDER FREEIMAGE_COLORORDER_BGR
#endif

#ifndef FREEIMAGE_BIGENDIAN
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
// Little Endian (x86 / MS Windows, Linux) : BGR(A) order
#define FI_RGBA_RED				2
#define FI_RGBA_GREEN			1
#define FI_RGBA_BLUE			0
#define FI_RGBA_ALPHA			3
#define FI_RGBA_RED_MASK		0x00FF0000
#define FI_RGBA_GREEN_MASK		0x0000FF00
#define FI_RGBA_BLUE_MASK		0x000000FF
#define FI_RGBA_ALPHA_MASK		0xFF000000
#define FI_RGBA_RED_SHIFT		16
#define FI_RGBA_GREEN_SHIFT		8
#define FI_RGBA_BLUE_SHIFT		0
#define FI_RGBA_ALPHA_SHIFT		24
#else
// Little Endian (x86 / MaxOSX) : RGB(A) order
#define FI_RGBA_RED				0
#define FI_RGBA_GREEN			1
#define FI_RGBA_BLUE			2
#define FI_RGBA_ALPHA			3
#define FI_RGBA_RED_MASK		0x000000FF
#define FI_RGBA_GREEN_MASK		0x0000FF00
#define FI_RGBA_BLUE_MASK		0x00FF0000
#define FI_RGBA_ALPHA_MASK		0xFF000000
#define FI_RGBA_RED_SHIFT		0
#define FI_RGBA_GREEN_SHIFT		8
#define FI_RGBA_BLUE_SHIFT		16
#define FI_RGBA_ALPHA_SHIFT		24
#endif // FREEIMAGE_COLORORDER
#else
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
// Big Endian (PPC / none) : BGR(A) order
#define FI_RGBA_RED				2
#define FI_RGBA_GREEN			1
#define FI_RGBA_BLUE			0
#define FI_RGBA_ALPHA			3
#define FI_RGBA_RED_MASK		0x0000FF00
#define FI_RGBA_GREEN_MASK		0x00FF0000
#define FI_RGBA_BLUE_MASK		0xFF000000
#define FI_RGBA_ALPHA_MASK		0x000000FF
#define FI_RGBA_RED_SHIFT		8
#define FI_RGBA_GREEN_SHIFT		16
#define FI_RGBA_BLUE_SHIFT		24
#define FI_RGBA_ALPHA_SHIFT		0
#else
// Big Endian (PPC / Linux, MaxOSX) : RGB(A) order
#define FI_RGBA_RED				0
#define FI_RGBA_GREEN			1
#define FI_RGBA_BLUE			2
#define FI_RGBA_ALPHA			3
#define FI_RGBA_RED_MASK		0xFF000000
#define FI_RGBA_GREEN_MASK		0x00FF0000
#define FI_RGBA_BLUE_MASK		0x0000FF00
#define FI_RGBA_ALPHA_MASK		0x000000FF
#define FI_RGBA_RED_SHIFT		24
#define FI_RGBA_GREEN_SHIFT		16
#define FI_RGBA_BLUE_SHIFT		8
#define FI_RGBA_ALPHA_SHIFT		0
#endif // FREEIMAGE_COLORORDER
#endif // FREEIMAGE_BIGENDIAN


/****************************************************************/

//--------------------------------------------------------------------------------------
// Return the BPP for a particular format.
//--------------------------------------------------------------------------------------
/*static size_t BitsPerPixel(_In_ DXGI_FORMAT fmt)
{
    switch (fmt)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 128;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return 96;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return 64;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return 32;

    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_B4G4R4A4_UNORM:
        return 16;

    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
        return 8;

    case DXGI_FORMAT_R1_UNORM:
        return 1;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        return 4;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return 8;

    default:
        return 0;
    }
}*/


//--------------------------------------------------------------------------------------
// Get surface information for a particular format.
//--------------------------------------------------------------------------------------
/*static void GetSurfaceInfo(
    _In_ size_t width,
    _In_ size_t height,
    _In_ DXGI_FORMAT fmt,
    _Out_opt_ size_t* outNumBytes,
    _Out_opt_ size_t* outRowBytes,
    _Out_opt_ size_t* outNumRows
    )
{
    size_t numBytes = 0;
    size_t rowBytes = 0;
    size_t numRows = 0;

    bool bc = false;
    bool packed  = false;
    size_t bcnumBytesPerBlock = 0;
    switch (fmt)
    {
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        bc = true;
        bcnumBytesPerBlock = 8;
        break;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        bc = true;
        bcnumBytesPerBlock = 16;
        break;

    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        packed = true;
        break;
    }

    if (bc)
    {
        size_t numBlocksWide = 0;
        if (width > 0)
        {
            numBlocksWide = std::max<size_t>(1, (width + 3) / 4);
        }
        size_t numBlocksHigh = 0;
        if (height > 0)
        {
            numBlocksHigh = std::max<size_t>(1, (height + 3) / 4);
        }
        rowBytes = numBlocksWide * bcnumBytesPerBlock;
        numRows = numBlocksHigh;
    }
    else if (packed)
    {
        rowBytes = ((width + 1) >> 1) * 4;
        numRows = height;
    }
    else
    {
        size_t bpp = BitsPerPixel(fmt);
        rowBytes = (width * bpp + 7) / 8; // Round up to the nearest byte.
        numRows = height;
    }

    numBytes = rowBytes * numRows;
    if (outNumBytes)
    {
        *outNumBytes = numBytes;
    }
    if (outRowBytes)
    {
        *outRowBytes = rowBytes;
    }
    if (outNumRows)
    {
        *outNumRows = numRows;
    }
}*/

/* From Utilities.h */
inline unsigned CalculateLine(unsigned width, unsigned bitdepth)
{
  return (unsigned)( ((unsigned long long)width * bitdepth + 7) / 8 );
}
/********************/

// ----------------------------------------------------------
//   Definitions for the DDS format
// ----------------------------------------------------------

#ifdef _WIN32
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct tagDDPIXELFORMAT {
  DWORD dwSize;	// size of this structure (must be 32)
  DWORD dwFlags;	// see DDPF_*
  DWORD dwFourCC;
  DWORD dwRGBBitCount;	// Total number of bits for RGB formats
  DWORD dwRBitMask;
  DWORD dwGBitMask;
  DWORD dwBBitMask;
  DWORD dwRGBAlphaBitMask;
} DDPIXELFORMAT;

// DIRECTDRAW PIXELFORMAT FLAGS
enum {
  DDPF_ALPHAPIXELS = 0x00000001l,	// surface has alpha channel
  DDPF_ALPHA		 = 0x00000002l,	// alpha only
  DDPF_FOURCC		 = 0x00000004l,	// FOURCC available
  DDPF_RGB		 = 0x00000040l	// RGB(A) bitmap
};

typedef struct tagDDCAPS2 {
  DWORD dwCaps1;	// Zero or more of the DDSCAPS_* members
  DWORD dwCaps2;	// Zero or more of the DDSCAPS2_* members
  DWORD dwReserved[2];
} DDCAPS2;

// DIRECTDRAWSURFACE CAPABILITY FLAGS
enum {
  DDSCAPS_ALPHA	= 0x00000002l, // alpha only surface
  DDSCAPS_COMPLEX	= 0x00000008l, // complex surface structure
  DDSCAPS_TEXTURE	= 0x00001000l, // used as texture (should always be set)
  DDSCAPS_MIPMAP	= 0x00400000l  // Mipmap present
};

enum {
  DDSCAPS2_CUBEMAP			= 0x00000200L,
  DDSCAPS2_CUBEMAP_POSITIVEX	= 0x00000400L,
  DDSCAPS2_CUBEMAP_NEGATIVEX	= 0x00000800L,
  DDSCAPS2_CUBEMAP_POSITIVEY	= 0x00001000L,
  DDSCAPS2_CUBEMAP_NEGATIVEY	= 0x00002000L,
  DDSCAPS2_CUBEMAP_POSITIVEZ	= 0x00004000L,
  DDSCAPS2_CUBEMAP_NEGATIVEZ	= 0x00008000L,
  DDSCAPS2_VOLUME				= 0x00200000L
};

typedef struct tagDDSURFACEDESC2 {
  DWORD dwSize;	// size of this structure (must be 124)
  DWORD dwFlags;	// combination of the DDSS_* flags
  DWORD dwHeight;
  DWORD dwWidth;
  DWORD dwPitchOrLinearSize;
  DWORD dwDepth;	// Depth of a volume texture
  DWORD dwMipMapCount;
  DWORD dwReserved1[11];
  DDPIXELFORMAT ddpfPixelFormat;
  DDCAPS2 ddsCaps;
  DWORD dwReserved2;
} DDSURFACEDESC2;

enum {
  DDSD_CAPS			= 0x00000001l,
  DDSD_HEIGHT			= 0x00000002l,
  DDSD_WITH			= 0x00000004l,
  DDSD_PITCH			= 0x00000008l,
  DDSD_ALPHABITDEPTH  = 0x00000080l,
  DDSD_PIXELFORMAT	= 0x00001000l,
  DDSD_MIPMAPCOUNT	= 0x00020000l,
  DDSD_LINEARSIZE		= 0x00080000l,
  DDSD_DEPTH			= 0x00800000l
};

typedef struct tagDDSHEADER {
  DWORD dwMagic;			// FOURCC: "DDS "
  DDSURFACEDESC2 surfaceDesc;
} DDSHEADER;

#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
  ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
    ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

#define FOURCC_DXT1	MAKEFOURCC('D','X','T','1')
#define FOURCC_DXT2	MAKEFOURCC('D','X','T','2')
#define FOURCC_DXT3	MAKEFOURCC('D','X','T','3')
#define FOURCC_DXT4	MAKEFOURCC('D','X','T','4')
#define FOURCC_DXT5	MAKEFOURCC('D','X','T','5')

// ----------------------------------------------------------
//   Structures used by DXT textures
// ----------------------------------------------------------

typedef struct tagColor8888 {
  BYTE b;
  BYTE g;
  BYTE r;
  BYTE a;
} Color8888;

typedef struct tagColor565 {
  WORD b : 5;
  WORD g : 6;
  WORD r : 5;
} Color565;

typedef struct tagDXTColBlock {
  Color565 colors[2];
  BYTE row[4];
} DXTColBlock;

typedef struct tagDXTAlphaBlockExplicit {
  WORD row[4];
} DXTAlphaBlockExplicit;

typedef struct tagDXTAlphaBlock3BitLinear {
  BYTE alpha[2];
  BYTE data[6];
} DXTAlphaBlock3BitLinear;

typedef struct tagDXT1Block
{
  DXTColBlock color;
} DXT1Block;

typedef struct tagDXT3Block {		// also used by dxt2
  DXTAlphaBlockExplicit alpha;
  DXTColBlock color;
} DXT3Block;

typedef struct tagDXT5Block {		// also used by dxt4
  DXTAlphaBlock3BitLinear alpha;
  DXTColBlock color;
} DXT5Block;

#ifdef _WIN32
#	pragma pack(pop)
#else
#	pragma pack()
#endif

// ----------------------------------------------------------
//   Internal functions
// ----------------------------------------------------------
#ifdef FREEIMAGE_BIGENDIAN
static void
SwapHeader(DDSHEADER *header) {
  SwapLong(&header->dwMagic);
  SwapLong(&header->surfaceDesc.dwSize);
  SwapLong(&header->surfaceDesc.dwFlags);
  SwapLong(&header->surfaceDesc.dwHeight);
  SwapLong(&header->surfaceDesc.dwWidth);
  SwapLong(&header->surfaceDesc.dwPitchOrLinearSize);
  SwapLong(&header->surfaceDesc.dwDepth);
  SwapLong(&header->surfaceDesc.dwMipMapCount);
  for(int i=0; i<11; i++) {
    SwapLong(&header->surfaceDesc.dwReserved1[i]);
  }

  // DDS_PIXELFORMAT
  SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwSize);
  SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwFlags);
  SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwFourCC);
  SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwRGBBitCount);
  SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwRBitMask);
  SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwGBitMask);
  SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwBBitMask);
  SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwRGBAlphaBitMask);

  SwapLong(&header->surfaceDesc.ddsCaps.dwCaps1);
  SwapLong(&header->surfaceDesc.ddsCaps.dwCaps2);
  SwapLong(&header->surfaceDesc.ddsCaps.dwReserved[0]);
  SwapLong(&header->surfaceDesc.ddsCaps.dwReserved[1]);
  SwapLong(&header->surfaceDesc.dwReserved2);
}
#endif

// ==========================================================

// Get the 4 possible colors for a block
//
static void GetBlockColors(const DXTColBlock &block, Color8888 colors[4], bool isDXT1)
{
  int i;
  // expand from 565 to 888
  for (i = 0; i < 2; i++)	{
    colors[i].a = 0xff;
    /*
    colors[i].r = (BYTE)(block.colors[i].r * 0xff / 0x1f);
    colors[i].g = (BYTE)(block.colors[i].g * 0xff / 0x3f);
    colors[i].b = (BYTE)(block.colors[i].b * 0xff / 0x1f);
    */
    colors[i].r = (BYTE)((block.colors[i].r << 3U) | (block.colors[i].r >> 2U));
    colors[i].g = (BYTE)((block.colors[i].g << 2U) | (block.colors[i].g >> 4U));
    colors[i].b = (BYTE)((block.colors[i].b << 3U) | (block.colors[i].b >> 2U));
  }

  WORD *wCol = (WORD *)block.colors;
  if (wCol[0] > wCol[1] || !isDXT1) {
    // 4 color block
    for (i = 0; i < 2; i++)	{
      colors[i + 2].a = 0xff;
      colors[i + 2].r = (BYTE)((WORD (colors[0].r) * (2 - i) + WORD (colors[1].r) * (1 + i)) / 3);
      colors[i + 2].g = (BYTE)((WORD (colors[0].g) * (2 - i) + WORD (colors[1].g) * (1 + i)) / 3);
      colors[i + 2].b = (BYTE)((WORD (colors[0].b) * (2 - i) + WORD (colors[1].b) * (1 + i)) / 3);
    }
  }
  else {
    // 3 color block, number 4 is transparent
    colors[2].a = 0xff;
    colors[2].r = (BYTE)((WORD (colors[0].r) + WORD (colors[1].r)) / 2);
    colors[2].g = (BYTE)((WORD (colors[0].g) + WORD (colors[1].g)) / 2);
    colors[2].b = (BYTE)((WORD (colors[0].b) + WORD (colors[1].b)) / 2);

    colors[3].a = 0x00;
    colors[3].g = 0x00;
    colors[3].b = 0x00;
    colors[3].r = 0x00;
  }
}

struct DXT_INFO_1 {
  typedef DXT1Block Block;
  enum {
    isDXT1 = 1,
    bytesPerBlock = 8
  };
};

struct DXT_INFO_3 {
  typedef DXT3Block Block;
  enum {
    isDXT1 = 1,
    bytesPerBlock = 16
  };
};

struct DXT_INFO_5 {
  typedef DXT5Block Block;
  enum
  {
    isDXT1 = 1,
    bytesPerBlock = 16
  };
};

template <class INFO> class DXT_BLOCKDECODER_BASE {
protected:
  Color8888 m_colors[4];
  const typename INFO::Block *m_pBlock;
  unsigned m_colorRow;

public:
  void Setup (const BYTE *pBlock) {
    m_pBlock = (const typename INFO::Block *)pBlock;
    GetBlockColors (m_pBlock->color, m_colors, INFO::isDXT1);
  }

  void SetY (int y) {
    m_colorRow = m_pBlock->color.row[y];
  }

  void GetColor (int x, int y, Color8888 &color) {
    unsigned bits = (m_colorRow >> (x * 2)) & 3;
    color = m_colors[bits];
  }
};

class DXT_BLOCKDECODER_1 : public DXT_BLOCKDECODER_BASE <DXT_INFO_1> {
public:
  typedef DXT_INFO_1 INFO;
};

class DXT_BLOCKDECODER_3 : public DXT_BLOCKDECODER_BASE <DXT_INFO_3> {
public:
  typedef DXT_BLOCKDECODER_BASE <DXT_INFO_3> base;
  typedef DXT_INFO_3 INFO;

protected:
  unsigned m_alphaRow;

public:
  void SetY (int y) {
    base::SetY (y);
    m_alphaRow = m_pBlock->alpha.row[y];
  }

  void GetColor (int x, int y, Color8888 &color) {
    base::GetColor (x, y, color);
    const unsigned bits = (m_alphaRow >> (x * 4)) & 0xF;
    color.a = (BYTE)((bits * 0xFF) / 0xF);
  }
};

class DXT_BLOCKDECODER_5 : public DXT_BLOCKDECODER_BASE <DXT_INFO_5> {
public:
  typedef DXT_BLOCKDECODER_BASE <DXT_INFO_5> base;
  typedef DXT_INFO_5 INFO;

protected:
  unsigned m_alphas[8];
  unsigned m_alphaBits;
  int m_offset;

public:
  void Setup (const BYTE *pBlock) {
    base::Setup (pBlock);

    const DXTAlphaBlock3BitLinear &block = m_pBlock->alpha;
    m_alphas[0] = block.alpha[0];
    m_alphas[1] = block.alpha[1];
    if (m_alphas[0] > m_alphas[1]) {
      // 8 alpha block
      for (int i = 0; i < 6; i++) {
        m_alphas[i + 2] = ((6 - i) * m_alphas[0] + (1 + i) * m_alphas[1] + 3) / 7;
      }
    }
    else {
      // 6 alpha block
      for (int i = 0; i < 4; i++) {
        m_alphas[i + 2] = ((4 - i) * m_alphas[0] + (1 + i) * m_alphas[1] + 2) / 5;
      }
      m_alphas[6] = 0;
      m_alphas[7] = 0xFF;
    }

  }

  void SetY (int y) {
    base::SetY (y);
    int i = y / 2;
    const DXTAlphaBlock3BitLinear &block = m_pBlock->alpha;
    m_alphaBits = unsigned(block.data[0 + i * 3]) | (unsigned(block.data[1 + i * 3]) << 8)
      | (unsigned(block.data[2 + i * 3]) << 16);
    m_offset = (y & 1) * 12;
  }

  void GetColor (int x, int y, Color8888 &color) {
    base::GetColor (x, y, color);
    unsigned bits = (m_alphaBits >> (x * 3 + m_offset)) & 7;
    color.a = (BYTE)m_alphas[bits];
  }
};

template <class DECODER> void DecodeDXTBlock (BYTE *dstData, const BYTE *srcBlock, long dstPitch, int bw, int bh) {
  DECODER decoder;
  decoder.Setup (srcBlock);
  for (int y = 0; y < bh; y++) {
    BYTE *dst = dstData - y * dstPitch;
    decoder.SetY (y);
    for (int x = 0; x < bw; x++) {
      decoder.GetColor (x, y, (Color8888 &)*dst);

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB 
      INPLACESWAP(dst[FI_RGBA_RED], dst[FI_RGBA_BLUE]);
#endif 
      dst += 4;
    }
  }
}

// ==========================================================
// Internal functions
// ==========================================================

struct IO {
    const std::vector<uint8_t>* data;
    unsigned int idx;
    void seek(unsigned idx) { this->idx = idx; }
    template<typename T>
    void read(T* dst, unsigned cnt) {
        memcpy(dst, &data->at(idx), cnt);
        idx += cnt;
    }
};

inline void load_rgb(DDSURFACEDESC2 &desc, IO& io, std::vector<uint8_t>& out_rgb_data) {
  int width = (int)desc.dwWidth & ~3;
  int height = (int)desc.dwHeight & ~3;
  int bpp = (int)desc.ddpfPixelFormat.dwRGBBitCount;
  
    int bytespp = bpp/8;
  out_rgb_data.resize(width * height * bytespp);
  
  // read the file
  int line   = CalculateLine(width, bpp);
  int filePitch = (desc.dwFlags & DDSD_PITCH) ? (int)desc.dwPitchOrLinearSize : line;
  long delta = (long)filePitch - (long)line;
  for (int i = 0; i < height; i++) {
        BYTE* pixels = &out_rgb_data[line*i];
        io.read(pixels, line);
        io.idx += delta;
     for(int x = 0; x < width; x++) {
            std::swap(pixels[FI_RGBA_RED],pixels[FI_RGBA_BLUE]);
      pixels += bytespp;
    }
  }
  
  // enable transparency
  //FreeImage_SetTransparent (dib, (desc.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) ? TRUE : FALSE);

  //if (!(desc.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) && bpp == 32) {
  //	// no transparency: convert to 24-bit
  //	FIBITMAP *old = dib;
  //	dib = FreeImage_ConvertTo24Bits (old);
  //	FreeImage_Unload (old);
  //}
}

template <class DECODER> static void 
load_dxt_helper (IO& io, std::vector<uint8_t>& result, int width, int height, int line) {
  typedef typename DECODER::INFO INFO;
  typedef typename INFO::Block Block;

  Block *input_buffer = new Block[(width + 3) / 4];

  int widthRest = (int) width & 3;
  int heightRest = (int) height & 3;
  int inputLine = (width + 3) / 4;
  int y = 0;

  if (height >= 4) {
        for (; y < height; y += 4) {
      io.read(input_buffer, sizeof (typename INFO::Block) * inputLine);
      // TODO: probably need some endian work here
      BYTE *pbSrc = (BYTE *)input_buffer;
            BYTE *pbDst = &result[width*4*(height-y-1)];

      if (width >= 4) {
        for (int x = 0; x < width; x += 4) {
          DecodeDXTBlock <DECODER> (pbDst, pbSrc,	line, 4, 4);
          pbSrc += INFO::bytesPerBlock;
          pbDst += 4 * 4;
        }
      }
      if (widthRest) {
        DecodeDXTBlock <DECODER> (pbDst, pbSrc, line, widthRest, 4);
      }
    }
  }
  if (heightRest)	{
    io.read(input_buffer, sizeof (typename INFO::Block) * inputLine);
    // TODO: probably need some endian work here
    BYTE *pbSrc = (BYTE *)input_buffer;
        BYTE *pbDst = &result[line*y];

    if (width >= 4) {
      for (int x = 0; x < width; x += 4) {
        DecodeDXTBlock <DECODER> (pbDst, pbSrc,	line, 4, heightRest);
        pbSrc += INFO::bytesPerBlock;
        pbDst += 4 * 4;
      }
    }
    if (widthRest) {
      DecodeDXTBlock <DECODER> (pbDst, pbSrc,	line, widthRest, heightRest);
    }

  }

  delete [] input_buffer;
}

inline void load_dxt(int type, DDSURFACEDESC2 &desc, IO& io, std::vector<uint8_t>& out_image_data)
{
  int width = (int)desc.dwWidth & ~3;
  int height = (int)desc.dwHeight & ~3;

  // allocate a 32-bit dib
  out_image_data.resize(width * height * 4);

  // TODO: Load 24 bit RGB only for DXT1
  int bpp = 32;
  int line = CalculateLine(width, bpp);

  // select the right decoder
  switch (type) {
    case 1:
      load_dxt_helper <DXT_BLOCKDECODER_1> (io, out_image_data, width, height, line);
      break;
    case 3:
      load_dxt_helper <DXT_BLOCKDECODER_3> (io, out_image_data, width, height, line);
      break;
    case 5:
      load_dxt_helper <DXT_BLOCKDECODER_5> (io, out_image_data, width, height, line);
      break;
  }
    // but now the lines are broken, so we need to fix it here
    std::vector<uint8_t> buf(width*4);
    for (int y = 0; y < height/2; y++) {
      unsigned up = y * width * 4;
      unsigned bot = (height - y - 1) * width * 4;
      memcpy(&buf.front(), &out_image_data[up], width * 4);
      memmove(&out_image_data[up], &out_image_data[bot], width * 4);
      memcpy(&out_image_data[bot], &buf.front(), width * 4);
    }

  if (type == 1) {
    // HACK: 24 bit RGB, so strip the alpha channel
    // NOTE: We also swap the red and blue channels
    std::vector<uint8_t> temp = out_image_data;

    out_image_data.resize(width * height * 3);

    size_t from = 0;
    size_t to = 0;

    for (size_t y = 0; y < size_t(width); y++) {
      for (size_t x = 0; x < size_t(height); x++) {
        out_image_data[to] = temp[from + 2];
        out_image_data[to + 1] = temp[from + 1];
        out_image_data[to + 2] = temp[from];

        from += 4;
        to += 3;
      }
    }
  } else {
    // 32 bit RGBA
    // Swap the red and blue channels
    size_t index = 0;

    for (size_t y = 0; y < size_t(width); y++) {
      for (size_t x = 0; x < size_t(height); x++) {
        std::swap(out_image_data[index], out_image_data[index + 2]);

        index += 4;
      }
    }
  }
}

}

namespace voodoo
{
  bool read_dds(const std::vector<uint8_t>& data, DDS_IMAGE_TYPE& out_dds_type, cImage& out_image)
  {
    out_dds_type = DDS_IMAGE_TYPE::UNKNOWN;
  
    DDSHEADER header;
    memset(&header, 0, sizeof(header));

    IO io { &data, 0 };
    io.read(&header, sizeof(header));
    #ifdef FREEIMAGE_BIGENDIAN
    SwapHeader(&header);
    #endif

    auto& desc = header.surfaceDesc;
    const size_t image_width = (int)desc.dwWidth & ~3;
    const size_t image_height = (int)desc.dwHeight & ~3;

    std::vector<uint8_t> image_data;

    if (header.surfaceDesc.ddpfPixelFormat.dwFlags & DDPF_RGB) {
      load_rgb(header.surfaceDesc, io, image_data);
      out_dds_type = DDS_IMAGE_TYPE::RGB;
      // TODO: Check the actual bit count, don't assume 24 bit RGB
      //const size_t image_bpp = (int)desc.ddpfPixelFormat.dwRGBBitCount;

      return out_image.CreateFromBuffer(image_data.data(), image_width, image_height, voodoo::PIXELFORMAT::R8G8B8);
    }

    if (header.surfaceDesc.ddpfPixelFormat.dwFlags & DDPF_FOURCC) {
      // https://www.khronos.org/opengl/wiki/S3_Texture_Compression
      switch (header.surfaceDesc.ddpfPixelFormat.dwFourCC) {
        case FOURCC_DXT1:
          // RGB (Although there is a variation with RGB + 1 bit alpha apparently?)
          load_dxt(1, header.surfaceDesc, io, image_data);
          out_dds_type = DDS_IMAGE_TYPE::DXT1;
          return out_image.CreateFromBuffer(image_data.data(), image_width, image_height, voodoo::PIXELFORMAT::R8G8B8);
        case FOURCC_DXT3:
          // RGBA
          load_dxt(3, header.surfaceDesc, io, image_data);
          out_dds_type = DDS_IMAGE_TYPE::DXT3;
          return out_image.CreateFromBuffer(image_data.data(), image_width, image_height, voodoo::PIXELFORMAT::R8G8B8A8);
        case FOURCC_DXT5:
          load_dxt(5, header.surfaceDesc, io, image_data);
          out_dds_type = DDS_IMAGE_TYPE::DXT5;
          return out_image.CreateFromBuffer(image_data.data(), image_width, image_height, voodoo::PIXELFORMAT::R8G8B8A8);
        default:
          break;
      }
    }

    return false;
  }
}
