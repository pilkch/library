// Standard headers
#include <cstring>
#include <iostream>

// libvoodoomm headers
#include <libvoodoomm/cTextureAtlas.h>

namespace voodoo
{
  
cTextureAtlasEntry::cTextureAtlasEntry() :
  x(0),
  y(0),
  width(0),
  height(0)
{
}


cTextureAtlas::cTextureAtlas() :
  currentX(0),
  currentY(0),
  currentTallestInRow(0)
{
}

void cTextureAtlas::Init(size_t width, size_t height, voodoo::PIXELFORMAT pixelFormat)
{
  textureAtlas.CreateEmptyImage(width, height, pixelFormat);
}

bool cTextureAtlas::AddImage(const std::string& textureFilePath)
{
  std::cout<<"cTextureAtlas::AddImage Loading image \""<<textureFilePath<<"\""<<std::endl;

  voodoo::cImage image;
  if (!image.LoadFromFile(textureFilePath)) {
    std::cout<<"cTextureAtlas::AddImage Error loading texture from file \""<<textureFilePath<<"\""<<std::endl;
    return false;
  }

  return AddImageInternal(textureFilePath, image);
}

bool cTextureAtlas::AddImageResizeNearestNeighbour(const std::string& textureFilePath, size_t cellWidthAndHeight)
{
  std::cout<<"cTextureAtlas::AddImage Loading image \""<<textureFilePath<<"\""<<std::endl;

  voodoo::cImage image;
  if (!image.LoadFromFile(textureFilePath)) {
    std::cout<<"cTextureAtlas::AddImage Error loading texture from file \""<<textureFilePath<<"\""<<std::endl;
    return false;
  }

  voodoo::cImage imageResized;
  imageResized.CreateFromImageResizeNearestNeighbour(image, cellWidthAndHeight, cellWidthAndHeight);

  return AddImageInternal(textureFilePath, imageResized);
}

bool cTextureAtlas::AddImageInternal(const std::string& textureFilePath, const voodoo::cImage& image)
{
  const size_t atlasWidth = textureAtlas.GetWidth();
  const size_t atlasHeight = textureAtlas.GetHeight();

  const size_t imageWidth = image.GetWidth();
  const size_t imageHeight = image.GetHeight();
  if ((imageWidth > atlasWidth) || (imageHeight > atlasHeight)) {
    std::cout<<"cTextureAtlas::AddImageInternal Error texture "<<imageWidth<<"x"<<imageHeight<<" is too large to fit in the atlas "<<atlasWidth<<"x"<<atlasHeight<<std::endl;
    return false;
  }

  std::cout<<"cTextureAtlas::AddImageInternal loaded texture"<<std::endl;
  // Try to place the image on this row
  if (imageWidth > (atlasWidth - currentX)) {
    // Move to the next row
    currentX = 0;
    currentY += currentTallestInRow;
    if (currentY > atlasHeight) currentY = atlasHeight;
    currentTallestInRow = 0;
  }

  // Try to place the image again, if it doesn't fit now it is never going to fit
  if ((imageWidth > (atlasWidth - currentX)) || (imageHeight > (atlasHeight - currentY))) {
    std::cout<<"cTextureAtlas::AddImageInternal Error no room left in texture atlas for texture \""<<textureFilePath<<"\""<<std::endl;
    return false;
  }

  std::cout<<"cTextureAtlas::AddImageInternal Adding entry"<<std::endl;
  // We found a place for the texture, so add en entry for it
  cTextureAtlasEntry entry;
  entry.x = currentX;
  entry.y = currentY;
  entry.width = imageWidth;
  entry.height = imageHeight;
  textureAtlasEntries.push_back(entry);

  std::cout<<"cTextureAtlas::AddImageInternal Copying image "<<imageWidth<<"x"<<imageHeight<<" into texture atlas "<<atlasWidth<<"x"<<atlasHeight<<" at point "<<currentX<<","<<currentY<<std::endl;
  
  assert(image.GetPixelFormat() == textureAtlas.GetPixelFormat());
  // Copy the image to the texture atlas
  //uint8_t* pTextureAtlasBuffer = textureAtlas.GetPointerToBuffer() + (currentY * textureAtlas.GetBytesPerRow()) + (currentX * textureAtlas.GetBytesPerPixel());
  uint8_t* pTextureAtlasBuffer = textureAtlas.GetPointerToBuffer();
  assert(pTextureAtlasBuffer != nullptr);
  const uint8_t* pImageBuffer = image.GetPointerToBuffer();
  assert(pImageBuffer != nullptr);
  for (size_t y = 0; y < imageHeight; y++) {
    //uint8_t* pDest = pTextureAtlasBuffer + (y * textureAtlas.GetBytesPerRow());
    uint8_t* pDest = pTextureAtlasBuffer + (currentY * textureAtlas.GetBytesPerRow()) + (currentX * textureAtlas.GetBytesPerPixel()) + (y * textureAtlas.GetBytesPerRow());
    const uint8_t* pSrc = pImageBuffer + (y * image.GetBytesPerRow());
    memcpy(pDest, pSrc, image.GetBytesPerRow());
  }

  // Move to the next position
  currentX += imageWidth;
  if (imageHeight > currentTallestInRow) currentTallestInRow = imageHeight;

  std::cout<<"cTextureAtlas::AddImageInternal returning"<<std::endl;

  return true;
}

}
