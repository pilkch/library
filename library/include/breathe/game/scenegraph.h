#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

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
  class cCamera;
  class cObject;

  namespace render
  {
    class cRender;
    class cTexture;
    class cShader
    {
    public:
    };
  }


  namespace physics
  {
    class cPhysicsObject;
  }

  namespace vehicle
  {
    class cVehicle;
  }

  namespace scenegraph
  {
    class cSceneGraph;

    /*class cSceneGraphModel : public cObject
    {
    public:
      render::model::cStaticRef pModel;

      void Update(sampletime_t currentTime)
      {

      }
    };

    class cSceneGraphCubemap
    {
    public:
      math::cVec3 v3Position;

      std::string sFilename;
    };

    class cSceneGraphSpawn
    {
    public:
      cSceneGraphSpawn();

      math::cVec3 v3Position;
      math::cVec3 v3Rotation;
    };


    const unsigned int NODE_INACTIVE=0;
    const unsigned int NODE_UNLOAD=1;
    const unsigned int NODE_ACTIVE=20; //Seconds since the player has been in the vicinity

    class cSceneNode : public cRenderable
    {
    public:
      cSceneNode(cSceneGraph* p, std::string sNewFilename);
      ~cSceneNode();

      void Load();
      void Unload();


      unsigned int uiStatus;

      float fFogDistance;

      math::cColour colourFog;

      std::string sFilename;
      std::string sName;
      std::string sCRC;

      std::vector<cSceneGraphModelRef>vModel;

    private:
      cSceneGraph* pLevel;
    };*/

    class cUpdateVisitor;
    class cCullVisitor;
    class cRenderVisitor;

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

    private:
      PRIORITY priority;
    };

    class cSceneNode;

    typedef cSmartPtr<cSceneNode> cSceneNodeRef;

    class cSceneNode
    {
    public:
      friend class cUpdateVisitor;

      cSceneNode();
      virtual ~cSceneNode() {}

      bool IsDirty() const { return bIsDirty; }

      void SetDirty() { bIsDirty = true; if (pParent != nullptr) pParent->SetDirty(); }

      void Update(cUpdateVisitor& visitor) { _Update(visitor); }
      void Cull(cCullVisitor& visitor) { _Cull(visitor); }

      const math::cSphere& GetBoundingSphere() const { return boundingSphere; }
      const math::cBox& GetBoundingBox() const { return boundingBox; }

      // We call this from cSceneGraph::Update
      void UpdateBoundingVolumeAndSetNotDirty();

    protected:
      // Only the derived classes can call use this constructor
      cSceneNode();

      float_t UpdateBoundingVolumeAndSetNotDirtyReturningBoundingVolumeRadius();

      // Only the derived class will know how to get the radius from our possible children
      virtual float_t _UpdateBoundingVolumeAndSetNotDirtyReturningBoundingVolumeRadius() { return math::cEpsilon; }

    private:
      cSceneNode(const cSceneNode&); // Prevent copying
      cSceneNode& operator=(const cSceneNode&); // Prevent copying

      bool bIsEnabled;
      bool bIsDirty;

      cSceneNodeRef pParent;

      bool bHasRelativePosition;
      math::cVec3 relativePosition;

      math::cSphere boundingSphere;
      math::cBox boundingBox;
    };

    class cGroupNode
    {
    public:
      void AddChild(cSceneNodeRef pChild);
      void RemoveChild(cSceneNodeRef pChild);

    private:
      std::vector<cSceneNodeRef> children;
    };

    class cModelNode : public cSceneNode
    {
    public:

    private:
      virtual void _Update(cUpdateVisitor& visitor);
      virtual void _Cull(cCullVisitor& visitor);
    };

    class cLightNode : public cSceneNode
    {
    public:

    private:
      virtual void _Update(cUpdateVisitor& visitor);
      virtual void _Cull(cCullVisitor& visitor);
    };

    // Octree for spatially representing the world
    class cSpatialGraphNode
    {
    public:
      cSpatialGraphNode() { memset(&pChild[0], 0, sizeof(cSpatialGraphNode*) * 8); }
      virtual ~cSpatialGraphNode() {}

      void Update(cUpdateVisitor& visitor) { _Update(visitor); }
      void Cull(cCullVisitor& visitor) { _Cull(visitor); }

    private:
      virtual void _Update(cUpdateVisitor& visitor);
      virtual void _Cull(cCullVisitor& visitor);

      cSpatialGraphNode* pChild[8];
    };




    class cUpdateVisitor
    {
    public:
      explicit cUpdateVisitor(cSceneGraph& scenegraph);

      void Visit(cSceneNode& node) { printf("cUpdateVisitor::Visit cSceneNode\n"); }
      void Visit(cModelNode& node) { printf("cUpdateVisitor::Visit cModelNode\n"); }
      void Visit(cLightNode& node) { printf("cUpdateVisitor::Visit cLightNode\n"); }

    private:

    };

    class cCullVisitor
    {
    public:
      explicit cCullVisitor(cSceneGraph& scenegraph);

      void Visit(cSceneNode& node) { printf("cCullVisitor::Visit cSceneNode\n"); }
      void Visit(cModelNode& node) { printf("cCullVisitor::Visit cModelNode\n"); }
      void Visit(cLightNode& node) { printf("cCullVisitor::Visit cLightNode\n"); }

      void Visit(cRenderableRef pRenderable)
      {
        AddRenderable(pRenderable);
      }

    private:
      void AddRenderable(cRenderableRef pRenderable) {}

      cSceneGraph& scenegraph;
    };

    class cRenderVisitor
    {
    public:
      explicit cRenderVisitor(cSceneGraph& scenegraph);

    private:
      cSceneGraph& scenegraph;
    };






    class cScopedEnable
    {
    public:
      explicit cScopedEnable(GLenum field);
      ~cScopedEnable();

    private:
      GLenum field;
    };

    inline cScopedEnable::cScopedEnable(GLenum _field) :
      field(_field)
    {
      glEnable(field);
    }

    inline cScopedEnable::~cScopedEnable()
    {
      glDisable(field);
    }

    class cScopedAttributes
    {
    public:
      explicit cScopedAttributes(GLbitfield attribute);
      ~cScopedAttributes();

    private:
      GLbitfield attributes;
    };

    inline cScopedAttributes::cScopedAttributes(GLbitfield _attribute) :
      attributes(_attribute)
    {
      glPushAttrib(attributes);
    }

    inline cScopedAttributes::~cScopedAttributes()
    {
      glPopAttrib();
    }




    // State classes for keeping track of what state we are in.
    // Default compiler generated copy constructors are fine for these simple classes.

    class cStateBoolean
    {
    public:
      cStateBoolean() : bHasValidValue(false), bTurnedOn(false) {}

      bool bHasValidValue;
      bool bTurnedOn;
    };

    class cStateBooleanWithFloat
    {
    public:
      cStateBooleanWithFloat() : bHasValidValue(false), bTurnedOn(false), fValue(0.0f) {}

      bool bHasValidValue;
      bool bTurnedOn;
      float fValue;
    };

    class cStateTexture
    {
    public:
      cStateTexture() : bHasValidValue(false), bTurnedOn(false) {}

      bool bHasValidValue;
      bool bTurnedOn;
      render::cTextureRef pTexture;
    };

    class cStateShader
    {
    public:
      cStateShader() : bHasValidValue(false), bTurnedOn(false) {}

      bool bHasValidValue;
      bool bTurnedOn;
      render::material::cShaderRef pShader;
    };

    class cStateList
    {
    private:
      cStateBoolean alphablending;
      cStateTexture texture;
      cStateShader shader;
    };

    typedef cSmartPtr<cSceneNode> cGroupNodeRef;

    class cRenderable
    {
    public:

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
      std::list<cRenderableRef> listOpaque;
      std::map<float, cRenderableRef> mTransparent;
    };

    inline void cRenderGraph::Clear()
    {
      listOpaque.clear();
      mTransparent.clear();
    }

    class cSceneGraph
    {
    public:
      friend class cCullVisitor;
      friend class cRenderVisitor;

      cSceneGraph();

      cGroupNodeRef GetRoot() const { return pRoot; }

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

#endif // SCENEGRAPH_H
