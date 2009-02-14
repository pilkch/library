/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*==========================================================================
 *
 * Project : Freyja
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://gooseegg.sourceforge.net
 * Email   : stu7440@westga.edu
 * Object  : Md3
 * Comments: This is the MD3 model class.
 *
 *           See file COPYING for license details.
 *
 *           Quake (c) id Software 1995 - 2000
 *
 *           MD3 file format specs provided by Mental Vortex,
 *           and that site says the specs may not be 100% correct
 *           so - since this class is desgined using their specs...
 *
 *-- Test Defines -----------------------------------------------------
 *
 * TEST_MD3      - Builds Md3 with it's test code as a console program
 *
 *-- History ----------------------------------------------------------
 *
 * 2000-10-07:
 * Mongoose - Interface update
 *
 * 2000-06-03:
 * Mongoose - Created, MD3 specs provided by Mental Vortex
 ==========================================================================*/


#ifndef __FREYJA_MONGOOSE_MD3_H
#define __FREYJA_MONGOOSE_MD3_H


#define MD3_IDALIASHEADER  0x33504449 // "IDP3"
#define MD3_ALIAS_VERSION  15

#ifndef vec3_t
typedef float vec3_t[3];
#endif

#ifndef mat3_t
typedef float mat3_t[3][3];
#endif


typedef struct md3_tag_s
{
  char name[64];    /* Name of 'tag' as it's usually 
		       called in the md3 files try to 
		       see it as a sub-mesh/seperate 
		       mesh-part. 

		       Sometimes this 64 string may 
		       contain some garbage, but 
		       i've been told this is because
		       some tools leave garbage in
		       those strings, but they ARE
		       strings... */

  vec3_t center;    /* Relative position of tag */
 
  mat3_t rotation;  /* The direction the tag is facing 
		       relative to the rest of the model */

} md3_tag_t;


typedef struct md3_boneframe_s
{
  float  mins[3];
  float  maxs[3];
  float  center[3];
  float  scale;
  char   creator[16]; 

} md3_boneframe_t;


typedef struct md3_skin_s
{
  char name[68];       /* Name of skin used by mesh
			  65 chars, 
			  32 bit aligned == 68 chars  */

  int index;              /* For shader use (skin is a shader) */

} md3_skin_t;


typedef struct md3_tri_index_s
{
  int  triangle[3];    /* Vertex 1, 2, 3 of triangle */

} md3_tri_index_t;


typedef struct md3_texel_s
{
  float st[2];         /* Texel (s, t) */

} md3_texel_t;


typedef struct md3_vertex_s
{
  signed short pos[3];       /* Vertex X/Y/Z coordinate */

  unsigned char st[2];       /* Enviromental mapping 
				texture coordinates */

} md3_vertex_t;


typedef struct md3_mesh_s
{
  // Start Mesh Header /////////////

  char id[4];          /* Mesh alias id, must be IDP3  */

  char name[68];       /* Name of mesh  
								  65 chars,
								  32 bit aligned == 68 chars */
  int flags;


  int  num_frames;     /* Number of VertexMorph frames 
			  in mesh */

  int num_shaders;     /* Number of shaders in mesh*/

  int  num_skins;      /* Number of skins in mesh, 
								  for backwards compatibility with
								  incorrect/obsolete spec  */

  int  num_vertices;   /* Number of vertices */
 
  int  num_triangles;  /* Number of triangles */
 
  int  tris_offset;    /* Starting position of 
			  Triangle data, relative 
			  to start of Mesh_Header  */

  int  header_size;    /* Size of header  */
  
  int  texel_offset;   /* Starting position of 
			  texvector data, relative 
			  to start of Mesh_Header */
 
  int  vertex_offset;  /* Starting position of 
			  vertex data,relative 
			  to start of Mesh_Header  */

  int  mesh_size;      /* Size of mesh  */

  // End Mesh Header ///////////////


  md3_skin_t *skin;       /* Skins */

  md3_tri_index_t *tris;  /* Triangles */

  md3_texel_t *texel;     /* Texels */
  
  md3_vertex_t *vertex;   /* Vertices */

} md3_mesh_t;


class Md3
{
public:

  Md3();
  /*--------------------------------------------
   * Created  : 2000-06-03 by Mongoose
   * Modified : 
   *
   * Pre  : None
   * Post : Md3 object is constructed
   --------------------------------------------*/

  ~Md3();
  /*--------------------------------------------
   * Created  : 2000-06-03 by Mongoose
   * Modified : 
   *
   * Pre  : Md3 object is allocated
   * Post : Md3 object is deconstructed
   --------------------------------------------*/

  void NumMeshes(int n);
  /*--------------------------------------------
   * Created  : 2000-10-24 by Mongoose
   * Modified : 
   *
   * Pre  : 
   * Post : Mesh count is set
   --------------------------------------------*/

  void NumTags(int n);
  /*--------------------------------------------
   * Created  : 2000-10-24 by Mongoose
   * Modified : 
   *
   * Pre  : 
   * Post : Tag count is set
   --------------------------------------------*/

  void NumBones(int n);
  /*--------------------------------------------
   * Created  : 2000-10-24 by Mongoose
   * Modified : 
   *
   * Pre  : 
   * Post : Bone count is set
   --------------------------------------------*/

  void Mesh(md3_mesh_t *mesh);
  /*--------------------------------------------
   * Created  : 2000-10-24 by Mongoose
   * Modified : 
   *
   * Pre  : 
   * Post : Mesh list is set
   --------------------------------------------*/

  void Tag(md3_tag_t *tag);
  /*--------------------------------------------
   * Created  : 2000-10-24 by Mongoose
   * Modified : 
   *
   * Pre  : 
   * Post : Tag list is set
   --------------------------------------------*/

  void Bone(md3_boneframe_t *bone);
  /*--------------------------------------------
   * Created  : 2000-10-24 by Mongoose
   * Modified : 
   *
   * Pre  : 
   * Post : Bone list is set
   --------------------------------------------*/
  
  int NumMeshes();
  /*--------------------------------------------
   * Created  : 2000-06-03 by Mongoose
   * Modified : 
   *
   * Pre  : 
   * Post : Mesh count is returned
   --------------------------------------------*/

  int NumTags();
  /*--------------------------------------------
   * Created  : 2000-10-06 by Mongoose
   * Modified : 
   *
   * Pre  : 
   * Post : Tag count is returned
   --------------------------------------------*/
  
  int NumBones();
  /*--------------------------------------------
   * Created  : 2000-10-06 by Mongoose
   * Modified : 
   *
   * Pre  : 
   * Post : Bone count is returned
   --------------------------------------------*/
  
  md3_mesh_t *Mesh();
  /*--------------------------------------------
   * Created  : 2000-06-03 by Mongoose
   * Modified : 
   *
   * Pre  : 
   * Post : Mesh list is returned
   --------------------------------------------*/
  
  md3_tag_t *Tag();
  /*--------------------------------------------
   * Created  : 2000-10-06 by Mongoose
   * Modified : 
   *
   * Pre  : 
   * Post : Tag list is returned
   --------------------------------------------*/
  
  md3_boneframe_t *Bone();
  /*--------------------------------------------
   * Created  : 2000-10-06 by Mongoose
   * Modified : 
   *
   * Pre  : 
   * Post : Bone list is returned
   --------------------------------------------*/
  
  int Load(const char *filename);
  /*--------------------------------------------
   * Created  : 2000-06-03 by Mongoose
   * Modified : 
   *
   * Pre  : 
   * Post : Error code is returned, 0 no error
   *        Md3 filename is read into memory
   --------------------------------------------*/
  
  int Save(const char *filename);
  /*--------------------------------------------
   * Created  : 2000-10-06 by Mongoose
   * Modified : 
   *
   * Pre  : 
   * Post : Error code is returned, 0 no error
   *        Md3 saved as filename
   --------------------------------------------*/

  bool IsValid() const { return bIsValid; }

private:
  bool bIsValid;

  // Start Header //////////////////////////////////////////////

  int _id;                 /* Alias id always "IDP3" */

  int  _version;           /* Version number, always 15 */

  char _filename[68];      /* Sometimes left blank... 
                              65 chars, 32bit aligned == 
                              68 chars */
 
  int  _num_bone_frames;   /* Number of BoneFrames */

  int  _num_tags;          /* Number of 'tags' per BoneFrame */

  int  _num_meshes;        /* Number of meshes/skins */ 

  int  _max_skins;         /* Maximum number of unique skins
                              used in md3 file */ 

  int  _header_length;     /* Always equal to the length of 
                              this header */ 

  int  _tag_start;         /* Starting position of 
                              tag-structures */ 

  int  _surfaces_start;    /* Starting position of 
                              geometeric data (mesh structures) */

  int  _file_size;         /* Size of file */

  // End Header ////////////////////////////////////////////////


  md3_tag_t *_tag;         /* Tags */

  md3_boneframe_t *_bone;  /* Bones */

  md3_mesh_t *_mesh;       /* Meshes */
};

#endif
