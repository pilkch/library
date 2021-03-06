// Breathe headers
#include <breathe/game/entity.h>

void cUpdateDirtyEntitiesVisitor::VisitDirtyEntitiesInThisList(durationms_t currentTime, cEntityList::iterator iter, const cEntityList::iterator iterEnd)
{
  while (iter != iterEnd) {
    cEntity& entity = *iter;
    if (entity.IsDirty()) {
      entity.Update(currentTime);
      entity.SetNotDirty();
    }
  }
}

namespace game
{
  std::map<string_t, cEntityBase*> entities;

  cEntityBase* GetEntityByName(const string_t& name)
  {
    std::map<string_t, cEntityBase*>::iterator iter = entities.find(name);
    return iter->second;
  }
}

class cPhysicsEntity : public cEntityBase
{
public:
  cPhysicsEntity();

  void SetPhysicsObject(physics::cPhysicsObject* pPhysicsObject);

private:
  virtual void _Update(durationms_t currentTime);

  physics::cPhysicsObject* _pPhysicsObject;
};

void cPhysicsEntity::SetPhysicsObject(physics::cPhysicsObject* _pPhysicsObject)
{
  ASSERT(_pPhysicsObject != nullptr);
  pPhysicsObject = _pPhysicsObject;
}

void cPhysicsEntity::_Update(durationms_t currentTime)
{
  ASSERT(pSceneNode != nullptr);
  ASSERT(pPhysicsObject != nullptr);

  pSceneNode->SetLocalPosition(pPhysicsObject->GetPosition());
  pSceneNode->SetLocalRotation(pPhysicsObject->GetRotation());
}
