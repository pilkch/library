#include <cmath>

#include <vector>

// Breathe
#include <breathe/breathe.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cColour.h>
#include <breathe/math/cOctree.h>

namespace breathe
{
	namespace math
	{
		cOctree::cOctree()
		{
			m_bSubDivided = false;

			g_CurrentSubdivision=0;
			g_MaxSubdivisions=10;
			g_MaxTriangles=2000;
			g_EndNodeCount=0;
			g_TotalNodesDrawn=0;


			m_pVertices = NULL;

			m_pOctreeNodes[0] = NULL;
			m_pOctreeNodes[1] = NULL;
			m_pOctreeNodes[2] = NULL;
			m_pOctreeNodes[3] = NULL;
			m_pOctreeNodes[4] = NULL;
			m_pOctreeNodes[5] = NULL;
			m_pOctreeNodes[6] = NULL;
			m_pOctreeNodes[7] = NULL;
		}

		cOctree::~cOctree()
		{
			SAFE_DELETE(m_pVertices);

			SAFE_DELETE(m_pOctreeNodes[0]);
			SAFE_DELETE(m_pOctreeNodes[1]);
			SAFE_DELETE(m_pOctreeNodes[2]);
			SAFE_DELETE(m_pOctreeNodes[3]);
			SAFE_DELETE(m_pOctreeNodes[4]);
			SAFE_DELETE(m_pOctreeNodes[5]);
			SAFE_DELETE(m_pOctreeNodes[6]);
			SAFE_DELETE(m_pOctreeNodes[7]);
		}

		void cOctree::create(cFrustum* newFrustum)
		{
			frustum = newFrustum;

			g_CurrentSubdivision=0;
			g_MaxSubdivisions=10;
			g_MaxTriangles=2000;
			g_EndNodeCount=0;
			g_TotalNodesDrawn=0;

			// Set the subdivided flag to false
			m_bSubDivided = false;

			// Set the dimensions of the box to false
			m_Width = 0;

			// Initialize the triangle count
			m_TriangleCount = 0;

			// Initialize the center of the box to the 0
			m_vCenter = cVec3(0, 0, 0);

			// Set the triangle list to NULL
			m_pVertices = NULL;

			// Set the sub nodes to NULL
			std::memset(m_pOctreeNodes, 0, sizeof(m_pOctreeNodes));
		}


		///////////////////////////////// cOctree \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
		/////
		/////	This sets our initial width of the scene, as well as our center point
		/////
		///////////////////////////////// cOctree \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

		void cOctree::GetSceneDimensions(cVec3* pVertices, size_t numberOfVerts)
		{
			// We pass in the list of vertices and the vertex count to get the
			// center point and width of the whole scene.  We use this information
			// to subdivide our octree.  Eventually, in the next tutorial it won't
			// just be a list of vertices, but another structure that holds all the
			// normals and texture information.  It's easy to do once you understand vertices.

			// Initialize some temporary variables to hold the max dimensions found
			float maxWidth = 0, maxHeight = 0, maxDepth = 0;

			// Return from this function if we passed in bad data
			if (!pVertices || numberOfVerts <= 0) return;

			// Below we calculate the center point of the scene.  To do this, all you
			// need to do is add up ALL the vertices, then divide that total by the
			// number of vertices added up.  So all the X's get added up together, then Y's, etc..
			// This doesn't mean in a single number, but 3 separate floats (totalX, totalY, totalZ).
			// Notice that we are adding 2 vectors together.  If you look in the cVec3 class
			// I overloaded the + and - operator to handle it correctly.  It cuts down on code
			// instead of added the x, then the y, then the z separately.  If you don't want
			// to use operator overloading just make a function called CVector AddVector(), etc...

			// Go through all of the vertices and add them up to eventually find the center
      size_t i = 0;
			for (i = 0; i < numberOfVerts; i++)
			{
				// Add the current vertex to the center variable (Using operator overloading)
				m_vCenter = m_vCenter + pVertices[i];
			}

			// Divide the total by the number of vertices to get the center point.
			// We could have overloaded the / symbol but I chose not to because we rarely use it.
			m_vCenter.x /= numberOfVerts;
			m_vCenter.y /= numberOfVerts;
			m_vCenter.z /= numberOfVerts;

			// Now that we have the center point, we want to find the farthest distance from
			// our center point.  That will tell us how big the width of the first node is.
			// Once we get the farthest height, width and depth, we then check them against each
			// other.  Which ever one is higher, we then use that value for the cube width.

			// Go through all of the vertices and find the max dimensions
			for (i = 0; i < numberOfVerts; i++)
			{
				// Get the current dimensions for this vertex.  We use the abs() function
				// to get the absolute value because it might return a negative number.
				float currentWidth  = fabs(pVertices[i].x - m_vCenter.x);
				float currentHeight = fabs(pVertices[i].y - m_vCenter.y);
				float currentDepth  = fabs(pVertices[i].z - m_vCenter.z);

				// Check if the current width value is greater than the max width stored.
				if (currentWidth  > maxWidth)	maxWidth  = currentWidth;

				// Check if the current height value is greater than the max height stored.
				if (currentHeight > maxHeight)	maxHeight = currentHeight;

				// Check if the current depth value is greater than the max depth stored.
				if (currentDepth > maxDepth)		maxDepth  = currentDepth;
			}

			// Set the member variable dimensions to the max ones found.
			// We multiply the max dimensions by 2 because this will give us the
			// full width, height and depth.  Otherwise, we just have half the size
			// because we are calculating from the center of the scene.
			maxWidth *= 2;		maxHeight *= 2;		maxDepth *= 2;

			// Check if the width is the highest value and assign that for the cube dimension
			if (maxWidth > maxHeight && maxWidth > maxDepth) m_Width = maxWidth;

			// Check if the height is the heighest value and assign that for the cube dimension
			else if (maxHeight > maxWidth && maxHeight > maxDepth) m_Width = maxHeight;

			// Else it must be the depth or it's the same value as some of the other ones
			else m_Width = maxDepth;
		}


		///////////////////////////////// GET NEW NODE CENTER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
		/////
		/////	This returns the center point of the new subdivided node, depending on the ID
		/////
		///////////////////////////////// GET NEW NODE CENTER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

		cVec3 cOctree::GetNewNodeCenter(const cVec3& vCenter, float width, size_t nodeID)
		{
			// I created this function which takes an enum ID to see which node's center
			// we need to calculate.  Once we find that we need to subdivide a node we find
			// the centers of each of the 8 new nodes.  This is what that function does.
			// We just tell it which node we want.

			// Initialize the new node center
			cVec3 vNodeCenter(0, 0, 0);

			// Create a dummy variable to cut down the code size
			cVec3 vCtr = vCenter;

			// Switch on the ID to see which subdivided node we are finding the center
			switch(nodeID)
			{
				case TOP_LEFT_FRONT:
					// Calculate the center of this new node
					vNodeCenter = cVec3(vCtr.x - width/4, vCtr.y + width/4, vCtr.z + width/4);
					break;

				case TOP_LEFT_BACK:
					// Calculate the center of this new node
					vNodeCenter = cVec3(vCtr.x - width/4, vCtr.y + width/4, vCtr.z - width/4);
					break;

				case TOP_RIGHT_BACK:
					// Calculate the center of this new node
					vNodeCenter = cVec3(vCtr.x + width/4, vCtr.y + width/4, vCtr.z - width/4);
					break;

				case TOP_RIGHT_FRONT:
					// Calculate the center of this new node
					vNodeCenter = cVec3(vCtr.x + width/4, vCtr.y + width/4, vCtr.z + width/4);
					break;

				case BOTTOM_LEFT_FRONT:
					// Calculate the center of this new node
					vNodeCenter = cVec3(vCtr.x - width/4, vCtr.y - width/4, vCtr.z + width/4);
					break;

				case BOTTOM_LEFT_BACK:
					// Calculate the center of this new node
					vNodeCenter = cVec3(vCtr.x - width/4, vCtr.y - width/4, vCtr.z - width/4);
					break;

				case BOTTOM_RIGHT_BACK:
					// Calculate the center of this new node
					vNodeCenter = cVec3(vCtr.x + width/4, vCtr.y - width/4, vCtr.z - width/4);
					break;

				case BOTTOM_RIGHT_FRONT:
					// Calculate the center of this new node
					vNodeCenter = cVec3(vCtr.x + width/4, vCtr.y - width/4, vCtr.z + width/4);
					break;
			}

			// Return the new node center
			return vNodeCenter;
		}


		///////////////////////////////// CREATE NEW NODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
		/////
		/////	This figures out the new node information and then passes it into CreateNode()
		/////
		///////////////////////////////// CREATE NEW NODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

		void cOctree::CreateNewNode(const cVec3* pVertices, std::vector<bool>& pList, size_t numberOfVerts,
      const cVec3& vCenter,	 float width, size_t triangleCount, size_t nodeID)
		{
			// This function helps us set up the new node that is being created.  We only
			// want to create a new node if it found triangles in it's area.  If there were
			// no triangle found in this node's cube, then we ignore it and don't create a node.

			// Check if the first node found some triangles in it
			if (triangleCount)
			{
				// Allocate memory for the triangles found in this node (tri's * 3 for vertices)
				cVec3* pNodeVertices = new cVec3 [triangleCount * 3];

				// Create an counter to count the current index of the new node vertices
				size_t index = 0;

				// Go through all the vertices and assign the vertices to the node's list
				for (size_t i = 0; i < numberOfVerts; i++)
				{
					// If this current triangle is in the node, assign it's vertices to it
					if (pList[i / 3])
					{
						pNodeVertices[index] = pVertices[i];
						index++;
					}
				}

				// Now comes the initialization of the node.  First we allocate memory for
				// our node and then get it's center point.  Depending on the nodeID,
				// GetNewNodeCenter() knows which center point to pass back (TOP_LEFT_FRONT, etc..)

				// Allocate a new node for this octree
				m_pOctreeNodes[nodeID] = new cOctree;

				// Get the new node's center point depending on the nodexIndex (which of the 8 subdivided cubes).
				cVec3 vNodeCenter = GetNewNodeCenter(vCenter, width, nodeID);

				// Below, before and after we recurse further down into the tree, we keep track
				// of the level of subdivision that we are in.  This way we can restrict it.

				// Increase the current level of subdivision
				g_CurrentSubdivision++;

				// Recurse through this node and subdivide it if necessary
				m_pOctreeNodes[nodeID]->CreateNode(pNodeVertices, triangleCount * 3, vNodeCenter, width / 2);

				// Decrease the current level of subdivision
				g_CurrentSubdivision--;

				// Free the allocated vertices for the triangles found in this node
				delete [] pNodeVertices;
			}
		}


		///////////////////////////////// CREATE NODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
		/////
		/////	This is our recursive function that goes through and subdivides our nodes
		/////
		///////////////////////////////// CREATE NODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

		void cOctree::CreateNode(const cVec3* pVertices, size_t numberOfVerts, const cVec3& vCenter, float width)
		{
			// This is our main function that creates the octree.  We will recurse through
			// this function until we finish subdividing.  Either this will be because we
			// subdivided too many levels or we divided all of the triangles up.

			// Create a variable to hold the number of triangles
			size_t numberOfTriangles = numberOfVerts / 3;

			// Initialize this node's center point.  Now we know the center of this node.
			m_vCenter = vCenter;

			// Initialize this nodes cube width.  Now we know the width of this current node.
			m_Width = width;

			// Check if we have too many triangles in this node and we haven't subdivided
			// above our max subdivisions.  If so, then we need to break this node into
			// 8 more nodes (hence the word OCTree).  Both must be true to divide this node.
			if ((numberOfTriangles > g_MaxTriangles) && (g_CurrentSubdivision < g_MaxSubdivisions) )
			{
				// Since we need to subdivide more we set the divided flag to true.
				// This let's us know that this node does NOT have any vertices assigned to it,
				// but nodes that perhaps have vertices stored in them (Or their nodes, etc....)
				// We will querey this variable when we are drawing the octree.
				m_bSubDivided = true;

				// Create a list for each new node to store if a triangle should be stored in it's
				// triangle list.  For each index it will be a true or false to tell us if that triangle
				// is in the cube of that node.  Below we check every point to see where it's
				// position is from the center (I.E. if it's above the center, to the left and
				// back it's the TOP_LEFT_BACK node).  Depending on the node we set the pList
				// index to true.  This will tell us later which triangles go to which node.
				// You might catch that this way will produce doubles in some nodes.  Some
				// triangles will intersect more than 1 node right?  We won't split the triangles
				// in this tutorial just to keep it simple, but the next tutorial we will.

				// Create the list of booleans for each triangle index
				std::vector<bool> pList1(numberOfTriangles);		// TOP_LEFT_FRONT node list
				std::vector<bool> pList2(numberOfTriangles);		// TOP_LEFT_BACK node list
				std::vector<bool> pList3(numberOfTriangles);		// TOP_RIGHT_BACK node list
				std::vector<bool> pList4(numberOfTriangles);		// TOP_RIGHT_FRONT node list
				std::vector<bool> pList5(numberOfTriangles);		// BOTTOM_LEFT_FRONT node list
				std::vector<bool> pList6(numberOfTriangles);		// BOTTOM_LEFT_BACK node list
				std::vector<bool> pList7(numberOfTriangles);		// BOTTOM_RIGHT_BACK node list
				std::vector<bool> pList8(numberOfTriangles);		// BOTTOM_RIGHT_FRONT node list

				// Create this variable to cut down the thickness of the code below (easier to read)
				cVec3 vCtr = vCenter;

				// Go through all of the vertices and check which node they belong too.  The way
				// we do this is use the center of our current node and check where the point
				// lies in relationship to the center.  For instance, if the point is
				// above, left and back from the center point it's the TOP_LEFT_BACK node.
				// You'll see we divide by 3 because there are 3 points in a triangle.
				// If the vertex index 0 and 1 are in a node, 0 / 3 and 1 / 3 is 0 so it will
				// just set the 0'th index to TRUE twice, which doesn't hurt anything.  When
				// we get to the 3rd vertex index of pVertices[] it will then be checking the
				// 1st index of the pList*[] array.  We do this because we want a list of the
				// triangles in the node, not the vertices.
        size_t i = 0;
				for (i = 0; i < numberOfVerts; i++)
				{
					// Create some variables to cut down the thickness of the code (easier to read)
					cVec3 vPoint = pVertices[i];

					// Check if the point lines within the TOP LEFT FRONT node
					if ((vPoint.x <= vCtr.x) && (vPoint.y >= vCtr.y) && (vPoint.z >= vCtr.z) )
						pList1[i / 3] = true;

					// Check if the point lines within the TOP LEFT BACK node
					if ((vPoint.x <= vCtr.x) && (vPoint.y >= vCtr.y) && (vPoint.z <= vCtr.z) )
						pList2[i / 3] = true;

					// Check if the point lines within the TOP RIGHT BACK node
					if ((vPoint.x >= vCtr.x) && (vPoint.y >= vCtr.y) && (vPoint.z <= vCtr.z) )
						pList3[i / 3] = true;

					// Check if the point lines within the TOP RIGHT FRONT node
					if ((vPoint.x >= vCtr.x) && (vPoint.y >= vCtr.y) && (vPoint.z >= vCtr.z) )
						pList4[i / 3] = true;

					// Check if the point lines within the BOTTOM LEFT FRONT node
					if ((vPoint.x <= vCtr.x) && (vPoint.y <= vCtr.y) && (vPoint.z >= vCtr.z) )
						pList5[i / 3] = true;

					// Check if the point lines within the BOTTOM LEFT BACK node
					if ((vPoint.x <= vCtr.x) && (vPoint.y <= vCtr.y) && (vPoint.z <= vCtr.z) )
						pList6[i / 3] = true;

					// Check if the point lines within the BOTTOM RIGHT BACK node
					if ((vPoint.x >= vCtr.x) && (vPoint.y <= vCtr.y) && (vPoint.z <= vCtr.z) )
						pList7[i / 3] = true;

					// Check if the point lines within the BOTTOM RIGHT FRONT node
					if ((vPoint.x >= vCtr.x) && (vPoint.y <= vCtr.y) && (vPoint.z >= vCtr.z) )
						pList8[i / 3] = true;
				}

				// Here we create a variable for each list that holds how many triangles
				// were found for each of the 8 subdivided nodes.
				size_t triCount1 = 0;	size_t triCount2 = 0;	size_t triCount3 = 0;	size_t triCount4 = 0;
				size_t triCount5 = 0;	size_t triCount6 = 0;	size_t triCount7 = 0;	size_t triCount8 = 0;

				// Go through each of the lists and increase the triangle count for each node.
				for (i = 0; i < numberOfTriangles; i++)
				{
					// Increase the triangle count for each node that has a "true" for the index i.
					if (pList1[i])	triCount1++;	if (pList2[i])	triCount2++;
					if (pList3[i])	triCount3++;	if (pList4[i])	triCount4++;
					if (pList5[i])	triCount5++;	if (pList6[i])	triCount6++;
					if (pList7[i])	triCount7++;	if (pList8[i])	triCount8++;
				}

				// Next we do the dirty work.  We need to set up the new nodes with the triangles
				// that are assigned to each node, along with the new center point of the node.
				// Through recursion we subdivide this node into 8 more nodes.

				// Create the subdivided nodes if necessary and then recurse through them.
				// The information passed into CreateNewNode() are essential for creating the
				// new nodes.  We pass the 8 ID's in so it knows how to calculate it's new center.
				CreateNewNode(pVertices, pList1, numberOfVerts, vCenter, width, triCount1, TOP_LEFT_FRONT);
				CreateNewNode(pVertices, pList2, numberOfVerts, vCenter, width, triCount2, TOP_LEFT_BACK);
				CreateNewNode(pVertices, pList3, numberOfVerts, vCenter, width, triCount3, TOP_RIGHT_BACK);
				CreateNewNode(pVertices, pList4, numberOfVerts, vCenter, width, triCount4, TOP_RIGHT_FRONT);
				CreateNewNode(pVertices, pList5, numberOfVerts, vCenter, width, triCount5, BOTTOM_LEFT_FRONT);
				CreateNewNode(pVertices, pList6, numberOfVerts, vCenter, width, triCount6, BOTTOM_LEFT_BACK);
				CreateNewNode(pVertices, pList7, numberOfVerts, vCenter, width, triCount7, BOTTOM_RIGHT_BACK);
				CreateNewNode(pVertices, pList8, numberOfVerts, vCenter, width, triCount8, BOTTOM_RIGHT_FRONT);
			}
			else
			{
				// Store the triangles here

				// Since we did not subdivide this node we want to set our flag to false
				m_bSubDivided = false;

				// Initialize the triangle count of this end node (total verts / 3 = total triangles)
				m_TriangleCount = numberOfVerts / 3;

				// Allocate enough memory to hold the needed vertices for the triangles
				m_pVertices = new cVec3 [numberOfVerts];

				// Initialize the vertices to 0 before we copy the data over to them
				std::memset(m_pVertices, 0, sizeof(cVec3) * numberOfVerts);

				// Copy the passed in vertex data over to our node vertice data
				std::memcpy(m_pVertices, pVertices, sizeof(cVec3) * numberOfVerts);

				// Increase the amount of end nodes created (Nodes with vertices stored)
				g_EndNodeCount++;
			}
		}

/*
Load a .t3d file from unreal tournament 1
bool test(const std::string& fileName, Set <class Polygon *>& polygons, TextureTable& textureTable){
	std::string str;
	Stack <STATE> states;
	STATE state = NONE;
	int i,len;

	// Polygon variables
	class Polygon* poly;
	//class Polygon* poly;
	Set <Vertex> vertices;
	unsigned int flags, pflags;
	Vertex normal;
	Vertex origo, textureU, textureV;
    float panU, panV;
    TextureID baseTexture, bumpMap;
    unsigned int sizeU, sizeV;

	// Actor variables
	bool usePolygons = true;
	Vertex location(0,0,0);
	int vertexComponent;




	tok.setFile(fileName);

	while (tok.next(str)){
		if (stricmp(str, "Begin") == 0){
			tok.next(str);
			states.push(state);

			if (stricmp(str, "Map") == 0){
				state = MAP;
			} else if (stricmp(str, "Polygon") == 0){
				state = POLYGON;
				if (usePolygons){
					vertices.clear();
					flags = pflags = 0;
                    panU = panV = 0.0f;
				}
			} else if (stricmp(str, "Actor") == 0){
				state = ACTOR;
				location = Vertex(0,0,0);
				usePolygons = true;
			} else {
				state = NONE;
			}
		} else if (stricmp(str, "End") == 0){
			tok.next(NULL);

			switch(state){
			case POLYGON:
				if (usePolygons){
					if ((flags & PF_Invisible) == 0){
						len = vertices.getSize();
						poly = new class Polygon(len);
						for (i = 0; i < len; i++){
							//poly->setVertex(i, vertices[i]);
                            poly->setVertex(len - i - 1, vertices[i]);
						}
						if (flags & PF_TwoSided){
							poly->setFlags(PF_DOUBLESIDED, true);
						}
						if (flags & PF_Unlit){
							poly->setFlags(PF_UNLIT, true);
						}
						if (flags & PF_Translucent){
							poly->setFlags(PF_TRANSLUCENT, true);
						}

                        //if (flags & PF_SpecialPoly){
                        //	poly->setFlags(PF_FOGVOLUME | PF_NONBLOCKING, true);
                        //}



                        poly->setFlags(pflags, true);

                        //origo -= (panU * (textureU / length(textureU)) + panV * (textureV / length(textureV)));
                        //origo += ((-panU - 2) * textureU * 2 + panV * textureV * 2);

                        origo -= (panU * textureU) / lengthSqr(textureU);
                        origo -= (panV * textureV) / lengthSqr(textureV);

                        textureU = fix(textureU);
                        textureV = fix(textureV);

						poly->setTexCoordSystem(fix(origo), textureU / (float) sizeU, textureV / (float) sizeV);
                        poly->setTextures(baseTexture,bumpMap);
						poly->finalize();
						//poly->setNormal(normal);
						polygons.add(poly);

					}
				}
				break;
			}

			state = states.pop();
		} else {


			switch(state){
			case POLYGON:
				if (usePolygons){
					if (stricmp(str, "Flags") == 0){
						tok.next(NULL); // =
						tok.next(str);  // flags
						flags = atoi(str);

					} else if ((flags & PF_Invisible) == 0){
						if (stricmp(str, "Vertex") == 0){
							Vertex v = readVertex();
							vertices.add(fix(v + location));
						} else if (stricmp(str, "Normal") == 0){
							normal = fix(readVertex());
						} else if (stricmp(str, "Texture") == 0){
    						tok.next(NULL); // =
	    					tok.next(str);  // texture name
                            textureTable.getTexturesFromName(str, sizeU, sizeV, baseTexture, bumpMap, pflags);
						} else if (stricmp(str, "TextureU") == 0){
							textureU = readVertex();
						} else if (stricmp(str, "TextureV") == 0){
							textureV = readVertex();
						} else if (stricmp(str, "Origin") == 0){
							origo = readVertex();
						} else if (stricmp(str, "Pan") == 0){
    						tok.next(NULL); // U
    						tok.next(NULL); // =
	    					tok.next(str);  // U pan or -
                            if (str[0] == '-'){
    	    					tok.next(str);  // U pan
		    				    panU = -(float) atof(str);
                            } else panU = (float) atof(str);

    						tok.next(NULL); // V
    						tok.next(NULL); // =
	    					tok.next(str);  // V pan or -
                            if (str[0] == '-'){
    	    					tok.next(str);  // V pan
		    				    panV = -(float) atof(str);
                            } else panV = (float) atof(str);
						}
					}
				}
				break;
			case ACTOR:
				if (stricmp(str, "Group") == 0){
					usePolygons = false;
				} else if (stricmp(str, "Location") == 0){
					float value;
					tok.next(NULL); // =
					tok.next(NULL); // (
					do {
						tok.next(str);
						vertexComponent = (*str - 'X');
						tok.next(NULL); // =
						tok.next(str);  // sign/value
						if (*str == '-'){
							tok.next(str);  // number
							value = -(float) atof(str);
						} else {
							value = (float) atof(str);
						}
						location[vertexComponent] = value;

						tok.next(str);
					} while (*str == ',');
				}
				break;
			}
		}
	}

	return true;
}*/
	}
}
