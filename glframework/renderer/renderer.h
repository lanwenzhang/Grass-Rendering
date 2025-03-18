#pragma once

#include <vector>
#include "../core.h"
#include "../mesh/mesh.h"
#include "../../application/camera/camera.h"
#include "../light/directionalLight.h"
#include "../light/pointLight.h"
#include "../light/spotLight.h"
#include "../light/ambientLight.h"
#include "../shader.h"
#include "../scene.h"

class Renderer {

public:

	Renderer();
	~Renderer();

	void render(
		Scene* scene,
		Camera* camera,
		DirectionalLight* dirLight,
		AmbientLight* ambLight,
		unsigned int fbo = 0
	);

	void renderObject(
		Object* object,
		Camera* camera,
		DirectionalLight* dirLight,
		AmbientLight* ambLight
	);

	void setClearColor(glm::vec3 color);

	Material* mGlobalMaterial{ nullptr };

private:
	void projectObject(Object* obj);

	Shader* pickShader(MaterialType type);

	void setDepthState(Material* material);
	void setPolygonOffsetState(Material* material);
	void setStencilState(Material* material);
	void setBlendState(Material* material);
	void setFaceCullingState(Material* material);

private:
	Shader* mPhongShader{ nullptr };
	Shader* mWhiteShader{ nullptr };
	Shader* mDepthShader{ nullptr };
	Shader* mOpacityMaskShader{ nullptr };
	Shader* mScreenShader{ nullptr };
	Shader* mCubeShader{ nullptr };
	Shader* mPhongEnvShader{ nullptr };
	Shader* mPhongInstanceShader{ nullptr };
	Shader* mGrassInstanceShader{ nullptr };

	std::vector<Mesh*> mOpacityObjects{};
	std::vector<Mesh*> mTransparentObjects{};
};