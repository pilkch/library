#ifndef PROFILE_H
#define PROFILE_H

/*
profiles.xml:

<profiles>
	<profile name="Chris" gender="male" cute="LittleBoy" realistic="Marine" colour="0.0, 1.0, 0.0"/>
	<profile name="Tina" gender="female" cute="LittleGirl" realistic="TankGirl" colour="1.0, 0.0, 0.0"/>
</profiles>

Usage:

firestarter::profile::iterator iter;
while (iter)
{
	std::cout<<"Profile"<<std::endl;
	std::cout<<"name="<<iter.GetName()<<std::endl;
	std::cout<<"gender="<<iter.IsFemale() ? "female" : "male"<<std::endl;
	std::cout<<"character="<<iter.GetCharacter()<<std::endl;
	std::cout<<"cute="<<iter.GetCharacterCute()<<std::endl;
	std::cout<<"realistic="<<iter.GetCharacterRealistic()<<std::endl;
	float fColour = iter.GetColour();
	std::cout<<"colour=("<<fColour[0]<<","<<fColour[1]<<","<<fColour[2]<<")"<<std::endl;
	iter++;
};
*/

namespace firestarter
{
	namespace profile
	{
		class iterator
		{
		public:
			iterator();
			~iterator();
		
			operator bool() const;
			iterator& operator++();
			void operator++(int);
 			
			std::string GetName() const;
			bool IsFemale() const;
			bool IsMale() const;
			std::string GetCharacter() const; // Default character: if cute has been chosen return that, else return realistic
			std::string GetCharacterCute() const;
			std::string GetCharacterRealistic() const;
			void GetColour(float* fRed, float* fGreen, float* fBlue) const;
		};
	}
}

#endif //PROFILE_H
