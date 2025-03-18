#pragma once

#include "../core.h"

enum class MaterialType {

	PhongMaterial,
	WhiteMaterial,
	DepthMaterial,
	OpacityMaskMaterial,
	ScreenMaterial,
	CubeMaterial,
	PhongEnvMaterial,
	PhongInstanceMaterial,
	GrassInstanceMaterial
};

class Material {

public:

	Material();
	~Material();

public:

	MaterialType mType;

	// Depth
	bool mDepthTest{ true };
	GLenum mDepthFunc{ GL_LEQUAL };
	bool mDepthWrite{ true };

	// PolygonOffset
	bool mPolygonOffset{ false };
	unsigned int mPolygonOffsetType{ GL_POLYGON_OFFSET_FILL };
	float mFactor{ 0.0f };
	float mUnit{ 0.0f };

	// Stencil
	bool mStencilTest{ false };
	unsigned int mSFail{ GL_KEEP }; // Stencil fail
	unsigned int mZFail{ GL_KEEP }; // Stencil pass but depth fail
	unsigned int mZPass{ GL_KEEP }; // Stencil and depth pass
	unsigned int mStencilMask{ 0xFF };

	unsigned int mStencilFunc{ GL_ALWAYS };
	unsigned int mStencilRef{ 0 };
	unsigned int mStencilFuncMask{ 0xFF };

	// Blend
	bool mBlend{ false };
	unsigned int mSFactor{ GL_SRC_ALPHA };
	unsigned int mDFactor{ GL_ONE_MINUS_SRC_ALPHA };

	float mOpacity{ 1.0f };

	// Face culling
	bool mFaceCulling{ false };
	unsigned int mFrontFace{ GL_CCW };
	unsigned int mCullFace{ GL_BACK };

};