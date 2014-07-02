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
      cLetterBox(size_t desiredWidth, size_t desiredHeight, size_t width, size_t height);

      const size_t desiredWidth;
      const size_t desiredHeight;
      float fDesiredRatio;

      float fRatio;

      size_t letterBoxedWidth;
      size_t letterBoxedHeight;
    };


    // ** Inlines

    // ** cLetterBox

    inline cLetterBox::cLetterBox(size_t _desiredWidth, size_t _desiredHeight, size_t width, size_t height) :
      desiredWidth(_desiredWidth),
      desiredHeight(_desiredHeight),
      fDesiredRatio(0.0f),
      fRatio(0.0f),
      letterBoxedWidth(0),
      letterBoxedHeight(0)
    {
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
