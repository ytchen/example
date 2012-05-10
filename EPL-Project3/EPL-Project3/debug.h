#ifndef _INC_DEBUG
#define _INC_DEBUG

//
// define _DEBUG .or.  _DBG_MSG to enable output debug message
// in debug mode (only)
//
#if defined(_DEBUG) || defined(_DBG_MSG) || defined(_DBG_REL)
	#define	_DPRINTF(argument) dprintf##argument
#else
	#define _DPRINTF(str)	// do nothing
#endif	// defined(_DEBUG) || defined(_DBG_MSG)

//
// always declare dprintf
//
void dprintf(char *fmt, ...);
#endif	//_INC_DEBUG
