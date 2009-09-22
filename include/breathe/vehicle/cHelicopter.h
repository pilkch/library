#ifndef CHELICOPTER_H
#define CHELICOPTER_H

#include <breathe/game/component.h>

namespace breathe
{
  namespace vehicle
  {
    class cVehicle
    {
    public:
      void Create(breathe::game::cGameObjectCollection& gameobjects, breathe::scenegraph3d::cGroupNodeRef pNode, breathe::render::cTextureFrameBufferObjectRef pCubeMapTexture);

      breathe::game::cGameObjectRef GetGameObject() { return pGameObject; }
      breathe::render::cTextureFrameBufferObjectRef GetCubeMapTexture() { return pCubeMapTexture; }

    private:
      breathe::game::cGameObjectRef pGameObject;
      breathe::render::cTextureFrameBufferObjectRef pCubeMapTexture;
    };

    inline void cVehicle::Create(breathe::game::cGameObjectCollection& gameobjects, breathe::scenegraph3d::cGroupNodeRef pNode, breathe::render::cTextureFrameBufferObjectRef _pCubeMapTexture)
    {
      LOG<<"cVehicle::Create"<<std::endl;
      pGameObject.reset(new breathe::game::cGameObject);


      breathe::game::cRenderComponent* pRenderComponent = new breathe::game::cRenderComponent(*pGameObject);
      pRenderComponent->SetSceneNode(pNode);
      pGameObject->AddComponent(breathe::game::COMPONENT_RENDERABLE, pRenderComponent);

      gameobjects.Add(pGameObject);


      pCubeMapTexture = _pCubeMapTexture;
    }



    class cVehicleFactory
    {
    public:
      //void CreateCar(breathe::physics::cWorld* pWorld, breathe::game::cGameObjectCollection& gameobjects, const spitfire::math::cVec3& position, const spitfire::math::cQuaternion& rotation, breathe::scenegraph3d::cGroupNodeRef pNode);
      //void CreatePlane(breathe::physics::cWorld* pWorld, breathe::game::cGameObjectCollection& gameobjects, const spitfire::math::cVec3& position, const spitfire::math::cQuaternion& rotation, breathe::scenegraph3d::cGroupNodeRef pNode);
      void CreateHelicopter(breathe::physics::cWorld* pWorld, breathe::game::cGameObjectCollection& gameobjects, breathe::scenegraph3d::cGroupNodeRef pNode, const spitfire::math::cVec3& position, const spitfire::math::cQuaternion& rotation, cVehicle& vehicle) const;
    };
  }
}

namespace breathe
{
  namespace game
  {
    // http://en.wikipedia.org/wiki/Helicopter
    // http://science.howstuffworks.com/helicopter.htm
    // http://science.howstuffworks.com/helicopter5.htm
    // Cyclic: Forward, back, left, right (For horizontal movement)
    // Collective: Accelerator and angle of attack of main rotor (For vertical movement)
    // Pedals: Angle of attack of rear rotor (For movement about the z axis)

    // We support
    // Increase/descrease speed of main rotor
    // Increase/descrease angle of attack of main rotor for left, right, forwards and backwards movement
    // Increase/descrease angle of attack of rear rotor

    // We do not support
    // Increase/descrease angle of attack of main rotor for increase/decrease thrust to move vertically without increasing engine speed
    // Probably a whole lot of other less obvious stuff

    class cVehicleHelicopter : public cVehicleBase
    {
    public:
      cVehicleHelicopter(cGameObject& object);

    private:
      virtual void _Update(sampletime_t currentTime);
    };
  }
}

#endif // CHELICOPTER_H
