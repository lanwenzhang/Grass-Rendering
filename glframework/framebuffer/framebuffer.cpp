#include "framebuffer.h"

Framebuffer::Framebuffer(unsigned int width, unsigned int height) {

    mWidth = width;
    mHeight = height;

    // 1 Create FBO
    glGenFramebuffers(1, &mFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

    // 2 Create color attachment
    mColorAttachment = Texture::createColorAttachment(mWidth, mHeight, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorAttachment->getTexture(), 0);

    // 3 Create depth and stencil attchment 
    mDepthStencilAttachment = Texture::createDepthStencilAttachment(mWidth, mHeight, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthStencilAttachment->getTexture(), 0);

    // 4 Check error
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error:FrameBuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

Framebuffer::~Framebuffer() {

	if (mFBO){

		glDeleteFramebuffers(1, &mFBO);

	}
	if (mColorAttachment != nullptr){

		delete mColorAttachment;
	}
	if (mDepthStencilAttachment != nullptr) {

		delete mDepthStencilAttachment;
	}
}