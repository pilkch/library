#ifndef SCENEGRAPH2D_H
#define SCENEGRAPH2D_H

#include <spitfire/spitfire.h>

#include <spitfire/algorithm/algorithm.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cQuaternion.h>

#include <breathe/render/cContext.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cFont.h>

#include <breathe/game/scenegraph.h>

// cSceneNode
//     |
//     v
// cSwitchNode, cSequenceNode list<cSceneNodeRef> frame, cRenderableNode


// cSceneNode pure virtual
//     |
//     v
// cRenderableNode pure virtual
//     |
//     v
// cRenderableParticleEffect


// cRenderableNode
//     |
//     v
// cRenderableGroupNode
// {
//   std::list<cRenderableNodeRef> children
// }

namespace breathe
{
  namespace scenegraph2d
  {
    class cSceneGraph;
    class cRenderGraph;
    class cUpdateVisitor;
    class cCullVisitor;
    class cRenderVisitor;

    class cVertexBuffer
    {
    public:
      const std::vector<float>& GetVertices() const { return vertices; }
      const std::vector<float>& GetTextureCoordinates() const { return textureCoordinates; }

      void Clear();

      void AddPoint(const math::cVec2& point);
      void AddPoint(const math::cVec2& point, const math::cVec2& textureCoordinate);

    private:
      std::vector<float> vertices;
      std::vector<float> textureCoordinates;
    };

    inline void cVertexBuffer::Clear()
    {
      vertices.clear();
      textureCoordinates.clear();
    }

    inline void cVertexBuffer::AddPoint(const math::cVec2& point)
    {
      vertices.push_back(point.x);
      vertices.push_back(point.y);
    }

    inline void cVertexBuffer::AddPoint(const math::cVec2& point, const math::cVec2& textureCoordinate)
    {
      vertices.push_back(point.x);
      vertices.push_back(point.y);
      vertices.push_back(textureCoordinate.u);
      vertices.push_back(textureCoordinate.v);
    }



    class cStateSet
    {
    public:
      friend class cRenderGraph;
      friend class cRenderVisitor;

      cStateSet();
      cStateSet(const cStateSet& rhs);

      cStateSet& operator=(const cStateSet& rhs);

      bool operator==(const cStateSet& rhs) const;
      bool operator!=(const cStateSet& rhs) const { return !(*this == rhs); }

      void SetColour(const math::cColour& colour);

    private:
      void Assign(const cStateSet& rhs);
      void Clear();

      scenegraph_common::cStateBoolean alphablending;
      scenegraph_common::cStateColour colour;
      scenegraph_common::cStateTexture texture[render::MAX_TEXTURE_UNITS];
      scenegraph_common::GEOMETRY_TYPE geometryType;
    };

    inline cStateSet::cStateSet()
    {
      Clear();
    }

    inline cStateSet::cStateSet(const cStateSet& rhs)
    {
      Assign(rhs);
    }

    inline cStateSet& cStateSet::operator=(const cStateSet& rhs)
    {
      Assign(rhs);
      return *this;
    }

    inline void cStateSet::Assign(const cStateSet& rhs)
    {
      ASSERT(render::MAX_TEXTURE_UNITS == 3);

      alphablending = rhs.alphablending;
      colour = rhs.colour;
      texture[0] = rhs.texture[0];
      texture[1] = rhs.texture[1];
      texture[2] = rhs.texture[2];
      geometryType = rhs.geometryType;
    }

    inline bool cStateSet::operator==(const cStateSet& rhs) const
    {
      ASSERT(render::MAX_TEXTURE_UNITS == 3);

      return (
        (alphablending == rhs.alphablending) && (colour == rhs.colour) &&
        (texture[0] == rhs.texture[0]) && (texture[1] == rhs.texture[1]) && (texture[2] == rhs.texture[2]) &&
        (geometryType == rhs.geometryType)
      );
    }

    inline void cStateSet::SetColour(const math::cColour& _colour)
    {
      colour.colour = _colour;
      colour.bHasValidValue = true;
      colour.bTurnedOn = true;
    }



    class cSceneNode;

    typedef std::shared_ptr<cSceneNode> cSceneNodeRef;

    // Uses a quaternion to work out world rotation
    // Only call GenerateBoundingVolume at the very end of setting rotations and positions for all nodes

    class cSceneNode : public std::enable_shared_from_this<cSceneNode>
    {
    public:
      friend class cUpdateVisitor;
      friend class cCullVisitor;

      typedef std::list<cSceneNodeRef>::iterator child_iterator;
      typedef std::list<cSceneNodeRef>::const_iterator child_const_iterator;

      virtual ~cSceneNode();

#ifndef NDEBUG
      bool IsWithinNode(cSceneNodeRef pNode) const; // Determines whether this node is in the scene graph, ie. whether it's ultimate ancestor is the root scene node.

      // Checks that pChild is one of our direct children
      bool IsParentOfChild(const cSceneNodeRef pChild) const;
#endif

      cSceneNodeRef GetParent() const { return pParent; }

      void AttachChild(cSceneNodeRef pChild); // Calls the virtual _AttachChild so that a scenegraph can choose how to add it or even not add it at all
      void DetachChildForUseLater(cSceneNodeRef pChild); // Doesn't touch children of pChild, just detaches from the scenegraph, so you can still reference pChild and reinsert it later
      void DeleteChildRecursively(cSceneNodeRef pChild); // Removes pChild from the scenegraph and also calls DeleteAllChildrenRecursively on each of the children of pChild
      void DeleteAllChildrenRecursively() { _DeleteAllChildrenRecursively(); }


      bool IsEnabled() const { return bIsEnabled; }
      void SetEnabled(bool _bIsEnabled) { bIsEnabled = _bIsEnabled; }

      bool IsDirty() const { return bIsDirty; }
      void SetDirty();

      void SetVisible(bool bVisible);
      void SetPosition(const math::cVec3& position) { SetRelativePosition(position); }
      void SetRotation(float_t rotation) { SetRelativeRotation(rotation); }

      const math::cVec3& GetRelativePosition() const { return relativePosition; }
      float_t GetRelativeRotation() const { return relativeRotation; }
      void SetRelativePosition(const math::cVec3& position);
      void SetRelativeRotation(float_t rotation);

      math::cVec3 GetAbsolutePosition() const; // Calculated base on the parents of this node

      void SetScale(const math::cVec3& scale) { relativeScale = scale; }
      const math::cVec2& GetScale() const { return relativeScale; }


      const math::cSphere& GetBoundingSphere() const { return boundingSphere; }
      const math::cBox& GetBoundingBox() const { return boundingBox; }

      // We call this from cSceneGraph::Update
      void UpdateBoundingVolumeAndSetNotDirty();

#ifdef BUILD_DEBUG
      bool IsShowingBoundingBox() const;
      void SetShowingBoundingBox(bool bShow) { bIsShowingBoundingBox = bShow; }
#endif

      void Update(cUpdateVisitor& visitor) { _Update(visitor); }
      void Cull(cCullVisitor& visitor) { _Cull(visitor); }

      cStateSet& GetStateSet() { return stateset; }
      const cStateSet& GetStateSet() const { return stateset; }

    protected:
      // Only the derived classes can call use this constructor
      cSceneNode();

      virtual void _Update(cUpdateVisitor& visitor);
      virtual void _Cull(cCullVisitor& visitor);

      float_t UpdateBoundingVolumeAndSetNotDirtyReturningBoundingVolumeRadius();

      // Only the derived class will know how to get the radius from our possible children
      virtual float_t _UpdateBoundingVolumeAndSetNotDirtyReturningBoundingVolumeRadius() { return math::cEPSILON; }

      cStateSet stateset;

    private:
      cSceneNode(const cSceneNode&); // Prevent copying
      cSceneNode& operator=(const cSceneNode&); // Prevent copying

      void GenerateBoundingVolume();

      // NOTE: If you override any of these methods you should override all of them
      virtual void _AttachChild(cSceneNodeRef pChild);
      virtual void _DetachChild(cSceneNodeRef pChild);
      virtual void _DeleteChildRecursively(cSceneNodeRef pChild);
      virtual void _DeleteAllChildrenRecursively();

      std::list<cSceneNodeRef> children; // The children will be rendered in order from begin() to end()

#ifdef BUILD_DEBUG
      bool bIsShowingBoundingBox;
#endif

      bool bIsVisible;
      bool bIsEnabled;

      bool bIsDirty;

      bool bUseIsDynamic;
      bool bIsDynamic;

      bool bHasRelativePosition;
      math::cVec3 relativePosition;

      bool bHasRelativeRotation;
      float_t relativeRotation;

      bool bUseRelativeScale;
      math::cVec2 relativeScale;

      math::cSphere boundingSphere;
      math::cBox boundingBox;

      cSceneNodeRef pParent; // Each node has exactly one parent, no more no less.  pRoot is the only exception, pRoot->pParent == nullptr;
    };

    inline cSceneNode::~cSceneNode()
    {
      DeleteAllChildrenRecursively();
    }


    class cGroupNode : public cSceneNode {};
    typedef std::shared_ptr<cGroupNode> cGroupNodeRef;



    class cModelNode : public cSceneNode
    {
    public:

    private:
      void _Update(cUpdateVisitor& visitor);
      void _Cull(cCullVisitor& visitor);
    };


    class cSwitchNode : public cSceneNode
    {
    public:
      cSwitchNode() : index(0) {}

      void SetIndex(size_t index);

    private:
#ifdef BUILD_DEBUG
      bool _IsParentOfChild(const cSceneNodeRef pChild) const;
#endif

      void _Update(cUpdateVisitor& visitor);
      void _Cull(cCullVisitor& visitor);

      size_t index;
      std::vector<cSceneNodeRef> node;
    };

    inline void cSwitchNode::SetIndex(size_t _index)
    {
   // Make sure that we set our new index to a reasonable value
      const size_t n = node.size();
      ASSERT(n != 0);
      ASSERT(_index < n);
      index = _index;
    }


    // Quadtree for spatially representing the world
    class cSpatialGraphNode
    {
    public:
      cSpatialGraphNode();
      virtual ~cSpatialGraphNode() {}

      void Update(cUpdateVisitor& visitor) { _Update(visitor); }
      void Cull(cCullVisitor& visitor) { _Cull(visitor); }

    private:
#ifdef BUILD_DEBUG
      bool _IsParentOfChild(const cSceneNodeRef pChild) const;
#endif

      virtual void _Update(cUpdateVisitor& visitor);
      virtual void _Cull(cCullVisitor& visitor);

      cSpatialGraphNode* pChild[4];
    };


    // 2D Spatial Nodes

    //class cScreenSpaceNode
    //{
    //public:
    //  bool bHasMaterial;
    //};

    //class cRectangleNode
    //{
    //public:
    //  cVec2 position;
    //  cVec2 size;
    //};

    // Draws a 2d string of text with a specified font
    class cTextNode : public cSceneNode
    {
    public:
      void SetFont(render::cFontRef _pFont) { pFont = _pFont; }
      void SetText(const string_t& _sText) { sText = _sText; }

    private:
      render::cFontRef pFont;
      string_t sText;
    };

    class cLineBufferNode : public cSceneNode
    {
    public:
      void AddLine(const math::cVec2& p0, const math::cVec2& p1);

    private:
      void _Update(cUpdateVisitor& visitor);

      // These are stored in pairs, ie. 0-1 is first line, 2-3 is second line, 4-5 is third line, etc.
      std::vector<math::cVec2> vertices;
    };


    class cGeometryBufferNode : public cSceneNode
    {
    public:
      void AddTriangle(const math::cVec2& p0, const math::cVec2& p1, const math::cVec2& p2, const math::cVec2& uv0, const math::cVec2& uv1, const math::cVec2& uv2);
      void AddRectangle(const math::cVec2& p0, const math::cVec2& p1, const math::cVec2& p2, const math::cVec2& p3, const math::cVec2& uv0, const math::cVec2& uv1, const math::cVec2& uv2, const math::cVec2& uv3);
      void AddAxisAlignedRectangle(const math::cVec2& p0, const math::cVec2& p1, const math::cVec2& uv0, const math::cVec2& uv1);
      void AddAxisAlignedRectangle(const math::cVec2& p0, const math::cVec2& p1);

    private:
      void _Update(cUpdateVisitor& visitor);

      // There should be a 1 to 1 mapping of vertices to textureCoordinates
      std::vector<math::cVec2> vertices;
      std::vector<math::cVec2> textureCoordinates;
    };


    // Draws a 2d graph with a fixed number of points
    class cGraphNode : public cSceneNode
    {
    public:
      explicit cGraphNode(size_t nPoints);

      float_t GetDistanceBetweenEachPoint() const { return fDistanceBetweenEachPoint; }
      void SetDistanceBetweenEachPoint(float_t _fDistanceBetweenEachPoint) { fDistanceBetweenEachPoint = _fDistanceBetweenEachPoint; }

      void SetRangeMax(float_t _fMax) { fMax = _fMax; }

      void AddPoint(float_t fValue);

    private:
      void _Update(cUpdateVisitor& visitor);
      void _Cull(cCullVisitor& visitor);

      float_t fDistanceBetweenEachPoint;
      float_t fMax;
      spitfire::cCircularBuffer<float> points;

      cVertexBuffer vertexBuffer;
    };

    typedef std::shared_ptr<cGraphNode> cGraphNodeRef;













    class cUpdateVisitor
    {
    public:
      explicit cUpdateVisitor(cSceneGraph& scenegraph);

      // TODO: Remove these, require explicit types, same for cull visitor
      void Visit(cSceneNode& node) { node.Update(*this); }
      void Visit(cGroupNode& node) { node.Update(*this); }
      void Visit(cModelNode& node) { node.Update(*this); }
      void Visit(cGraphNode& node) { node.Update(*this); }

    private:

    };


    class cCullVisitor
    {
    public:
      cCullVisitor(cSceneGraph& scenegraph, const render::cCamera& camera);

      void Visit(cSceneNode& node) { node.Cull(*this); }
      void Visit(cGroupNode& node) { node.Cull(*this); }
      void Visit(cModelNode& node) { node.Cull(*this); }
      void Visit(cGraphNode& node) { node.Cull(*this); }

      void Visit(cStateSet* pStateSet, const cVertexBuffer* pVertexBuffer);

    private:
      cSceneGraph& scenegraph;
      const render::cCamera& camera;
    };


    // Render visitor does not visit cSceneGraph nodes, it visits the cRenderGraph nodes that have been collected
    class cRenderVisitor
    {
    public:
      cRenderVisitor(cSceneGraph& scenegraph, render::cContext& context);

    private:
      void ApplyStateSet(cStateSet& stateSet);
      void UnApplyStateSet(cStateSet& stateSet);
    };











    class cRenderGraph
    {
    public:
      friend class cCullVisitor;
      friend class cRenderVisitor;

      void AddRenderable(cStateSet* pStateSet, const cVertexBuffer* pVertexBuffer);
      void Clear();

    private:
      typedef std::pair<cStateSet*, const cVertexBuffer*> cRenderablePair;

      // NOTE: These are sorted by depth, we render back to front, 0 is the furthest away, n - 1 is the closest node and is rendered last
      std::vector<cRenderablePair> items;
    };

    inline void cRenderGraph::AddRenderable(cStateSet* pStateSet, const cVertexBuffer* pVertexBuffer)
    {
      cRenderablePair pair(pStateSet, pVertexBuffer);
      items.push_back(pair);
    }

    inline void cRenderGraph::Clear()
    {
      items.clear();
    }



    class cSceneGraph
    {
    public:
      friend class cUpdateVisitor;
      friend class cCullVisitor;
      friend class cRenderVisitor;

      cSceneGraph();

      void Create() {}

      cGroupNodeRef GetRoot() const { ASSERT(pRoot != nullptr); return pRoot; }

      bool IsCullingEnabled() const { return bIsCullingEnabled; }
      void SetCulling(bool bEnable) { bIsCullingEnabled = bEnable; }

      void Update(durationms_t currentTime);
      void Cull(durationms_t currentTime, const render::cCamera& camera);
      void Render(durationms_t currentTime, render::cContext& context);

    protected:
      cRenderGraph& GetRenderGraph() { return renderGraph; }

    private:
      bool bIsCullingEnabled;

      cRenderGraph renderGraph;
      cGroupNodeRef pRoot;
    };
  }
}

#endif // SCENEGRAPH2D_H
