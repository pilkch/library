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

// Boost headers
#include <boost/smart_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>


#include <GL/GLee.h>


#include <SDL/SDL.h>
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

    return bResult;
  }

  void cLevel::LoadNode(const string_t& sNewFilename)
  {
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
    /*//TODO: Calculate the current nodes
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
    }*/

    if ((currentTime - previousTime) > 1000.0f) {
      previousTime = currentTime;

      const size_t n = vNode.size();
      for (size_t i = 0; i < n; i++) vNode[i]->Update(currentTime);
    }


    {
      vehicle::cVehicleRef pVehicle;

      std::list<vehicle::cVehicleRef>::iterator iter = lVehicle.begin();
      const std::list<vehicle::cVehicleRef>::iterator iterEnd = lVehicle.end();
      while (iter != iterEnd) {
        pVehicle = *iter;
        //pVehicle->Update(currentTime);
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

    return uiTriangles;
  }

  unsigned int cLevel::RenderVehicles(sampletime_t currentTime, vehicle::cVehicleRef pOwnVehicle)
  {
    unsigned int uiTriangles = 0;
    /*vehicle::cVehicleRef pVehicle;

    std::list<vehicle::cVehicleRef>::iterator iter = lVehicle.begin();
    const std::list<vehicle::cVehicleRef>::iterator iterEnd = lVehicle.end();
    while (iter != iterEnd) {
      pVehicle = *iter;

      if ((nullptr == pOwnVehicle) || (pVehicle != pOwnVehicle)) {
        breathe::math::cColour colour(1.0f, 0.0f, 0.0f);

        glPushMatrix();
          {
            spitfire::math::cMat4 mat(pVehicle->rotation.GetMatrix());
            mat.SetTranslationPart(pVehicle->position);
            glMultMatrixf(mat.GetOpenGLMatrix());
          }
          uiTriangles += pRender->RenderStaticModel(breathe::render::model::cStaticRef(pVehicle->pModel), colour);
        glPopMatrix();


        glPushMatrix();
          {
            spitfire::math::cMat4 mat(pVehicle->lfWheel_->rotation.GetMatrix());
            mat.SetTranslationPart(pVehicle->lfWheel_->position);
            glMultMatrixf(mat.GetOpenGLMatrix());
          }
          uiTriangles += pRender->RenderStaticModel(static_cast<breathe::render::model::cStaticRef>(pVehicle->lfWheel_->pModel));
        glPopMatrix();

        glPushMatrix();
          {
            spitfire::math::cMat4 mat(pVehicle->lrWheel_->rotation.GetMatrix());
            mat.SetTranslationPart(pVehicle->lrWheel_->position);
            glMultMatrixf(mat.GetOpenGLMatrix());
          }
          uiTriangles += pRender->RenderStaticModel(static_cast<breathe::render::model::cStaticRef>(pVehicle->lfWheel_->pModel));
        glPopMatrix();


        breathe::math::cMat4 r;
        r.SetRotationZ(breathe::math::cPI);

        glPushMatrix();
          {
            spitfire::math::cMat4 mat(pVehicle->rfWheel_->rotation.GetMatrix());
            mat.SetTranslationPart(pVehicle->rfWheel_->position);
            glMultMatrixf((mat * r).GetOpenGLMatrix());
          }
          uiTriangles += pRender->RenderStaticModel(static_cast<breathe::render::model::cStaticRef>(pVehicle->lfWheel_->pModel));
        glPopMatrix();

        glPushMatrix();
          {
            spitfire::math::cMat4 mat(pVehicle->rrWheel_->rotation.GetMatrix());
            mat.SetTranslationPart(pVehicle->rrWheel_->position);
            glMultMatrixf((mat * r).GetOpenGLMatrix());
          }
          uiTriangles += pRender->RenderStaticModel(static_cast<breathe::render::model::cStaticRef>(pVehicle->lfWheel_->pModel));
        glPopMatrix();
      }

      iter++;
    };*/

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

  void cLevel::AddVehicle(vehicle::cVehicleRef v)
  {
    lVehicle.push_back(v);
  }

  void cLevel::RemoveVehicle(vehicle::cVehicleRef v)
  {
    lVehicle.remove(v);
  }

  vehicle::cVehicleRef cLevel::FindClosestVehicle(math::cVec3 pos, float fMaxDistance)
  {
    breathe::vehicle::cVehicleRef v;

    if (lVehicle.empty()) return v;

    /*float d = fMaxDistance;
    float t = fMaxDistance;

    std::list<vehicle::cVehicleRef>::iterator iter = lVehicle.begin();
    const std::list<vehicle::cVehicleRef>::iterator iterEnd = lVehicle.end();
    while (iter != iterEnd) {
      t = ((*iter)->position - pos).GetLength();
      if (t < d) {
        d = t;
        v = (*iter);
      }

      iter++;
    }*/

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
            }

            iter.Next("model");
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

    return uiTriangles;
  }
}
