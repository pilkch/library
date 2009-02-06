#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <string>

#include <iostream>
#include <fstream>

#include <sstream>
#include <vector>
#include <map>
#include <list>
#include <set>

// Boost includes
#include <boost/shared_ptr.hpp>


#include <GL/GLee.h>


#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>


#ifdef BUILD_PHYSICS_3D
#include <ode/ode.h>
#endif

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/xml.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/geometry.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

#include <breathe/physics/physics.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>

#include <breathe/game/cLevel.h>

#include <breathe/game/cPlayer.h>
#include <breathe/game/cPetrolBowser.h>
#include <breathe/vehicle/cPart.h>
#include <breathe/vehicle/cWheel.h>
#include <breathe/vehicle/cSeat.h>
#include <breathe/vehicle/cVehicle.h>

breathe::cLevel* pLevel = NULL;

const unsigned int uiNodeNameDisplayTime = 100;

namespace breathe
{
  /*
  std::string s=breathe::filesystem::GetMD5(sFilename);*/

  cLevel::cLevel() :
    fWaterLevel(1.0f),
    pCurrentNode(nullptr),
    previousTime(0)
  {
    pRender->pLevel = this;

    uiDisplayNodeName = uiNodeNameDisplayTime;
  }

  cLevel::~cLevel()
  {
  }

  bool cLevel::LoadXML(const string_t& sNewFilename)
  {
    LOG<<"cLevel::LoadXML sNewFilename="<<sNewFilename<<std::endl;
    if (!breathe::filesystem::FindResourceFile(TEXT("levels/"), sNewFilename, sFilename)) {
      LOG<<"cLevel::LoadXML File "<<sNewFilename<<" not found in levels/"<<std::endl;
    }
    LOG<<"cLevel::LoadXML Resolved sFilename="<<sFilename<<std::endl;

    cLevelSpawn* pSpawn;

    {
      LOG.Success("Level", "cLevelNode::Load " + breathe::string::ToUTF8(sFilename));
      breathe::xml::cNode root(sFilename);

      breathe::xml::cNode::iterator iter(root);
      if (!iter.IsValid()) return breathe::BAD;

      iter.FindChild("level");
      if (iter.IsValid()) {
        iter.GetAttribute("fWaterLevel", fWaterLevel);

        iter.FirstChild();

        while (iter.IsValid()) {
          if ("nodes" == iter.GetName()) {
            iter.GetAttribute("fWidth", fNodeWidth);
            iter.GetAttribute("uiWidth", uiNodeWidth);
            iter.GetAttribute("uiHeight", uiNodeHeight);
            iter.GetAttribute("uiHeightMapPixelWidth", uiNodeHeightMapPixelWidth);
            iter.GetAttribute("uiHeightMapPixelHeight", uiNodeHeightMapPixelHeight);

            breathe::xml::cNode::iterator iterParent = iter;
              iter.FindChild("node");
              while (iter.IsValid()) {
                string_t sPath;
                if (iter.GetAttribute("path", sPath))
                  LoadNode(sPath);

                iter.Next("node");
              };
            iter = iterParent;
          } else if ("spawns" == iter.GetName()) {
            breathe::xml::cNode::iterator iterParent = iter;

            iter.FindChild("spawn");
            while (iter.IsValid()) {
              pSpawn = new cLevelSpawn;
              vSpawn.push_back(pSpawn);

              iter.GetAttribute("position", pSpawn->v3Position);
              iter.GetAttribute("rotation", pSpawn->v3Rotation);

              iter.Next("spawn");
            };

            iter = iterParent;
          }

          iter++;
        };

      }
    }

    return breathe::GOOD;
  }

  bool cLevel::Load(const string_t& sNewFilename)
  {
    bool bResult = breathe::GOOD;

    LoadXML(sNewFilename);


    // We don't have any spawns yet, add a default one
    if (vSpawn.empty()) {
      LOG.Error("Level", "No spawns defined");
      cLevelSpawn *p = new cLevelSpawn;
      p->v3Position = math::cVec3(0.0f, 0.0f, 0.0f);
      p->v3Rotation = math::cVec3(0.0f, 0.0f, 90.0f);
      vSpawn.push_back(p);
    }

    size_t i = 0;
    size_t n = vCubemap.size();

    if (0 == n) {
      LOG.Error("Level", "No cubemaps defined");
      //bResult = breathe::BAD;
    }
    else {
      for (i=0;i<n;i++)
        pRender->AddCubeMap(vCubemap[i]->sFilename);
    }

    return bResult;
  }

  void cLevel::LoadNode(const string_t& sNewFilename)
  {
    render::model::cStaticRef p = pRender->AddModel(TEXT("level/") + sNewFilename + TEXT("/mesh.3ds"));
    if (p != nullptr) {
      cLevelNode* pNode = new cLevelNode(this, TEXT("level/") + sNewFilename + TEXT("/"));
      pNode->Load();

      /*
      size_t i = 0;
      const size_t n = p->vCamera.size();
      for (i=0;i<n;i++) {
        LOG.Success("Level", "Spawn");

        vSpawn.push_back(new cLevelSpawn);
        cLevelSpawn* pSpawn=vSpawn.back();

        math::cVec3 cam=p->vCamera[i]->eye;
        math::cVec3 objPos=p->vCamera[i]->target;

        math::cVec3 objToCamProj(cam.x - objPos.x, 0.0f, cam.z - objPos.z);

        objToCamProj.Normalise();

        math::cVec3 v3out;

        // compute the angle
        float angleCosine = cam.DotProduct(objToCamProj);

        float a=acosf(angleCosine)*math::c180_DIV_PI;

        // perform the rotation. The if statement is used for stability reasons
        // if the lookAt and objToCamProj vectors are too close together then
        // |angleCosine| could be bigger than 1 due to lack of precision
        if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
          v3out.z=90.0f+a;
        else
          v3out.z=-90.0f;

        pSpawn->v3Rotation=v3out;
        pSpawn->v3Position=p->vCamera[i]->eye;
        LOG.Error("Level", "Upate in 3ds and remove this");
        pSpawn->v3Position.z=-1.5f;
      }*/

      vNode.push_back(pNode);
    } else LOG.Error("Node", "Mesh not found " + breathe::string::ToUTF8(sNewFilename));
  }

  void cLevel::LoadCubemap(const string_t& line)
  {
    vCubemap.push_back(new cLevelCubemap);

    cLevelCubemap* p = vCubemap.back();

    stringstream_t stm(line);
    stm >> p->sFilename;

    stm >> p->v3Position.x;
    stm >> p->v3Position.y;
    stm >> p->v3Position.z;
  }

  /*void cModel::staticMeshAddToWorld(int i, WORLD * world,
                                              COLLISIONPACKET * collisionPacket,
                                              float x, float y, float z)
  {
    if (i>-1 && i<animation.size()) {
      cModel_StaticMesh* m = &vStatic[i];
      if (nullptr == m) {
        LOGFAIL("Model", "vStatic failed");
        return;
      }

      int index=0;
      int f=0;
      const float ex=1.0f/collisionPacket->eRadius.x;
      const float ey=1.0f/collisionPacket->eRadius.y;
      const float ez=1.0f/collisionPacket->eRadius.z;

      x*=ex;
      y*=ey;
      z*=ez;

      TRI_T2_N3_V3 * t;
      TRI_COL temp;

      for (f=0;f<m->uiTriangles;f++) {
        t=&m->mesh[f];

        temp.p0.vx=t->p0.vx*ex+x;
        temp.p0.vy=t->p0.vy*ey+y;
        temp.p0.vz=t->p0.vz*ez+z;

        temp.p1.vx=t->p1.vx*ex+x;
        temp.p1.vy=t->p1.vy*ey+y;
        temp.p1.vz=t->p1.vz*ez+z;

        temp.p2.vx=t->p2.vx*ex+x;
        temp.p2.vy=t->p2.vy*ey+y;
        temp.p2.vz=t->p2.vz*ez+z;

        world->AddTriangle(&temp);
      }
    }
  }*/

  void cLevel::Update(sampletime_t currentTime)
  {
    //TODO: Calculate the current nodes
    if (pRender->pFrustum->eye.x > 0.0f && pRender->pFrustum->eye.y > 0.0f)
    {
      unsigned int currentX=static_cast<unsigned int>(pRender->pFrustum->eye.x/fNodeWidth);
      unsigned int currentY=static_cast<unsigned int>(pRender->pFrustum->eye.y/fNodeWidth);

      unsigned int uiCurrentNode = currentY * uiNodeWidth + currentX;

      if (uiCurrentNode < vNode.size()) {
        cLevelNode* pNode = vNode[uiCurrentNode];

        // Have we changed nodes in the last time step?
        if (pNode && pCurrentNode != pNode) {
          if (pCurrentNode && (pCurrentNode->sName != pNode->sName))
            uiDisplayNodeName = uiNodeNameDisplayTime;

          // Anything that must happen on the boundary happens here
          pCurrentNode = pNode;
        }
      }
    }

    {
      std::list<physics::cPhysicsObject *>::iterator iter=physics::begin();
      std::list<physics::cPhysicsObject *>::iterator end=physics::end();

      while(end != iter)
        (*iter++)->Update(currentTime);
    }


    if ((currentTime - previousTime)>1000.0f) {
      previousTime = currentTime;

      size_t n = vNode.size();
      size_t i = 0;
      for (i=0;i<n;i++)
        vNode[i]->Update(currentTime);
    }


    {
      vehicle::cVehicle *pVehicle = nullptr;

      std::list<vehicle::cVehicle*>::iterator iter = lVehicle.begin();
      const std::list<vehicle::cVehicle*>::iterator iterEnd = lVehicle.end();
      while (iter != iterEnd) {
        pVehicle = (*iter);
        pVehicle->Update(currentTime);
        iter++;
      };
    }

    // Update the counter that says whether to display the node name
    if (uiDisplayNodeName) uiDisplayNodeName--;
  }

  unsigned int cLevel::Render(sampletime_t currentTime)
  {
    unsigned int uiTriangles = 0;
    size_t i = 0;
    const size_t n = vNode.size();
    for (i=0;i<n;i++) uiTriangles += vNode[i]->Render();

    //n = vStatic.size();
    //for (i=0;i<n;i++)
    //  uiTriangles+=RenderStaticModel(vStatic[i]);));

    std::list<physics::cPhysicsObject*>::iterator iter = lPhysicsObject.begin();
    const std::list<physics::cPhysicsObject*>::iterator end = lPhysicsObject.end();
    while (end != iter) {
      glPushMatrix();
        glMultMatrixf((*iter)->m);

        uiTriangles += pRender->RenderStaticModel(static_cast<breathe::render::model::cStaticRef>((*iter++)->pModel));

        //pRender->SetMaterial();
      glPopMatrix();
    }

    return uiTriangles;
  }

  unsigned int cLevel::RenderVehicles(sampletime_t currentTime, vehicle::cVehicle *pOwnVehicle)
  {
    unsigned int uiTriangles = 0;
    vehicle::cVehicle* pVehicle = nullptr;

    std::list<vehicle::cVehicle*>::iterator iter = lVehicle.begin();
    const std::list<vehicle::cVehicle*>::iterator iterEnd = lVehicle.end();
    while (iter != iterEnd) {
      pVehicle = *iter;

      if ((nullptr == pOwnVehicle) || (pVehicle != pOwnVehicle)) {
        breathe::math::cColour colour(1.0f, 0.0f, 0.0f);

        glPushMatrix();
          glMultMatrixf(pVehicle->m);
          uiTriangles += pRender->RenderStaticModel(breathe::render::model::cStaticRef(pVehicle->pModel), colour);
        glPopMatrix();


        glPushMatrix();
          glMultMatrixf(pVehicle->lfWheel_->m);
          uiTriangles += pRender->RenderStaticModel(static_cast<breathe::render::model::cStaticRef>(pVehicle->lfWheel_->pModel));
        glPopMatrix();

        glPushMatrix();
          glMultMatrixf(pVehicle->lrWheel_->m);
          uiTriangles += pRender->RenderStaticModel(static_cast<breathe::render::model::cStaticRef>(pVehicle->lfWheel_->pModel));
        glPopMatrix();


        breathe::math::cMat4 r;
        r.SetRotationZ(breathe::math::cPI);

        glPushMatrix();
          glMultMatrixf(pVehicle->rfWheel_->m*r);
          uiTriangles += pRender->RenderStaticModel(static_cast<breathe::render::model::cStaticRef>(pVehicle->lfWheel_->pModel));
        glPopMatrix();

        glPushMatrix();
          glMultMatrixf(pVehicle->rrWheel_->m*r);
          uiTriangles += pRender->RenderStaticModel(static_cast<breathe::render::model::cStaticRef>(pVehicle->lfWheel_->pModel));
        glPopMatrix();
      }

      iter++;
    };

    return uiTriangles;
  }

  cLevelSpawn cLevel::GetSpawn()
  {
    size_t i = rand()%(vSpawn.size());
    return *vSpawn[i];
  }

  cLevelSpawn cLevel::GetSpawn(const math::cVec3& p)
  {
    //Have to have a spawn in the level before calling this
    const size_t n = vSpawn.size();
    if (n < 1) LOG.Error("Level", "No spawns defined");

    float d = (vSpawn[0]->v3Position - p).GetLength();
    float t = 0.0f;
    cLevelSpawn* s = vSpawn[0];

    for (size_t i = 1; i < n; i++) {
      t = (vSpawn[i]->v3Position - p).GetLength();
      if (t<d) {
        d = t;
        s = vSpawn[i];
      }
    }

    return *s;
  }

  void cLevel::AddPhysicsObject(physics::cPhysicsObject *d)
  {
    physics::AddPhysicsObject(d);
    lPhysicsObject.push_back(d);
  }

  void cLevel::RemovePhysicsObject(physics::cPhysicsObject *d)
  {
    physics::RemovePhysicsObject(d);
    lPhysicsObject.remove(d);
  }


  void cLevel::AddVehicle(vehicle::cVehicle *v)
  {
    lVehicle.push_back(v);
  }

  void cLevel::RemoveVehicle(vehicle::cVehicle *v)
  {
    lVehicle.remove(v);
  }

  render::cTextureRef cLevel::FindClosestCubeMap(math::cVec3 pos)
  {
    const size_t n = vCubemap.size();
    if (n == 0) return render::cTextureRef();

    cLevelCubemap* c = vCubemap[0];
    float f = (vCubemap[0]->v3Position - pos).GetLength();
    float a = 0.0f;

    size_t i = 0;
    for (i = 1; i < n; i++) {
      a = (vCubemap[i]->v3Position - pos).GetLength();
      if (a < f) {
        c = vCubemap[i];
        f = a;
      }
    }

    return pRender->GetCubeMap(c->sFilename);
  }

  vehicle::cVehicle* cLevel::FindClosestVehicle(math::cVec3 pos, float fMaxDistance)
  {
    if (lVehicle.empty()) return nullptr;


    float d = fMaxDistance;
    float t = fMaxDistance;
    breathe::vehicle::cVehicle* v = nullptr;

    std::list<vehicle::cVehicle *>::iterator iter = lVehicle.begin();
    const std::list<vehicle::cVehicle *>::iterator iterEnd = lVehicle.end();
    while (iter != iterEnd) {
      t = ((*iter)->position - pos).GetLength();
      if (t < d) {
        d = t;
        v = (*iter);
      }

      iter++;
    }

    return v;
  }


  cLevelSpawn::cLevelSpawn() :
    v3Position(0.0f, 0.0f, 0.0f),
    v3Rotation(1.0f, 0.0f, 0.0f)
  {

  }


  cLevelNode::cLevelNode(cLevel* p, const string_t& sNewFilename) :
    uiStatus(0),
    pLevel(p)
  {
    LOG<<"cLevelNode::cLevelNode sNewFilename="<<sNewFilename<<std::endl;
    if (!breathe::filesystem::FindResourceFile(TEXT("levels/") + sNewFilename, TEXT("mesh.3ds"), sFilename)) {
      LOG<<"cLevel::LoadXML File mesh.3ds not found in levels/"<<sNewFilename<<std::endl;
    }
    pModel = pRender->GetModel(sFilename);
  }

  void cLevelNode::Load()
  {
    LOG<<"cLevelNode::Load sFilename="<<sFilename<<"node.xml"<<std::endl;

    uiStatus = NODE_ACTIVE;


    xml::cNode root(sFilename + TEXT("node.xml"));

    xml::cNode::iterator iter(root);
    if (!iter.IsValid()) return;

    iter.FindChild("node");
    if (iter.IsValid()) {
      iter.GetAttribute("crc", sCRC);
      iter.GetAttribute("name", sName);
    }

    iter.FirstChild();
    while (iter.IsValid()) {
      if ("fog" == iter.GetName()) {
        iter.GetAttribute("colour", colourFog);
        iter.GetAttribute("distance", fFogDistance);
      } else if ("models" == iter.GetName()) {
        breathe::xml::cNode::iterator iterParent = iter;
          iter.FindChild("model");
          while (iter.IsValid()) {
            std::string sPath;
            if (iter.GetAttribute("path", sPath)) {
              cLevelModel* pModel = new cLevelModel;
              vModel.push_back(pModel);

              // Pre load the mesh for this model
              LOG<<"cLevelNode::Load Loading mesh for model "<<breathe::string::ToString_t(sPath) + TEXT("/mesh.3ds")<<std::endl;
              pModel->pModel = pRender->AddModel(breathe::string::ToString_t(sPath) + TEXT("/mesh.3ds"));

              iter.GetAttribute("position", pModel->position);

              math::cVec3 v;
              if (iter.GetAttribute("position", v)) pModel->m.SetTranslation(v);
            }

            iter.Next("model");
          };
        iter = iterParent;
      } else if ("cubemaps" == iter.GetName()) {
        breathe::xml::cNode::iterator iterParent = iter;
          iter.FindChild("cubemap");
          while (iter.IsValid()) {
            string_t sPath;
            if (iter.GetAttribute("texture", sPath)) pLevel->LoadCubemap(sPath);

            //TODO: position="10.0, 10.0, 0.0"

            iter.Next("cubemap");
          };
        iter = iterParent;
      }

      iter++;
    };

    LOG.Success("LevelNode", "Load returning");
  }

  void cLevelNode::Unload()
  {
    uiStatus = NODE_INACTIVE;
  }

  void cLevelNode::Update(sampletime_t currentTime)
  {
    if (NODE_INACTIVE!=uiStatus) uiStatus--;
    if (NODE_UNLOAD==uiStatus) Unload();
  }

  unsigned int cLevelNode::Render()
  {
    unsigned int uiTriangles = 0;

    uiTriangles += pRender->RenderStaticModel(pModel);

    std::vector<breathe::cLevelModel*>::iterator iter = vModel.begin();
    const std::vector<breathe::cLevelModel*>::iterator iterEnd = vModel.end();
    while (iter != iterEnd) {
      glPushMatrix();
        glMultMatrixf((*iter)->m);
        uiTriangles += pRender->RenderStaticModel((*iter)->pModel);
      glPopMatrix();

      iter++;
    }

    return uiTriangles;
  }
}
