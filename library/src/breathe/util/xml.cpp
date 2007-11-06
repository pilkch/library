#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

// writing on a text file
#include <iostream>
#include <fstream>

#include <list>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>

#include <breathe/breathe.h>

#include <breathe/util/cString.h>

#ifndef FIRESTARTER
#include <breathe/util/log.h>
#endif

#include <breathe/util/xml.h>

#ifndef FIRESTARTER
#include <breathe/math/math.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cColour.h>
#endif

namespace breathe
{
	namespace xml
	{
		//cNode

		cNode::cNode(const std::string& inFilename)
		{
			bContentOnly=false;
			pParent=pNext=NULL;

			std::ifstream f(inFilename.c_str());

			if(f.is_open())
			{
				std::string sData;
				std::string line;
				while(!f.eof())
				{
					std::getline(f, line);
					
					// Get rid of leading tabs
					sData += string::StripLeading(line, "\t");
				}
				f.close();

				ParseFromString(sData, NULL);
			}
#ifndef FIRESTARTER
			else
        LOG.Error("XML", inFilename + " not found");
#endif
		}

		cNode::cNode(cNode *inParent)
		{
			bContentOnly=false;
			pNext=NULL;
			pParent=inParent;
		}

		cNode::~cNode()
		{
			size_t n=vChild.size();
			for(size_t i=0;i<n;i++)
         SAFE_DELETE(vChild[i]);
		}
	
		std::string cNode::ParseFromString(const std::string& data, cNode* pPrevious)
		{
			std::string sData(data);
			while(sData.length())
			{
				size_t length=sData.length();
				size_t lsize=sData.size();
				bool isEmpty=sData.empty();

				sData=string::StripLeading(string::StripLeading(sData, " "), "\t");

				while(sData.find("<!--") == 0)
				{
					size_t nEndComment=sData.find("-->");

					if(nEndComment == std::string::npos)
					{
#ifndef FIRESTARTER
						LOG.Error("XML", "Unterminated comment");
#endif
						return "";
					}

					sData=string::StripLeading(string::StripLeading(sData.substr(nEndComment), " "), "\t");
				}

				std::string::size_type angleBracket=sData.find("<");
				if(angleBracket != std::string::npos)
				{
					std::string sContent;
					
					if(angleBracket)
					{
						//Content</name>
						sContent = sData.substr(0, angleBracket);
					}

					// example attribute="value"...
					sData=string::StripLeading(sData.substr(angleBracket+1), " ");

					if(sData[0] == '/')
					{
						// </name>
						if(sContent.size() && sContent != "/>")
						{
							AddContent(sContent);
						}

						angleBracket=sData.find(">");

						if(angleBracket != std::string::npos)
						{
							std::string sClose=string::StripTrailing(string::StripLeading(sData.substr(1, angleBracket-1), " "), " ");

							if(sClose!=sName)
							{
#ifndef FIRESTARTER
								LOG.Error("XML", "Opening tag \"" + sName + "\" doesn't match closing tag \"" + sClose + "\"");
#endif
								return "";
							}

              sData=string::StripLeading(sData.substr(angleBracket+1), " ");
						}
						else
						{
#ifndef FIRESTARTER
							LOG.Error("XML", "Tag \"" + sName + "\" doesn't have a closing tag");
#endif
							return "";
						}

						return sData;
					}
					
					std::string sSlashSpaceRightBracket="/ >";

					std::string::size_type n=sData.find_first_of(sSlashSpaceRightBracket);

					if(std::string::npos != n)
					{
						// Collect name
						std::string inName=string::StripTrailing(sData.substr(0, n), " ");

						// attribute="value"...
						sData=string::StripLeading(sData.substr(n), " ");

						// Fill in attributes if any, and find the end of the opening tag
						if('/' == sData[0])
						{
							// />
							cNode* p=AddNode();

							p->sName=inName;

              n=sData.find(">");
							if(std::string::npos==n)
								return "";
              
							sData=sData.substr(n+1);
						}
						else
						{
							// attribute="value">
							cNode* p=AddNode();

							p->sName=inName;
							
							std::string::iterator iter=sData.begin();
							
							bool bInValue=false;
							std::string sAttributeName;
							std::string sAttributeValue;

							while(iter!=sData.end() && *iter!='/' && *iter!='>')
							{
								breathe::string::unicode_char c=*iter;
								if(*iter == ' ')
								{
									p->AddAttribute(sAttributeName, "");
									sAttributeName = "";
									sData=string::StripLeading(&*iter, " ");
									iter=sData.begin();
									continue;
								}
								else if(*iter == '=')
								{
									iter++;
									if(iter!=sData.end() && *iter == '\"')
									{
										sData.erase(sData.begin(), ++iter);
										sData=string::StripLeading(sData, " ");

										std::string::size_type nQuote=sData.find("\"");
										std::string::size_type nSlashQuote=sData.find("\\\"");

										while(std::string::npos!=nQuote)
										{
											if(nQuote<nSlashQuote)
											{
												// attribute="value"
												p->AddAttribute(sAttributeName, sAttributeValue + sData.substr(0, nQuote));
												sAttributeName="";
												sAttributeValue="";
												sData=string::StripLeading(sData.substr(nQuote+1), " ");
												break;
											}
											else
											{
												// attribute="value\"innervalue..."
												sAttributeValue+=sData.substr(0, nSlashQuote);
												sData=string::StripLeading(sData.substr(nSlashQuote+2), " ");
											}

											nQuote=sData.find("\"");
											nSlashQuote=sData.find("\\\"");
										};

										iter=sData.begin();
										continue;
									}
								}
								else
								{
									sAttributeName+=*iter;
								}

								iter++;
							};

							if('>'==*iter)
							{
								// >...
								iter++;
								if (sAttributeName != "")
								{
									// attribute>
									p->AddAttribute(sAttributeName, sAttributeValue);
									sAttributeName = "";
								}
								sData.erase(sData.begin(), iter);
								sData=(*vChild.rbegin())->ParseFromString(sData, this);
								iter = sData.begin();
							}
						}
					}
					else
					{
						return "";
					}
				}
				else
				{
					// No tags, just content for the parent tag (this)
					cNode* p=AddNode();

					p->bContentOnly = true;
					p->sContentOnly += string::StripTrailing(sData, "\n\r\t");
					return "";
				}
			}

			return sData;
		}


		void cNode::SaveToFile(const std::string& inFilename)
		{
			std::ofstream f(inFilename.c_str());

			if(f.is_open())
			{
				size_t n=vChild.size();
				for(size_t i=0;i<n;i++)
					vChild[i]->WriteToFile(f, "");
				f.close();
			}
#ifndef FIRESTARTER
			else
        LOG.Error("XML", inFilename + " not found");
#endif
		}

		void cNode::WriteToFile(std::ofstream& f, const std::string& sTab)
		{
			assert(f.is_open());
			
			size_t i, n;
			if(IsNameAndAttributesAndChildren())
			{
				if(sName != "")
				{
					std::string sTag = sTab + "<" + sName;
					attribute_iterator iter=mAttribute.begin();
					for(;iter!=mAttribute.end();iter++)
					{
						if (iter->second.length() > 0)
              sTag += " " + iter->first + "=\"" + iter->second + "\"";
						else
							sTag += " " + iter->first;
					}

					if(vChild.size() == 0)
					{
						f<<sTag<<"/>"<<std::endl;
						return;
					}
					
					f<<sTag<<">";
					
					if (!vChild[0]->IsContentOnly())
						f<<std::endl;
				}
			}
			else
				f<<sContentOnly;

			n = vChild.size();
			for(i=0;i<n;i++)
				vChild[i]->WriteToFile(f, sTab + "\t");

			if (sName != "")
			{
				if(vChild.size() == 0 || (vChild.size() > 0 && !vChild[0]->IsContentOnly()))
					f<<sTab<<"</"<<sName<<">"<<std::endl;
				else
					f<<"</"<<sName<<">"<<std::endl;
			}
		}

#ifdef BUILD_DEBUG
		void cNode::PrintToLog(const std::string& sTab)
		{
			size_t i, n;
			if(IsNameAndAttributesAndChildren())
			{
				if(sName != "")
				{
					std::string sTag = sTab + "&lt;" + sName;
					attribute_iterator iter=mAttribute.begin();
					for(;iter!=mAttribute.end();iter++)
						sTag += " " + iter->first + "=\"" + iter->second + "\"";

					if(vChild.size()>0)
						sTag+="&gt;";
					else			
						sTag+="/&gt;";

#ifndef FIRESTARTER
					LOG.Success("XML", sTag.c_str());
#endif
				}
			}
#ifndef FIRESTARTER
			else
				LOG.Success("XML", sTab + "Content=\"" + sContentOnly + "\"");
#endif

			n=vChild.size();
			for(i=0;i<n;i++)
				vChild[i]->PrintToLog(sTab + "&nbsp;");

#ifndef FIRESTARTER
			if(vChild.size()>0 && sName != "")
				LOG.Success("XML", (sTab + "&lt;/" + sName + "&gt;").c_str());
#endif
		}
#endif //BUILD_DEBUG

		cNode* cNode::FirstChild()
		{
			if(vChild.size()) return vChild[0];

			return NULL;
		}

		cNode* cNode::FindChild(const std::string& sName)
		{
			if(vChild.size())
			{
				cNode* p=vChild[0];
        while(p != nullptr)
				{
					if(sName == p->sName) return p;
					p = p->pNext;
				}
			}
			
			return NULL;
		}

		cNode* cNode::GetNext()
		{
			return pNext;
		}

		cNode* cNode::GetNext(const std::string& sName)
		{
			cNode* p=pNext;
      while(p)
			{
				if(sName == p->sName) return p;
				p=p->pNext;
			};
			
			return NULL;
		}

		cNode* cNode::AddNode()
		{
			cNode* p=new cNode(this);
			if(vChild.size())
				vChild.back()->pNext=p;
			vChild.push_back(p);
			return p;
		}
	
		void cNode::AddContent(const std::string& inContent)
		{
			cNode* p=AddNode();

			if(p) {
				p->sContentOnly = string::StripTrailing(inContent, "\n\r\t");
				p->bContentOnly = true;
			}
		}

		std::string cNode::GetName() const
		{
			return sName;
		}

		void cNode::AddAttribute(const std::string& inAttribute, const std::string& inValue)
		{
			mAttribute[inAttribute]=inValue;
		}
	
		bool cNode::GetAttribute(const std::string& sAttribute, std::string& value)
		{
			attribute_iterator iter = mAttribute.find(sAttribute);
			attribute_iterator iterEnd = mAttribute.end();
			if(iter != iterEnd)
			{
				value = iter->second;
				return true;
			}

			return false;
		}
		
		bool cNode::GetAttribute(const std::string& sAttribute, bool& value)
		{
			attribute_iterator iter = mAttribute.find(sAttribute);
			if(iter != mAttribute.end())
			{
				value = breathe::string::ToBool(iter->second);

				return true;
			}

			return false;
		}
		
		bool cNode::GetAttribute(const std::string& sAttribute, float* pValue, size_t nValues)
		{
			assert(pValue != nullptr);
			attribute_iterator iter = mAttribute.find(sAttribute);
			if(iter != mAttribute.end())
			{
				breathe::string::unicode_char c;
				std::stringstream stm(iter->second);
				stm >> std::skipws;

				std::string s(iter->second);
				char sz[100];
				strcpy(sz, s.c_str());

				size_t i = 0;
				while (i != nValues) {
					stm >> pValue[i++];
					stm >> c;
				};

				return true;
			}

			return false;
		}

#ifndef FIRESTARTER
		bool cNode::GetAttribute(const std::string& sAttribute, math::cVec3& value)
		{
			attribute_iterator iter = mAttribute.find(sAttribute);
			if(iter != mAttribute.end())
			{
				std::vector<std::string> vSplit;
				breathe::string::Split(iter->second, ',', vSplit);

				if (vSplit.size() > 0) value.x = breathe::string::ToFloat(vSplit[0]);
				if (vSplit.size() > 1) value.y = breathe::string::ToFloat(vSplit[1]);
				if (vSplit.size() > 2) value.z = breathe::string::ToFloat(vSplit[2]);
				
				return true;
			}

			return false;
		}
		
		bool cNode::GetAttribute(const std::string& sAttribute, math::cColour& value)
		{
			attribute_iterator iter = mAttribute.find(sAttribute);
			if(iter != mAttribute.end())
			{
				std::vector<std::string> vSplit;
				breathe::string::Split(iter->second, ',', vSplit);

				if (vSplit.size() > 0) value.r = breathe::string::ToFloat(vSplit[0]);
				if (vSplit.size() > 1) value.g = breathe::string::ToFloat(vSplit[1]);
				if (vSplit.size() > 2) value.b = breathe::string::ToFloat(vSplit[2]);
				if (vSplit.size() > 3) value.a = breathe::string::ToFloat(vSplit[3]);

				return true;
			}

			return false;
		}
#endif
	}
}
