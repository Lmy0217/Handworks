#include "com_android_handworks_CameraPreview.h"
#include <jni.h>
#include <string.h>
#include "works.h"

JNIEXPORT jbyteArray JNICALL Java_com_android_handworks_CameraPreview_Handworks
  (JNIEnv *env, jobject _obj, jint width, jint height, jbyteArray yuv) {

	jbyte *yuvp = env -> GetByteArrayElements(yuv, NULL);
	jint length = env -> GetArrayLength(yuv);

	jbyteArray fter = env -> NewByteArray(length);
	jbyte *fterp = env -> GetByteArrayElements(fter, NULL);

	jbyteArray binary = env -> NewByteArray(length * 2 / 3);
	jbyte *binaryp = env -> GetByteArrayElements(binary, NULL);

	jint d0 = -1;

	filter(fterp, yuvp, width, height);
	getbinaryData(binaryp, &d0, fterp, width, height);
	gethandData(binaryp, d0, width, height);

	env -> ReleaseByteArrayElements(yuv, yuvp, 0);
	env -> ReleaseByteArrayElements(fter, fterp, 0);
	env -> ReleaseByteArrayElements(binary, binaryp, 0);

	return binary;
}
