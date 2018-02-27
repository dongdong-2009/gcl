#ifndef _AUTO_LINK_H_
#define _AUTO_LINK_H_

#ifdef NDEBUG
#undef _DEBUG
#endif

#ifdef _DEBUG
	#define ICS_SUFFIX "d"
#else
	#define ICS_SUFFIX ""
#endif

#ifdef WIN32
	#define	STDCALL	__stdcall
	#define ICS_EXPORT_API __declspec(dllexport)		
	#define ICS_IMPORT_API __declspec(dllimport)		
#else
	#define ICS_EXPORT_API __declspec(dllexport)		
	#define ICS_IMPORT_API __declspec(dllimport)	
#endif

#endif  //_AUTO_LINK_H_
