#include <stdio.h>
#include <string.h>
#include <linux/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "i2c-dev.h"
#include "i2c.h"
#include "i2c_data.h"
#include <jni.h>

#include "android/log.h"
static const char *TAG="EEPROM_aplex";
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO, TAG, fmt, ##args)
/*
 * Class:     com_android_i2capi_I2CAPI
 * Method:    open
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_android_i2capi_I2CAPI_open
(JNIEnv * env, jobject thiz, jstring path, jint flags){
	int fd = -1;
	// Opening device
	{
		jboolean iscopy;
		// 将Java的字符串转换成C中的字符串
		const char *path_utf = (*env)->GetStringUTFChars(env, path, &iscopy);
		//LOGI("Open file %s with flags 0x%x", path_utf, O_RDWR | flags);
		fd = open(path_utf, O_RDWR | flags);
		LOGI("open() fd = %d", fd);
		// 和前面的GetStringUTFChars一对用法，相当于malloc和free
		(*env)->ReleaseStringUTFChars(env, path, path_utf);
		if (fd == -1)
		{
			// Throw an exception
			LOGE("Cannot open file");
			return -1;
		}
	}
    return (jint)fd;
}

/*
 * Class:     com_android_i2capi_I2CAPI
 * Method:    readStr
 * Signature: (IIII)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_android_i2capi_I2CAPI_readStr
(JNIEnv * env, jobject thiz, jint fd, jint addr, jint offset, jint count){
    unsigned char buf[count+1];
    bzero(buf, count+1);

    if (i2c_data_read_str (fd, (unsigned char)addr, (unsigned char)offset, buf, count) == 0) {
    	return (*env)->NewStringUTF(env, buf);
    } else {
    	return (*env)->NewStringUTF(env, "-1");
    }
}

/*
 * Class:     com_android_i2capi_I2CAPI
 * Method:    writeStr
 * Signature: (IIII)Ljava/lang/String;
 */
JNIEXPORT jint JNICALL Java_com_android_i2capi_I2CAPI_writeStr
(JNIEnv * env, jobject thiz, jint fd, jint addr, jint offset, jstring buffer, jint count){
    unsigned char* buf = NULL;
    jclass clsstring = (*env)->FindClass(env, "java/lang/String");
    jstring strencode = (*env)->NewStringUTF(env, "utf-8");
    jmethodID mid = (*env)->GetMethodID(env, clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr= (jbyteArray)(*env)->CallObjectMethod(env, buffer, mid, strencode);
    jsize alen = (*env)->GetArrayLength(env, barr);
    jbyte* ba = (*env)->GetByteArrayElements(env, barr, JNI_FALSE);
    if (alen > 0)
    {
        buf = (unsigned char*)malloc(alen + 1);
    
        memcpy(buf, ba, alen);
        buf[alen] = 0;
    }
    (*env)->ReleaseByteArrayElements(env, barr, ba, 0);
	//LOGI("fd: %d, write String: %s, string.length: %d", fd, buf, alen);
    return i2c_data_write_str (fd, (unsigned char)addr, (unsigned char)offset, buf, count > alen ? alen : count);
}

/*
 * Class:     com_android_i2capi_I2CAPI
 * Method:    close
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_android_i2capi_I2CAPI_close
(JNIEnv * env, jobject thiz, jint fd){
	close( (int)fd );
	//LOGI("close() fd = %d", fd);
}

