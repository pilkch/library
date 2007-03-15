#ifndef CVAR_H
#define CVAR_H

namespace BREATHE
{
	//
	//	Right, cVars.  The constructor is
	//	cVar(std::string name, std::string value, unsigned int flags, PFNVARNOPARAMETERSPROC NoParameters, PFNVARCHANGEPROC Change);
	//
	//	name=The name of the cvar.  For example "sv_gravity".  
	//	value=The value of the cvar.  For example "800.0".  
	//	flags=The flags that specify how the cvar acts.  For example VAR_CLIENT | VAR_SERVER means that it will be saved to the client and server config files.  

	//
	//	server.cfg
	//	sv_map //This cVar doesn't change the map, just holds what map we are on
	//	sv_changelevel //This cVar changes the map
	//	sv_maxplayers
	//	sv_name
	//	sv_maps //List the maps that we can play
	//

	/*extern "C"
	{
		typedef void (*PFNVARNOPARAMETERSPROC)(BREATHE::std::string &value);
		typedef void (*PFNVARCHANGEPROC)(BREATHE::std::string previous, BREATHE::std::string &value);
	}*/

	enum VAR
	{
		VAR_NULL=0,

		VAR_SAVE_CL=1,				// Written to client.cfg
		VAR_SAVE_SV=2,				// Written to server.cfg

		VAR_READ_ONLY_IN_GAME=4,		// Can't change once in game.  For example: "sv_maxplayers" etc.  
		VAR_READ_ONLY_IN_CONSOLE=8,	// Can't change once in console.  For example: "r_driver" etc.  
		VAR_READ_ONLY=16,				// Can't change at all ever, even programatically.  Used for functions.  For example: "help", "version" etc.  
	};

	class cVar
	{
	protected:
		//unsigned int uiFlags;
	
		std::string sValue;

	public:
		cVar(const cVar& rhs);
		cVar(std::string value, unsigned int flags=VAR_NULL);
		~cVar();

		//Convert sValue into these types
		operator bool ();
		operator bool () const;
		operator unsigned int () const;
		operator int () const;
		operator float () const;
		operator std::string () const;

		//Convert these types into sValue
		void operator=(const std::string &rhs);

		std::string operator*(const std::string & rhs) const
		{	return std::string(sValue + rhs);	}
	};
}

#endif //CVAR_H
