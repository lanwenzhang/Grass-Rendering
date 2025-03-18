#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../application/stb_image.h"

std::map<std::string, Texture*> Texture::mTextureCache{};

// Texture cache from drive
Texture* Texture::createTexture(const std::string& path, unsigned int unit) {
    
    // 1 Check if generate before
    auto iter = mTextureCache.find(path);
    if (iter != mTextureCache.end()) {

        return iter->second;
    }


    auto texture = new Texture(path, unit);
    mTextureCache[path] = texture;

    return texture;
}

// Texture cache from memory
Texture* Texture::createTextureFromMemory(
    const std::string& path,
    unsigned int unit,
    unsigned char* dataIn,
    uint32_t widthIn,
    uint32_t heightIn
) {

    auto iter = mTextureCache.find(path);
    if (iter != mTextureCache.end()) {

        return iter->second;
    }

    auto texture = new Texture(unit, dataIn, widthIn, heightIn);

    mTextureCache[path] = texture;

    return texture;
}


Texture* Texture::createColorAttachment(
    unsigned int width,
    unsigned int height,
    unsigned int unit) {

    return new Texture(width, height, unit);
}

Texture* Texture::createDepthStencilAttachment(
    unsigned int width,
    unsigned int height,
    unsigned int unit) {

    Texture* dsTex = new Texture();


    unsigned int depthStencil;
    glGenTextures(1, &depthStencil);
    glBindTexture(GL_TEXTURE_2D, depthStencil);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencil, 0);


    dsTex->mTexture = depthStencil;
    dsTex->mWidth = width;
    dsTex->mHeight = height;
    dsTex->mUnit = unit;

    return dsTex;
}

Texture::Texture() {}

// Load from drive
Texture::Texture(const std::string &path, unsigned int unit){
    
    mUnit = unit;
    // 1 stb read the texture
    // 1,1 Define variable
    int channels;

    // 1.2 Flip y-axis
    stbi_set_flip_vertically_on_load(true);

    // 1.3 Read texture
    unsigned char* data = stbi_load(path.c_str(), &mWidth, &mHeight, &channels, STBI_rgb_alpha);


    // 2 Generate
    glGenTextures(1, &mTexture);
    glActiveTexture(GL_TEXTURE0 + mUnit);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    // 3 Send data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // 4 Release data
    stbi_image_free(data);

    // 5 Set filter 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // 6 Set wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

// Load texture from memory
Texture::Texture(
    unsigned int unit,
    unsigned char* dataIn,
    uint32_t widthIn,
    uint32_t heightIn
    ) 
{
    mUnit = unit;
    // 1 stb read the texture
    // 1,1 Define variable
    int channels;

    // 1.2 Flip y-axis
    stbi_set_flip_vertically_on_load(true);

    // 1.3 Read texture
    // Assimp rules: png or jpg file, height = 0, width is the size of image
    uint32_t dataInSize = 0;
    if (!heightIn) {

        dataInSize = widthIn;
    }
    else {

        dataInSize = widthIn * heightIn * 4;
    }

    unsigned char* data = stbi_load_from_memory(dataIn, dataInSize, &mWidth, &mHeight, &channels, STBI_rgb_alpha);


    // 2 Generate
    glGenTextures(1, &mTexture);
    glActiveTexture(GL_TEXTURE0 + mUnit);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    // 3 Send data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // 4 Release data
    stbi_image_free(data);

    // 5 Set filter 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // 6 Set wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


}

Texture::Texture(unsigned int width, unsigned int height, unsigned int unit){

    mWidth = width;
    mHeight = height;
    mUnit = unit;

    glGenTextures(1, &mTexture);
    glActiveTexture(GL_TEXTURE0 + mUnit);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// Order: right, left, up, bottom, back, front
Texture::Texture(const std::vector<std::string>& paths, unsigned int unit) {

    mUnit = unit;
    mTextureTarget = GL_TEXTURE_CUBE_MAP;

    stbi_set_flip_vertically_on_load(false);

    // 1 Create cubemap texture object
    glGenTextures(1, &mTexture);
    glActiveTexture(GL_TEXTURE0 + mUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);

    // 2 stb read the texture
    // 2,1 Define variable
    int channels;
    int width = 0, height = 0;
    unsigned char* data = nullptr;
    for (int i = 0; i < paths.size(); i++) {

        data = stbi_load(paths[i].c_str(), &width, &height, &channels, STBI_rgb_alpha);

        if (data != nullptr) {

            // 2.2 Send data
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {

            std::cout << "Error: CubeMap Texture failed to load at path - " << paths[i] << std::endl;
            stbi_image_free(data);
        }
    }
    
    // 6 Set filter 
    glTexParameteri(mTextureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(mTextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // 7 Set wrapping
    glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);

}

Texture::~Texture(){
    if (mTexture != 0) {
        glDeleteTextures(1, &mTexture);
    }
}

void Texture::bind() {

    glActiveTexture(GL_TEXTURE0 + mUnit);
    glBindTexture(mTextureTarget, mTexture);
}