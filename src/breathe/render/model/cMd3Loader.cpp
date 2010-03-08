// Standard Library headers
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>

// STL headers
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include <list>
#include <vector>
#include <map>

// Boost headers
#include <boost/shared_ptr.hpp>

// OpenGL headers
#include <GL/GLee.h>

// SDL headers
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cTimer.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/file.h>

// Breathe headers
#include <breathe/render/model/cMd3Loader.h>

#include <breathe/render/cResourceManager.h>

// The vertices positions need to be multiplied by 1.0f / 64.0f to get the correct position.
// If you use another ratio, it screws up the model's body part position.
const float fMD3Scale = 1.0f;// / 64.0f;

// There is a lot of random scaling going on throughout the whole code, I think we can reducing this to
// just scaling as we load and not scale at all after that

namespace breathe
{
  namespace character
  {
    void CreateTexture(uint32_t& texture, const char* strFileName)
    {
      LOG<<"CreateTexture strFileName=\""<<strFileName<<"\""<<std::endl;

      texture = 0;

      const string_t sFileName(breathe::string::Trim(breathe::string::ToString_t(strFileName)));
      LOG<<"CreateTexture sFileName=\""<<sFileName<<"\""<<std::endl;
      breathe::render::cTextureRef pTexture = pResourceManager->AddTexture(sFileName);
      if (pTexture != nullptr) texture = pTexture->uiTexture;
    }


    /////   This returns true if the string strSubString is inside of strString
    /////
    ///////////////////////////////// IS IN STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    bool IsInString(const std::string& strString, const std::string& strSubString)
    {
      // Make sure both of these strings are valid, return false if any are empty
      if (strString.empty() || strSubString.empty()) return false;

      // grab the starting index where the sub string is in the original string
      size_t found = strString.find(strSubString);

      // Make sure the index returned was valid
      return (found != std::string::npos);
    }




    ///////////////////////////////// GET BODY PART \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This returns a specific model from the character (kLower, kUpper, kHead, kWeapon)
    /////
    ///////////////////////////////// GET BODY PART \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    cModel* cMd3::GetModel(int whichPart)
    {
      // Return the legs model if desired
      if (whichPart == kLower) return &m_Lower;

      // Return the torso model if desired
      if (whichPart == kUpper) return &m_Upper;

      // Return the head model if desired
      if (whichPart == kHead) return &m_Head;

      // Return the weapon model
      return &m_Weapon;
    }


    ///////////////////////////////// LOAD MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This loads our Quake3 model from the given path and character name
    /////
    ///////////////////////////////// LOAD MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    bool cMd3::LoadModel(const char* strPath, const char* strModelHead, const char* strModelUpper, const char* strModelLower, const char* strSkinHead, const char* strSkinUpper, const char* strSkinLower)
    {
      LOG<<"cMd3::LoadModel \""<<strPath<<"\" \""<<strModelHead<<"\" \""<<strModelUpper<<"\" \""<<strModelLower<<"\" \""<<strSkinHead<<"\" \""<<strSkinUpper<<"\" \""<<strSkinLower<<"\""<<std::endl;

      // Make sure valid path and model names were passed in
      if ((strPath == nullptr)
        || (strModelHead == nullptr) || (strModelUpper == nullptr) || (strModelLower == nullptr)
        || (strSkinHead == nullptr) || (strSkinUpper == nullptr) || (strSkinLower == nullptr)
      ) return false;

      char strLowerModel[255] = {0};  // This stores the file name for the lower.md3 model
      char strUpperModel[255] = {0};  // This stores the file name for the upper.md3 model
      char strHeadModel[255]  = {0};  // This stores the file name for the head.md3 model
      char strLowerSkin[255]  = {0};  // This stores the file name for the lower.md3 skin
      char strUpperSkin[255]  = {0};  // This stores the file name for the upper.md3 skin
      char strHeadSkin[255]   = {0};  // This stores the file name for the head.md3 skin

      // Store the correct files names for the .md3 and .skin file for each body part.
      // We concatinate this on top of the path name to be loaded from.
      sprintf(strHeadModel,  "%s%s.md3",  strPath, strModelHead);
      sprintf(strUpperModel, "%s%s.md3", strPath, strModelUpper);
      sprintf(strLowerModel, "%s%s.md3", strPath, strModelLower);

      // Get the skin file names with their path
      sprintf(strHeadSkin,  "%s%s.skin",  strPath, strSkinHead);
      sprintf(strUpperSkin, "%s%s.skin", strPath, strSkinUpper);
      sprintf(strLowerSkin, "%s%s.skin", strPath, strSkinLower);

      CLoadMD3 loadMd3;               // This object allows us to load each .md3 and .skin file

      // Load the head mesh (*_head.md3) and make sure it loaded properly
      if (!loadMd3.ImportMD3(&m_Head,  strHeadModel)) return false;

      // Load the upper mesh (*_head.md3) and make sure it loaded properly
      if (!loadMd3.ImportMD3(&m_Upper, strUpperModel)) return false;

      // Load the lower mesh (*_lower.md3) and make sure it loaded properly
      if (!loadMd3.ImportMD3(&m_Lower, strLowerModel)) return false;

      // Load the lower skin (*_upper.skin) and make sure it loaded properly
      if  (!loadMd3.LoadSkin(&m_Lower, strLowerSkin)) return false;

      // Load the upper skin (*_upper.skin) and make sure it loaded properly
      if (!loadMd3.LoadSkin(&m_Upper, strUpperSkin)) return false;

      // Load the head skin (*_head.skin) and make sure it loaded properly
      if(!loadMd3.LoadSkin(&m_Head,  strHeadSkin)) return false;

      // Load the lower, upper and head textures.
      LoadModelTextures(&m_Lower, strPath);
      LoadModelTextures(&m_Upper, strPath);
      LoadModelTextures(&m_Head,  strPath);

      // We added to this function the code that loads the animation config file

      // This stores the file name for the .cfg animation file
      char strConfigFile[255] = {0};

      // Add the path and file name prefix to the animation.cfg file
      sprintf(strConfigFile,  "%sanimation.cfg",  strPath);

      // Load the animation config file (*_animation.config) and make sure it loaded properly
      if (!LoadAnimations(strConfigFile)) return false;

      // Link the lower body to the upper body when the tag "tag_torso" is found in our tag array
      LinkModel(&m_Lower, &m_Upper, "tag_torso");

      // Link the upper body to the head when the tag "tag_head" is found in our tag array
      LinkModel(&m_Upper, &m_Head, "tag_head");

      // The character was loaded correctly so return true
      return true;
    }


    ///////////////////////////////// LOAD WEAPON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This loads a Quake3 weapon model from the given path and weapon name
    /////
    ///////////////////////////////// LOAD WEAPON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    bool cMd3::LoadWeapon(const char* strPath, const char* strModel)
    {
      // Make sure the path and model were valid, otherwise return false
      if(!strPath || !strModel) return false;

      char strWeaponModel[255]  = {0};        // This stores the file name for the weapon model
      char strWeaponShader[255] = {0};        // This stores the file name for the weapon shader.

      // Concatenate the path and model name together
      sprintf(strWeaponModel, "%s%s.md3", strPath, strModel);

      CLoadMD3 loadMd3;                                       // This object allows us to load the.md3 and .shader file

      // Load the weapon mesh (*.md3) and make sure it loaded properly
      if (!loadMd3.ImportMD3(&m_Weapon,  strWeaponModel)) return false;

      // Add the path, file name and .shader extension together to get the file name and path
      sprintf(strWeaponShader, "%s%s.shader", strPath, strModel);

      // Load our textures associated with the gun from the weapon shader file
      if (!loadMd3.LoadShader(&m_Weapon, strWeaponShader)) return false;

      // We should have the textures needed for each weapon part loaded from the weapon's
      // shader, so let's load them in the given path.
      LoadModelTextures(&m_Weapon, strPath);

      // Link the weapon to the model's hand that has the weapon tag
      LinkModel(&m_Upper, &m_Weapon, "tag_weapon");

      // The weapon loaded okay, so let's return true to reflect this
      return true;
    }


    ///////////////////////////////// LOAD MODEL TEXTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This loads the textures for the current model passed in with a directory
    /////
    ///////////////////////////////// LOAD WEAPON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    void cMd3::LoadModelTextures(cModel* pModel, const char* strPath)
    {
      // Go through all the materials that are assigned to this model
      const size_t numOfMaterials = pModel->materials.size();
      for (size_t i = 0; i < numOfMaterials; i++) {
        // Check to see if there is a file name to load in this material
        if (strlen(pModel->materials[i].strFile) != 0) {
          // Create a boolean to tell us if we have a new texture to load
          bool bNewTexture = true;

          // Go through all the textures in our string list to see if it's already loaded
          for (size_t j = 0; j < strTextures.size(); j++) {
            // If the texture name is already in our list of texture, don't load it again.
            if (!strcmp(pModel->materials[i].strFile, strTextures[j].c_str())) {
              // We don't need to load this texture since it's already loaded
              bNewTexture = false;

              // Assign the texture index to our current material textureID.
              // This ID will them be used as an index into m_Textures[].
              pModel->materials[i].texureId = j;
            }
          }

          // Make sure before going any further that this is a new texture to be loaded
          if (!bNewTexture) continue;

          char strFullPath[255] = {0};

          // Add the file name and path together so we can load the texture
          sprintf(strFullPath, "%s%s", strPath, pModel->materials[i].strFile);

          // We pass in a reference to an index into our texture array member variable.
          // The size() function returns the current loaded texture count.  Initially
          // it will be 0 because we haven't added any texture names to our strTextures list.
          CreateTexture(m_Textures[strTextures.size()], strFullPath);

          // Set the texture ID for this material by getting the current loaded texture count
          pModel->materials[i].texureId = strTextures.size();

          // Now we increase the loaded texture count by adding the texture name to our
          // list of texture names.  Remember, this is used so we can check if a texture
          // is already loaded before we load 2 of the same textures.  Make sure you
          // understand what an STL vector list is.  We have a tutorial on it if you don't.
          strTextures.push_back(pModel->materials[i].strFile);
        }
      }
    }


    ///////////////////////////////// LOAD ANIMATIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This loads the .cfg file that stores all the animation information
    /////
    ///////////////////////////////// LOAD ANIMATIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    bool cMd3::LoadAnimations(const char* strConfigFile)
    {
      LOG<<"cMd3::LoadAnimations strConfigFile=\""<<strConfigFile<<"\""<<std::endl;

      // This function is given a path and name to an animation config file to load.
      // The implementation of this function is arbitrary, so if you have a better way
      // to parse the animation file, that is just as good.  Whatever works.
      // Basically, what is happening here, is that we are grabbing an animation line:
      //
      // "0   31      0       25              // BOTH_DEATH1"
      //
      // Then parsing it's values.  The first number is the starting frame, the next
      // is the frame count for that animation (endFrame would equal startFrame + frameCount),
      // the next is the looping frames (ignored), and finally the frames per second that
      // the animation should run at.  The end of this line is the name of the animation.
      // Once we get that data, we store the information in our tAnimationInfo object, then
      // after we finish parsing the file, the animations are assigned to each model.
      // Remember, that only the torso and the legs objects have animation.  It is important
      // to note also that the animation prefixed with BOTH_* are assigned to both the legs
      // and the torso animation list, hence the name "BOTH" :)

      // Open the config file
      std::vector<std::wstring> contents;
      spitfire::storage::ReadText(spitfire::string::ToString_t(strConfigFile), contents);

      // Here we make sure that the file was found and could be opened
      if (contents.empty()) {
        // Return an unsuccessful retrieval
        return false;
      }

      int currentAnim = 0;                            // This stores the current animation count
      int torsoOffset = 0;                            // The offset between the first torso and leg animation

      // Create an animation object for every valid animation in the Quake3 Character
      cAnimationSet animations[MAX_ANIMATIONS];

      // Here we go through every word in the file until a numeric number if found.
      // This is how we know that we are on the animation lines, and past header info.
      // This of course isn't the most solid way, but it works fine.  It wouldn't hurt
      // to put in some more checks to make sure no numbers are in the header info.
      const size_t n = contents.size();
      for (size_t i = 0; i < n; i++) {
        LOG<<"cMd3::LoadAnimations contents["<<i<<"]=\""<<contents[i]<<"\""<<std::endl;

        if (contents[i].empty()) continue;

        std::istringstream line(spitfire::string::ToUTF8(contents[i]));

        // If the first character of the word is NOT a number, we haven't hit an animation line
        std::string strWord;
        line>>strWord;
        if (isdigit(strWord[0]) == 0) {
          LOG<<"cMd3::LoadAnimations strWord=\""<<strWord<<"\", \""<<contents[i]<<"\" is not an animation line, continue"<<std::endl;
          // Ok we haven't got an animation line, let's go to the next one
          continue;
        }

        // If we get here, we must be on an animation line, so let's parse the data.
        // We should already have the starting frame stored in strWord, so let's extract it.

        // Get the number stored in the strWord string and create some variables for the rest
        int startFrame = atoi(strWord.c_str());
        int numOfFrames = 0, loopingFrames = 0, framesPerSecond = 0;

        // Read in the number of frames, the looping frames, then the frames per second
        // for this current animation we are on.
        line>>numOfFrames>>loopingFrames>>framesPerSecond;

        // Initialize the current animation structure with the data just read in
        animations[currentAnim].startFrame = startFrame;
        animations[currentAnim].endFrame = startFrame + numOfFrames;
        animations[currentAnim].loopingFrames = loopingFrames;
        animations[currentAnim].framesPerSecond = framesPerSecond;

        // Read past the "//" and read in the animation name (I.E. "BOTH_DEATH1").
        // This might not be how every config file is set up, so make sure.
        std::string strComment;
        std::string strLine;
        line>>strComment>>strLine;

        // Copy the name of the animation to our animation structure
        strcpy(animations[currentAnim].strName, strLine.c_str());

        LOG<<"cMd3::LoadAnimations strComment=\""<<strComment<<"\", strLine=\""<<strLine<<"\""<<std::endl;

        // If the animation is for both the legs and the torso, add it to their animation list
        if (IsInString(strLine, "BOTH")) {
          // Add the animation to each of the upper and lower mesh lists
          m_Upper.animations.push_back(animations[currentAnim]);
          m_Lower.animations.push_back(animations[currentAnim]);
        }
        // If the animation is for the torso, add it to the torso's list
        else if (IsInString(strLine, "TORSO")) {
          m_Upper.animations.push_back(animations[currentAnim]);
        }
        // If the animation is for the legs, add it to the legs's list
        else if (IsInString(strLine, "LEGS")) {
          // Because I found that some config files have the starting frame for the
          // torso and the legs a different number, we need to account for this by finding
          // the starting frame of the first legs animation, then subtracting the starting
          // frame of the first torso animation from it.  For some reason, some exporters
          // might keep counting up, instead of going back down to the next frame after the
          // end frame of the BOTH_DEAD3 anim.  This will make your program crash if so.

          // If the torso offset hasn't been set, set it
          if (!torsoOffset) torsoOffset = animations[LEGS_WALKCR].startFrame - animations[TORSO_GESTURE].startFrame;

          // Minus the offset from the legs animation start and end frame.
          animations[currentAnim].startFrame -= torsoOffset;
          animations[currentAnim].endFrame -= torsoOffset;

          // Add the animation to the list of leg animations
          m_Lower.animations.push_back(animations[currentAnim]);
        }

        // Increase the current animation count
        currentAnim++;
      }

      // Return a success
      return true;
    }


    ///////////////////////////////// LINK MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This links the body part models to each other, along with the weapon
    /////
    ///////////////////////////////// LINK MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    void cMd3::LinkModel(cModel* pModel, cModel* pLink, const char* strTagName)
    {
      // Make sure we have a valid model, link and tag name, otherwise quit this function
      if (!pModel || !pLink || !strTagName) return;

      // Go through all of our tags and find which tag contains the strTagName, then link'em
      for (int i = 0; i < pModel->numOfTags; i++) {
        // If this current tag index has the tag name we are looking for
        if (pModel->pTags[i].strName != strTagName) {
          // Link the model's link index to the link (or model/mesh) and return
          pModel->pLinks[i] = pLink;
          return;
        }
      }
    }


    std::string cMd3::GetHeadAnimation() const
    {
      if (m_Head.animations.empty()) return "";

      return std::string(m_Head.animations[m_Head.currentAnim].strName);
    }

    std::string cMd3::GetTorsoAnimation() const
    {
      if (m_Upper.animations.empty()) return "";

      return std::string(m_Upper.animations[m_Upper.currentAnim].strName);
    }

    std::string cMd3::GetLegsAnimation() const
    {
      if (m_Lower.animations.empty()) return "";

      return std::string(m_Lower.animations[m_Lower.currentAnim].strName);
    }

    std::string cMd3::GetWeaponAnimation() const
    {
      if (m_Weapon.animations.empty()) return "";

      return std::string(m_Weapon.animations[m_Weapon.currentAnim].strName);
    }

    void cMd3::SetTorsoAnimation(const char* strAnimation)
    {
      // Go through all of the animations in this model
      const size_t n = m_Upper.animations.size();
      for (size_t i = 0; i < n; i++) {
        // If the animation name passed in is the same as the current animation's name
        if (!strcasecmp(m_Upper.animations[i].strName, strAnimation)) {
          // Set the legs animation to the current animation we just found and return
          m_Upper.currentAnim = i;
          m_Upper.currentFrame = m_Upper.animations[m_Upper.currentAnim].startFrame;
          return;
        }
      }
    }


    ///////////////////////////////// SET LEGS ANIMATION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This sets the current animation that the lower body will be performing
    /////
    ///////////////////////////////// SET LEGS ANIMATION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    void cMd3::SetLegsAnimation(const char* strAnimation)
    {
      // Go through all of the animations in this model
      const size_t n = m_Lower.animations.size();
      for (size_t i = 0; i < n; i++) {
        // If the animation name passed in is the same as the current animation's name
        if (!strcasecmp(m_Lower.animations[i].strName, strAnimation)) {
          // Set the legs animation to the current animation we just found and return
          m_Lower.currentAnim = i;
          m_Lower.currentFrame = m_Lower.animations[m_Lower.currentAnim].startFrame;
          return;
        }
      }
    }



    // *** CLoadMD3

    CLoadMD3::CLoadMD3()
    {
      // Here we initialize our structures to 0
      memset(&m_Header, 0, sizeof(cMd3Header));

      // Set the pointers to null
      m_pSkins=NULL;
      m_pTexCoords=NULL;
      m_pTriangles=NULL;
      m_pBones=NULL;
    }


    ///////////////////////////////// IMPORT MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This is called by the client to open the .Md3 file, read it, then clean up
    /////
    ///////////////////////////////// IMPORT MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    bool CLoadMD3::ImportMD3(cModel* pModel, const char* strFileName)
    {
      char strMessage[255] = {0};

      // Open the MD3 file in binary
      m_FilePointer = fopen(strFileName, "rb");

      // Make sure we have a valid file pointer (we found the file)
      if (m_FilePointer == nullptr) {
        // Display an error message and don't load anything if no file was found
        sprintf(strMessage, "Unable to find the file: %s!", strFileName);
        LOG<<strMessage<<std::endl;
        return false;
      }

      // Read the header data and store it in our m_Header member variable
      fread(&m_Header, 1, sizeof(cMd3Header), m_FilePointer);

      // Get the 4 character ID
      char* ID = m_Header.fileID;

      // The ID MUST equal "IDP3" and the version MUST be 15, or else it isn't a valid
      // .MD3 file.  This is just the numbers ID Software chose.

      // Make sure the ID == IDP3 and the version is this crazy number '15' or else it's a bad egg
      if ((ID[0] != 'I' || ID[1] != 'D' || ID[2] != 'P' || ID[3] != '3') || m_Header.version != 15) {
        // Display an error message for bad file format, then stop loading
        sprintf(strMessage, "Invalid file format (Version not 15): %s!", strFileName);
        LOG<<strMessage<<std::endl;
        return false;
      }

      // Read in the model and animation data
      ReadMD3Data(pModel);

      // Clean up after everything
      CleanUp();

      // Return a success
      return true;
    }


    ///////////////////////////////// READ MD3 DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This function reads in all of the model's data, except the animation frames
    /////
    ///////////////////////////////// READ MD3 DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    void CLoadMD3::ReadMD3Data(cModel* pModel)
    {
      // Here we allocate memory for the bone information and read the bones in.
      m_pBones = new cMd3Bone [m_Header.numFrames];
      fread(m_pBones, sizeof(cMd3Bone), m_Header.numFrames, m_FilePointer);

      // Since we don't care about the bone positions, we just free it immediately.
      // It might be cool to display them so you could get a visual of them with the model.

      // Free the unused bones
      delete [] m_pBones;

      // Next, after the bones are read in, we need to read in the tags.  Below we allocate
      // memory for the tags and then read them in.  For every frame of animation there is
      // an array of tags.
      {
        const size_t n = m_Header.numFrames * m_Header.numTags;

        // Read the MD3 tags from the file
        cMd3Tag* pMd3Tag = new cMd3Tag[n];
        fread(pMd3Tag, sizeof(cMd3Tag), n, m_FilePointer);

        // Now move all the data from cMd3Tag structures to cModelTag structures
        pModel->pTags = new cModelTag[n];
        for (size_t i = 0; i < n; i++) {
          pModel->pTags[i].strName = pMd3Tag[i].strName;
          pModel->pTags[i].vPosition = pMd3Tag[i].vPosition;
          std::memcpy(&pModel->pTags[i].rotation, &pMd3Tag[i].rotation, 3 * 3 * sizeof(float));
        }

        // Now destroy the temporary MD3 tags as we have copies and don't need them any more
        delete [] pMd3Tag;
      }

      // Assign the number of tags to our model
      pModel->numOfTags = m_Header.numTags;

      // Now we want to initialize our links.  Links are not read in from the .MD3 file, so
      // we need to create them all ourselves.  We use a double array so that we can have an
      // array of pointers.  We don't want to store any information, just pointers to cModels.
      pModel->pLinks = (cModel**)malloc(sizeof(cModel) * m_Header.numTags);

      // Initilialize our link pointers to NULL
      for (int i = 0; i < m_Header.numTags; i++) pModel->pLinks[i] = NULL;

      // Get the current offset into the file
      long meshOffset = ftell(m_FilePointer);

      // Create a local meshHeader that stores the info about the mesh
      cMd3MeshInfo meshHeader;

      // Go through all of the sub-objects in this mesh
      for (int i = 0; i < m_Header.numMeshes; i++) {
        // Seek to the start of this mesh and read in it's header
        fseek(m_FilePointer, meshOffset, SEEK_SET);
        fread(&meshHeader, sizeof(cMd3MeshInfo), 1, m_FilePointer);

        // Here we allocate all of our memory from the header's information
        m_pSkins     = new cMd3Skin [meshHeader.numSkins];
        m_pTexCoords = new cMd3TexCoord [meshHeader.numVertices];
        m_pTriangles = new cMd3Face [meshHeader.numTriangles];
        m_pVertices  = new cMd3Triangle [meshHeader.numVertices * meshHeader.numMeshFrames];

        // Read in the skin information
        fread(m_pSkins, sizeof(cMd3Skin), meshHeader.numSkins, m_FilePointer);

        // Seek to the start of the triangle/face data, then read it in
        fseek(m_FilePointer, meshOffset + meshHeader.triStart, SEEK_SET);
        fread(m_pTriangles, sizeof(cMd3Face), meshHeader.numTriangles, m_FilePointer);

        // Seek to the start of the UV coordinate data, then read it in
        fseek(m_FilePointer, meshOffset + meshHeader.uvStart, SEEK_SET);
        fread(m_pTexCoords, sizeof(cMd3TexCoord), meshHeader.numVertices, m_FilePointer);

        // Seek to the start of the vertex/face index information, then read it in.
        fseek(m_FilePointer, meshOffset + meshHeader.vertexStart, SEEK_SET);
        fread(m_pVertices, sizeof(cMd3Triangle), meshHeader.numMeshFrames * meshHeader.numVertices, m_FilePointer);

        // Now that we have the data loaded into the Quake3 structures, let's convert them to
        // our data types like cModel and t3DObject.
        ConvertDataStructures(pModel, meshHeader);

        // Free all the memory for this mesh since we just converted it to our structures
        delete [] m_pSkins;
        delete [] m_pTexCoords;
        delete [] m_pTriangles;
        delete [] m_pVertices;

        // Increase the offset into the file
        meshOffset += meshHeader.meshSize;
      }
    }


    // *** This function converts the .md3 structures to our own model and object structures

    void CLoadMD3::ConvertDataStructures(cModel* pModel, cMd3MeshInfo meshHeader)
    {
      // Create a empty object structure to store the object's info before we add it to our list
      cStaticModel currentMesh;

      // Copy the name of the object to our object structure
      strcpy(currentMesh.strName, meshHeader.strName);

      // Assign the vertex, texture coord and face count to our new structure
      currentMesh.numOfVerts   = meshHeader.numVertices;
      currentMesh.numTexVertex = meshHeader.numVertices;
      currentMesh.numOfFaces   = meshHeader.numTriangles;

      // Allocate memory for the vertices, texture coordinates and face data.
      // Notice that we multiply the number of vertices to be allocated by the
      // number of frames in the mesh.  This is because each frame of animation has a
      // totally new set of vertices.  This will be used in the next animation tutorial.
      currentMesh.pVerts    = new spitfire::math::cVec3[currentMesh.numOfVerts * meshHeader.numMeshFrames];
      currentMesh.pTexVerts = new spitfire::math::cVec2[currentMesh.numOfVerts];
      currentMesh.pFaces    = new cFace[currentMesh.numOfFaces];

      // Go through all of the vertices and assign them over to our structure
      for (int i = 0; i < currentMesh.numOfVerts * meshHeader.numMeshFrames; i++) {
        currentMesh.pVerts[i].x =  m_pVertices[i].vertex[0] * fMD3Scale;
        currentMesh.pVerts[i].y =  m_pVertices[i].vertex[1] * fMD3Scale;
        currentMesh.pVerts[i].z =  m_pVertices[i].vertex[2] * fMD3Scale;
      }

      // Go through all of the uv coords and assign them over to our structure
      for (int i = 0; i < currentMesh.numTexVertex; i++) {
        // Since I changed the image to bitmaps, we need to negate the V ( or y) value.
        // This is because I believe that TARGA (.tga) files, which were originally used
        // with this model, have the pixels flipped horizontally.  If you use other image
        // files and your texture mapping is crazy looking, try deleting this negative.
        currentMesh.pTexVerts[i].x =  m_pTexCoords[i].textureCoord[0];
        currentMesh.pTexVerts[i].y = -m_pTexCoords[i].textureCoord[1];
      }

      // Go through all of the face data and assign it over to OUR structure
      for(int i = 0; i < currentMesh.numOfFaces; i++) {
        // Assign the vertex indices to our face data
        currentMesh.pFaces[i].vertIndex[0] = m_pTriangles[i].vertexIndices[0];
        currentMesh.pFaces[i].vertIndex[1] = m_pTriangles[i].vertexIndices[1];
        currentMesh.pFaces[i].vertIndex[2] = m_pTriangles[i].vertexIndices[2];

        // Assign the texture coord indices to our face data (same as the vertex indices)
        currentMesh.pFaces[i].coordIndex[0] = m_pTriangles[i].vertexIndices[0];
        currentMesh.pFaces[i].coordIndex[1] = m_pTriangles[i].vertexIndices[1];
        currentMesh.pFaces[i].coordIndex[2] = m_pTriangles[i].vertexIndices[2];
      }

      // Here we add the current object to our list object list
      pModel->objects.push_back(currentMesh);
    }


    ///////////////////////////////// LOAD SKIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This loads the texture information for the model from the *.skin file
    /////
    ///////////////////////////////// LOAD SKIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    bool CLoadMD3::LoadSkin(cModel* pModel, const char* strSkin)
    {
      // Make sure valid data was passed in
      if(!pModel || !strSkin) return false;

      // Open the skin file
      std::ifstream fin(strSkin);

      // Make sure the file was opened
      if(fin.fail()) {
        // Display the error message and return false
        LOG<<"Unable to load skin!"<<std::endl;
        return false;
      }

      // These 2 variables are for reading in each line from the file, then storing
      // the index of where the bitmap name starts after the last '/' character.
      std::string strLine = "";
      int textureNameStart = 0;

      // Go through every line in the .skin file
      while (getline(fin, strLine)) {
        // Loop through all of our objects to test if their name is in this line
        const size_t n = pModel->objects.size();
        for (size_t i = 0; i < n; i++) {
          // Check if the name of this object appears in this line from the skin file
          if (IsInString(strLine, pModel->objects[i].strName) ) {
            // To abstract the texture name, we loop through the string, starting
            // at the end of it until we find a '/' character, then save that index + 1.
            for (int j = strLine.length() - 1; j > 0; j--) {
              // If this character is a '/', save the index + 1
              if (strLine[j] == '/') {
                // Save the index + 1 (the start of the texture name) and break
                textureNameStart = j + 1;
                break;
              }
            }

            // Create a local material info structure
            cMaterial texture;

            // Copy the name of the file into our texture file name variable.
            strcpy(texture.strFile, &strLine[textureNameStart]);

            // The tile or scale for the UV's is 1 to 1
            texture.uTile = texture.uTile = 1;

            // Store the material ID for this object and set the texture boolean to true
            pModel->objects[i].materialID = pModel->materials.size();
            pModel->objects[i].bHasTexture = true;

            // Add the local material info structure to our model's material list
            pModel->materials.push_back(texture);
          }
        }
      }

      // Close the file and return a success
      fin.close();
      return true;
    }


    ///////////////////////////////// LOAD SHADER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This loads the basic shader texture info associated with the weapon model
    /////
    ///////////////////////////////// LOAD SHADER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    bool CLoadMD3::LoadShader(cModel* pModel, const char* strShader)
    {
      // Make sure valid data was passed in
      if (!pModel || !strShader) return false;

      // Open the shader file
      std::ifstream fin(strShader);

      // Make sure the file was opened
      if (fin.fail()) {
        // Display the error message and return false
        LOG<<"Unable to load shader!"<<std::endl;
        return false;
      }

      // These variables are used to read in a line at a time from the file, and also
      // to store the current line being read so that we can use that as an index for the
      // textures, in relation to the index of the sub-object loaded in from the weapon model.
      std::string strLine = "";
      int currentIndex = 0;

      // Go through and read in every line of text from the file
      while (getline(fin, strLine)) {
        // Create a local material info structure
        cMaterial texture;

        // Copy the name of the file into our texture file name variable
        strcpy(texture.strFile, strLine.c_str());

        // The tile or scale for the UV's is 1 to 1
        texture.uTile = texture.uTile = 1;

        // Store the material ID for this object and set the texture boolean to true
        pModel->objects[currentIndex].materialID = pModel->materials.size();
        pModel->objects[currentIndex].bHasTexture = true;

        // Add the local material info structure to our model's material list
        pModel->materials.push_back(texture);

        // Here we increase the material index for the next texture (if any)
        currentIndex++;
      }

      // Close the file and return a success
      fin.close();
      return true;
    }


    ///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This function cleans up our allocated memory and closes the file
    /////
    ///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    void CLoadMD3::CleanUp()
    {
      // Close the current file pointer
      fclose(m_FilePointer);
    }








    ///////////////////////////////// UPDATE MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This sets the current frame of animation, depending on it's fps and t
    /////
    ///////////////////////////////// UPDATE MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    void cMd3::UpdateModel(cModel* pModel)
    {
      // Initialize a start and end frame, for models with no animation
      int startFrame = 0;
      int endFrame = 1;

      // This function is used to keep track of the current and next frames of animation
      // for each model, depending on the current animation.  Some models down have animations,
      // so there won't be any change.

      // Here we grab the current animation that we are on from our model's animation list
      cAnimationSet* pAnim = &(pModel->animations[pModel->currentAnim]);

      // If there is any animations for this model
      if (!pModel->materials.empty()) {
        // Set the starting and end frame from for the current animation
        startFrame = pAnim->startFrame;
        endFrame   = pAnim->endFrame;
      }

      // This gives us the next frame we are going to.  We mod the current frame plus
      // 1 by the current animations end frame to make sure the next frame is valid.
      pModel->nextFrame = (pModel->currentFrame + 1) % endFrame;

      // If the next frame is zero, that means that we need to start the animation over.
      // To do this, we set nextFrame to the starting frame of this animation.
      if (pModel->nextFrame == 0) pModel->nextFrame =  startFrame;

      // Next, we want to get the current time that we are interpolating by.  Remember,
      // if t = 0 then we are at the beginning of the animation, where if t = 1 we are at the end.
      // Anything from 0 to 1 can be thought of as a percentage from 0 to 100 percent complete.
      SetCurrentTime(pModel);
    }


    void cMd3::Update(sampletime_t currentTime)
    {
      // Since we have animation now, when we draw the model the animation frames need
      // to be updated.  To do that, we pass in our lower and upper models to UpdateModel().
      // There is no need to pass in the head or weapon, since they don't have any animation.

      // Update the leg and torso animations
      UpdateModel(&m_Lower);
      UpdateModel(&m_Upper);
    }


    ///////////////////////////////// DRAW LINK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This draws the current mesh with an effected matrix stack from the last mesh
    /////
    ///////////////////////////////// DRAW LINK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    void cMd3::DrawLink(const cModel* pModel)
    {
      // Draw the current model passed in (Initially the legs)
      RenderModel(pModel);

      // Though the changes to this function from the previous tutorial aren't huge, they
      // are pretty powerful.  Since animation is in effect, we need to create a rotational
      // matrix for each key frame, at each joint, to be applied to the child nodes of that
      // object.  We can also slip in the interpolated translation into that same matrix.
      // The big thing in this function is interpolating between the 2 rotations.  The process
      // involves creating 2 quaternions from the current and next key frame, then using
      // slerp (spherical linear interpolation) to find the interpolated quaternion, then
      // converting that quaternion to a 4x4 matrix, adding the interpolated translation
      // to that matrix, then finally applying it to the current model view matrix in OpenGL.
      // This will then effect the next objects that are somehow explicitly or inexplicitly
      // connected and drawn from that joint.

      spitfire::math::cQuaternion qQuat;
      spitfire::math::cQuaternion qNextQuat;
      spitfire::math::cQuaternion qInterpolatedQuat;
      float* pMatrix;
      float* pNextMatrix;
      float finalMatrix[16] = { 0 };

      // Now we need to go through all of this models tags and draw them.
      for (int i = 0; i < pModel->numOfTags; i++) {
        // Get the current link from the models array of links (Pointers to models)
        const cModel* pLink = pModel->pLinks[i];
        if (pLink != nullptr) {

          // To find the current translation position for this frame of animation, we multiply
          // the currentFrame by the number of tags, then add i.  This is similar to how
          // the vertex key frames are interpolated.
          spitfire::math::cVec3 vPosition = pModel->pTags[pModel->currentFrame * pModel->numOfTags + i].vPosition;

          // Grab the next key frame translation position
          spitfire::math::cVec3 vNextPosition = pModel->pTags[pModel->nextFrame * pModel->numOfTags + i].vPosition;

          // By using the equation: p(t) = p0 + t(p1 - p0), with a time t,
          // we create a new translation position that is closer to the next key frame.
          vPosition.x = vPosition.x + pModel->t * (vNextPosition.x - vPosition.x),
          vPosition.y = vPosition.y + pModel->t * (vNextPosition.y - vPosition.y),
          vPosition.z = vPosition.z + pModel->t * (vNextPosition.z - vPosition.z);

          // Now comes the more complex interpolation.  Just like the translation, we
          // want to store the current and next key frame rotation matrix, then interpolate
          // between the 2.

          // Get a pointer to the start of the 3x3 rotation matrix for the current frame
          pMatrix = &pModel->pTags[pModel->currentFrame * pModel->numOfTags + i].rotation[0][0];

          // Get a pointer to the start of the 3x3 rotation matrix for the next frame
          pNextMatrix = &pModel->pTags[pModel->nextFrame * pModel->numOfTags + i].rotation[0][0];

          // Now that we have 2 1D arrays that store the matrices, let's interpolate them

          // Convert the current and next key frame 3x3 matrix into a quaternion
          qQuat.SetFromMD3Matrix(pMatrix, 3);
          qNextQuat.SetFromMD3Matrix(pNextMatrix, 3);

          // Using spherical linear interpolation, we find the interpolated quaternion
          qInterpolatedQuat.SlerpForMD3(qQuat, qNextQuat, pModel->t);

          // Here we convert the interpolated quaternion into a 4x4 matrix
          spitfire::math::cMat4 m = qInterpolatedQuat.GetMD3Matrix();
          for (size_t i = 0; i < 16; i++) finalMatrix[i] = m[i];

          // To cut out the need for 2 matrix calls, we can just slip the translation
          // into the same matrix that holds the rotation.  That is what index 12-14 holds.
          finalMatrix[12] = 64.0f * vPosition.x;
          finalMatrix[13] = 64.0f * vPosition.y;
          finalMatrix[14] = 64.0f * vPosition.z;

          // Start a new matrix scope
          glPushMatrix();

            // Finally, apply the rotation and translation matrix to the current matrix
            glMultMatrixf(finalMatrix);

            // Recursively draw the next model that is linked to the current one.
            // This could either be a body part or a gun that is attached to
            // the hand of the upper body model.
            DrawLink(pLink);

          // End the current matrix scope
          glPopMatrix();
        }
      }
    }


    ///////////////////////////////// SET CURRENT TIME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This sets time t for the interpolation between the current and next key frame
    /////
    ///////////////////////////////// SET CURRENT TIME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    void cMd3::SetCurrentTime(cModel* pModel)
    {
      // This function is very similar to finding the frames per second.
      // Instead of checking when we reach a second, we check if we reach
      // 1 second / our animation speed. (1000 ms / animationSpeed).
      // That's how we know when we need to switch to the next key frame.
      // In the process, we get the t value for how far we are at to going to the
      // next animation key frame.  We use time to do the interpolation, that way
      // it runs the same speed on any persons computer, regardless of their specs.
      // It might look choppier on a junky computer, but the key frames still be
      // changing the same time as the other persons, it will just be not as smooth
      // of a transition between each frame.  The more frames per second we get, the
      // smoother the animation will be.  Since we are working with multiple models
      // we don't want to create static variables, so the t and elapsedTime data are
      // stored in the model's structure.

      // Return if there is no animations in this model
      if (!pModel->animations.size()) return;

      // Get the current time in milliseconds
      const float time = spitfire::util::GetTimeMS();

      // Find the time that has elapsed since the last time that was stored
      float elapsedTime = time - pModel->lastTime;

      // Store the animation speed for this animation in a local variable
      const int animationSpeed = pModel->animations[pModel->currentAnim].framesPerSecond;

      // To find the current t we divide the elapsed time by the ratio of:
      //
      // (1_second / the_animation_frames_per_second)
      //
      // Since we are dealing with milliseconds, we need to use 1000
      // milliseconds instead of 1 because we are using GetTickCount(), which is in
      // milliseconds. 1 second == 1000 milliseconds.  The t value is a value between
      // 0 to 1.  It is used to tell us how far we are from the current key frame to
      // the next key frame.
      const float t = elapsedTime / (1000.0f / animationSpeed);

      // If our elapsed time goes over the desired time segment, start over and go
      // to the next key frame.
      if (elapsedTime >= (1000.0f / animationSpeed)) {
        // Set our current frame to the next key frame (which could be the start of the anim)
        pModel->currentFrame = pModel->nextFrame;

        // Set our last time for the model to the current time
        pModel->lastTime = time;
      }

      // Set the t for the model to be used in interpolation
      pModel->t = t;
    }



    ///////////////////////////////// RENDER MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This renders the model data to the screen
    /////
    ///////////////////////////////// RENDER MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    void cMd3::RenderModel(const cModel* pModel)
    {
      size_t nTriangles = 0;

      // Make sure we have valid objects just in case. (size() is in the STL vector class)
      if (pModel->objects.size() <= 0) return;

      // Go through all of the objects stored in this model
      const size_t n = pModel->objects.size();
      for (size_t i = 0; i < n; i++) {
        // Get the current object that we are displaying
        const cStaticModel* pObject = &pModel->objects[i];

        // Now that we have animation for our model, we need to interpolate between
        // the vertex key frames.  The .md3 file format stores all of the vertex
        // key frames in a 1D array.  This means that in order to go to the next key frame,
        // we need to follow this equation:  currentFrame * numberOfVertices
        // That will give us the index of the beginning of that key frame.  We just
        // add that index to the initial face index, when indexing into the vertex array.

        // Find the current starting index for the current key frame we are on
        size_t currentIndex = pModel->currentFrame * pObject->numOfVerts;

        // Since we are interpolating, we also need the index for the next key frame
        size_t nextIndex = pModel->nextFrame * pObject->numOfVerts;

        // If the object has a texture assigned to it, let's bind it to the model.
        // This isn't really necessary since all models have textures, but I left this
        // in here to keep to the same standard as the rest of the model loaders.
        if (pObject->bHasTexture) {
          // Turn on texture mapping
          glEnable(GL_TEXTURE_2D);

          // Grab the texture index from the materialID index into our material list
          int textureID = pModel->materials[pObject->materialID].texureId;

          // Bind the texture index that we got from the material textureID
          glBindTexture(GL_TEXTURE_2D, m_Textures[textureID]);
        } else {
          // Turn off texture mapping
          glDisable(GL_TEXTURE_2D);
        }

        // Start drawing our model triangles
        glBegin(GL_TRIANGLES);

          // Go through all of the faces (polygons) of the object and draw them
          for(int j = 0; j < pObject->numOfFaces; j++) {
            // Go through each vertex of the triangle and draw it.
            for (size_t whichVertex = 0; whichVertex < 3; whichVertex++) {
              // Get the index for the current point in the face list
              size_t index = pObject->pFaces[j].vertIndex[whichVertex];

              // Make sure there is texture coordinates for this (%99.9 likelyhood)
              if (pObject->pTexVerts) {
                // Assign the texture coordinate to this vertex
                glTexCoord2f(pObject->pTexVerts[ index ].x, -pObject->pTexVerts[ index ].y);
              }

              // Like in the MD2 Animation tutorial, we use linear interpolation
              // between the current and next point to find the point in between,
              // depending on the model's "t" (0.0 to 1.0).

              // Store the current and next frame's vertex by adding the current
              // and next index to the initial index given from the face data.
              const spitfire::math::cVec3 vPoint1 = pObject->pVerts[currentIndex + index];
              const spitfire::math::cVec3 vPoint2 = pObject->pVerts[nextIndex + index];

              // By using the equation: p(t) = p0 + t(p1 - p0), with a time t,
              // we create a new vertex that is closer to the next key frame.
              glVertex3f(
                vPoint1.x + pModel->t * (vPoint2.x - vPoint1.x),
                vPoint1.y + pModel->t * (vPoint2.y - vPoint1.y),
                vPoint1.z + pModel->t * (vPoint2.z - vPoint1.z)
              );
            }
          }

          nTriangles += pObject->numOfFaces;

        // Stop drawing polygons
        glEnd();
      }
    }


    ///////////////////////////////// DRAW MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////   This recursively draws all the character nodes, starting with the legs
    /////
    ///////////////////////////////// DRAW MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

    void cMd3::Render(sampletime_t currentTime)
    {
      const float r = 1.0f;
      const float g = 1.0f;
      const float b = 1.0f;
      const float a = 1.0f;

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
        // MD3 models are centred about the hips, let's translate our model up by the length of the legs
        glTranslatef(0.0f, 0.0f, 6.5f);

        // MD3 models are apparently scaled up 64x
        glScalef(1.0f / 64.0f, 1.0f / 64.0f, 1.0f / 64.0f);

        // This scale is still to large, so let's shrink it even more
        glScalef(0.28f, 0.28f, 0.28f);

        // Correct for incorrectly facing model
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);

        glEnable(GL_CULL_FACE);        // Turn back face culling on
        glCullFace(GL_FRONT);          // MD3 models use front face culling apparently

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);

        glColor4f(r, g, b, a);

        // Draw the first link, which is the lower body.  This will then recursively go
        // through the models attached to this model and drawn them.
        DrawLink(&m_Lower);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glEnable(GL_CULL_FACE);        // Turn back face culling on
        glCullFace(GL_BACK);           // Quake3 uses front face culling apparently

        glDisable(GL_TEXTURE_2D);

        glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
    }
  }
}
