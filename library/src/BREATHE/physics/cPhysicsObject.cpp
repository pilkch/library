#include <cmath>

#include <list>
#include <vector>

// writing on a text file
#include <iostream>
#include <fstream>


#include <ode/ode.h>


#include <BREATHE/cBreathe.h>

#include <BREATHE/MATH/cMath.h>
#include <BREATHE/MATH/cVec2.h>
#include <BREATHE/MATH/cVec3.h>
#include <BREATHE/MATH/cVec4.h>
#include <BREATHE/MATH/cMat4.h>
#include <BREATHE/MATH/cPlane.h>
#include <BREATHE/MATH/cQuaternion.h>
#include <BREATHE/MATH/cFrustum.h>
#include <BREATHE/MATH/cOctree.h>
#include <BREATHE/MATH/cColour.h>

#include <BREATHE/UTIL/cBase.h>
#include <BREATHE/RENDER/MODEL/cMesh.h>
#include <BREATHE/RENDER/MODEL/cModel.h>

#include <BREATHE/PHYSICS/cPhysics.h>
#include <BREATHE/PHYSICS/cContact.h>
#include <BREATHE/PHYSICS/cRayCast.h>
#include <BREATHE/PHYSICS/cPhysicsObject.h>


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



const int VertexCount=5;
const int IndexCount=12;

dVector3 Size;
dVector3 Vertices[VertexCount];
int Indices[IndexCount];

const unsigned int uiDirectionX = 1;
const unsigned int uiDirectionY = 2;
const unsigned int uiDirectionZ = 3;

namespace BREATHE
{
	namespace PHYSICS
	{	
		cPhysicsObject::cPhysicsObject()
			: cObject()
		{
			bBody=true;
			bDynamic=true;

			body=NULL;
			geom=NULL;

			

			fWeight=1.0f;
			fRadius=2.0f;



			p[0]=0;
			p[1]=0;
			p[2]=0;

			v[0]=0;
			v[1]=0;
			v[2]=0;

			m.LoadIdentity();

			pModel=NULL;
		}

		cPhysicsObject::~cPhysicsObject()
		{
			RemoveFromWorld();
		}

		void cPhysicsObject::RemoveFromWorld()
		{
			if(geom) {
				dGeomDestroy(geom);
				geom = NULL;
			}

			if(body) {
        dBodyDestroy(body);
				body = NULL;
			}
		}
		
		void cPhysicsObject::SetTrimeshSource(std::vector<float> &coords, std::vector<unsigned int> &indicies)
		{
			vCoords=coords;
			vIndicies=indicies;
		}

		void cPhysicsObject::InitCommon(MATH::cVec3& pos, MATH::cVec3& rot)
		{
			pos.z+=fHeight;

			m.LoadIdentity();
			m.SetRotationZ(rot.z*MATH::cPI_DIV_180);

			dMatrix3 r;
			r[0] = m[0];		r[1] = m[4];		r[2] = m[8];		r[3] = 0;
			r[4] = m[1];		r[5] = m[5];		r[6] = m[9];		r[7] = 0;
			r[8] = m[2];		r[9] = m[6];		r[10] = m[10];	r[11] = 0;

			m.SetTranslation(pos);

			p=pos;

			dGeomSetPosition(geom, p.x, p.y, p.z);
			dGeomSetRotation(geom, r);

			if(bBody)
			{
				body = dBodyCreate(PHYSICS::world);
				dBodySetPosition(body, p.x, p.y, p.z);
				dBodySetRotation(body, r);
				dBodySetAutoDisableFlag(body, 1);

				dGeomSetBody(geom, body);
			}
		}
		
		void cPhysicsObject::CreateBox(MATH::cVec3 pos, MATH::cVec3 rot)
		{
			geom = dCreateBox(bDynamic ? PHYSICS::spaceDynamic : PHYSICS::spaceStatic, 
				2.0f*fWidth, 2.0f*fLength, 2.0f*fHeight);
			
			InitCommon(pos, rot);

			if(bBody)
			{
				dMass mass;
				dMassSetBoxTotal(&mass, fWeight, 2.0f*fWidth, 2.0f*fLength, 2.0f*fHeight);
				dBodySetMass(body, &mass);
			}
		}
		
		void cPhysicsObject::CreateSphere(MATH::cVec3 pos, MATH::cVec3 rot)
		{
			geom = dCreateSphere(bDynamic ? PHYSICS::spaceDynamic : PHYSICS::spaceStatic, fRadius);

			InitCommon(pos, rot);

			if(bBody)
			{
				dMass mass;
				dMassSetSphereTotal(&mass, fWeight, 2.0f*fRadius);
				dBodySetMass(body, &mass);
			}
		}

		void cPhysicsObject::CreateCapsule(MATH::cVec3 pos, MATH::cVec3 rot)
		{
			geom = dCreateCapsule(bDynamic ? PHYSICS::spaceDynamic : PHYSICS::spaceStatic, fRadius, fLength);
		
			InitCommon(pos, rot);

			if(bBody)
			{
				dMass mass;
				dMassSetCapsuleTotal(&mass, fWeight, uiDirectionX, 2.0f*fRadius, fLength);
				dBodySetMass(body, &mass);
			}
		}

		void cPhysicsObject::CreateCylinder(MATH::cVec3 pos, MATH::cVec3 rot)
		{
			geom = dCreateCylinder(bDynamic ? PHYSICS::spaceDynamic : PHYSICS::spaceStatic, fRadius, fLength);

			InitCommon(pos, rot);

			if(bBody)
			{
				dMass mass;
				dMassSetCylinderTotal(&mass, fWeight, uiDirectionX, 2.0f*fRadius, fLength);
				dBodySetMass(body, &mass);
			}
		}

		void cPhysicsObject::CreateTrimesh(MATH::cVec3 pos, MATH::cVec3 rot)
		{
			bBody=false;
			bDynamic=false;

			m.LoadIdentity();
			m.SetRotationZ(rot.z*MATH::cPI_DIV_180);

			dMatrix3 r;
			r[0] = m[0];		r[1] = m[4];		r[2] = m[8];		r[3] = 0;
			r[4] = m[1];		r[5] = m[5];		r[6] = m[9];		r[7] = 0;
			r[8] = m[2];		r[9] = m[6];		r[10] = m[10];	r[11] = 0;

			m.SetTranslation(pos);

			p=pos;

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

			dGeomTriMeshDataBuildSimple(Data, (dReal*)Vertices, VertexCount, Indices, IndexCount);
			
			geom = dCreateTriMesh(PHYSICS::spaceStatic, Data, NULL , NULL , NULL);

			dGeomSetBody(geom, 0);

			dGeomSetPosition(geom, p.x, p.y, p.z);
			dGeomSetRotation(geom, r);


			/*
      vertices_ = new D3DXVECTOR3[ vcount ];
      for( size_t i = 0; i < vcount; ++i ) {
        vertices_[i] = *(D3DXVECTOR3 const *)((int8_t *)verts + vstride * i);
      }
      indices_ = new int[ icount ];
      for( size_t i = 0; i < icount; ++i ) {
        indices_[i] = inds[i];
      }
      normals_ = new D3DXVECTOR3[ icount/3 ];
      for( size_t i = 0; i < icount; i += 3 ) {
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

		void cPhysicsObject::Update(float fCurrentTime)
		{
			if(bDynamic)
			{
				dReal *p0;
				dReal *r0;

				if(bBody)
				{
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
			p=m.GetPosition();
		}


		// *****************************************************************************************************
		// Upright Capsule
		// *****************************************************************************************************
		cUprightCapsule::cUprightCapsule()
			: cPhysicsObject()
		{

		}
		
		void cUprightCapsule::Update(float fCurrentTime)
		{
			// Stop the capsule from rotating forwards, backwards, left, right
			dBodySetAngularVel(body, 0.0f, 0.0f, 0.0f);
			dBodySetTorque(body, 0.0f, 0.0f, 0.0f);
			
			cPhysicsObject::Update(fCurrentTime);
		}
	}
}
