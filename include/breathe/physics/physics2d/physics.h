#ifndef CPHYSICS2D_H
#define CPHYSICS2D_H


//TODO:
// 1) New version of box2d to download
// 2) http://www.box2d.org/wiki/index.php?title=Programming_FAQ#Game_Engine_Issues

// Explosion creator
/*class cCustomWorld : public b2World
{
public:
  cCustomWorld(b2AABB worldAABB, b2Vec2 gravity, bool doSleep) :
    b2World(worldAABB, gravity, doSleep)
  {
  }

  void explode(b2Vec2 position)
  {
    b2AABB aabb;
    b2Vec2 vMin(position - b2Vec2(100, 100));
    b2Vec2 vMax(position + b2Vec2(100, 100));
    aabb.minVertex = vMin;
    aabb.maxVertex = vMax;
    b2Shape* shapes;
    const size_t n = Query(aabb, &shapes, 100);
    for (size_t i = 0; i < n; i++) {
      b2Body& b = *shapes[i].GetBody();
      b2Vec2 fv = b.GetCenterPosition();
      fv -= position;
      fv.Normalize();
      fv *= 500000;
      b.WakeUp();
      b.ApplyForce(fv, b.GetCenterPosition());
    }
  }
};*/



namespace breathe
{
  namespace physics
  {
    void Init(float width, float height);

    float GetWidth();
    float GetHeight();

    b2World* GetWorld();

    cPhysicsObject* GetBorder0();
    cPhysicsObject* GetBorder1();
    cPhysicsObject* GetBorder2();
    cPhysicsObject* GetBorder3();

    bool CanSleep();
  }
}

#endif // CPHYSICS2D_H
