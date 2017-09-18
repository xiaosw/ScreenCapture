#include <jni.h>
#include "packt.h"


RTMP *rtmp = 0;
RTMP_VIDEO *rtmp_video = 0;


JNIEXPORT jboolean JNICALL
Java_com_dongnao_screencapture_RtmpManager_connect(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = (*env)->GetStringUTFChars(env, url_, 0);
    int ret;
    do {
        rtmp = RTMP_Alloc();
        if (!rtmp) break;
        RTMP_Init(rtmp);
        rtmp->Link.timeout = 10;
        LOGI("connect %s", url);
        if (!(ret = RTMP_SetupURL(rtmp, url))) break;
        RTMP_EnableWrite(rtmp);
        LOGI("RTMP_Connect");
        if (!(ret = RTMP_Connect(rtmp, 0))) break;
        LOGI("RTMP_ConnectStream ");
        if (!(ret = RTMP_ConnectStream(rtmp, 0))) break;
        LOGI("connect success");
        rtmp_video = malloc(sizeof(RTMP_VIDEO));
        memset(rtmp_video, 0, sizeof(RTMP_VIDEO));
        rtmp_video->stream_id = rtmp->m_stream_id;
    } while (0);
    (*env)->ReleaseStringUTFChars(env, url_, url);
    return ret;
}

JNIEXPORT jboolean JNICALL
Java_com_dongnao_screencapture_RtmpManager_isConnect(JNIEnv *env, jobject instance) {
    return rtmp && RTMP_IsConnected(rtmp);
}

JNIEXPORT void JNICALL
Java_com_dongnao_screencapture_RtmpManager_disConnect(JNIEnv *env, jobject instance) {

    if (rtmp) {
        RTMP_Close(rtmp);
        RTMP_Free(rtmp);
        rtmp = 0;
    }
    if (rtmp_video) {
        if (rtmp_video->sps) {
            free(rtmp_video->sps);
        }
        if (rtmp_video->pps) {
            free(rtmp_video->pps);
        }
        rtmp_video = 0;
    }
}

int sendPacket(RTMPPacket *packet) {
    int r = RTMP_SendPacket(rtmp, packet, 1);
    RTMPPacket_Free(packet);
    free(packet);
    return r;
}

int sendVideo(char *buf, int len, long tms) {
    int ret;
    do {
        if (buf[4] == 0x67) {//sps pps
            if (rtmp_video && (!rtmp_video->pps || !rtmp_video->sps)) {
                parseVideoConfiguration(buf, len, rtmp_video);
            }
        } else {
            if (buf[4] == 0x65) {//关键帧
                RTMPPacket *packet = packetVideoDecode(rtmp_video);
                if (!(ret = sendPacket(packet))) {
                    break;
                }
            }
            RTMPPacket *packet = packetVideoData(buf, len, tms, rtmp_video);
            ret = sendPacket(packet);
        }
    } while (0);
    return ret;
}

int sendAudio(char *buf, int len, int type, int tms) {
    int ret;
    do {
        RTMPPacket *packet = packetAudioData(buf, len, type, tms, rtmp_video);
        ret = sendPacket(packet);
    } while (0);
    return ret;
}

JNIEXPORT  jboolean JNICALL
Java_com_dongnao_screencapture_RtmpManager_sendData(JNIEnv *env, jobject instance, jbyteArray data_,
                                                    jint len, jint type, jlong tms) {
    jbyte *data = (*env)->GetByteArrayElements(env, data_, NULL);
    int ret;
    switch (type) {
        case 0: //video
            ret = sendVideo(data, len, tms);
            break;
        default: //audio
            ret = sendAudio(data, len, type, tms);
            break;
    }
    (*env)->ReleaseByteArrayElements(env, data_, data, 0);
    return ret;
}