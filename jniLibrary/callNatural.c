#include "realHTML_tomcat_connector_JNINatural.h"

JNIEXPORT jint JNICALL Java_realHTML_tomcat_connector_JNINatural_jni_1callNatural
  (JNIEnv *env, jobject obj, jobjectArray keys, jobjectArray vals, jstring req_type, jobjectArray natinfos, jstring tmp_file, jstring settings_str, jstring nat_parms)
{
    FILE *test = fopen("/tmp/jnilog.log", "w");
    fprintf(test, "ping\n");

    const char *tmpfile  = (*env)->GetStringUTFChars(env, tmp_file, 0);
    const char *settings = (*env)->GetStringUTFChars(env, settings_str, 0);

    fprintf(test, "Tmp File:     [%s]\n", tmpfile);
    fprintf(test, "Settings:     [%s]\n", settings);

    int key_length = (*env)->GetArrayLength(env, keys);
    int val_length = (*env)->GetArrayLength(env, vals);

    if(key_length != val_length)
        return(-1);

    int i = 0;
    jstring string;
    const char *rawString = NULL;

    for(; i < key_length; i++)
    {
        string = (jstring)((*env)->GetObjectArrayElement(env, keys, i));
        rawString = (*env)->GetStringUTFChars(env, string, 0);
        fprintf(test, "[%s] = ", rawString);

        string = (jstring)((*env)->GetObjectArrayElement(env, vals, i));
        rawString = (*env)->GetStringUTFChars(env, string, 0);
        fprintf(test, "[%s]\n", rawString);
    }
    fclose(test);

    return 0;
}
