// Note: This file is not named file.cpp because of a stupid bug in Visual Studio 2003
// When we upgrade to 2005+ rename it to file.cpp

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <algorithm>

#include <breathe/breathe.h>
#include <breathe/util/cString.h>

#include <breathe/storage/filesystem.h>
#include <breathe/storage/file.h>

#ifndef FIRESTARTER
#include <breathe/util/log.h>
#include <breathe/algorithm/md5.h>
#endif


#ifdef __WIN__
#include <windows.h>
#endif

namespace breathe
{
	namespace storage
	{
		/*
			DETECTED:
			UTF-8 EF BB BF
			UTF-16 (big-endian) FE FF
			UTF-16 (little-endian) FF FE
			UTF-32 (big-endian) 00 00 FE FF
			UTF-32 (little-endian) FF FE 00 00
			SCSU (compression) 0E FE FF
			
			NOT DETECTED:
			UTF-16BE, UTF-32BE (big-endian) No BOM!
			UTF-16LE, UTF-32LE (little-endian) No BOM!
		*/

		BYTEORDER DetectByteOrderMark(const string_t& filename, size_t& bytes)
		{
			bytes = 0;
			
			std::ifstream file;
			file.open(breathe::string::ToUTF8(filename).c_str(), std::ios::in | std::ios::binary);

			// Default to UTF8 which will be changed if it is not UTF8
			uint8_t signature[4] = { 0x88, 0x88, 0x88, 0x88 };
			file.read((char*)signature, 4);
			size_t count = file.gcount();
			file.close();

			if (count >= 4) {
				bytes = 4;
				//UTF-32 (big-endian) 00 00 FE FF
				if ((signature[0] == 0x00) && (signature[1] == 0x00) && (signature[2] == 0xFE) && (signature[3] == 0xFF))  {
					SCREEN<<"Error 32bit big endian file signature detected"<<std::endl;
					assert(false);
					return BYTEORDER_UTF32BE;
				}

				//UTF-32 (little-endian) FF FE 00 00
				if ((signature[0] == 0xFF) && (signature[1] == 0xFE) && (signature[2] == 0x00) && (signature[3] == 0x00)) {
					SCREEN<<"Error 32bit little endian file signature detected"<<std::endl;
					assert(false); 
					return BYTEORDER_UTF32LE;
				}
			}
			
			if (count >= 3) {
				bytes = 3;
				//UTF-8 EF BB BF
				if ((signature[0] == 0xEF) && (signature[1] == 0xBB) && (signature[2] == 0xBF)) return BYTEORDER_UTF8;
				//SCSU (compression) 0E FE FF
				if ((signature[0] == 0x0E) && (signature[1] == 0xFE) && (signature[2] == 0xFF)) {
					SCREEN<<"Error unhandled file signature detected"<<std::endl;
					assert(false);
					return BYTEORDER_SCSU;
				}
			}

			if (count >= 2) {
				bytes = 2;
				//UTF-16 (big-endian) FE FF
				if ((signature[0] == 0xFE) && (signature[1] == 0xFF)) {
					SCREEN<<"Error big endian file signature detected"<<std::endl;
					assert(false);
					return BYTEORDER_UTF16BE;
				}
				//UTF-16 (little-endian) FF FE
				if ((signature[0] == 0xFF) && (signature[1] == 0xFE)) return BYTEORDER_UTF16LE;
			}

			bytes = 0;
			return BYTEORDER_UTF8;
		}

		void ReadText(const string_t& filename, std::vector<std::wstring>& contents)
		{
			// Get byte order and size
			
			size_t signature_bytes = 0;
			BYTEORDER byteOrder = DetectByteOrderMark(filename, signature_bytes);

			// Open as a test file for UTF8, binary for anything else
			std::wifstream file;
			if (BYTEORDER_UTF8 == byteOrder) file.open(breathe::string::ToUTF8(filename).c_str(), std::ios::in);
			else file.open(breathe::string::ToUTF8(filename).c_str(), std::ios::in | std::ios::binary);
			
			// Seek to after the byte order mark
			file.seekg(signature_bytes, std::ios::beg);

			if ((BYTEORDER_UTF8 == byteOrder) || (BYTEORDER_UTF32LE == byteOrder)) {
				std::wstring line;
				while (std::getline(file, line)) contents.push_back(line);
			} else if (BYTEORDER_UTF16LE == byteOrder) {
				std::wstring line;
				const size_t bufferSize = 1024;
				uint16_t buffer[bufferSize];
				size_t n = 0;
				size_t i = 0;
				std::vector<std::wstring> lines;
				while (file.good()) {
					file.read((wchar_t *) buffer, bufferSize * sizeof(uint16_t));
					n = file.gcount() / sizeof(uint16_t);

					line.clear();
					line.reserve(n);
					n += n;
					for (i = 0; i < n; i+=2) line += wchar_t(buffer[i]);

					lines.clear();
					breathe::string::SplitOnNewLines(line, lines);

					n = lines.size();
					for (i = 0; i < n; i++) contents.push_back(lines[i]);
				}
			} else {
				SCREEN<<"Not handled"<<std::endl;
				assert(false);
				
				// Swap
				//size_t i = 0;
				//for (;i<count; i++) buffer[i] = (buffer[i]<<8)|(buffer[i]>>8);
						
				//UTF-16 (big-endian) FE FF
				//UTF-16BE, UTF-32BE (big-endian) No BOM!
				//UTF-32 (big-endian) 00 00 FE FF
				//SCSU (compression) 0E FE FF
			}

			file.close();
		}

		
		void AppendText(const string_t& filename, const std::string& contents)
		{
			//size_t signature_bytes = 0;
			//BYTEORDER byteOrder = DetectByteOrderMark(filename, signature_bytes);

			std::ofstream file;
			file.open(breathe::string::ToUTF8(filename).c_str(), std::ios::out | std::ios::app);
	
			file<<contents;
			/*if (BYTEORDER_UTF8 == byteOrder)
			{
				
			}*/
			
			file.close();
		}

		void AppendText(const string_t& filename, const std::wstring& contents)
		{
			AppendText(filename, breathe::string::ToUTF8(contents));
		}
	}
}
