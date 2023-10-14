#if defined _android
#include "main.h"

#ifndef LSG_ANDROID_JNI_H
#define LSG_ANDROID_JNI_H

const std::string LSG_ACTIVITY_CLASS_PATH = "com/libsdl2gui/lib/Sdl2GuiActivity";

class LSG_AndroidJNI
{
private:
	LSG_AndroidJNI()  {}
	~LSG_AndroidJNI() {}

public:
	static AAssetManager* GetAssetManager();
	static jclass         GetClass(const std::string& classPath);
	static JNIEnv*        GetEnvironment();

};

#endif
#endif
