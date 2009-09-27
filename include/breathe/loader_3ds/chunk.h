#ifndef CHUNK_H
#define CHUNK_H

namespace breathe
{
  namespace loader_3ds
  {
    class Model3DSChunk
    {
    public:
      Model3DSChunk(std::ifstream& infile, int csend);
      Model3DSChunk(const Model3DSChunk& chunk);
      ~Model3DSChunk();

      // BUG: Making 2 seperate file stream chunks = each other.  Is this a bug?
      Model3DSChunk operator=(const Model3DSChunk &chunk);

      bool IsValid() const;

      unsigned int ID() const;

      short Short();
      int Int();
      float Float();
      string_t Str();

      Model3DSChunk Child();
      Model3DSChunk Sibling();

      void Finish();
      void Goto(int iPosition);
      int Position();

    private:
      operator bool() const; // Forbidden

      std::ifstream &file;

      int begin;
      int end;
      int chunkset_end;
      int id;
    };
  }
}

#endif // CHUNK_H
