#ifndef ENTITY_H
#define ENTITY_H

class cEntityBase
{
public:
  virtual ~cEntityBase() {}

  void SetSceneNode(cSceneNodeRef _pSceneNode);

  void Update(sampletime_t currentTime) { _Update(currentTime); }

protected:
  cSceneNodeRef pSceneNode;

private:
  virtual void _Update(sampletime_t currentTime) {}
};

inline void cEntityBase::SetSceneNode(cSceneNodeRef _pSceneNode)
{
  ASSERT(_pSceneNode != nullptr);
  pSceneNode = _pSceneNode;
}

namespace game
{
  cEntityBase* GetEntityByName(const string_t& name);
}

#endif // ENTITY_H
