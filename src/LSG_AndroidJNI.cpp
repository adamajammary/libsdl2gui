#if defined _android
#include "LSG_AndroidJNI.h"

/**
 * @throws runtime_error
 */
AAssetManager* LSG_AndroidJNI::GetAssetManager()
{
	auto jniEnvironment    = LSG_AndroidJNI::GetEnvironment();
	auto jniObjectActivity = (jobject)SDL_AndroidGetActivity();

	if (!jniObjectActivity)
		throw std::runtime_error("Failed to get a valid Android JNI Activity.");

	auto jniClassActivity  = LSG_AndroidJNI::GetClass("android/app/Activity",          jniEnvironment);
	auto jniClassResources = LSG_AndroidJNI::GetClass("android/content/res/Resources", jniEnvironment);

	auto jniMethodGetAssets    = jniEnvironment->GetMethodID(jniClassResources, "getAssets",    "()Landroid/content/res/AssetManager;");
	auto jniMethodGetResources = jniEnvironment->GetMethodID(jniClassActivity,  "getResources", "()Landroid/content/res/Resources;");

	auto jniObjectResources    = jniEnvironment->CallObjectMethod(jniObjectActivity,  jniMethodGetResources);
	auto jniObjectAssetManager = jniEnvironment->CallObjectMethod(jniObjectResources, jniMethodGetAssets);

	auto jniAssetManager = AAssetManager_fromJava(jniEnvironment, jniObjectAssetManager);

	if (!jniAssetManager)
		throw std::runtime_error("Failed to get a valid Android JNI Asset Manager.");

	jniEnvironment->DeleteLocalRef(jniObjectAssetManager);
	jniEnvironment->DeleteLocalRef(jniObjectResources);
	jniEnvironment->DeleteLocalRef(jniObjectActivity);
	jniEnvironment->DeleteLocalRef(jniClassResources);
	jniEnvironment->DeleteLocalRef(jniClassActivity);

	return jniAssetManager;
}

/**
 * @throws runtime_error
 */
jclass LSG_AndroidJNI::GetClass(const std::string& classPath, JNIEnv* jniEnvironment)
{
	auto jniClass = jniEnvironment->FindClass(classPath.c_str());

	if (!jniClass)
		throw std::runtime_error(LSG_Text::Format("Failed to find class: '%s'", classPath.c_str()));

	return jniClass;
}

/**
 * @throws runtime_error
 */
JNIEnv* LSG_AndroidJNI::GetEnvironment()
{
	auto jniEnvironment = (JNIEnv*)SDL_AndroidGetJNIEnv();

	if (!jniEnvironment)
		throw std::runtime_error("Failed to get a valid Android JNI Environment.");

	return jniEnvironment;
}
#endif
