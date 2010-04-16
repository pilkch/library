#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include <spitfire/math/math.h>
#include <spitfire/math/units.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>

#include <breathe/render/cContext.h>

namespace breathe
{
  namespace brightness
  {
    // We attempt to simulate the automatic pupil dilation of the human eye.  Poorly.
    // We use big words like "apeture" to make it look like we have some amount of accuracy and give the impression we know what we are doing.

    // From Wikipedia:
    // The retina has a static contrast ratio of around 100:1 (about 6 1/2 f-stops). As soon as the eye moves (saccades) it re-adjusts
    // its exposure both chemically and by adjusting the iris. Initial dark adaptation takes place in approximately four seconds[citation needed]
    // of profound, uninterrupted darkness; full adaptation through adjustments in retinal chemistry (the Purkinje effect) are mostly complete in
    // thirty minutes[citation needed]. Hence, a dynamic contrast ratio of about 1,000,000:1 (about 20 f-stops) is possible. The process is nonlinear
    // and multifaceted, so an interruption by light merely starts the adaptation process over again. Full adaptation is dependent on good blood flow;
    // thus dark adaptation may be hampered by poor circulation, and vasoconstrictors like alcohol or tobacco.
    // The entrance pupil is typically about 4 mm in diameter, although it can range from 2 mm (f/8.3) in a brightly lit place to 8 mm (f/2.1) in the dark.
    // The pupil gets wider in the dark but narrower in light. When narrow, the diameter is 3 to 4 millimeters. In the dark it will be the same at first,
    // but will approach the maximum distance for a wide pupil 5 to 9 mm. In any human age group there is however considerable variation in maximal pupil size.
    // For example, at the peak age of 15, the dark-adapted pupil can vary from 5 mm to 9 mm with different individuals.
    // The pupil also dilates in extreme psychical situations (e.g., fear) or contact of a sensory nerve, such as pain.
    // The pupil is an aperture, allowing light to travel to the retina.

    class cHumanEyeExposureControl
    {
    public:
      cHumanEyeExposureControl();

      float_t GetPerceivedBrightness0To1() const;

      void Update(sampletime_t currentTime, float_t fSceneBrightness0To1);

    private:
      // How much light does the eye let in?
      float_t fEyeApeture0To1;
      float_t fSceneBrightness0To1;

      // This is just the amount of dilation of the pupil in response to the brightness in the area
      // If fPupilDilation0To1 is approximately equal to fFocalPointBrightness0To1 then the scene will look normal.
      // If fPupilDilation0To1 is greater than fFocalPointBrightness0To1 then the scene will look normal.
      // If fPupilDilation0To1 is approximately equal to fFocalPointBrightness0To1 then the scene will look normal.
      //float_t fPupilDilation0To1;
      //float_t fFocalPointBrightness0To1;
    };
  }
}

#endif // BRIGHTNESS_H

