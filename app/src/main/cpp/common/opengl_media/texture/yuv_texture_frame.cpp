#include "yuv_texture_frame.h"

#define LOG_TAG "YUVTextureFrame"

YUVTextureFrame::YUVTextureFrame() {
//	writeFlag = true;
}

YUVTextureFrame::~YUVTextureFrame() {

}

bool YUVTextureFrame::createTexture() {
	LOGI("enter YUVTextureFrame::createTexture");
	textures[0] = 0;
	textures[1] = 0;
	textures[2] = 0;
	int ret = initTexture();
	if (ret < 0) {
		LOGI("init texture failed...");
		this->dealloc();
		return false;
	}
	return true;
}

void YUVTextureFrame::setVideoFrame(VideoFrame *yuvFrame){
	this->frame = yuvFrame;
//	if (writeFlag) {
//		LOGI("after glReadPixels... ");
//		FILE* textureFrameFile = fopen("/mnt/sdcard/a_songstudio/texture.yuv", "wb+");
//		if (NULL != textureFrameFile) {
//			int width = yuvFrame->width;
//			int height = yuvFrame->height;
//			fwrite(yuvFrame->luma, sizeof(byte), width*height, textureFrameFile);
//			fwrite(yuvFrame->chromaB, sizeof(byte), width*height / 4, textureFrameFile);
//			fwrite(yuvFrame->chromaR, sizeof(byte), width*height / 4, textureFrameFile);
//			fclose(textureFrameFile);
//			LOGI("write textureFrameFile success ... ");
//		}
//		writeFlag = false;
//	}
}

void YUVTextureFrame::updateTexImage() {
//	LOGI("YUVTextureFrame::updateTexImage");
	if (frame) {
//		LOGI("start upload texture");
		int frameWidth = frame->width;
		int frameHeight = frame->height;
		if (frameWidth % 16 != 0) {
			/**
			 * lPixelStorei用于设置像素存储模式。下面将简单介绍一下这个接口。
			 * 它含有两个参数：
			 * pname：指定所要被设置参数的符号名。这里，参数的符号名有两种，
			 * 一种是GL_PACK_ALIGNMENT，它影响将像素数据写回到主存的打包形式，对glReadPixels的调用产生影响；
			 * 还有一种是GL_UNPACK_ALIGNMENT，它影响从主存读到的像素数据的解包形式，对glTexImage2D以及glTexSubImage2D产生影响。
			 * param：指定相应的pname设置为什么值。这个数值一般是1、2、4或8，
			 * 用于指定存储器中每个像素行有多少个字节对齐。对齐的字节数越高，系统就越能优化。
			 * 在实际代码中，我们看到的是glPixelStorei(GL_UNPACK_ALIGNMENT, 1);没做什么优化
			 *
			 */
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		}
		uint8_t *pixels[3] = {frame->luma, frame->chromaB, frame->chromaR};
		int widths[3] = {frameWidth, frameWidth >> 1, frameWidth >> 1};
		int heights[3] = {frameHeight, frameHeight >> 1, frameHeight >> 1};
		for (int i = 0; i < 3; ++i) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			if (checkGlError("glBindTexture")) {
				return;
			}
			/**
			 *
			 *
    		 * 第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理
    		 * （任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。
    		 * 第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。
    		 * 第三个参数告诉OpenGL我们希望把纹理储存为何种格式。比如是RGB、LUMINANCE
    		 * 第四个和第五个参数设置最终的纹理的宽度和高度。
    		 * 下个参数应该总是被设为0（历史遗留的问题）。
    		 * 第七第八个参数定义了源图的格式和数据类型。如果我们使用RGB值加载这个图像，并把它们储存为char(byte)数组，我们将会传入对应值。
    		 * 如果是LUMINANCE模式的图片，这里是YUV420，YUV分量变成3个纹理图片。所以填 LUMINANCE
    		 * 最后一个参数是真正的图像数据。
			 *
			 *
			 */
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widths[i], heights[i], 0, GL_LUMINANCE,
						 GL_UNSIGNED_BYTE, pixels[i]);
		}
	}
}

bool YUVTextureFrame::bindTexture(GLint* uniformSamplers) {
	for (int i = 0; i < 3; ++i) {
		/**
		 * 因为 updateTexImage已经对具体的纹理，比如 GL_TEXTURE0 绑定了 Y 分量的纹理图片数据，
		 * 这里再绘制时，只需要从新激活即可把数据输入到正确的 片段着色器的 纹理坐标属性
		 */
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		if (checkGlError("glBindTexture")) {
			return false;
		}
		glUniform1i(uniformSamplers[i], i);
	}
	return true;
}

void YUVTextureFrame::dealloc() {
	LOGI("enter YUVTextureFrame::dealloc");
	if (textures[0]) {
		glDeleteTextures(3, textures);
	}
}

int YUVTextureFrame::initTexture() {
	//创建3个纹理对象，并把纹理id存在 textures数组 里面
	glGenTextures(3, textures);
	for (int i = 0; i < 3; i++) {
		//绑定具体的纹理id，这样操作 GL_TEXTURE_2D 时，就会把状态设置到具体的纹理对象
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		if (checkGlError("glBindTexture")) {
			return -1;
		}
		//设置纹理过滤模式
		/**
		 * 因为纹理坐标是不依赖于分辨率的，一张照片是由一个个点组成的，采样的时候都是根据坐标，大概找出该纹理坐标对应
		 * 着图片上哪里像素点，如果选择纹理坐标所处的点就是 GL_NEAREST 过滤模式。
		 *
		 * 当然也可以设置为选择纹理坐标 临边的4个像素，按照加权值来获取像素值，这种依照纹理坐标周围像素点来找像素的值的过滤模式就是
		 * 线性模式
		 *
		 * GL_NEAREST产生了颗粒状的图案，我们能够清晰看到组成纹理的像素，而GL_LINEAR能够产生更平滑的图案，很难看出单个的纹理像素。
		 * GL_LINEAR可以产生更真实的输出，但有些开发者更喜欢8-bit风格，所以他们会用GL_NEAREST选项。
		 *
		 * 当进行放大(Magnify)和缩小(Minify)操作的时候可以设置纹理过滤的选项，
		 * 比如你可以在纹理被缩小的时候使用邻近过滤，被放大时使用线性过滤。
		 * 我们需要使用glTexParameter*函数为放大和缩小指定过滤方式。
		 */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (checkGlError("glTexParameteri")) {
			return -1;
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		if (checkGlError("glTexParameteri")) {
			return -1;
		}
		/**
		 * 纹理环绕方式
		 *
		 *  因为图片不一定大于显示的view的，如果纹理图片的大小要小于要渲染的view的大小的话，就会涉及到超出部分如何选取采样值了
		 *
		 *  GL_REPEAT 	对纹理的默认行为。重复纹理图像。
			GL_MIRRORED_REPEAT 	和GL_REPEAT一样，但每次重复图片是镜像放置的。
			GL_CLAMP_TO_EDGE 	纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。
			GL_CLAMP_TO_BORDER 	超出的坐标为用户指定的边缘颜色。
		 *
		 * 设置好纹理坐标和纹理图片的像素对应，即4个顶点都对应着纹理图片的4个顶点，就不需要担心环绕方式，纹理坐标的点点都会映射到
		 * 纹理图片上
		 *
		 */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		if (checkGlError("glTexParameteri")) {
			return -1;
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if (checkGlError("glTexParameteri")) {
			return -1;
		}
	}
	return 1;
}
