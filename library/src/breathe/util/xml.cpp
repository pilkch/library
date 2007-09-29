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

#include <breathe/util/log.h>
#include <breathe/util/xml.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cColour.h>

namespace breathe
{
	namespace xml
	{
		//cNode

		cNode::cNode(std::string inFilename)
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
					sData+=string::StripLeading(line, "\t");
				}
				f.close();

				ParseFromString(sData, NULL);
			}
			else
        LOG.Error("XML", inFilename + " not found");
		}

		cNode::cNode(cNode *inParent)
		{
			bContentOnly=false;
			pNext=NULL;
			pParent=inParent;
		}

		cNode::~cNode()
		{
			int n=vChild.size();
			for(int i=0;i<n;i++)
         SAFE_DELETE(vChild[i]);
		}
	
		std::string cNode::ParseFromString(std::string sData, cNode* pPrevious)
		{
			while(sData.length())
			{
				int length=sData.length();
				int lsize=sData.size();
				bool isEmpty=sData.empty();

				sData=string::StripLeading(string::StripLeading(sData, " "), "\t");

				while(sData.find("<!--") == 0)
				{
					int nEndComment=sData.find("-->");

					if(nEndComment == std::string::npos)
					{
						LOG.Error("XML", "Unterminated comment");
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
								LOG.Error("XML", "Opening tag \"" + sName + "\" doesn't match closing tag \"" + sClose + "\"");
								return "";
							}

              sData=string::StripLeading(sData.substr(angleBracket+1), " ");
						}
						else
						{
							LOG.Error("XML", "Tag \"" + sName + "\" doesn't have a closing tag");
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
								breathe::unicode_char c=*iter;
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


		void cNode::SaveToFile(std::string inFilename)
		{
			std::ofstream f(inFilename.c_str());

			if(f.is_open())
			{
				int n=vChild.size();
				for(int i=0;i<n;i++)
					vChild[i]->WriteToFile(f, "");
				f.close();
			}
			else
        LOG.Error("XML", inFilename + " not found");
		}

		void cNode::WriteToFile(std::ofstream& f, std::string sTab)
		{
			assert(f.is_open());
			
			int i, n;
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
		void cNode::PrintToLog(std::string sTab)
		{
			int i, n;
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
					LOG.Success("XML", sTag.c_str());
				}
			}
			else
				LOG.Success("XML", sTab + "Content=\"" + sContentOnly + "\"");

			n=vChild.size();
			for(i=0;i<n;i++)
				vChild[i]->PrintToLog(sTab + "&nbsp;");

			if(vChild.size()>0 && sName != "")
				LOG.Success("XML", (sTab + "&lt;/" + sName + "&gt;").c_str());
		}
#endif //BUILD_DEBUG

		cNode* cNode::FirstChild()
		{
			if(vChild.size()) return vChild[0];

			return NULL;
		}

		cNode* cNode::FindChild(std::string sName)
		{
			if(vChild.size())
			{
				cNode* p=vChild[0];
        while(p)
				{
					if(sName == p->sName) return p;
					p=p->pNext;
				}
			}
			
			return NULL;
		}

		cNode* cNode::GetNext()
		{
			return pNext;
		}

		cNode* cNode::GetNext(std::string sName)
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
	
		void cNode::AddContent(std::string inContent)
		{
			cNode* p=AddNode();

			if(p) {
				p->sContentOnly = string::StripTrailing(inContent, "\n\r\t");
				p->bContentOnly = true;
			}
		}

		std::string cNode::GetName()
		{
			return sName;
		}

		void cNode::AddAttribute(std::string inAttribute, std::string inValue)
		{
			mAttribute[inAttribute]=inValue;
		}
	
		bool cNode::GetAttribute(std::string sAttribute, std::string* pValue)
		{
			assert(pValue);
			attribute_iterator iter = mAttribute.find(sAttribute);
			attribute_iterator iterEnd = mAttribute.end();
			if(iter != iterEnd)
			{
				*pValue = iter->second;
				return true;
			}

			return false;
		}
		
		bool cNode::GetAttribute(std::string sAttribute, bool* pValue)
		{
			assert(pValue);
			attribute_iterator iter = mAttribute.find(sAttribute);
			if(iter != mAttribute.end())
			{
				std::string v = iter->second;
				*pValue = ("false" != v);
				return true;
			}

			return false;
		}

		bool cNode::GetAttribute(std::string sAttribute, math::cVec3* pValue)
		{
			assert(pValue);
			attribute_iterator iter = mAttribute.find(sAttribute);
			if(iter != mAttribute.end())
			{
				breathe::unicode_char c;
				std::stringstream stm(iter->second);
				stm >> std::skipws;

				stm >> pValue->x;
				stm >> c;

				stm >> pValue->y;
				stm >> c;

				stm >> pValue->z;
				return true;
			}

			return false;
		}
		
		bool cNode::GetAttribute(std::string sAttribute, math::cColour* pValue)
		{
			assert(pValue);
			attribute_iterator iter = mAttribute.find(sAttribute);
			if(iter != mAttribute.end())
			{
				std::stringstream stm(iter->second);
				stm >> pValue->r;
				stm >> pValue->g;
				stm >> pValue->b;
				stm >> pValue->a;
				return true;
			}

			return false;
		}
	}
}
