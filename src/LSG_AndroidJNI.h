#if defined _android
#include "main.h"

#ifndef LSG_ANDROID_JNI_H
#define LSG_ANDROID_JNI_H

class LSG_AndroidJNI
{
private:
	LSG_AndroidJNI()  {}
	~LSG_AndroidJNI() {}

public:
	static AAssetManager* GetAssetManager();
	static jclass         GetClass(const std::string& classPath, JNIEnv* jniEnvironment);
	static JNIEnv*        GetEnvironment();

};

#endif
#endif
