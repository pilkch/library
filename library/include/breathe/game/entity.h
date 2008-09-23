#ifndef ENTITY_H
#define ENTITY_H

class cUpdateDirtyEntitiesVisitor;

class cEntityBase
{
public:
  friend class cUpdateDirtyEntitiesVisitor;

  cEntity();
  virtual ~cEntityBase() {}

  void SetSceneNode(cSceneNodeRef _pSceneNode);

  bool IsDirty() const { return bIsDirty; }
  void SetDirty() { bIsDirty = true; }

  void SetRotation(const math::cQuaternion& rotation);

protected:
  void Update(sampletime_t currentTime) { _Update(currentTime); }
  void SetNotDirty() { ASSERT(IsDirty()); bIsDirty = false; }

  cSceneNodeRef pSceneNode;

private:
  virtual void _Update(sampletime_t currentTime) {}

  bool bIsDirty;
};

inline cEntity::cEntity() :
  bIsDirty(false)
{
}

inline void cEntityBase::SetSceneNode(cSceneNodeRef _pSceneNode)
{
  ASSERT(_pSceneNode != nullptr);
  pSceneNode = _pSceneNode;
}

inline void cEntity::SetRotation(const math::cQuaternion& rotation)
{
  pNode->SetRotation(rotation);
}

typedef std::list<cEntity*> cEntityList;
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

#endif // ENTITY_H
