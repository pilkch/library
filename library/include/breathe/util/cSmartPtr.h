#ifndef CSMARTPTR_H
#define CSMARTPTR_H

namespace breathe
{
	namespace util
	{
		/*template <class T>
		class cSmartPtr
		{
		public:
			class reference
			{
			public:
				reference(cSmartPtr<T>& _ref) :
					ref(_ref)
				{
					_ref.aquire();
				}

				~reference()
				{
					ref.release();
				}

				operator=(const cSmartPtr<T>& other)
				{
					ref = other.ref;
				}

			private:
				cSmartPtr<T>& ref;
			};

			void aquire()
			{
				refCount++;
			}

			void release()
			{
				refCount--;
				assert(refCount > 0);
			}

			explicit cSmartPtr(T* _ptr) : 
				refCount(0), 
				ptr(_ptr)
			{
				assert(_ptr);
			}

			~cSmartPtr()
			{
				assert(refCount == 0);
				assert(ptr);
				delete ptr;
			}

			T* operator->() const
			{
				assert(ptr);
				return ptr;
			}
			T& operator*() const
			{
				assert(ptr);
				return *ptr;
			}

		private:
			T* ptr;
			int refCount;


			// Prevent the use of these functions outside this class
			cSmartPtr(cSmartPtr& refCount);
			cSmartPtr& operator=(const cSmartPtr& other);
			T** operator&();
		};*/

		template <class T>
		class cSmartPtr
		{
		public:
			typedef T element_type;

			// allocate a new counter
			explicit cSmartPtr(T* p = NULL) :
				refCount(0)
			{
				if (p) refCount = new counter(p);
			}

			~cSmartPtr()
			{
				release();
			}

			cSmartPtr(const cSmartPtr& r) throw()
			{
				acquire(r.refCount);
			}
			cSmartPtr& operator=(const cSmartPtr& r)
			{
				if (this != &r) {
					release();
					acquire(r.refCount);
				}
				return *this;
			}

			T& operator*()  const throw()	{return *refCount->ptr;}
			T* operator->() const throw()	{return refCount->ptr;}
			T* get()        const throw()	{return refCount ? refCount->ptr : 0;}
			bool unique()   const throw()	{return (refCount ? refCount->count == 1 : true);}

		private:
			struct counter {
				counter(T* p = 0, unsigned c = 1) : ptr(p), count(c) {}
				T*          ptr;
				unsigned    count;
			}* refCount;

			void acquire(counter* c) throw()
			{
				// increment the count
				refCount = c;
				if (c) ++c->count;
			}

			void release()
			{
				// decrement the count, delete if it is 0
				if ((refCount) && (--refCount->count == 0))
				{
					delete refCount->ptr;
					delete refCount;
				}

				refCount = 0;
			}
		};
	}
}

#endif //CSMARTPTR_H
