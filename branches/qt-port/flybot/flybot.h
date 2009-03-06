// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FLYBOT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FLYBOT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef FLYBOT_EXPORTS
#define FLYBOT_API __declspec(dllexport)
#else
#define FLYBOT_API __declspec(dllimport)
#endif

// This class is exported from the flybot.dll
class FLYBOT_API Cflybot {
public:
	Cflybot(void);
	// TODO: add your methods here.
};

extern FLYBOT_API int nflybot;

FLYBOT_API int fnflybot(void);
