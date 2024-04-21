#pragma once


#ifdef ENABLELOG

#define BEGIN_LOG(stuff) std::cout<< stuff

#define END_LOG << std::endl;

#define FLUSH std::cout.flush();

#else

#define BEGIN_LOG(stuff)

#define END_LOG

#define FLUSH
#endif
