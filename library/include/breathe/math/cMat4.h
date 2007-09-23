#ifndef CMAT4_H
#define CMAT4_H

namespace breathe
{
	namespace MATH
	{
		class cVec4;

		class cMat4
		{
		public:
			cMat4(float e0=1.0f, float e1=0.0f, float e2=0.0f, float e3=0.0f,
						float e4=0.0f, float e5=1.0f, float e6=0.0f, float e7=0.0f,
						float e8=0.0f, float e9=0.0f, float e10=1.0f, float e11=0.0f,
						float e12=0.0f, float e13=0.0f, float e14=0.0f, float e15=1.0f);
			cMat4(const cMat4 & rhs);
			~cMat4() {}	//empty

			cVec3 GetRight();
			cVec3 GetFront();
			cVec3 GetUp();

			cVec3 GetPosition();
			cVec3 GetRotation();

			void SetEntry(int position, float value);
			float GetEntry(int position) const;
			cVec4 GetRow(int position) const;
			cVec4 GetColumn(int position) const;
			
			void LoadIdentity(void);
			void LoadZero(void);


			float & operator[](const int i) const;
			
			//binary operators
			cMat4 operator+(const cMat4 & rhs) const;
			cMat4 operator-(const cMat4 & rhs) const;
			cMat4 operator*(const cMat4 & rhs) const;
			cMat4 operator*(const float rhs) const;
			cMat4 operator/(const float rhs) const;

			bool operator==(const cMat4 & rhs) const;
			bool operator!=(const cMat4 & rhs) const;

			//self-add etc
			void operator+=(const cMat4 & rhs);
			void operator-=(const cMat4 & rhs);
			void operator*=(const cMat4 & rhs);
			void operator*=(const float rhs);
			void operator/=(const float rhs);

			//unary operators
			cMat4 operator-(void) const;
			cMat4 operator+(void) const {return (*this);}
			
			//multiply a vector by this matrix
			cVec4 operator*(const cVec4 rhs) const;

			//rotate a 3d vector by rotation part
			void RotateVec3(cVec3 & rhs) const
			{rhs=GetRotatedVec3(rhs);}

			void InverseRotateVec3(cVec3 & rhs) const
			{rhs=GetInverseRotatedVec3(rhs);}

			cVec3 GetRotatedVec3(const cVec3 & rhs) const;
			cVec3 GetInverseRotatedVec3(const cVec3 & rhs) const;

			//translate a 3d vector by translation part
			void TranslateVec3(cVec3 & rhs) const
			{rhs=GetTranslatedVec3(rhs);}

			void InverseTranslateVec3(cVec3 & rhs) const
			{rhs=GetInverseTranslatedVec3(rhs);}
			
			cVec3 GetTranslatedVec3(const cVec3 & rhs) const;
			cVec3 GetInverseTranslatedVec3(const cVec3 & rhs) const;

			//Other methods
			void Invert(void);
			cMat4 GetInverse(void);
			void Transpose(void);
			cMat4 GetTranspose(void);
			void InvertTranspose(void);
			cMat4 GetInverseTranspose(void);

			//set to perform an operation on space - removes other entries
			void SetTranslation(const cVec3 & translation);
			void SetScale(const cVec3 & scaleFactor);
			void SetUniformScale(const float scaleFactor);
			void SetRotationAxis(const double angle, const cVec3 & axis);
			void SetRotationX(const double angle);
			void SetRotationY(const double angle);
			void SetRotationZ(const double angle);
			void SetRotationEuler(const double angleX, const double angleY, const double angleZ);
			void SetPerspective(float left, float right, float bottom, float top, float n, float f);
			void SetPerspective(float fovy, float aspect, float n, float f);
			void SetOrtho(float left, float right, float bottom, float top, float n, float f);

			//set parts of the matrix
			void SetTranslationPart(const cVec3 & translation);
			void SetRotationPartEuler(const double angleX, const double angleY, const double angleZ);
			void SetRotationPartEuler(const cVec3 & rotations)
			{
				SetRotationPartEuler((float)rotations.x, (float)rotations.y, (float)rotations.z);
			}

			//cast to pointer to a (float *) for glGetFloatv etc
			operator float* () const {return (float*) this;}
			operator const float* () const {return (const float*) this;}

			void LookAt(const cVec3 &eye,const cVec3 &dir,const cVec3 &up);

		private:
			float entries[16];
		};
	}
}

#endif	//CMAT4_H
