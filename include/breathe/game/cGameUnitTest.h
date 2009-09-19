#ifndef CGAMEUNITTEST_H
#define CGAMEUNITTEST_H

#include <breathe/util/app.h>

namespace breathe
{
  class cApplication;

#ifdef BUILD_DEBUG
  class cGameUnitTest
  {
  public:
    explicit cGameUnitTest(cApplication& app);
    ~cGameUnitTest();

    bool Init();
    bool Destroy();

    void Update(sampletime_t currentTime);
    void UpdatePhysics(sampletime_t currentTime);

    void PreRender(sampletime_t currentTime);
    void RenderScene(sampletime_t currentTime);
    void RenderScreenSpace(sampletime_t currentTime);

  private:
    cApplication& app;
  };
#endif
}

#endif // CGAMEUNITTEST_H
