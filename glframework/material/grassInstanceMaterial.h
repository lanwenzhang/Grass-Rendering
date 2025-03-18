#pragma once

#include "material.h"
#include "../texture.h"


class GrassInstanceMaterial :public Material {
public:

	GrassInstanceMaterial();
	~GrassInstanceMaterial();

public:
	Texture* mDiffuse{ nullptr };
	Texture* mOpacityMask{ nullptr };
	float mShiness{ 1.0f };

	float mUVScale{ 1.0f };
	float mBrightness{ 1.0f };

	float mWindScale{ 0.1f };
	glm::vec3 mWindDirection{ 1.0f, 1.0f, 1.0f };
	float  mPhaseScale{ 1.0f };

	Texture* mCloudMask{ nullptr };
	glm::vec3 mCloudWhiteColor{ 0.576, 1.0, 0.393 };
	glm::vec3 mCloudBlackColor{ 0.994, 0.3, 0.426 };
	float mCloudUVScale{ 1.0f };
	float mCloudSpeed{ 0.1f };
	float mCloudLerp{ 0.5f };
};