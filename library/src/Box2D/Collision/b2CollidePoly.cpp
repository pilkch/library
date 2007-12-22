/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include <Box2D/Collision/b2Collision.h>
#include <Box2D/Collision/b2Shape.h>

struct ClipVertex
{
	b2Vec2 v;
	b2ContactID id;
};

static int32 ClipSegmentToLine(ClipVertex vOut[2], ClipVertex vIn[2],
					  const b2Vec2& normal, float32 offset)
{
	// Start with no output points
	int32 numOut = 0;

	// Calculate the distance of end points to the line
	float32 distance0 = b2Dot(normal, vIn[0].v) - offset;
	float32 distance1 = b2Dot(normal, vIn[1].v) - offset;

	// If the points are behind the plane
	if (distance0 <= 0.0f) vOut[numOut++] = vIn[0];
	if (distance1 <= 0.0f) vOut[numOut++] = vIn[1];

	// If the points are on different sides of the plane
	if (distance0 * distance1 < 0.0f)
	{
		// Find intersection point of edge and plane
		float32 interp = distance0 / (distance0 - distance1);
		vOut[numOut].v = vIn[0].v + interp * (vIn[1].v - vIn[0].v);
		if (distance0 > 0.0f)
		{
			vOut[numOut].id = vIn[0].id;
		}
		else
		{
			vOut[numOut].id = vIn[1].id;
		}
		++numOut;
	}

	return numOut;
}

static float32 EdgeSeparation(const b2PolyShape* poly1, int32 edge1, const b2PolyShape* poly2)
{
	int32 count1 = poly1->m_vertexCount;
	const b2Vec2* vert1s = poly1->m_vertices;
	int32 count2 = poly2->m_vertexCount;
	const b2Vec2* vert2s = poly2->m_vertices;

	// Get the vertices associated with edge1.
	int32 vertexIndex11 = edge1;
	int32 vertexIndex12 = edge1 + 1 == count1 ? 0 : edge1 + 1;

	// Get the normal of edge1.
	b2Vec2 normalLocal1 = b2Cross(vert1s[vertexIndex12] - vert1s[vertexIndex11], 1.0f);
	normalLocal1.Normalize();
	b2Vec2 normal = b2Mul(poly1->m_R, normalLocal1);
	b2Vec2 normalLocal2 = b2MulT(poly2->m_R, normal);

	// Find support vertex on poly2 for -normal.
	int32 vertexIndex2 = 0;
	float32 minDot = FLT_MAX;
	for (int32 i = 0; i < count2; ++i)
	{
		float32 dot = b2Dot(vert2s[i], normalLocal2);
		if (dot < minDot)
		{
			minDot = dot;
			vertexIndex2 = i;
		}
	}

	b2Vec2 v1 = poly1->m_position + b2Mul(poly1->m_R, vert1s[vertexIndex11]);
	b2Vec2 v2 = poly2->m_position + b2Mul(poly2->m_R, vert2s[vertexIndex2]);
	float32 separation = b2Dot(v2 - v1, normal);
	return separation;
}

// Find the max separation between poly1 and poly2 using face normals
// from poly1.
static float32 FindMaxSeparation(int32* edge, const b2PolyShape* poly1, const b2PolyShape* poly2)
{
	int32 count1 = poly1->m_vertexCount;
	const b2Vec2* vert1s = poly1->m_vertices;

	// Vector pointing from the origin of poly1 to the origin of poly2.
	b2Vec2 d = poly2->m_position - poly1->m_position;
	b2Vec2 dLocal1 = b2MulT(poly1->m_R, d);

	// Get support vertex as a hint for our search
	int32 vertexIndex1 = 0;
	float32 maxDot = -FLT_MAX;
	for (int32 i = 0; i < count1; ++i)
	{
		float32 dot = b2Dot(vert1s[i], dLocal1);
		if (dot > maxDot)
		{
			maxDot = dot;
			vertexIndex1 = i;
		}
	}

	// Check the separation for the edges straddling the vertex.
	int32 prevFaceIndex = vertexIndex1 - 1 >= 0 ? vertexIndex1 - 1 : count1 - 1;
	float32 sPrev = EdgeSeparation(poly1, prevFaceIndex, poly2);
	if (sPrev > 0.0f)
	{
		return sPrev;
	}

	int32 nextFaceIndex = vertexIndex1;
	float32 sNext = EdgeSeparation(poly1, nextFaceIndex, poly2);
	if (sNext > 0.0f)
	{
		return sNext;
	}

	// Find the best edge and the search direction.
	int32 bestFaceIndex;
	float32 bestSeparation;
	int32 increment;
	if (sPrev > sNext)
	{
		increment = -1;
		bestFaceIndex = prevFaceIndex;
		bestSeparation = sPrev;
	}
	else
	{
		increment = 1;
		bestFaceIndex = nextFaceIndex;
		bestSeparation = sNext;
	}

	for ( ; ; )
	{
		int32 edgeIndex;
		if (increment == -1)
			edgeIndex = bestFaceIndex - 1 >= 0 ? bestFaceIndex - 1 : count1 - 1;
		else
			edgeIndex = bestFaceIndex + 1 < count1 ? bestFaceIndex + 1 : 0;

		float32 separation = EdgeSeparation(poly1, edgeIndex, poly2);
		if (separation > 0.0f)
		{
			return separation;
		}

		if (separation > bestSeparation)
		{
			bestFaceIndex = edgeIndex;
			bestSeparation = separation;
		}
		else
		{
			break;
		}
	}

	*edge = bestFaceIndex;
	return bestSeparation;
}

static void FindIncidentEdge(ClipVertex c[2], const b2PolyShape* poly1, int32 edge1, const b2PolyShape* poly2)
{
	int32 count1 = poly1->m_vertexCount;
	const b2Vec2* vert1s = poly1->m_vertices;
	int32 count2 = poly2->m_vertexCount;
	const b2Vec2* vert2s = poly2->m_vertices;

	// Get the vertices associated with edge1.
	int32 vertex11 = edge1;
	int32 vertex12 = edge1 + 1 == count1 ? 0 : edge1 + 1;

	// Get the normal of edge1.
	b2Vec2 normal1Local1 = b2Cross(vert1s[vertex12] - vert1s[vertex11], 1.0f);
	normal1Local1.Normalize();
	b2Vec2 normal1 = b2Mul(poly1->m_R, normal1Local1);
	b2Vec2 normal1Local2 = b2MulT(poly2->m_R, normal1);

	// Find the incident edge on poly2.
	int32 vertex21 = 0, vertex22 = 0;
	float32 minDot = FLT_MAX;
	for (int32 i = 0; i < count2; ++i)
	{
		int32 i1 = i;
		int32 i2 = i + 1 < count2 ? i + 1 : 0;

		b2Vec2 normal2Local2 = b2Cross(vert2s[i2] - vert2s[i1], 1.0f);
		normal2Local2.Normalize();
		float32 dot = b2Dot(normal2Local2, normal1Local2);
		if (dot < minDot)
		{
			minDot = dot;
			vertex21 = i1;
			vertex22 = i2;
		}
	}

	// Build the clip vertices for the incident edge.
	c[0].v = poly2->m_position + b2Mul(poly2->m_R, vert2s[vertex21]);
	c[0].id.features.referenceFace = (uint8)edge1;
	c[0].id.features.incidentEdge = (uint8)vertex21;
	c[0].id.features.incidentVertex = (uint8)vertex21;

	c[1].v = poly2->m_position + b2Mul(poly2->m_R, vert2s[vertex22]);
	c[1].id.features.referenceFace = (uint8)edge1;
	c[1].id.features.incidentEdge = (uint8)vertex21;
	c[1].id.features.incidentVertex = (uint8)vertex22;
}

// Find edge normal of max separation on A - return if separating axis is found
// Find edge normal of max separation on B - return if separation axis is found
// Choose reference edge as min(minA, minB)
// Find incident edge
// Clip

// The normal points from 1 to 2
void b2CollidePoly(b2Manifold* manifold, const b2PolyShape* polyA, const b2PolyShape* polyB)
{
	manifold->pointCount = 0;

	int32 edgeA = 0;
	float32 separationA = FindMaxSeparation(&edgeA, polyA, polyB);
	if (separationA > 0.0f)
		return;

	int32 edgeB = 0;
	float32 separationB = FindMaxSeparation(&edgeB, polyB, polyA);
	if (separationB > 0.0f)
		return;

	const b2PolyShape* poly1;	// reference poly
	const b2PolyShape* poly2;	// incident poly
	int32 edge1;		// reference edge
	uint8 flip;
	const float32 k_relativeTol = 0.98f;
	const float32 k_absoluteTol = 0.001f;

	// TODO_ERIN use "radius" of poly for absolute tolerance.
	if (separationB > k_relativeTol * separationA + k_absoluteTol)
	{
		poly1 = polyB;
		poly2 = polyA;
		edge1 = edgeB;
		flip = 1;
	}
	else
	{
		poly1 = polyA;
		poly2 = polyB;
		edge1 = edgeA;
		flip = 0;
	}

	ClipVertex incidentEdge[2];
	FindIncidentEdge(incidentEdge, poly1, edge1, poly2);

	int32 count1 = poly1->m_vertexCount;
	const b2Vec2* vert1s = poly1->m_vertices;

	b2Vec2 v11 = vert1s[edge1];
	b2Vec2 v12 = edge1 + 1 < count1 ? vert1s[edge1+1] : vert1s[0];

	b2Vec2 dv = v12 - v11;
	b2Vec2 sideNormal = b2Mul(poly1->m_R, v12 - v11);
	sideNormal.Normalize();
	b2Vec2 frontNormal = b2Cross(sideNormal, 1.0f);
	
	v11 = poly1->m_position + b2Mul(poly1->m_R, v11);
	v12 = poly1->m_position + b2Mul(poly1->m_R, v12);

	float32 frontOffset = b2Dot(frontNormal, v11);
	float32 sideOffset1 = -b2Dot(sideNormal, v11);
	float32 sideOffset2 = b2Dot(sideNormal, v12);

	// Clip incident edge against extruded edge1 side edges.
	ClipVertex clipPoints1[2];
	ClipVertex clipPoints2[2];
	int np;

	// Clip to box side 1
	np = ClipSegmentToLine(clipPoints1, incidentEdge, -sideNormal, sideOffset1);

	if (np < 2)
		return;

	// Clip to negative box side 1
	np = ClipSegmentToLine(clipPoints2, clipPoints1,  sideNormal, sideOffset2);

	if (np < 2)
		return;

	// Now clipPoints2 contains the clipped points.
	manifold->normal = flip ? -frontNormal : frontNormal;

	int32 pointCount = 0;
	for (int32 i = 0; i < b2_maxManifoldPoints; ++i)
	{
		float32 separation = b2Dot(frontNormal, clipPoints2[i].v) - frontOffset;

		if (separation <= 0.0f)
		{
			b2ContactPoint* cp = manifold->points + pointCount;
			cp->separation = separation;
			cp->position = clipPoints2[i].v;
			cp->id = clipPoints2[i].id;
			cp->id.features.flip = flip;
			++pointCount;
		}
	}

	manifold->pointCount = pointCount;
}
