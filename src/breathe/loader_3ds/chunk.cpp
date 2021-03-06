#include <string>

#include <iostream>
#include <fstream>

#include <list>
#include <vector>
#include <sstream>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/loader_3ds/build3ds.h>
#include <breathe/loader_3ds/chunk.h>

namespace breathe
{
  namespace loader_3ds
  {
    int nRead=0;

    Model3DSChunk::Model3DSChunk(std::ifstream &infile , int csend) :
      file(infile),
      chunkset_end(csend)
    {
#ifdef DEBUG3DS
      std::ostringstream t;

      t<< "Model3DSChunk::Model3DSChunk()";
      LOG.Success("c3ds", t.str());
#endif // DEBUG3DS

      begin = file.tellg();

#ifdef DEBUG3DS
      unsigned int nPos = nRead;
#endif

      id=Short() & 0x0000ffff;

      end=Int()+begin;  // Compute absolute position

#ifdef DEBUG3DS
      t.str("");
      t  << "(" << nPos << ") id = (0x" << std::hex << id << ") " << std::dec
        << "begin = (" << begin << ") "
        << "end = (" << end << ") "
        << "chunkset_end = (" << chunkset_end << ")";

      if (0==id || -1==begin || end > chunkset_end)
        LOG.Error("c3ds", t.str());
      else
        LOG.Success("c3ds", t.str());
#endif // DEBUG3DS
    }

    Model3DSChunk::Model3DSChunk(const Model3DSChunk &chunk) :
      file(chunk.file),
      begin(chunk.begin),
      end(chunk.end),
      chunkset_end(chunk.chunkset_end),
      id(chunk.id)
    {

    }
    Model3DSChunk::~Model3DSChunk()
    {

    }

    Model3DSChunk Model3DSChunk::operator=(const Model3DSChunk &chunk)
    {
      int iPosition=chunk.file.tellg();

#ifdef DEBUG3DS
      std::ostringstream t;
      t  <<"SEEK " << iPosition;
      LOG.Success("c3ds", t.str());
#endif // DEBUG3DS

      file.seekg(iPosition, std::ios::beg);

      chunkset_end = chunk.chunkset_end;
      id = chunk.id;
      begin = chunk.begin;
      end = chunk.end;

      return *this;
    }

    bool Model3DSChunk::IsValid() const
    {
#ifdef DEBUG3DS
      int curr_pos = file.tellg();

      std::ostringstream t;
      t  <<"Model3DSChunk::operator bool() @ " << curr_pos << "== "
        << ((0 != id) && (begin < chunkset_end) && (begin >= 0))
        << " ((0x0!=0x" << std::hex << id << ") && ("
        << std::dec << begin << " < " << chunkset_end << ") && (" << begin << ">= 0))";
      LOG.Success("c3ds", t.str());
#endif // DEBUG3DS

      return (0 != id) && (begin < chunkset_end) && (begin >= 0);
    }

    unsigned int Model3DSChunk::ID() const
    {
      return id;
    }

    short Model3DSChunk::Short()
    {
      short s = 0;
      file.read((char*)&s , 2);

      nRead += 2;

      return s;
    }

    int Model3DSChunk::Int()
    {
      int i;
      file.read((char*)&i , 4);

      nRead += 4;

      return i;
    }

    float Model3DSChunk::Float()
    {
      float f;
      file.read((char*)&f , 4);

      nRead += 4;

      return f;
    }

    string_t Model3DSChunk::Str()
    {
      ostringstream_t o;
      char c;

      do {
        file.read(&c, 1);
        if (c != '\0') o<<c;

        nRead++;

      } while (c != '\0');

      return o.str();
    }

    Model3DSChunk Model3DSChunk::Child()
    {
#ifdef DEBUG3DS
      std::ostringstream t;
      t << "Child() end = (" << end << ")";
      LOG.Success("c3ds", t.str());
#endif //DEBUG3DS

      return Model3DSChunk(file , end);
    }
    Model3DSChunk Model3DSChunk::Sibling()
    {
#ifdef DEBUG3DS
      std::ostringstream t;
      t  <<"SEEK " << end;
      LOG.Success("c3ds", t.str());
#endif //DEBUG3DS

      Finish();

      return Model3DSChunk(file , chunkset_end);
    }

    void Model3DSChunk::Finish()
    {
      file.seekg(end, std::ios::beg);// travel to next chunk
      nRead = end;
    }

    void Model3DSChunk::Goto(int iPosition)
    {
      file.seekg(iPosition, std::ios::beg);// travel to iPosition
      nRead=iPosition;
    }

    int Model3DSChunk::Position()
    {
      return nRead;
    }
  }
}
