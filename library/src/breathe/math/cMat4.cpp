#include <cmath>

#include <vector>

// Breathe
#include <breathe/breathe.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cOctree.h>
#include <breathe/math/cColour.h>

namespace breathe
{
	namespace math
	{
		cMat4::cMat4(float e0, float e1, float e2, float e3,
							float e4, float e5, float e6, float e7,
							float e8, float e9, float e10, float e11,
							float e12, float e13, float e14, float e15)
		{
			entries[0]=e0;
			entries[1]=e1;
			entries[2]=e2;
			entries[3]=e3;
			entries[4]=e4;
			entries[5]=e5;
			entries[6]=e6;
			entries[7]=e7;
			entries[8]=e8;
			entries[9]=e9;
			entries[10]=e10;
			entries[11]=e11;
			entries[12]=e12;
			entries[13]=e13;
			entries[14]=e14;
			entries[15]=e15;
		}

		cMat4::cMat4(const cMat4 & rhs)
		{
			for (int i=0; i<16; i++)
				entries[i]=rhs.entries[i];
		}

		float & cMat4::operator[](const int i) const
		{
			return (float)entries[i];
		}

		void cMat4::SetEntry(int position, float value)
		{
			if (position>=0 && position<=15)
				entries[position]=value;
			//else
				//"Illegal argument to cMat4::SetEntry()"
		}
			
		float cMat4::GetEntry(int position) const
		{
			if (position>=0 && position<=15)
				return entries[position];
			else
			{
				//"Illegal argument to cMat4::GetEntry()"
				return 0;
			}
		}

		cVec4 cMat4::GetRow(int position) const
		{
			if (position==0)
				return cVec4(entries[0], entries[4], entries[8], entries[12]);
			
			if (position==1)
				return cVec4(entries[1], entries[5], entries[9], entries[13]);
			
			if (position==2)
				return cVec4(entries[2], entries[6], entries[10], entries[14]);
			
			if (position==3)
				return cVec4(entries[3], entries[7], entries[11], entries[15]);

			//"Illegal argument to cMat4::GetRow()"
			return cVec4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		cVec4 cMat4::GetColumn(int position) const
		{
			if (position==0)
				return cVec4(entries[0], entries[1], entries[2], entries[3]);
			
			if (position==1)
				return cVec4(entries[4], entries[5], entries[6], entries[7]);
			
			if (position==2)
				return cVec4(entries[8], entries[9], entries[10], entries[11]);
			
			if (position==3)
				return cVec4(entries[12], entries[13], entries[14], entries[15]);

			//"Illegal argument to cMat4::GetColumn()"
			return cVec4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		void cMat4::LoadIdentity(void)
		{
			entries[0]=1.0f;
			entries[1]=0;
			entries[2]=0;
			entries[3]=0;
			entries[4]=0;
			entries[5]=1.0f;
			entries[6]=0;
			entries[7]=0;
			entries[8]=0;
			entries[9]=0;
			entries[10]=1.0f;
			entries[11]=0;
			entries[12]=0;
			entries[13]=0;
			entries[14]=0;
			entries[15]=1.0f;
		}

		void cMat4::LoadZero(void)
		{
			for (int entry=0; entry<16; entry++)
				entries[entry]=0;
		}

		cMat4 cMat4::operator+(const cMat4 & rhs) const		//overloaded operators
		{
			cMat4 result;
			for (int entry=0; entry<16; entry++)
			{
				result.SetEntry(entry, entries[entry]+rhs.GetEntry(entry));	//add entries
			}
			return result;
		}

		cMat4 cMat4::operator-(const cMat4 & rhs) const		//overloaded operators
		{
			cMat4 result;
			for (int entry=0; entry<16; entry++)
			{
				result.SetEntry(entry, entries[entry]-rhs.GetEntry(entry));	//subtract entries
			}
			return result;
		}

		cMat4 cMat4::operator*(const cMat4 & rhs) const
		{
			cMat4 result;
			result.SetEntry(0, (entries[0]*rhs.GetEntry(0))+(entries[4]*rhs.GetEntry(1))+(entries[8]*rhs.GetEntry(2))+(entries[12]*rhs.GetEntry(3)));
			result.SetEntry(4, (entries[0]*rhs.GetEntry(4))+(entries[4]*rhs.GetEntry(5))+(entries[8]*rhs.GetEntry(6))+(entries[12]*rhs.GetEntry(7)));
			result.SetEntry(8, (entries[0]*rhs.GetEntry(8))+(entries[4]*rhs.GetEntry(9))+(entries[8]*rhs.GetEntry(10))+(entries[12]*rhs.GetEntry(11)));
			result.SetEntry(12, (entries[0]*rhs.GetEntry(12))+(entries[4]*rhs.GetEntry(13))+(entries[8]*rhs.GetEntry(14))+(entries[12]*rhs.GetEntry(15)));
			
			result.SetEntry(1, (entries[1]*rhs.GetEntry(0))+(entries[5]*rhs.GetEntry(1))+(entries[9]*rhs.GetEntry(2))+(entries[13]*rhs.GetEntry(3)));
			result.SetEntry(5, (entries[1]*rhs.GetEntry(4))+(entries[5]*rhs.GetEntry(5))+(entries[9]*rhs.GetEntry(6))+(entries[13]*rhs.GetEntry(7)));
			result.SetEntry(9, (entries[1]*rhs.GetEntry(8))+(entries[5]*rhs.GetEntry(9))+(entries[9]*rhs.GetEntry(10))+(entries[13]*rhs.GetEntry(11)));
			result.SetEntry(13, (entries[1]*rhs.GetEntry(12))+(entries[5]*rhs.GetEntry(13))+(entries[9]*rhs.GetEntry(14))+(entries[13]*rhs.GetEntry(15)));
			
			result.SetEntry(2, (entries[2]*rhs.GetEntry(0))+(entries[6]*rhs.GetEntry(1))+(entries[10]*rhs.GetEntry(2))+(entries[14]*rhs.GetEntry(3)));
			result.SetEntry(6, (entries[2]*rhs.GetEntry(4))+(entries[6]*rhs.GetEntry(5))+(entries[10]*rhs.GetEntry(6))+(entries[14]*rhs.GetEntry(7)));
			result.SetEntry(10, (entries[2]*rhs.GetEntry(8))+(entries[6]*rhs.GetEntry(9))+(entries[10]*rhs.GetEntry(10))+(entries[14]*rhs.GetEntry(11)));
			result.SetEntry(14, (entries[2]*rhs.GetEntry(12))+(entries[6]*rhs.GetEntry(13))+(entries[10]*rhs.GetEntry(14))+(entries[14]*rhs.GetEntry(15)));
			
			result.SetEntry(3, (entries[3]*rhs.GetEntry(0))+(entries[7]*rhs.GetEntry(1))+(entries[11]*rhs.GetEntry(2))+(entries[15]*rhs.GetEntry(3)));
			result.SetEntry(7, (entries[3]*rhs.GetEntry(4))+(entries[7]*rhs.GetEntry(5))+(entries[11]*rhs.GetEntry(6))+(entries[15]*rhs.GetEntry(7)));
			result.SetEntry(11, (entries[3]*rhs.GetEntry(8))+(entries[7]*rhs.GetEntry(9))+(entries[11]*rhs.GetEntry(10))+(entries[15]*rhs.GetEntry(11)));
			result.SetEntry(15, (entries[3]*rhs.GetEntry(12))+(entries[7]*rhs.GetEntry(13))+(entries[11]*rhs.GetEntry(14))+(entries[15]*rhs.GetEntry(15)));

			return result;
		}

		cMat4 cMat4::operator*(const float rhs) const
		{
			cMat4 result;
			for (int entry=0; entry<16; entry++)
			{
				result.SetEntry(entry, entries[entry]*rhs);		//multiply entries by rhs
			}
			return result;
		}

		cMat4 cMat4::operator/(const float rhs) const
		{
			cMat4 result;
			
			if (rhs==0.0f)										//cannot divide by zero
			{
				result.LoadZero();
				return result;
			}
				
			float temp=1/rhs;

			for (int entry=0; entry<16; entry++)
			{
				result.SetEntry(entry, entries[entry]*temp);		//divide entries by rhs
			}
			return result;
		}

		bool cMat4::operator==(const cMat4 & rhs) const
		{
			for (int i=0; i<16; i++)
			{
				if (entries[i]!=rhs.entries[i])
					return false;
			}
			return true;
		}

		bool cMat4::operator!=(const cMat4 & rhs) const
		{
			return !((*this)==rhs);
		}

		void cMat4::operator+=(const cMat4 & rhs)
		{
			(*this)=(*this)+rhs;
		}

		void cMat4::operator-=(const cMat4 & rhs)
		{
			(*this)=(*this)-rhs;
		}

		void cMat4::operator*=(const cMat4 & rhs)
		{
			(*this)=(*this)*rhs;
		}

		void cMat4::operator*=(const float rhs)
		{
			(*this)=(*this)*rhs;
		}

		void cMat4::operator/=(const float rhs)
		{
			(*this)=(*this)/rhs;
		}

		cMat4 cMat4::operator-(void) const
		{
			cMat4 result(*this);

			for (int i=0; i<16; i++)
				result.entries[i]=-result.entries[i];

			return result;
		}

		cVec4 cMat4::operator*(const cVec4 rhs) const
		{
			cVec4 result;

			result.SetX(		entries[0]*rhs.GetX()
										+	entries[4]*rhs.GetY()
										+	entries[8]*rhs.GetZ()
										+	entries[12]*rhs.GetW());

			result.SetY(		entries[1]*rhs.GetX()
										+	entries[5]*rhs.GetY()
										+	entries[9]*rhs.GetZ()
										+	entries[13]*rhs.GetW());

			result.SetZ(		entries[2]*rhs.GetX()
										+	entries[6]*rhs.GetY()
										+	entries[10]*rhs.GetZ()
										+	entries[14]*rhs.GetW());

			result.SetW(		entries[3]*rhs.GetX()
										+	entries[7]*rhs.GetY()
										+	entries[11]*rhs.GetZ()
										+	entries[15]*rhs.GetW());

			return result;
		}

		cVec3 cMat4::GetRotatedVec3(const cVec3 & rhs) const
		{
			cVec3 result;

			result.x=(entries[0]*rhs.x + entries[4]*rhs.y + entries[8]*rhs.z);
			result.y=(entries[1]*rhs.x + entries[5]*rhs.y + entries[9]*rhs.z);
			result.z=(entries[2]*rhs.x + entries[6]*rhs.y + entries[10]*rhs.z);

			return result;
		}

		cVec3 cMat4::GetInverseRotatedVec3(const cVec3 & rhs) const
		{
			cVec3 result;

			//rotate by transpose:
			result.x=(entries[0]*rhs.x + entries[1]*rhs.y + entries[2]*rhs.z);
			result.y=(entries[4]*rhs.x + entries[5]*rhs.y + entries[6]*rhs.z);
			result.z=(entries[8]*rhs.x + entries[9]*rhs.y + entries[10]*rhs.z);

			return result;
		}

		cVec3 cMat4::GetRight()
		{
			return cVec3(entries[0], entries[1], entries[2]);
		}
		
		cVec3 cMat4::GetFront()
		{
			return cVec3(entries[4], entries[5], entries[6]);
		}

		cVec3 cMat4::GetUp()
		{
			return cVec3(entries[8], entries[9], entries[10]);
		}

		// Returned in absolute units
		cVec3 cMat4::GetPosition()
		{
			return cVec3(entries[12], entries[13], entries[14]);
		}

		// Returned in degrees
		cVec3 cMat4::GetRotation()
		{
			return cVec3(	(entries[0] + entries[4] + entries[8]),
										(entries[1] + entries[5] + entries[9]),
										(	entries[0] + entries[1] + 
											entries[4] + entries[5]));//entries[2] + entries[6] + entries[10]));
		}

		cVec3 cMat4::GetTranslatedVec3(const cVec3 & rhs) const
		{
			return cVec3(rhs.x+entries[12], rhs.y+entries[13], rhs.z+entries[14]);
		}

		cVec3 cMat4::GetInverseTranslatedVec3(const cVec3 & rhs) const
		{
			return cVec3(rhs.x-entries[12], rhs.y-entries[13], rhs.z-entries[14]);
		}

		void cMat4::Invert(void)
		{
			*this=GetInverse();
		}

		cMat4 cMat4::GetInverse(void)
		{
			cMat4 result;

			float tmp[12];												//temporary pair storage
			float det;													//determinant

			//calculate pairs for first 8 elements (cofactors)
			tmp[0] = entries[10] * entries[15];
			tmp[1] = entries[11] * entries[14];
			tmp[2] = entries[9] * entries[15];
			tmp[3] = entries[11] * entries[13];
			tmp[4] = entries[9] * entries[14];
			tmp[5] = entries[10] * entries[13];
			tmp[6] = entries[8] * entries[15];
			tmp[7] = entries[11] * entries[12];
			tmp[8] = entries[8] * entries[14];
			tmp[9] = entries[10] * entries[12];
			tmp[10] = entries[8] * entries[13];
			tmp[11] = entries[9] * entries[12];

			//calculate first 8 elements (cofactors)
			result.SetEntry(0,		tmp[0]*entries[5] + tmp[3]*entries[6] + tmp[4]*entries[7]
							-	tmp[1]*entries[5] - tmp[2]*entries[6] - tmp[5]*entries[7]);

			result.SetEntry(1,		tmp[1]*entries[4] + tmp[6]*entries[6] + tmp[9]*entries[7]
							-	tmp[0]*entries[4] - tmp[7]*entries[6] - tmp[8]*entries[7]);

			result.SetEntry(2,		tmp[2]*entries[4] + tmp[7]*entries[5] + tmp[10]*entries[7]
							-	tmp[3]*entries[4] - tmp[6]*entries[5] - tmp[11]*entries[7]);

			result.SetEntry(3,		tmp[5]*entries[4] + tmp[8]*entries[5] + tmp[11]*entries[6]
							-	tmp[4]*entries[4] - tmp[9]*entries[5] - tmp[10]*entries[6]);

			result.SetEntry(4,		tmp[1]*entries[1] + tmp[2]*entries[2] + tmp[5]*entries[3]
							-	tmp[0]*entries[1] - tmp[3]*entries[2] - tmp[4]*entries[3]);

			result.SetEntry(5,		tmp[0]*entries[0] + tmp[7]*entries[2] + tmp[8]*entries[3]
							-	tmp[1]*entries[0] - tmp[6]*entries[2] - tmp[9]*entries[3]);

			result.SetEntry(6,		tmp[3]*entries[0] + tmp[6]*entries[1] + tmp[11]*entries[3]
							-	tmp[2]*entries[0] - tmp[7]*entries[1] - tmp[10]*entries[3]);

			result.SetEntry(7,		tmp[4]*entries[0] + tmp[9]*entries[1] + tmp[10]*entries[2]
							-	tmp[5]*entries[0] - tmp[8]*entries[1] - tmp[11]*entries[2]);

			//calculate pairs for second 8 elements (cofactors)
			tmp[0] = entries[2]*entries[7];
			tmp[1] = entries[3]*entries[6];
			tmp[2] = entries[1]*entries[7];
			tmp[3] = entries[3]*entries[5];
			tmp[4] = entries[1]*entries[6];
			tmp[5] = entries[2]*entries[5];
			tmp[6] = entries[0]*entries[7];
			tmp[7] = entries[3]*entries[4];
			tmp[8] = entries[0]*entries[6];
			tmp[9] = entries[2]*entries[4];
			tmp[10] = entries[0]*entries[5];
			tmp[11] = entries[1]*entries[4];

			//calculate second 8 elements (cofactors)
			result.SetEntry(8,		tmp[0]*entries[13] + tmp[3]*entries[14] + tmp[4]*entries[15]
							-	tmp[1]*entries[13] - tmp[2]*entries[14] - tmp[5]*entries[15]);

			result.SetEntry(9,		tmp[1]*entries[12] + tmp[6]*entries[14] + tmp[9]*entries[15]
							-	tmp[0]*entries[12] - tmp[7]*entries[14] - tmp[8]*entries[15]);

			result.SetEntry(10,		tmp[2]*entries[12] + tmp[7]*entries[13] + tmp[10]*entries[15]
							-	tmp[3]*entries[12] - tmp[6]*entries[13] - tmp[11]*entries[15]);

			result.SetEntry(11,		tmp[5]*entries[12] + tmp[8]*entries[13] + tmp[11]*entries[14]
							-	tmp[4]*entries[12] - tmp[9]*entries[13] - tmp[10]*entries[14]);

			result.SetEntry(12,		tmp[2]*entries[10] + tmp[5]*entries[11] + tmp[1]*entries[9]
							-	tmp[4]*entries[11] - tmp[0]*entries[9] - tmp[3]*entries[10]);

			result.SetEntry(13,		tmp[8]*entries[11] + tmp[0]*entries[8] + tmp[7]*entries[10]
							-	tmp[6]*entries[10] - tmp[9]*entries[11] - tmp[1]*entries[8]);

			result.SetEntry(14,		tmp[6]*entries[9] + tmp[11]*entries[11] + tmp[3]*entries[8]
							-	tmp[10]*entries[11] - tmp[2]*entries[8] - tmp[7]*entries[9]);

			result.SetEntry(15,		tmp[10]*entries[10] + tmp[4]*entries[8] + tmp[9]*entries[9]
							-	tmp[8]*entries[9] - tmp[11]*entries[10] - tmp[5]*entries[8]);

			// calculate determinant
			det	=	 entries[0]*result.GetEntry(0)
					+entries[1]*result.GetEntry(1)
					+entries[2]*result.GetEntry(2)
					+entries[3]*result.GetEntry(3);

			if (det==0.0f)
			{
				//"Non-Invertible Matrix sent to Invert()"
				cMat4 id;
				return id;
			}
			
			result=result/det;

			//transpose matrix
			result.Transpose();

			return result;
		}


		void cMat4::Transpose(void)
		{
			*this=GetTranspose();
		}

		cMat4 cMat4::GetTranspose(void)
		{
			//set up temp matrix
			cMat4 temp;
			
			temp.SetEntry( 0, entries[ 0]);
			temp.SetEntry( 1, entries[ 4]);
			temp.SetEntry( 2, entries[ 8]);
			temp.SetEntry( 3, entries[12]);
			temp.SetEntry( 4, entries[ 1]);
			temp.SetEntry( 5, entries[ 5]);
			temp.SetEntry( 6, entries[ 9]);
			temp.SetEntry( 7, entries[13]);
			temp.SetEntry( 8, entries[ 2]);
			temp.SetEntry( 9, entries[ 6]);
			temp.SetEntry(10, entries[10]);
			temp.SetEntry(11, entries[14]);
			temp.SetEntry(12, entries[ 3]);
			temp.SetEntry(13, entries[ 7]);
			temp.SetEntry(14, entries[11]);
			temp.SetEntry(15, entries[15]);

			return temp;	
		}

		void cMat4::InvertTranspose(void)
		{
			*this=GetInverseTranspose();
		}

		cMat4 cMat4::GetInverseTranspose(void)
		{
			cMat4 result;

			float tmp[12];												//temporary pair storage
			float det;													//determinant

			//calculate pairs for first 8 elements (cofactors)
			tmp[0] = entries[10] * entries[15];
			tmp[1] = entries[11] * entries[14];
			tmp[2] = entries[9] * entries[15];
			tmp[3] = entries[11] * entries[13];
			tmp[4] = entries[9] * entries[14];
			tmp[5] = entries[10] * entries[13];
			tmp[6] = entries[8] * entries[15];
			tmp[7] = entries[11] * entries[12];
			tmp[8] = entries[8] * entries[14];
			tmp[9] = entries[10] * entries[12];
			tmp[10] = entries[8] * entries[13];
			tmp[11] = entries[9] * entries[12];

			//calculate first 8 elements (cofactors)
			result.SetEntry(0,		tmp[0]*entries[5] + tmp[3]*entries[6] + tmp[4]*entries[7]
							-	tmp[1]*entries[5] - tmp[2]*entries[6] - tmp[5]*entries[7]);

			result.SetEntry(1,		tmp[1]*entries[4] + tmp[6]*entries[6] + tmp[9]*entries[7]
							-	tmp[0]*entries[4] - tmp[7]*entries[6] - tmp[8]*entries[7]);

			result.SetEntry(2,		tmp[2]*entries[4] + tmp[7]*entries[5] + tmp[10]*entries[7]
							-	tmp[3]*entries[4] - tmp[6]*entries[5] - tmp[11]*entries[7]);

			result.SetEntry(3,		tmp[5]*entries[4] + tmp[8]*entries[5] + tmp[11]*entries[6]
							-	tmp[4]*entries[4] - tmp[9]*entries[5] - tmp[10]*entries[6]);

			result.SetEntry(4,		tmp[1]*entries[1] + tmp[2]*entries[2] + tmp[5]*entries[3]
							-	tmp[0]*entries[1] - tmp[3]*entries[2] - tmp[4]*entries[3]);

			result.SetEntry(5,		tmp[0]*entries[0] + tmp[7]*entries[2] + tmp[8]*entries[3]
							-	tmp[1]*entries[0] - tmp[6]*entries[2] - tmp[9]*entries[3]);

			result.SetEntry(6,		tmp[3]*entries[0] + tmp[6]*entries[1] + tmp[11]*entries[3]
							-	tmp[2]*entries[0] - tmp[7]*entries[1] - tmp[10]*entries[3]);

			result.SetEntry(7,		tmp[4]*entries[0] + tmp[9]*entries[1] + tmp[10]*entries[2]
							-	tmp[5]*entries[0] - tmp[8]*entries[1] - tmp[11]*entries[2]);

			//calculate pairs for second 8 elements (cofactors)
			tmp[0] = entries[2]*entries[7];
			tmp[1] = entries[3]*entries[6];
			tmp[2] = entries[1]*entries[7];
			tmp[3] = entries[3]*entries[5];
			tmp[4] = entries[1]*entries[6];
			tmp[5] = entries[2]*entries[5];
			tmp[6] = entries[0]*entries[7];
			tmp[7] = entries[3]*entries[4];
			tmp[8] = entries[0]*entries[6];
			tmp[9] = entries[2]*entries[4];
			tmp[10] = entries[0]*entries[5];
			tmp[11] = entries[1]*entries[4];

			//calculate second 8 elements (cofactors)
			result.SetEntry(8,		tmp[0]*entries[13] + tmp[3]*entries[14] + tmp[4]*entries[15]
							-	tmp[1]*entries[13] - tmp[2]*entries[14] - tmp[5]*entries[15]);

			result.SetEntry(9,		tmp[1]*entries[12] + tmp[6]*entries[14] + tmp[9]*entries[15]
							-	tmp[0]*entries[12] - tmp[7]*entries[14] - tmp[8]*entries[15]);

			result.SetEntry(10,		tmp[2]*entries[12] + tmp[7]*entries[13] + tmp[10]*entries[15]
							-	tmp[3]*entries[12] - tmp[6]*entries[13] - tmp[11]*entries[15]);

			result.SetEntry(11,		tmp[5]*entries[12] + tmp[8]*entries[13] + tmp[11]*entries[14]
							-	tmp[4]*entries[12] - tmp[9]*entries[13] - tmp[10]*entries[14]);

			result.SetEntry(12,		tmp[2]*entries[10] + tmp[5]*entries[11] + tmp[1]*entries[9]
							-	tmp[4]*entries[11] - tmp[0]*entries[9] - tmp[3]*entries[10]);

			result.SetEntry(13,		tmp[8]*entries[11] + tmp[0]*entries[8] + tmp[7]*entries[10]
							-	tmp[6]*entries[10] - tmp[9]*entries[11] - tmp[1]*entries[8]);

			result.SetEntry(14,		tmp[6]*entries[9] + tmp[11]*entries[11] + tmp[3]*entries[8]
							-	tmp[10]*entries[11] - tmp[2]*entries[8] - tmp[7]*entries[9]);

			result.SetEntry(15,		tmp[10]*entries[10] + tmp[4]*entries[8] + tmp[9]*entries[9]
							-	tmp[8]*entries[9] - tmp[11]*entries[10] - tmp[5]*entries[8]);

			// calculate determinant
			det	=	 entries[0]*result.GetEntry(0)
					+entries[1]*result.GetEntry(1)
					+entries[2]*result.GetEntry(2)
					+entries[3]*result.GetEntry(3);

			if (det==0.0f)
			{
				//"Non-Invertible Matrix sent to GetInverseTranspose()"
				cMat4 id;
				return id;
			}
			
			result=result/det;

			return result;
		}

		void cMat4::SetTranslation(const cVec3 & translation)
		{
			LoadIdentity();

			SetTranslationPart(translation);
		}

		void cMat4::SetScale(const cVec3 & scaleFactor)
		{
			LoadIdentity();

			entries[0]=scaleFactor.x;
			entries[5]=scaleFactor.y;
			entries[10]=scaleFactor.z;
		}

		void cMat4::SetUniformScale(const float scaleFactor)
		{
			LoadIdentity();

			entries[0]=entries[5]=entries[10]=scaleFactor;
		}

		void cMat4::SetRotationAxis(const double angle, const cVec3 & axis)
		{
			cVec3 u=axis.GetNormalized();

			float sinAngle=sin((float)angle);
			float cosAngle=cos((float)angle);
			float oneMinusCosAngle=1.0f-cosAngle;

			LoadIdentity();

			entries[0]=(u.x)*(u.x) + cosAngle*(1-(u.x)*(u.x));
			entries[4]=(u.x)*(u.y)*(oneMinusCosAngle) - sinAngle*u.z;
			entries[8]=(u.x)*(u.z)*(oneMinusCosAngle) + sinAngle*u.y;

			entries[1]=(u.x)*(u.y)*(oneMinusCosAngle) + sinAngle*u.z;
			entries[5]=(u.y)*(u.y) + cosAngle*(1-(u.y)*(u.y));
			entries[9]=(u.y)*(u.z)*(oneMinusCosAngle) - sinAngle*u.x;
			
			entries[2]=(u.x)*(u.z)*(oneMinusCosAngle) - sinAngle*u.y;
			entries[6]=(u.y)*(u.z)*(oneMinusCosAngle) + sinAngle*u.x;
			entries[10]=(u.z)*(u.z) + cosAngle*(1-(u.z)*(u.z));
		}

		void cMat4::SetRotationX(const double angle)
		{
			LoadIdentity();

			entries[5]=(float)cos((float)angle);
			entries[6]=(float)sin((float)angle);

			entries[9]=-(float)sin((float)angle);
			entries[10]=(float)cos((float)angle);
		}

		void cMat4::SetRotationY(const double angle)
		{
			LoadIdentity();

			entries[0]=(float)cos((float)angle);
			entries[2]=-(float)sin((float)angle);

			entries[8]=(float)sin((float)angle);
			entries[10]=(float)cos((float)angle);
		}

		void cMat4::SetRotationZ(const double angle)
		{
			LoadIdentity();

			entries[0]=(float)cos((float)angle);
			entries[1]=(float)sin((float)angle);

			entries[4]=-(float)sin((float)angle);
			entries[5]=(float)cos((float)angle);
		}

		void cMat4::SetRotationEuler(const double angleX, const double angleY, const double angleZ)
		{
			LoadIdentity();

			SetRotationPartEuler(angleX, angleY, angleZ);
		}

		void cMat4::SetPerspective(	float left, float right, float bottom,
										float top, float n, float f)
		{
			LoadZero();

			//check for division by 0
			if (left==right || top==bottom || n==f)
				return;

			entries[0]=(2*n)/(right-left);

			entries[5]=(2*n)/(top-bottom);

			entries[8]=(right+left)/(right-left);
			entries[9]=(top+bottom)/(top-bottom);

			if (f!=-1)
			{
				entries[10]=-(f+n)/(f-n);
			}
			else		//if f==-1, use an infinite far plane
			{
				entries[10]=-1;
			}

			entries[11]=-1;

			if (f!=-1)
			{
				entries[14]=-(2*f*n)/(f-n);
			}
			else		//if f==-1, use an infinite far plane
			{
				entries[14]=-2*n;
			}
		}

		void cMat4::SetPerspective(float fovy, float aspect, float n, float f)
		{
			float left, right, top, bottom;

			//convert fov from degrees to radians
			fovy*=0.017453295f;

			top=n*(float)tan(double(fovy/2.0f));
			bottom=-top;

			left=aspect*bottom;
			right=aspect*top;

			SetPerspective(left, right, bottom, top, n, f);
		}

		void cMat4::SetOrtho(	float left, float right, float bottom,
									float top, float n, float f)
		{
			LoadIdentity();

			entries[0]=2.0f/(right-left);

			entries[5]=2.0f/(top-bottom);

			entries[10]=-2.0f/(f-n);

			entries[12]=-(right+left)/(right-left);
			entries[13]=-(top+bottom)/(top-bottom);
			entries[14]=-(f+n)/(f-n);
		}

		void cMat4::SetTranslationPart(const cVec3 & translation)
		{
			entries[12]=translation.x;
			entries[13]=translation.y;
			entries[14]=translation.z;
		}

		void cMat4::SetRotationPartEuler(const double angleX, const double angleY, const double angleZ)
		{
			double cr = cos((float)angleX );
			double sr = sin((float)angleX );
			double cp = cos((float)angleY );
			double sp = sin((float)angleY );
			double cy = cos((float)angleZ );
			double sy = sin((float)angleZ );

			entries[0] = ( float )( cp*cy );
			entries[1] = ( float )( cp*sy );
			entries[2] = ( float )( -sp );

			double srsp = sr*sp;
			double crsp = cr*sp;

			entries[4] = ( float )( srsp*cy-cr*sy );
			entries[5] = ( float )( srsp*sy+cr*cy );
			entries[6] = ( float )( sr*cp );

			entries[8] = ( float )( crsp*cy+sr*sy );
			entries[9] = ( float )( crsp*sy-sr*cy );
			entries[10] = ( float )( cr*cp );
		}

		void cMat4::LookAt(const cVec3 &eye,const cVec3 &dir,const cVec3 &up)
		{
			cVec3 x,y,z;
			cMat4 m0,m1;
			z = eye - dir;
			z.Normalize();
			x.Cross(up, z);
			x.Normalize();
			y.Cross(z,x);
			y.Normalize();
			m0.entries[0] = x.x; m0.entries[4] = x.y; m0.entries[8] = x.z; m0.entries[12] = 0.0;
			m0.entries[1] = y.x; m0.entries[5] = y.y; m0.entries[9] = y.z; m0.entries[13] = 0.0;
			m0.entries[2] = z.x; m0.entries[6] = z.y; m0.entries[10] = z.z; m0.entries[14] = 0.0;
			m0.entries[3] = 0.0; m0.entries[7] = 0.0; m0.entries[11] = 0.0; m0.entries[15] = 1.0;
			m1.SetTranslation(-eye);
			*this = m0 * m1;
		}
	}
}
