#ifndef CHUNK_H
#define CHUNK_H

namespace breathe
{
  namespace loader_3ds
  {
    class Model3DSChunk
    {
    public:
      Model3DSChunk(std::ifstream &infile , int csend);
      Model3DSChunk(const Model3DSChunk &chunk);
      ~Model3DSChunk();

      /**bug : making 2 seperate file stream chunks = each other*/
      Model3DSChunk operator=(const Model3DSChunk &chunk);

      operator bool();

      unsigned int ID();

      short Short();
      int Int();
      float Float();
      std::string Str();

      Model3DSChunk Child();
      Model3DSChunk Sibling();

      void Finish();
      void Goto(int iPosition);
      int Position();

    private:
      std::ifstream &file;
      
      int begin, end, chunkset_end;
      int id;
    };
  }
}

#endif //CHUNK_H
