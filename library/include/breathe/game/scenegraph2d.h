#ifndef SCENEGRAPH2D_H
#define SCENEGRAPH2D_H

#include <spitfire/spitfire.h>

#include <spitfire/algorithm/algorithm.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cQuaternion.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>
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
    class cUpdateVisitor;
    class cCullVisitor;

    class cStateSet
    {
    public:
      enum PRIORITY
      {
        PRIORITY_HIGH3 = 0,
        PRIORITY_HIGH2,
        PRIORITY_HIGH1,
        PRIORITY_HIGH0,
        PRIORITY_DIFFUSE,
        PRIORITY_TRANSPARENT,

        // Helper states
        PRIORITY_NORMAL = PRIORITY_DIFFUSE
      };

      cStateSet();
      cStateSet(const cStateSet& rhs);

      cStateSet& operator=(const cStateSet& rhs);

      bool operator==(const cStateSet& rhs) const;
      bool operator!=(const cStateSet& rhs) const { return !(*this == rhs); }

    private:
      PRIORITY priority;
      render::material::cMaterialRef material;

      scenegraph_common::cStateBoolean alphablending;
      scenegraph_common::cStateTexture texture;
      scenegraph_common::cStateShader shader;
    };

    inline cStateSet::cStateSet() :
      priority(PRIORITY_NORMAL)
    {
    }

    inline cStateSet::cStateSet(const cStateSet& rhs) :
      priority(rhs.priority),
      material(rhs.material),
      alphablending(rhs.alphablending),
      texture(rhs.texture),
      shader(rhs.shader)
    {
    }

    inline cStateSet& cStateSet::operator=(const cStateSet& rhs)
    {
      priority = rhs.priority;
      material = rhs.material;
      alphablending = rhs.alphablending;
      texture = rhs.texture;
      shader = rhs.shader;

      return *this;
    }

    inline bool cStateSet::operator==(const cStateSet& rhs) const
    {
      return (priority == rhs.priority) && (material == material) &&
        (alphablending == rhs.alphablending) && (texture == rhs.texture) && (shader == rhs.shader);
    }

    class cSceneNode;

    typedef cSmartPtr<cSceneNode> cSceneNodeRef;

    // Uses a quaternion to work out world rotation
    // Only call GenerateBoundingVolume at the very end of setting rotations and positions for all nodes

    class cSceneNode : public boost::enable_shared_from_this<cSceneNode>
    {
    public:
      friend class cUpdateVisitor;
      friend class cCullVisitor;

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
      void SetRotation(const math::cQuaternion& rotation) { SetRelativeRotation(rotation); }

      const math::cVec3& GetRelativePosition() const { return relativePosition; }
      const math::cQuaternion& GetRelativeRotation() const { return relativeRotation; }
      void SetRelativePosition(const math::cVec3& position);
      void SetRelativeRotation(const math::cQuaternion& rotation);

      math::cVec3 GetAbsolutePosition() const; // Calculated base on the parents of this node

      void SetScale(const math::cVec3& scale) { relativeScale = scale; }
      const math::cVec3& GetScale() const { return relativeScale; }


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

    protected:
      // Only the derived classes can call use this constructor
      cSceneNode();

      virtual void _Update(cUpdateVisitor& visitor);
      virtual void _Cull(cCullVisitor& visitor);

      float_t UpdateBoundingVolumeAndSetNotDirtyReturningBoundingVolumeRadius();

      // Only the derived class will know how to get the radius from our possible children
      virtual float_t _UpdateBoundingVolumeAndSetNotDirtyReturningBoundingVolumeRadius() { return math::cEPSILON; }

    private:
      cSceneNode(const cSceneNode&); // Prevent copying
      cSceneNode& operator=(const cSceneNode&); // Prevent copying

      void GenerateBoundingVolume();

      virtual void _AttachChild(cSceneNodeRef pChild) {}
      virtual void _DetachChild(cSceneNodeRef pChild) {}
      virtual void _DeleteChildRecursively(cSceneNodeRef pChild) {}
      virtual void _DeleteAllChildrenRecursively() {}

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
      math::cQuaternion relativeRotation;

      bool bUseRelativeScale;
      math::cVec3 relativeScale;

      cStateSet stateset;
      math::cSphere boundingSphere;
      math::cBox boundingBox;

      cSceneNodeRef pParent; // Each node has exactly one parent, no more no less.  pRoot is the only exception, pRoot->pParent == nullptr;
    };

    inline cSceneNode::~cSceneNode()
    {
      DeleteAllChildrenRecursively();
    }



    class cGroupNode : public cSceneNode
    {
    public:
      cGroupNode() {}

      void AddChild(cSceneNodeRef pChild);
      void RemoveChild(cSceneNodeRef pChild);

    protected:
      virtual void _Update(cUpdateVisitor& visitor);
      virtual void _Cull(cCullVisitor& visitor);

    private:
      std::vector<cSceneNodeRef> children;
    };


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

    // Draws a 2d graph with a fixed number of points
    class cGraphNode : public cSceneNode
    {
    public:
      explicit cGraphNode(size_t nPoints);

      float_t GetDistanceBetweenEachPoint() const { return fDistanceBetweenEachPoint; }
      void SetDistanceBetweenEachPoint(float_t _fDistanceBetweenEachPoint) { fDistanceBetweenEachPoint = _fDistanceBetweenEachPoint; }

    private:
      float_t fDistanceBetweenEachPoint;
      spitfire::cCircularBuffer<float> points;
    };






    class cUpdateVisitor
    {
    public:
      explicit cUpdateVisitor(cSceneGraph& scenegraph);

      void Visit(cSceneNode& node) {}
      void Visit(cModelNode& node) {}

    private:

    };



    enum RENDER_PRIORITY
    {
      RENDER_PRIORITY_FIRST = -3,
      RENDER_PRIORITY_SECOND = -2,
      RENDER_PRIORITY_THIRD = -1,
      RENDER_PRIORITY_OPAQUE = 0,
      RENDER_PRIORITY_NORMAL = 0,
      RENDER_PRIORITY_TRANSPARENT,
      RENDER_PRIORITY_LAST
    };

    class cRenderable;
    typedef cSmartPtr<cRenderable> cRenderableRef;

    class cCullVisitor
    {
    public:
      explicit cCullVisitor(cSceneGraph& scenegraph);

      void Visit(cSceneNode& node) {}
      void Visit(cModelNode& node) {}

      void Visit(cRenderableRef pRenderable)
      {
        AddRenderable(pRenderable);
      }

    private:
      void AddRenderable(cRenderableRef pRenderable) {}

      cSceneGraph& scenegraph;
    };


    // Render visitor does not visit cSceneGraph nodes, it visits the cRenderGraph nodes that have been collected
    class cRenderVisitor
    {
    public:
      explicit cRenderVisitor(cSceneGraph& scenegraph);

    private:
      cSceneGraph& scenegraph;
    };






    typedef cSmartPtr<cSceneNode> cGroupNodeRef;


    class cRenderable
    {
    public:


    private:
      bool bUseIsShadowCasting;
      bool bIsShadowCasting;

      bool bUseRenderPriority;
      RENDER_PRIORITY renderPriority;
    };

    typedef cSmartPtr<cRenderable> cRenderableRef;


    class cRenderGraph
    {
    public:
      friend class cCullVisitor;
      friend class cRenderVisitor;

      void AddRenderable(const cRenderableRef renderable);
      void Clear();

    private:
      typedef std::list<cRenderable*> cRenderableList;

      // Opaque (states : list of renderables with that state)
      std::map<cStateSet*, cRenderableList*> mOpaque;

      // Transparent (distance from the camera : renderable at that position)
      std::map<float, cRenderableRef> mTransparent;
    };

    inline void cRenderGraph::Clear()
    {
      mOpaque.clear();
      mTransparent.clear();
    }

    class cSceneGraph
    {
    public:
      friend class cUpdateVisitor;
      friend class cCullVisitor;
      friend class cRenderVisitor;

      cSceneGraph();

      cGroupNodeRef GetRoot() const { ASSERT(pRoot != nullptr); return pRoot; }

      bool IsCullingEnabled() const { return bIsCullingEnabled; }
      void SetCulling(bool bEnable) { bIsCullingEnabled = bEnable; }

      void Update(sampletime_t currentTime);
      void Cull(sampletime_t currentTime);
      void Render(sampletime_t currentTime);

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
