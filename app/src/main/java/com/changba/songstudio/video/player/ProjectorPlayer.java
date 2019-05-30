package com.changba.songstudio.video.player;

import android.view.Surface;

public class ProjectorPlayer {


    public native void onSurfaceCreated(final Surface surface, int width, int height);

    public native void onSurfaceDestroyed(final Surface surface);

    /**
     * 初始化
     *
     * @param srcFilenameParam
     *            文件路径或者直播地址
     * @return 是否正确初始化
     */
    public native boolean prepare(String srcFilenameParam);

    /**
     * 暂停播放
     */
    public native void pause();

    /**
     * 继续播放
     */
    public native void play();


    public native void stop();

}
