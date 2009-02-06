#ifndef CMODEL_H
#define CMODEL_H

#include <breathe/breathe.h>

#include <breathe/util/base.h>

namespace breathe
{
  namespace render
  {
    namespace model
    {
      // Base class for cModel_Animation, cModel_Roam, cModel_Static
      // Doesn't actually contain cMesh objects, but the idea is that you inherit this class and then
      // either add a cMesh *pMesh or std::vector<cMesh *>vMesh in which to store data
      // Not including this means that we can also override this object with for example cBillboard which doesn't
      // have to contain an actual mesh, you can specify your own coords etc.

      class cModel : public cRenderable
      {
      public:
        cModel();
        ~cModel();

        virtual int Load(const string_t& sFilename)=0;

        unsigned int uiTriangles;
      };

      typedef cSmartPtr<cModel> cModelRef;
    }
  }
}

#endif // CMODEL_H
