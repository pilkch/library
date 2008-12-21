#ifndef ENTITY_H
#define ENTITY_H

#include <breathe/game/scenegraph.h>

namespace breathe
{
  class cUpdateDirtyEntitiesVisitor;

  class cEntityBase
  {
  public:
    friend class cUpdateDirtyEntitiesVisitor;

    cEntityBase();
    virtual ~cEntityBase() {}

    void SetSceneNode(scenegraph3d::cSceneNodeRef _pSceneNode);

    bool IsDirty() const { return bIsDirty; }
    void SetDirty() { bIsDirty = true; }

    void SetVisible(bool bVisible) { ASSERT(pSceneNode != nullptr); pSceneNode->SetVisible(bVisible); }
    void SetPosition(const math::cVec3& position) { ASSERT(pSceneNode != nullptr); pSceneNode->SetPosition(position); }
    void SetRotation(const math::cQuaternion& rotation) { ASSERT(pSceneNode != nullptr); pSceneNode->SetRotation(rotation); }

  protected:
    void Update(sampletime_t currentTime) { _Update(currentTime); }
    void SetNotDirty() { ASSERT(IsDirty()); bIsDirty = false; }

    scenegraph3d::cSceneNodeRef pSceneNode;

  private:
    virtual void _Update(sampletime_t currentTime) {}

    bool bIsDirty;
  };

  inline cEntityBase::cEntityBase() :
    bIsDirty(false)
  {
  }

  inline void cEntityBase::SetSceneNode(scenegraph3d::cSceneNodeRef _pSceneNode)
  {
    ASSERT(_pSceneNode != nullptr);
    pSceneNode = _pSceneNode;
  }

  typedef std::list<cEntityBase*> cEntityList;
  // Entities are stored in a flat std::list, so just iterator through each one

  class cUpdateDirtyEntitiesVisitor
  {
  public:
    void VisitDirtyEntitiesInThisList(sampletime_t currentTime, cEntityList::iterator iter, const cEntityList::iterator iterEnd);
  };



  namespace game
  {
    cEntityBase* GetEntityByName(const string_t& name);
  }
}

#endif // ENTITY_H
