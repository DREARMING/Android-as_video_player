//
// Created by mvcoder on 2019/5/30.
//

#ifndef ANDROID_AS_VIDEO_PLAYER_PROJECTOR_VIDEO_OUT_H
#define ANDROID_AS_VIDEO_PLAYER_PROJECTOR_VIDEO_OUT_H

#include <android/native_window.h>
#include "common/egl_core/egl_core.h"
#include "common/opengl_media/render/video_gl_surface_render.h"

class ProjectorVideoOutput {

public:
    ProjectorVideoOutput();
    virtual ~ProjectorVideoOutput();
    /** 初始化Output **/
    bool initOutput(ANativeWindow* window, int screenWidth, int screenHeight);
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
    bool renderVideo();
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

    bool surfaceExists;
    bool isANativeWindowValid;
};


#endif //ANDROID_AS_VIDEO_PLAYER_PROJECTOR_VIDEO_OUT_H
