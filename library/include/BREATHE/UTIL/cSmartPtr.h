namespace BREATHE
{
	namespace UTIL
	{
		template <class T>
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
				ref++;
			}

			void release()
			{
				ref--;
			}

			explicit cSmartPtr(T* _ptr) : ref(0), ptr(_ptr)
			{
			
			}
			~cSmartPtr()
			{
				assert(ref == 0);
				delete ptr;
			}

			T* operator->() const
			{
				return ptr;
			}
			T& operator*() const
			{
				return *ptr;
			}

		private:
			T* ptr;
			int ref;


			// Prevent the use of these functions outside this class
			cSmartPtr(cSmartPtr& ref);
			cSmartPtr& operator=(const cSmartPtr& other);
			T** operator&();
		};
	}
}
