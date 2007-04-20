#include <string>

// writing on a text file
#include <iostream>
#include <fstream>

#include <sstream>

// Breathe
#include <BREATHE/cBreathe.h>

#include <BREATHE/UTIL/cLog.h>

#include "BREATHE/LOADER_3DS/build3ds.h"
#include "BREATHE/LOADER_3DS/chunk.h"

namespace BREATHE
{
	namespace LOADER_3DS
	{
		int nRead=0;

		Model3DSChunk::Model3DSChunk(std::ifstream &infile , int csend)
		: file(infile) , chunkset_end(csend)
		{
#ifdef DEBUG3DS
			std::ostringstream t;
			
			t<< "Model3DSChunk::Model3DSChunk()";
			pLog->Success("c3ds", t.str());
#endif //DEBUG3DS

			begin = file.tellg();

			unsigned int nPos=nRead;

			id=Short() & 0x0000ffff;

			end=Int()+begin;	// compute absolute position

#ifdef DEBUG3DS
			t.str("");
			t	<< "(" << nPos << ") id = (0x" << std::hex << id << ") " << std::dec
				<< "begin = (" << begin << ") "
				<< "end = (" << end << ") "
				<< "chunkset_end = (" << chunkset_end << ")";
			
			if(0==id || -1==begin || end > chunkset_end)
				pLog->Error("c3ds", t.str());
			else
				pLog->Success("c3ds", t.str());
#endif //DEBUG3DS
		}

		Model3DSChunk::Model3DSChunk(const Model3DSChunk &chunk)
		: file(chunk.file) ,
			chunkset_end(chunk.chunkset_end) ,
			id(chunk.id) , begin(chunk.begin) , end(chunk.end)
		{

		}
		Model3DSChunk::~Model3DSChunk()
		{

		}

		void Model3DSChunk::operator=(const Model3DSChunk &chunk)
		{
			int iPosition=chunk.file.tellg();

#ifdef DEBUG3DS
			std::ostringstream t;
			t	<<"SEEK " << iPosition;
			pLog->Success("c3ds", t.str());
#endif //DEBUG3DS

			file.seekg(iPosition, std::ios::beg);

			chunkset_end = chunk.chunkset_end;
			id = chunk.id;
			begin = chunk.begin;
			end = chunk.end;
		}

		Model3DSChunk::operator bool()
		{
			int curr_pos = file.tellg();

#ifdef DEBUG3DS
			std::ostringstream t;
			t	<<"Model3DSChunk::operator bool() @ " << curr_pos << "== "
				<< ((0!=id) && (begin < chunkset_end) && (begin >= 0))
				<< " ((0x0!=0x" << std::hex << id << ") && (" 
				<< std::dec << begin << " < " << chunkset_end << ") && (" << begin << ">= 0))";
			pLog->Success("c3ds", t.str());
#endif //DEBUG3DS

			return (0!=id) && (begin < chunkset_end) && (begin >= 0);
		}

		unsigned int Model3DSChunk::ID()
		{
			return id;
		}

		short Model3DSChunk::Short()
		{
			short s = 0;
			file.read((char*)&s , 2);

			nRead+=2;

			return s;
		}

		int Model3DSChunk::Int()
		{
			int i;
			file.read((char*)&i , 4);

			nRead+=4;

			return i;
		}

		float Model3DSChunk::Float()
		{
			float f;
			file.read((char*)&f , 4);

			nRead+=4;

			return f;
		}

		std::string Model3DSChunk::Str()
		{
			std::string s;
			char c;

			do
			{
				file.read(&c , 1);
				s += c;

				nRead++;

			}while(c != '\0');

			return s;
		}

		Model3DSChunk Model3DSChunk::Child()
		{
#ifdef DEBUG3DS
			std::ostringstream t;
			t << "Child() end = (" << end << ")";
			pLog->Success("c3ds", t.str());
#endif //DEBUG3DS

			return Model3DSChunk(file , end);
		}
		Model3DSChunk Model3DSChunk::Sibling()
		{
#ifdef DEBUG3DS
			std::ostringstream t;
			t	<<"SEEK " << end;
			pLog->Success("c3ds", t.str());
#endif //DEBUG3DS

			Finish();
			
			return Model3DSChunk(file , chunkset_end);
		}

		void Model3DSChunk::Finish()
		{
			file.seekg(end, std::ios::beg);// travel to next chunk
			nRead=end;
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
