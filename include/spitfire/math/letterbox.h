#ifndef SPITFIRE_LETTERBOX_H
#define SPITFIRE_LETTERBOX_H

#include <spitfire/math/math.h>

namespace spitfire
{
  namespace math
  {
    // ** A simple class for calculating letter box dimensions

    class cLetterBox
    {
    public:
      cLetterBox(size_t width, size_t height);

      size_t desiredWidth;
      size_t desiredHeight;
      float fDesiredRatio;

      float fRatio;

      size_t letterBoxedWidth;
      size_t letterBoxedHeight;
    };


    // ** Inlines

    // ** cLetterBox

    inline cLetterBox::cLetterBox(size_t width, size_t height) :
      desiredWidth(0),
      desiredHeight(0),
      fDesiredRatio(0.0f),
      fRatio(0.0f),
      letterBoxedWidth(0),
      letterBoxedHeight(0)
    {
      desiredWidth = 1920;
      desiredHeight = 1080;
      fDesiredRatio = float(desiredWidth) / float(desiredHeight);

      fRatio = float(width) / float(height);

      // Apply letter boxing
      letterBoxedWidth = width;
      letterBoxedHeight = height;

      if (fRatio < fDesiredRatio) {
        // Taller (4:3, 16:10 for example)
        letterBoxedHeight = width / fDesiredRatio;
      } else {
        // Wider
        letterBoxedWidth = height * fDesiredRatio;
      }

      // Round up to the next even number
      if ((letterBoxedWidth % 2) != 0) letterBoxedWidth++;
      if ((letterBoxedHeight % 2) != 0) letterBoxedHeight++;
    }
  }
}

#endif // SPITFIRE_LETTERBOX_H
