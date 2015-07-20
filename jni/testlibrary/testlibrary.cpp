#include "testlibrary.h"

#include <curl/curl.h>


#ifdef ANDROID
	#include <android/log.h>
	#include <jni.h>
#include <sys/types.h>
#include <string>
#include<sstream>
#include <iostream>
#include <string>
#include <algorithm>

	#ifdef __LP64__
		#define SIZE_T_TYPE "%lu"
	#else
		#define SIZE_T_TYPE "%u"
	#endif	
#endif

#ifdef ANDROID
	#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "TestLibrary", __VA_ARGS__))
#else
	#define LOGI(...) printf(__VA_ARGS__)
#endif

//const char *caCertPtr;
static std::string caCertPtr;

size_t curlCallback(char *data, size_t size, size_t count, void* userdata);

BOOL downloadUrl(const char* url, LPCURL_DOWNLOAD_OBJECT downloadObject ) {
	//static const char *pCACertFile="/data/cacert.pem";
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
/*	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, TRUE);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, TRUE);*/
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curlCallback);
	curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, downloadObject);
	curl_easy_setopt(curl,CURLOPT_CAINFO,caCertPtr.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

	curl_version_info_data * vinfo = curl_version_info( CURLVERSION_NOW );
	if( vinfo->features & CURL_VERSION_SSL )
	    // SSL support enabled
		 LOGI("SSL support enabled");
	else
	{// No SSL
		 LOGI("NO SSL");
	}

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK){
	    LOGI("CURL failed with error code %d", res);
	}

	LOGI("CURL download is OK, result:%d", res);
	curl_easy_cleanup(curl);
	return res == CURLE_OK;
}

size_t curlCallback(char *data, size_t size, size_t count, void* userdata) {
	LOGI("Downloaded data size is " SIZE_T_TYPE, size*count);

    LPCURL_DOWNLOAD_OBJECT downloadObject = (LPCURL_DOWNLOAD_OBJECT) userdata;
    long newSize = 0;
    long offset = 0;
    LPBYTE dataPtr;

    if (downloadObject->data == NULL){
        newSize = size * count * sizeof(BYTE);
        dataPtr = (LPBYTE)malloc(newSize);
    }else{
        newSize = downloadObject->size + (size * count * sizeof(BYTE));
        dataPtr = (LPBYTE)realloc(downloadObject->data, newSize);
        offset = downloadObject->size;
    }

    if (dataPtr==NULL){//malloc or realloc failed
        if (downloadObject->data != NULL){//realloc failed
            free(downloadObject->data);
            downloadObject->data = NULL;
            downloadObject->size = 0;
        }

        return 0; //this will abort the download
    }
    downloadObject->data = dataPtr;
    downloadObject->size = newSize;

    memcpy(downloadObject->data + offset, data, size * count * sizeof(BYTE));
	return size*count;
}

#ifdef ANDROID
extern "C"
{
	JNIEXPORT jbyteArray JNICALL
	Java_com_example_androidtest_TestActivity_downloadUrl(JNIEnv* env, jobject obj, jstring url ){
		const char* url_c = env->GetStringUTFChars(url, NULL);
		if (!url_c)
			return NULL;

		LOGI( "Download URL: %s", url_c );
		CURL_DOWNLOAD_OBJECT* downloadObject = new CURL_DOWNLOAD_OBJECT;
        downloadObject->data = NULL;
        downloadObject->size=0;

		if (downloadUrl(url_c, downloadObject)){
			env->ReleaseStringUTFChars(url, url_c);
			jbyteArray ret = env->NewByteArray(downloadObject->size);
			env->SetByteArrayRegion(ret, 0, downloadObject->size, (jbyte*)downloadObject->data);
			free(downloadObject->data);
			delete downloadObject;
			return ret;
		}else{
			env->ReleaseStringUTFChars(url, url_c);
			return NULL;
		}
	}


	JNIEXPORT void JNICALL Java_com_example_androidtest_TestActivity_setDir(JNIEnv* env, jobject obj, jstring caCertDir)
	{
		if(!caCertDir) return;

		const char* caCertDir_c = env->GetStringUTFChars(caCertDir, NULL);
				if (!caCertDir_c) return ;
		const jsize len = env->GetStringUTFLength(caCertDir);
				LOGI( "CaCertDir: %s", caCertDir_c );
				std::string caCert(caCertDir_c,len);
				caCertPtr=caCert;
				LOGI( "CaCertDirptr in std string: %s", caCertPtr.c_str());
				env->ReleaseStringUTFChars(caCertDir, caCertDir_c);
	}

}
#endif
