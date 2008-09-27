#include <cmath>

#include <list>
#include <vector>

// writing on a text file
#include <iostream>
#include <fstream>

// Boost includes
#include <boost/shared_ptr.hpp>


#include <ode/ode.h>

#include <breathe/breathe.h>

#include <breathe/util/cSmartPtr.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cOctree.h>
#include <breathe/math/cColour.h>
#include <breathe/math/geometry.h>

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>

#include <breathe/physics/physics.h>


/*// Wedge
float sPrefabVertsTriangle[ 6 ][ 3 ] = {
  { -0.5f , -0.5f , 0.5f },
  { 0.5f , 0.5f , 0.5f },
  { -0.5f , 0.5f , 0.5f },
  { -0.5f , -0.5f , -0.5f },
  { 0.5f , 0.5f , -0.5f },
  { -0.5f , 0.5f , -0.5f }
};

int sPrefabIndicesTriangle[ 6 ][ 3 ] = {
  { 0 , 3 , 1 },
  { 1 , 3 , 4 },
  { 3 , 0 , 5 },
  { 5 , 0 , 2 },
  { 2 , 1 , 5 },
  { 1 , 4 , 5 },
};*/



const int VertexCount = 5;
const int IndexCount = 12;

dVector3 Size;
dVector3 Vertices[VertexCount];
dTriIndex Indices[IndexCount];

const unsigned int uiDirectionX = 1;
const unsigned int uiDirectionY = 2;
const unsigned int uiDirectionZ = 3;

namespace breathe
{
  namespace physics
  {
    cPhysicsObject::cPhysicsObject() :
      bBody(true),
      bDynamic(true),
      body(NULL),
      geom(NULL)
    {
      position.Set(0.0f, 0.0f, 0.0f);
      v.Set(0.0f, 0.0f, 0.0f);
      fWeight = 1.0f;
      fRadius = 2.0f;
      m.LoadIdentity();
    }

		cPhysicsObject::~cPhysicsObject()
		{
			RemoveFromWorld();
		}

		void cPhysicsObject::RemoveFromWorld()
		{
			if (geom) {
				dGeomDestroy(geom);
				geom = NULL;
			}

			if (body) {
        dBodyDestroy(body);
				body = NULL;
			}
		}

#ifdef BUILD_PHYSICS_3D
    void cPhysicsObject::SetTrimeshSource(std::vector<float> &coords, std::vector<unsigned int> &indicies)
		{
			vCoords=coords;
			vIndicies=indicies;
		}

    void cPhysicsObject::InitCommon(const physvec_t& posOriginal, const physvec_t& rot)
		{
      math::cVec3 pos(posOriginal.x, posOriginal.y, posOriginal.z + fHeight);

			m.LoadIdentity();
			m.SetRotationZ(rot.z*math::cPI_DIV_180);

			dMatrix3 r;
			r[0] = m[0];		r[1] = m[4];		r[2] = m[8];		r[3] = 0;
			r[4] = m[1];		r[5] = m[5];		r[6] = m[9];		r[7] = 0;
			r[8] = m[2];		r[9] = m[6];		r[10] = m[10];	r[11] = 0;

			m.SetTranslation(pos);

      position = pos;

      dGeomSetPosition(geom, position.x, position.y, position.z);
			dGeomSetRotation(geom, r);

			if (bBody) {
				body = dBodyCreate(physics::GetWorld());
        dBodySetPosition(body, position.x, position.y, position.z);
				dBodySetRotation(body, r);
				dBodySetAutoDisableFlag(body, 1);

				dGeomSetBody(geom, body);
			}
		}

    void cPhysicsObject::CreateBox(const math::cVec3& pos, const math::cVec3& rot)
		{
      geom = dCreateBox(bDynamic ? physics::GetSpaceDynamic() : physics::GetSpaceStatic(),
				2.0f*fWidth, 2.0f*fLength, 2.0f*fHeight);

			InitCommon(pos, rot);

			if (bBody) {
				dMass mass;
				dMassSetBoxTotal(&mass, fWeight, 2.0f*fWidth, 2.0f*fLength, 2.0f*fHeight);
				dBodySetMass(body, &mass);
			}
    }

		void cPhysicsObject::CreateSphere(const math::cVec3& pos, const math::cVec3& rot)
		{
      geom = dCreateSphere(bDynamic ? physics::GetSpaceDynamic() : physics::GetSpaceStatic(), fRadius);

			InitCommon(pos, rot);

			if (bBody) {
				dMass mass;
				dMassSetSphereTotal(&mass, fWeight, 2.0f*fRadius);
				dBodySetMass(body, &mass);
			}
		}

    void cPhysicsObject::CreateCapsule(const math::cVec3& pos, const math::cVec3& rot)
		{
      geom = dCreateCapsule(bDynamic ? physics::GetSpaceDynamic() : physics::GetSpaceStatic(), fRadius, fLength);

			InitCommon(pos, rot);

			if (bBody) {
				dMass mass;
				dMassSetCapsuleTotal(&mass, fWeight, uiDirectionX, 2.0f*fRadius, fLength);
				dBodySetMass(body, &mass);
			}
		}

    void cPhysicsObject::CreateCylinder(const math::cVec3& pos, const math::cVec3& rot)
		{
      geom = dCreateCylinder(bDynamic ? physics::GetSpaceDynamic() : physics::GetSpaceStatic(), fRadius, fLength);

			InitCommon(pos, rot);

			if (bBody) {
				dMass mass;
				dMassSetCylinderTotal(&mass, fWeight, uiDirectionX, 2.0f*fRadius, fLength);
				dBodySetMass(body, &mass);
			}
    }

    void cPhysicsObject::CreateTrimesh(const std::vector<float>& coords, const std::vector<unsigned int>& indicies, const physvec_t& pos, const physvec_t& rot)
		{
			bBody = false;
			bDynamic = false;

			m.LoadIdentity();
			m.SetRotationZ(rot.z*math::cPI_DIV_180);

			dMatrix3 r;
			r[0] = m[0];		r[1] = m[4];		r[2] = m[8];		r[3] = 0;
			r[4] = m[1];		r[5] = m[5];		r[6] = m[9];		r[7] = 0;
			r[8] = m[2];		r[9] = m[6];		r[10] = m[10];	r[11] = 0;

			m.SetTranslation(pos);

      position = pos;

			//Trimeshes are static for the moment
			v[0]=0.0f;
			v[1]=0.0f;
			v[2]=0.0f;



			Size[0] = 500.0f;
			Size[1] = 500.0f;
			Size[2] = 0.0f;

			Vertices[0][0] = -Size[0];
			Vertices[0][1] = -Size[1];
			Vertices[0][2] = Size[2];

			Vertices[1][0] = Size[0];
			Vertices[1][1] = -Size[1];
			Vertices[1][2] = Size[2];

			Vertices[2][0] = Size[0];
			Vertices[2][1] = Size[1];
			Vertices[2][2] = Size[2];

			Vertices[3][0] = -Size[0];
			Vertices[3][1] = Size[1];
			Vertices[3][2] = Size[2];

			Vertices[4][0] = 0;
			Vertices[4][1] = 0;
			Vertices[4][2] = 0;

			Indices[0] = 0;
			Indices[1] = 1;
			Indices[2] = 4;

			Indices[3] = 1;
			Indices[4] = 2;
			Indices[5] = 4;

			Indices[6] = 2;
			Indices[7] = 3;
			Indices[8] = 4;

			Indices[9] = 3;
			Indices[10] = 0;
			Indices[11] = 4;

			dTriMeshDataID Data = dGeomTriMeshDataCreate();

      dGeomTriMeshDataBuildSimple(Data, (const dReal*)Vertices, VertexCount, Indices, IndexCount);

      geom = dCreateTriMesh(physics::GetSpaceStatic(), Data, NULL , NULL , NULL);

			dGeomSetBody(geom, 0);

      dGeomSetPosition(geom, position.x, position.y, position.z);
			dGeomSetRotation(geom, r);


			/*
      vertices_ = new D3DXVECTOR3[ vcount ];
      for (size_t i = 0; i < vcount; ++i ) {
        vertices_[i] = *(D3DXVECTOR3 const *)((int8_t *)verts + vstride * i);
      }
      indices_ = new int[ icount ];
      for (size_t i = 0; i < icount; ++i ) {
        indices_[i] = inds[i];
      }
      normals_ = new D3DXVECTOR3[ icount/3 ];
      for (size_t i = 0; i < icount; i += 3 ) {
        normals_[i/3] = NormalCalc( vertices_, &indices_[i] );
      }

			id_, vertices_, (int)sizeof(D3DXVECTOR3), (int)vcount,
      indices_, (int)icount, (int)sizeof(int)*3,
			normals_

			const void* Vertices, int VertexStride, int VertexCount,
      const void* Indices, int IndexCount, int TriStride,
      const void* Normals
      dGeomTriMeshDataBuildSingle1(Data,
				(dReal*)Vertices, (int)sizeof(D3DXVECTOR3), VertexCount,
        indices_, Indices, TriStride);
				//normals_ );*/


			//dGeomTriMeshDataBuildSingle( pTriMeshData , &m_Vertices[ 0 ] , 3 * sizeof(float) , m_Vertices.GetCount() , &m_Indices[ 0 ] , m_Indices.GetCount(), 3 * sizeof( int ) );
    }
#endif

		void cPhysicsObject::Update(sampletime_t currentTime)
		{
			if (bDynamic)
			{
				dReal *p0;
				dReal *r0;

				if (bBody) {
					p0=const_cast<dReal*>(dBodyGetPosition(body));
					r0=const_cast<dReal*>(dBodyGetRotation(body));
					const dReal *v0=dBodyGetLinearVel(body);
					//const dReal *a0=dBodyGetAngularVel(body);


					v[0]=v0[0];
					v[1]=v0[1];
					v[2]=v0[2];
				}
				else
				{
					p0=const_cast<dReal*>(dGeomGetPosition(geom));
					r0=const_cast<dReal*>(dGeomGetRotation(geom));

					//Trimeshes are static for the moment
					v[0]=0.0f;
					v[1]=0.0f;
					v[2]=0.0f;
				}

				m[0]  =     r0[0];
				m[1]  =     r0[4];
				m[2]  =     r0[8];
				m[3]  =     0;
				m[4]  =     r0[1];
				m[5]  =     r0[5];
				m[6]  =     r0[9];
				m[7]  =     0;
				m[8]  =     r0[2];
				m[9]  =     r0[6];
				m[10] =     r0[10];
				m[11] =     0;
				m[12] =     p0[0];
				m[13] =     p0[1];
				m[14] =     p0[2];
				m[15] =     1;
			}

			UpdateComponents();
		}

		void cPhysicsObject::UpdateComponents()
		{
      position = m.GetPosition();
    }

    void cPhysicsObject::DestroyBody()
    {
      if (body) {
        dBodyDestroy(body);
        body = NULL;
      }
    }

    void cPhysicsObject::DestroyGeom()
    {
      if (geom) {
        dGeomDestroy(geom);
        geom = NULL;
      }
    }

#if 1
		// *****************************************************************************************************
		// Upright Capsule
		// *****************************************************************************************************
		cUprightCapsule::cUprightCapsule()
			: cPhysicsObject()
		{

		}

		void cUprightCapsule::Update(sampletime_t currentTime)
		{
			// Stop the capsule from rotating forwards, backwards, left, right
			dBodySetAngularVel(GetBody(), 0.0f, 0.0f, 0.0f);
      dBodySetTorque(GetBody(), 0.0f, 0.0f, 0.0f);

			cPhysicsObject::Update(currentTime);
		}
#endif
  }
}
