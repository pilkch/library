#ifndef CVAR_H
#define CVAR_H

#include <sstream>

namespace breathe
{
	//	cVar
	//	
	//	name=The name of the cvar.  For example "sv_gravity".  
	//	value=The value of the cvar.  For example "800.0".  
	//	flags=The flags that specify how the cvar acts.  For example VAR_CLIENT | VAR_SERVER means that it will be saved to the client and server config files.  
	//	

	enum VAR
	{
		VAR_NULL=0,

		VAR_SAVE_CL=1,				// Written to client.cfg
		VAR_SAVE_SV=2,				// Written to server.cfg

		VAR_READ_ONLY_IN_GAME=4,		// Can't change once in game.  For example: "sv_maxplayers" etc.  
		VAR_READ_ONLY_IN_CONSOLE=8,	// Can't change once in console.  For example: "r_driver" etc.  
		VAR_READ_ONLY=16,				// Can't change at all ever, even programatically.  Used for functions.  For example: "help", "version" etc.  
	};

	template <class T>
	class cVar
	{
	private:
		//unsigned int uiFlags;
	
		T value;

	public:
		cVar(const cVar<T>& rhs);
		cVar(const T& value, unsigned int flags=VAR_NULL);
		
		template <class C>
		cVar(const C& rhs)
		{
			value = rhs;
		}


		//Convert these types into value
		cVar<T>& operator=(const cVar<T>& rhs);

		template <class C>
		cVar<T>& operator=(const C& rhs)
		{
			value = rhs;
			return *this;
		}

		//Convert value into these types
		bool GetBool() const;
		unsigned int GetUnsignedInt() const;
		int GetInt() const;
		float GetFloat() const;
		std::string GetString() const;

	private:
		cVar();
	};


	
	template <>
	class cVar <std::string>
	{
	private:
		//unsigned int uiFlags;
	
		std::string value;

	public:
		cVar(const cVar<std::string>& rhs);
		cVar(const std::string& value, unsigned int flags=VAR_NULL);
		
		template <class C>
		cVar(const C& rhs)
		{
			std::ostringstream t;
			t<<rhs;
			value = t.str();
		}


		//Convert these types into value
		cVar<std::string>& operator=(const cVar<std::string>& rhs);

		template <class C>
		cVar<std::string>& operator=(const C& rhs)
		{
			std::ostringstream t;
			t<<rhs;
			value = t.str();
			return *this;
		}

		//Convert value into these types
		bool GetBool() const;
		unsigned int GetUnsignedInt() const;
		int GetInt() const;
		float GetFloat() const;
		std::string GetString() const;

	private:
		cVar();
	};


	// *** Inlines

	

	template <class T>
	inline cVar<T>::cVar(const cVar<T>& rhs)
	{
		//uiFlags = rhs.uiFlags;
		value = rhs.value;
	}

	template <class T>
	inline cVar<T>::cVar(const T& rhs, unsigned int flags)
	{
		//uiFlags = rhs.uiFlags;
		value = rhs;
	}


	template <class T>
	inline bool cVar<T>::GetBool() const
	{
		return (0 == value);
	}

	template <class T>
	inline unsigned int cVar<T>::GetUnsignedInt() const 
	{
		std::stringstream t(value);
		unsigned int i;
		t>>i;
		return i;
	}
	
	template <class T>
	inline int cVar<T>::GetInt() const 
	{
		//std::stringstream t(value);
		//int i;
		//t>>i;
		return (T)value;
	}

	template <class T>
	inline float cVar<T>::GetFloat() const 
	{
		//std::stringstream t(value);
		//float i;
		//t>>i;
		return (T)value;
	}
	
	template <class T>
	inline std::string cVar<T>::GetString() const 
	{
		//std::stringstream t(value);
		//std::string i;
		//t>>i;
		return (T)value;
	}

	template <class T>
	inline cVar<T>& cVar<T>::operator=(const cVar<T>& rhs)
	{
		value = rhs.value;
		return *this;
	}

	// Specialised for std::string
	
	
	template <>
	inline cVar<std::string>::cVar(const cVar<std::string>& rhs)
	{
		//uiFlags = rhs.uiFlags;
		value = rhs.value;
	}

	/*template <>
	inline cVar<std::string>::cVar(const std::string& rhs, unsigned int flags)
	{
		//uiFlags = rhs.uiFlags;
		value = rhs;
	}*/

	
	inline bool cVar<std::string>::GetBool() const
	{
		return (strcmp(value.c_str(), "true") == 0 || strcmp(value.c_str(), "1") == 0);
	}

	
	inline unsigned int cVar<std::string>::GetUnsignedInt() const 
	{
		std::stringstream t(value);
		unsigned int i = 0;
		t>>i;
		return i;
	}
	
	inline int cVar<std::string>::GetInt() const 
	{
		std::stringstream t(value);
		int i = 0;
		t>>i;
		return i;
	}

	inline float cVar<std::string>::GetFloat() const 
	{
		std::stringstream t(value);
		float i = 0.0f;
		t>>i;
		return i;
	}
	
	inline std::string cVar<std::string>::GetString() const 
	{
		return value;
	}

	inline cVar<std::string>& cVar<std::string>::operator=(const cVar<std::string>& rhs)
	{
		value = rhs.value;
		return *this;
	}


	
	class var
	{
	public:
		template <class T>
		static void VarSet(const std::string& name, const T& value)
		{
			cVar<std::string>* pVar = VarFind(name);
			if (pVar != nullptr) {
				*pVar = value;
				return;
			}

			mVar[name] = new cVar<std::string>(value);
		}

		static cVar<std::string>* VarFind(const std::string& name);
		static void PrintAll();

	private:
		static std::map<std::string, cVar<std::string>*> mVar;
	};
}

#endif //CVAR_H
