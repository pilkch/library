/*==========================================================================
 * 
 * Project : GooseEgg
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440
 * Email   : stu7440@westga.edu
 * Object  : Md3
 * Comments: This is the MD3 model module.
 *
 *           See file COPYING for license details.
 *
 *           Quake (c) id Software 1995 - 2000
 *
 *           MD3 file format specs provided by Mental Vortex,
 *           and that site says the specs may not be 100% correct
 *           so - since this class is desgined using their specs...
 *
 *
 *-- History ---------------------------------------------------------- 
 *
 * 2000-10-06:
 * Mongoose - The new code for the new interface
 *
 * 2000-06-03:
 * Mongoose - Those specs where FUBAR - but I figured out the ordering
 * Mongoose - Created from MD3 specs provided by Mental Vortex
 ==========================================================================*/

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <sys/types.h>

#include <breathe/render/model/cMd3Loader.h>



Md3::Md3() :
  bIsValid(false)
{
  memset(_filename, 0, 68);     
 
  _id = 0;
  _version = 0;           
  _num_bone_frames = 0;   
  _num_tags = 0;          
  _num_meshes = 0;        
  _max_skins = 0;

  _header_length = 0;     
  _tag_start = 0;         
  _surfaces_start = 0;          
  _file_size = 0;        

  _tag = NULL;     
  _bone = NULL;
  _mesh = NULL;
}


Md3::~Md3()
{
  int i;


  if (_tag)
    delete [] _tag;     

  if (_bone)
    delete [] _bone;

  for (i = 0; i < _num_meshes && _mesh; i++)
  {
    if (_mesh[i].skin)
      delete [] _mesh[i].skin;

    if (_mesh[i].tris)
      delete [] _mesh[i].tris;

    if (_mesh[i].texel)
      delete [] _mesh[i].texel;

    if (_mesh[i].vertex)
      delete [] _mesh[i].vertex;
  }

  if (_mesh)
    delete [] _mesh;
} 


void Md3::NumMeshes(int n)
{
  _num_meshes = n;
}


void Md3::NumTags(int n)
{
  _num_tags = n;
}


void Md3::NumBones(int n)
{
  _num_bone_frames = n;
}


void Md3::Mesh(md3_mesh_t *mesh)
{
  _mesh = mesh;
}


void Md3::Tag(md3_tag_t *tag)
{
  _tag = tag;
}


void Md3::Bone(md3_boneframe_t *bone)
{
  _bone = bone;
}


int Md3::NumMeshes()
{
  return _num_meshes;
}


int Md3::NumTags()
{
  return _num_tags;
}


int Md3::NumBones()
{
  return _num_bone_frames;
}


md3_mesh_t *Md3::Mesh()
{
  return _mesh;
}


md3_tag_t *Md3::Tag()
{
  return _tag;
}


md3_boneframe_t *Md3::Bone()
{
  return _bone;
}


int Md3::Load(const char *filename)
{
  bIsValid = false;

  FILE *f;
  int i, j, k, npherno_warn;
  bool dump_skin;
  int16_t ss;
  int8_t sb;

  
  npherno_warn = 0;

  f = fopen(filename, "rb");

  if (!f)
  {
    perror("Md3::Load> \n");
    return -1;
  }

  // Start Header ////////////////

  fread(&_id, sizeof(int), 1, f);
  printf("Md3::Load> _id = 0x%x\n", _id);
  
  if (_id != MD3_IDALIASHEADER)
  {
    printf("Md3::Load> ERROR: header not 0x%x\n", MD3_IDALIASHEADER);
    return -2;
  }
  
  fread(&_version, sizeof(int), 1, f);
  printf("Md3::Load> _version = %i\n", _version);

  if (_version != MD3_ALIAS_VERSION)
  {
    printf("Md3::Load> ERROR: version not %i\n", MD3_ALIAS_VERSION);
    return -3;
  }
  
  fread(&_filename, 68, 1, f);
  printf("Md3::Load> _filename = '%s'\n", _filename);

  fread(&_num_bone_frames, sizeof(int), 1, f);
  printf("Md3::Load> _num_bone_frames = %i\n", _num_bone_frames);

  fread(&_num_tags, sizeof(int), 1, f);
  printf("Md3::Load> _num_tags = %i\n", _num_tags);

  fread(&_num_meshes, sizeof(int), 1, f);
  printf("Md3::Load> _num_meshes = %i\n", _num_meshes);

  fread(&_max_skins, sizeof(int), 1, f);
  printf("Md3::Load> _max_skins = %i\n", _max_skins);

  fread(&_header_length, sizeof(int), 1, f);
  printf("Md3::Load> _header_length = %i\n", _header_length);

  fread(&_tag_start, sizeof(int), 1, f);
  printf("Md3::Load> _tag_start = %i\n", _tag_start);

  fread(&_surfaces_start, sizeof(int), 1, f);
  printf("Md3::Load> _surfaces_start = %i\n", _surfaces_start);

  fread(&_file_size, sizeof(int), 1, f);
  printf("Md3::Load> _file_size = %i\n", _file_size);
 
  // End Header //////////////////

  printf("Md3::Load> Loading %i bones\n", _num_bone_frames);
  _bone = new md3_boneframe_t[_num_bone_frames];

  for (i = 0; i < _num_bone_frames; i++)
  {
    //fread(&_bone[i].mins, 12, 1, f);
    fread(&_bone[i].mins[0], sizeof(float), 1, f);
    fread(&_bone[i].mins[1], sizeof(float), 1, f);
    fread(&_bone[i].mins[2], sizeof(float), 1, f);
    //fread(&_bone[i].maxs, 12, 1, f);
    fread(&_bone[i].maxs[0], sizeof(float), 1, f);
    fread(&_bone[i].maxs[1], sizeof(float), 1, f);
    fread(&_bone[i].maxs[2], sizeof(float), 1, f);
    //fread(&_bone[i].center, 12, 1, f);
    fread(&_bone[i].center[0], sizeof(float), 1, f);
    fread(&_bone[i].center[1], sizeof(float), 1, f);
    fread(&_bone[i].center[2], sizeof(float), 1, f);
    //fread(&_bone[i].scale, 4, 1, f);
    fread(&_bone[i].scale, sizeof(float), 1, f);
    fread(&_bone[i].creator, 16, 1, f);

    if (!i && strcmp("NPherno->MD3", _bone[i].creator) == 0)
      npherno_warn = 1;

    printf("Md3::Load> _bone[%i].mins = %f %f %f\n", i, 
	   _bone[i].mins[0], _bone[i].mins[1], _bone[i].mins[2]);
    printf("Md3::Load> _bone[%i].maxs = %f %f %f\n", i, 
	   _bone[i].maxs[0], _bone[i].maxs[1], _bone[i].maxs[2]);
    printf("Md3::Load> _bone[%i].center = %f %f %f\n", i, 
	   _bone[i].center[0], _bone[i].center[1], _bone[i].center[2]);
    printf("Md3::Load> _bone[%i].scale = %f\n", i, _bone[i].scale);
    printf("Md3::Load> _bone[%i].creator = '%s'\n", i, _bone[i].creator);
  }


  if (_tag_start != ftell(f))
  {
    printf("Md3::Load> _tag_start %i != file pos %lu\n", _tag_start,ftell(f));

    fseek(f, _tag_start, SEEK_SET);
    printf("Md3::Load> File position set to %lu\n", ftell(f));
  }


  printf("Md3::Load> Loading %i tags\n", _num_tags * _num_bone_frames);
  _tag = new md3_tag_t[_num_tags * _num_bone_frames];

  for (i = 0; i < _num_tags * _num_bone_frames; i++)
  {
    fread(&_tag[i].name, 64, 1, f);
    //fread(&_tag[i].center, 12, 1, f);
    fread(&_tag[i].center[0], sizeof(float), 1, f);
    fread(&_tag[i].center[1], sizeof(float), 1, f);
    fread(&_tag[i].center[2], sizeof(float), 1, f);
    fread(&_tag[i].rotation, 36, 1, f);

    printf("Md3::Load> _tag[%i].name = '%s'\n", i, _tag[i].name);
  }

  if (_surfaces_start != ftell(f))
  {
    printf("Md3::Load> _surfaces_start %i != file pos %lu\n", 
	   _surfaces_start, ftell(f));

    fseek(f, _surfaces_start, SEEK_SET);
    printf("Md3::Load> File position set to %lu\n", ftell(f));
  }
 
  _mesh = new md3_mesh_t[_num_meshes];

  for(i = 0; i < _num_meshes; i++)
  {
    // Start Mesh Header /////////////////

    fread(&_mesh[i].id, 4, 1, f);
    fread(&_mesh[i].name, 64, 1, f);
    fread(&_mesh[i].flags, 4, 1, f);
    fread(&_mesh[i].num_frames, 4, 1, f);
    
    fread(&_mesh[i].num_shaders, 4, 1, f);
    _mesh[i].num_skins = _mesh[i].num_shaders;
    
    fread(&_mesh[i].num_vertices, 4, 1, f);
    fread(&_mesh[i].num_triangles, 4, 1, f);
    fread(&_mesh[i].tris_offset, 4, 1, f);
    fread(&_mesh[i].header_size, 4, 1, f);   // offset from start of surface
                                             // to shaders
    fread(&_mesh[i].texel_offset, 4, 1, f);
    fread(&_mesh[i].vertex_offset, 4, 1, f); // offset to normals
                                             // num_verts * num_frames
    fread(&_mesh[i].mesh_size, 4, 1, f);     // next surface

    printf("\nMd3::Load> _mesh[%i].id = '%c%c%c%c'\n", i, 
	   _mesh[i].id[0], _mesh[i].id[1], _mesh[i].id[2], _mesh[i].id[3]);
    printf("Md3::Load> _mesh[%i].name = '%s'\n", i, 
	   _mesh[i].name);
    printf("Md3::Load> _mesh[%i].flags = %i\n", i, 
	   _mesh[i].flags);
    printf("Md3::Load> _mesh[%i].num_frames = %i\n", i, 
	   _mesh[i].num_frames);
    printf("Md3::Load> _mesh[%i].num_shaders = %i\n", i, 
	   _mesh[i].num_shaders);
    printf("Md3::Load> _mesh[%i].num_vertices = %i\n", i, 
	   _mesh[i].num_vertices);
    printf("Md3::Load> _mesh[%i].num_triangles = %i\n", i, 
	   _mesh[i].num_triangles);
    printf("Md3::Load> _mesh[%i].tris_offset = %i\n", i, 
	   _mesh[i].tris_offset+_surfaces_start);
    printf("Md3::Load> _mesh[%i].shader_offset = %i\n", i, 
	   _mesh[i].header_size+_surfaces_start);
    printf("Md3::Load> _mesh[%i].texel_offset = %i\n", i, 
	   _mesh[i].texel_offset+_surfaces_start);
    printf("Md3::Load> _mesh[%i].vertex_offset = %i\n", i, 
	   _mesh[i].vertex_offset+_surfaces_start);
    printf("Md3::Load> _mesh[%i].mesh_end = %i\n", i, 
	   _mesh[i].mesh_size+_surfaces_start);

    // This will kind of handle bad mesh reads here
    if (*(int*)&_mesh[i].id != MD3_IDALIASHEADER)
    {
      printf("Md3::Load> ERROR: Bad/damaged mesh, handling...\n");

      printf("Md3::Load> HANDLE: Keeping valid meshes... %i/%i total\n",
	     i, _num_meshes);

      _num_meshes = i; // don't use more than prev valid meshes
      break;
    }

    // End Mesh Header ///////////////////

    _mesh[i].skin = NULL;
    _mesh[i].tris = NULL;
    _mesh[i].texel = NULL;
    _mesh[i].vertex = NULL;

    // Start shaders /////////////////////
    if (_surfaces_start+_mesh[i].header_size != ftell(f))
    {
      printf("\nMd3::Load> WARNING: _mesh[%i] shader offset %i != file pos %lu\n",
	     i, _surfaces_start+_mesh[i].header_size, ftell(f));
      
      fseek(f, _surfaces_start+_mesh[i].header_size, SEEK_SET);
      printf("Md3::Load> HANDLE: File position set to %lu\n", ftell(f));
    }

    _mesh[i].skin = new md3_skin_t[_mesh[i].num_skins];

    dump_skin = false;

    for (j = 0; j < _mesh[i].num_skins; j++)
    {
      fread(&_mesh[i].skin[j].name, 68, 1, f);

      if (!_mesh[i].skin[j].name[0])
      {
	_mesh[i].skin[j].name[0] = 'm';

	printf("Md3::Load> WARNING: Error in skin name %i/%i.\n",
	       j+1, _mesh[i].num_skins);

	dump_skin = true;
      }

#ifdef TEST_MD3_STRICT
      dump_skin = true;
#endif

      if (dump_skin)
      {
	printf("Dumping skin buffer:\n'");

	for (k = 0; k < 68; k++)
	{
	  if (_mesh[i].skin[j].name[k] > 32 && 
	      _mesh[i].skin[j].name[k] < 127)
	  {
	    printf("%c", _mesh[i].skin[j].name[k]);
	  }
	  else
	  {
	    printf("%c", 183); // print a dot for invalid text
	  }
	}

	printf("'\n");
      }

      printf("Md3::Load> _mesh[%i].skin[%i].name = '%s'\n", 
	     i, j, _mesh[i].skin[j].name);
    }

    // Start triangles ////////////////////////
    if (_surfaces_start+_mesh[i].tris_offset != ftell(f))
    {
      printf("\nMd3::Load> WARNING: _mesh[%i] tris offset %i != file pos %lu\n", i, 
	     _surfaces_start+_mesh[i].tris_offset, ftell(f));
      
      fseek(f, _surfaces_start+_mesh[i].tris_offset, SEEK_SET);
      printf("Md3::Load> HANDLE: File position set to %lu\n", ftell(f));
    }

    _mesh[i].tris = new md3_tri_index_t[_mesh[i].num_triangles];

    for (j = 0; j < _mesh[i].num_triangles; j++)
    {
      fread(&_mesh[i].tris[j].triangle, 12, 1, f);
    }

    // Start texels /////////////////////
    if (_surfaces_start+_mesh[i].texel_offset != ftell(f))
    {
      printf("\nMd3::Load> WARNING: _mesh[%i] texel offset %i != file pos %lu\n", i, 
	     _surfaces_start+_mesh[i].texel_offset, ftell(f));
      
      fseek(f, _surfaces_start+_mesh[i].texel_offset, SEEK_SET);
      printf("Md3::Load> HANDLE: File position set to %lu\n", ftell(f));
    }

    _mesh[i].texel = new md3_texel_t[_mesh[i].num_vertices];

    for (j = 0; j < _mesh[i].num_vertices; j++)
    {
      fread(&_mesh[i].texel[j].st[0], sizeof(float), 1, f);
      fread(&_mesh[i].texel[j].st[1], sizeof(float), 1, f);
    }

    // Start vertices /////////////////////
    if (_surfaces_start+_mesh[i].vertex_offset != ftell(f))
    {
      printf("\nMd3::Load> WARNING: _mesh[%i] vertex offset %i != file pos %lu\n", i, 
	     _surfaces_start+_mesh[i].vertex_offset, ftell(f));
      
      fseek(f, _surfaces_start+_mesh[i].vertex_offset, SEEK_SET);
      printf("Md3::Load> HANDLE: File position set to %lu\n", ftell(f));
    }

    // Should be num_vertices * num_frames??
    _mesh[i].vertex = 
      new md3_vertex_t[_mesh[i].num_vertices * _mesh[i].num_frames];

    for (j = 0; j < _mesh[i].num_vertices * _mesh[i].num_frames; j++)
    {
      fread(&ss, 2, 1, f);
      _mesh[i].vertex[j].pos[0] = ss;
      fread(&ss, 2, 1, f);
      _mesh[i].vertex[j].pos[1] = ss;
      fread(&ss, 2, 1, f);
      _mesh[i].vertex[j].pos[2] = ss;

      fread(&sb, 1, 1, f);
      _mesh[i].vertex[j].st[0] = sb;

      fread(&sb, 1, 1, f);
      _mesh[i].vertex[j].st[1] = sb;
    }

    // Start next surface/mesh /////////////////////
    if (_surfaces_start+_mesh[i].mesh_size != ftell(f))
    {
      printf("\nMd3::Load> WARNING: _mesh[%i] next mesh offset %i != file pos %lu\n", i, _surfaces_start+_mesh[i].mesh_size, ftell(f));

      fseek(f, _surfaces_start+_mesh[i].mesh_size, SEEK_SET);
      printf("Md3::Load> HANDLE: File position set to %lu\n", ftell(f));
    }

    // Now acculate this mesh offset
    _surfaces_start = ftell(f);
  }

  fclose(f);

  bIsValid = true;

  return 0;
}


int Md3::Save(const char *filename)
{
  FILE *f;
  int i, ii, tmp, hms;
  int32_t si;
  

  if ((!_tag && _num_tags) || (!_bone && _num_bone_frames) || !_mesh)
  {
    printf("Md3::Save> ERROR invalid md3\n");
    return -1;
  }

  f = fopen(filename, "wb");

  if (!f)
  {
    perror("Md3::Save> \n");
    return -1;
  }

  // Start Header ////////////////
  si = _id;
  fwrite(&si, 4, 1, f);
  printf("Md3::Save> _id = 0x%x\n", _id);
  
  _version = MD3_ALIAS_VERSION;

  fwrite(&_version, 4, 1, f);
  printf("Md3::Save> _version = %i\n", _version);

  fwrite(&_filename, sizeof(_filename), 1, f);
  printf("Md3::Save> _filename = '%s'\n", _filename);

  fwrite(&_num_bone_frames, 4, 1, f);
  printf("Md3::Save> _num_bone_frames = %i\n", _num_bone_frames);

  fwrite(&_num_tags, 4, 1, f);
  printf("Md3::Save> _num_tags = %i\n", _num_tags);

  fwrite(&_num_meshes, 4, 1, f);
  printf("Md3::Save> _num_meshes = %i\n", _num_meshes);

  fwrite(&_max_skins, 4, 1, f);
  printf("Md3::Save> _max_skins = %i\n", _max_skins);

  // Seek back and write in actual value later
  // store file postion for now
  _header_length = ftell(f);
  fwrite(&_header_length, 4, 1, f);

  // Seek back and write in actual value later
  // store file postion for now
  _tag_start = ftell(f);
  fwrite(&_tag_start, 4, 1, f);

  // Seek back and write in actual value later
  // store file postion for now
  _surfaces_start = ftell(f);
  fwrite(&_surfaces_start, 4, 1, f);

  // Seek back and write in actual value later
  // store file postiion for now
  _file_size = ftell(f);
  fwrite(&_file_size, 4, 1, f);
 
  // End Header //////////////////
  tmp = _header_length;
  _header_length = ftell(f);
  fseek(f, tmp, SEEK_SET);
  fwrite(&_header_length, 4, 1, f);
  printf("Md3::Save> _header_length = %i\n", _header_length);
  fseek(f, _header_length, SEEK_SET);

  for (i = 0; i < _num_bone_frames; i++)
  {
    fwrite(&_bone[i].mins, sizeof(_bone[i].mins), 1, f);
    fwrite(&_bone[i].maxs, sizeof(_bone[i].maxs), 1, f);
    fwrite(&_bone[i].center, sizeof(_bone[i].center), 1, f);
    fwrite(&_bone[i].scale, sizeof(float), 1, f);
    fwrite(&_bone[i].creator, 16, 1, f);

    printf("Md3::Save> _bone[%i].creator = '%s'\n", i, _bone[i].creator);
  }

  printf("Md3::Save> Saving %i tags\n", _num_tags * _num_bone_frames);

  // Start Tags ////////////////
  tmp = _tag_start;
  _tag_start = ftell(f);
  fseek(f, tmp, SEEK_SET);
  fwrite(&_tag_start, 4, 1, f);
  printf("Md3::Save> _tag_start = %i\n", _tag_start);
  fseek(f, _tag_start, SEEK_SET);

  for (i = 0; i < _num_tags * _num_bone_frames; i++)
  {
    fwrite(&_tag[i].name, 64, 1, f);
    fwrite(&_tag[i].center, sizeof(_tag[i].center), 1, f);
    fwrite(&_tag[i].rotation, sizeof(_tag[i].rotation), 1, f);

    printf("Md3::Save> _tag[%i].name = '%s'\n", i, _tag[i].name);
  }

  // Start Geometeric data ////////////////
  tmp = _surfaces_start;
  _surfaces_start = ftell(f);
  fseek(f, tmp, SEEK_SET);
  fwrite(&_surfaces_start, 4, 1, f);
  printf("Md3::Save> _surfaces_start = %i\n", _surfaces_start);
  fseek(f, _surfaces_start, SEEK_SET);

  for(i = 0; i < _num_meshes; i++)
  {
    // Start Mesh Header /////////////////
    hms = ftell(f);

    fwrite(&_mesh[i].id, 4, 1, f);
    fwrite(&_mesh[i].name, 68, 1, f);
    fwrite(&_mesh[i].num_frames, 4, 1, f);
    fwrite(&_mesh[i].num_skins, 4, 1, f);
    fwrite(&_mesh[i].num_vertices, 4, 1, f);
    fwrite(&_mesh[i].num_triangles, 4, 1, f);

    _mesh[i].tris_offset = ftell(f);
    fwrite(&_mesh[i].tris_offset, 4, 1, f);

    _mesh[i].header_size = ftell(f);
    fwrite(&_mesh[i].header_size, 4, 1, f);

    _mesh[i].texel_offset = ftell(f);
    fwrite(&_mesh[i].texel_offset, 4, 1, f);

    _mesh[i].vertex_offset = ftell(f);
    fwrite(&_mesh[i].vertex_offset, 4, 1, f);

    _mesh[i].mesh_size = ftell(f);
    fwrite(&_mesh[i].mesh_size, 4, 1, f);

    printf("Md3::Save> _mesh[%i].name = '%s'\n", i, _mesh[i].name);

    // End Mesh Header ///////////////////
    tmp = ftell(f);
    fseek(f, _mesh[i].header_size, SEEK_SET);
    _mesh[i].header_size = tmp - hms;
    fwrite(&_mesh[i].header_size, 4, 1, f);
    printf("Md3::Save> _mesh[%i].header_size = %i\n",i,_mesh[i].header_size);
    fseek(f, tmp, SEEK_SET);

    for(ii = 0; ii < _mesh[i].num_skins; ii++)
    {
      fwrite(&_mesh[i].skin[ii].name, 68, 1, f);

      if (!_mesh[i].skin[ii].name[0])
	_mesh[i].skin[ii].name[0] = 'm';

      #ifdef TEST_MD3
      printf("Viewing full skin buffer:\n'");

      for (int foo = 0; foo < 68; foo++)
	printf("%c", _mesh[i].skin[ii].name[foo]);

      printf("'\n");
      #endif

      printf("Md3::Save> _mesh[%i].skin[%i].name = '%s'\n", 
	     i, ii, _mesh[i].skin[ii].name);
    }

    // Tris Start //////////////
    tmp = ftell(f);
    fseek(f, _mesh[i].tris_offset, SEEK_SET);
    _mesh[i].tris_offset = tmp - hms;
    fwrite(&_mesh[i].tris_offset, 4, 1, f);
    printf("Md3::Save> _mesh[%i].tris_offset = %i\n",i,_mesh[i].tris_offset);
    fseek(f, tmp, SEEK_SET);   

    for(ii = 0; ii < _mesh[i].num_triangles; ii++)
    {
      fwrite(&_mesh[i].tris[ii].triangle, 12, 1, f);
    }

    // Texel Start //////////////
    tmp = ftell(f);
    fseek(f, _mesh[i].texel_offset, SEEK_SET);
    _mesh[i].texel_offset = tmp - hms;
    fwrite(&_mesh[i].texel_offset, 4, 1, f);
    printf("Md3::Save> _mesh[%i].texel_offset = %i\n",
	   i,_mesh[i].texel_offset);
    fseek(f, tmp, SEEK_SET);   

    for(ii = 0; ii < _mesh[i].num_vertices; ii++)
    {
      fwrite(&_mesh[i].texel[ii].st, sizeof(_mesh[i].texel[ii].st), 1, f);
    }

    // Vertex Start //////////////
    tmp = ftell(f);
    fseek(f, _mesh[i].vertex_offset, SEEK_SET);
    _mesh[i].vertex_offset = tmp - hms;
    fwrite(&_mesh[i].vertex_offset, 4, 1, f);
    printf("Md3::Save> _mesh[%i]._vertexoffset = %i\n",
	   i,_mesh[i].vertex_offset);
    fseek(f, tmp, SEEK_SET);  

    // Should be num_vertices * num_frames??
    for(ii = 0; ii < _mesh[i].num_vertices * _mesh[i].num_frames; ii++)
    {
      fwrite(&_mesh[i].vertex[ii].pos, sizeof(_mesh[i].vertex[ii].pos), 1, f);
      fwrite(&_mesh[i].vertex[ii].st, sizeof(_mesh[i].vertex[ii].st), 1, f);
    }
    // End Mesh  ///////////////////
    tmp = ftell(f);
    fseek(f, _mesh[i].mesh_size, SEEK_SET);
    _mesh[i].mesh_size = tmp - hms;
    fwrite(&_mesh[i].mesh_size, 4, 1, f);
    printf("Md3::Save> _mesh[%i].mesh_size = %i\n",i,_mesh[i].mesh_size);
    fseek(f, tmp, SEEK_SET);

  }

  // End File /////////////
  tmp = _file_size;
  _file_size = ftell(f);
  fseek(f, tmp, SEEK_SET);
  fwrite(&_file_size, 4, 1, f);
  printf("Md3::Save> _file_size = %i\n", _file_size);
  fseek(f, _file_size, SEEK_SET);

  fclose(f);

  return 0;
}



#ifdef TEST_MD3
int main(int argc, char *argv[])
{
  Md3 md3;


  printf("[MD3 class test]\n");

  if (argc > 2)
  {
    if (strcmp(argv[1], "load") == 0)
    {
      if (!md3.Load(argv[2]))
	printf("main: Load reports success.\n");
    }
    else if (strcmp(argv[1], "save") == 0)
    {
      // FIXME: Add code here to generate a small md3

      if (!md3.Save(argv[2]))
	printf("main: Save reports success.\n");
    }
    else if (strcmp(argv[1], "test") == 0 && argc > 3)
    {
      if (!md3.Load(argv[2]))
	printf("main: Load reports success.\n");
      if (!md3.Save(argv[3]))
	printf("main: Save reports success.\n");
    }
    else
      printf("\n\n%s [save | load | test] filename.md3 [testout.md3]\n", 
	     argv[0]);
  }
  else
  {
    printf("\n\n%s [save | load | test] filename.md3 [testout.md3]\n", 
	   argv[0]);
  }
}
#endif
