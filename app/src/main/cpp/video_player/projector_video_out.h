//
// Created by mvcoder on 2019/5/30.
//

#ifndef ANDROID_AS_VIDEO_PLAYER_PROJECTOR_VIDEO_OUT_H
#define ANDROID_AS_VIDEO_PLAYER_PROJECTOR_VIDEO_OUT_H

#include <android/native_window.h>
#include "common/egl_core/egl_core.h"
#include "common/opengl_media/render/video_gl_surface_render.h"
#include "CommonTools.h"
#include "message_queue/handler.h"
#include "message_queue/message_queue.h"
#include "./common/circle_texture_queue.h"

typedef enum {
    P_VIDEO_OUTPUT_MESSAGE_CREATE_EGL_CONTEXT,
    P_VIDEO_OUTPUT_MESSAGE_CREATE_WINDOW_SURFACE,
    P_VIDEO_OUTPUT_MESSAGE_DESTROY_WINDOW_SURFACE,
    P_VIDEO_OUTPUT_MESSAGE_DESTROY_EGL_CONTEXT,
    P_VIDEO_OUTPUT_MESSAGE_RENDER_FRAME
} ProjectorVideoOutputMSGType;

class ProjectorVideoOuputHandler;

class ProjectorVideoOutput {

public:
    ProjectorVideoOutput();
    virtual ~ProjectorVideoOutput();
    /** 初始化Output **/
    void initOutput(ANativeWindow* window, int screenWidth, int screenHeight);
    /** 重置视频绘制区域的大小 **/
    void resetRenderSize(int left, int top, int width, int height);
    /** 当surface创建的时候的调用 **/
    void onSurfaceCreated(ANativeWindow* window);
    /** 当surface销毁的时候调用 **/
    void onSurfaceDestroyed();
    /** 销毁Output **/
    void stopOutput();

    bool createEGLContext(ANativeWindow* window);
    void createWindowSurface(ANativeWindow* window);
    bool renderVideo(FrameTexture* frameTexture);
    void destroyWindowSurface();
    void destroyEGLContext();

    int getScreenWidth(){
        return screenWidth;
    };
    int getScreenHeight(){
        return screenHeight;
    };

    bool eglHasDestroyed;

private:
    EGLCore* eglCore;
    EGLSurface renderTexSurface;
    ANativeWindow* surfaceWindow;
    VideoGLSurfaceRender* renderer;

    int screenWidth;
    int screenHeight;
    pthread_t _threadId;
    ProjectorVideoOuputHandler* handler;
    MessageQueue* queue;
    static void* threadStartCallback(void *myself);
    void processMessage();

    bool surfaceExists;
    bool isANativeWindowValid;
};

class ProjectorVideoOuputHandler: public Handler {
private:
    ProjectorVideoOutput* videoOutput;
    bool initPlayerResourceFlag;
public:
    ProjectorVideoOuputHandler(ProjectorVideoOutput* videoOutput, MessageQueue* queue) :
            Handler(queue) {
        this->videoOutput = videoOutput;
        initPlayerResourceFlag = false;
    }
    void handleMessage(Message* msg) {
        int what = msg->getWhat();
        ANativeWindow* obj;
        switch (what) {
            case P_VIDEO_OUTPUT_MESSAGE_CREATE_EGL_CONTEXT:
                if (videoOutput->eglHasDestroyed){
                    break;
                }

                obj = (ANativeWindow*) (msg->getObj());
                initPlayerResourceFlag = videoOutput->createEGLContext(obj);
                break;
            case P_VIDEO_OUTPUT_MESSAGE_RENDER_FRAME:
                if (videoOutput->eglHasDestroyed) {
                    break;
                }

                if(initPlayerResourceFlag){
                    videoOutput->renderVideo(static_cast<FrameTexture *>(msg->getObj()));
                }
                break;
            case P_VIDEO_OUTPUT_MESSAGE_CREATE_WINDOW_SURFACE:
                if (videoOutput->eglHasDestroyed) {
                    break;
                }
                if(initPlayerResourceFlag){
                    obj = (ANativeWindow*) (msg->getObj());
                    videoOutput->createWindowSurface(obj);
                }
                break;
            case P_VIDEO_OUTPUT_MESSAGE_DESTROY_WINDOW_SURFACE:
                if(initPlayerResourceFlag){
                    videoOutput->destroyWindowSurface();
                }
                break;
            case P_VIDEO_OUTPUT_MESSAGE_DESTROY_EGL_CONTEXT:
                videoOutput->destroyEGLContext();
                break;
        }
    }
};

#endif //ANDROID_AS_VIDEO_PLAYER_PROJECTOR_VIDEO_OUT_H
