#ifndef CSMARTPTR_H
#define CSMARTPTR_H

// Now using boost::smart_ptr directly
// c++0x: Change this to something like
//typedef boost::shared_ptr cSmartPtr;
#define cSmartPtr boost::shared_ptr

#endif // CSMARTPTR_H
