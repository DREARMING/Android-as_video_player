//
// Created by mvcoder on 2019/5/29.
//

#include "./video_projector_player_controller.h"
#include "video_projector_player_controller.h"


void ProjectorCallbackImpl::setParent(VideoProjector *projector) {
    this->projector = projector;
}

void ProjectorCallbackImpl::onRenderTexture(FrameTexture *frameTexture) {
    if(projector){
        projector->onRenderTexture(frameTexture);
    }
}

void ProjectorCallbackImpl::onAudioDataAvailable(byte *data, size_t bufferSize) {
    if(projector){
        projector->onAudioDataAvailable(data, bufferSize);
    }
}

VideoProjector::VideoProjector() {

}

void VideoProjector::init(char *url) {
    int len = strlen(url);
    videoPath = new char[len+1];
    memcpy(videoPath, url, sizeof(char) * (len + 1));


}

VideoProjector::~VideoProjector() {
    //回收资源
    if(videoPath) delete videoPath;

}


