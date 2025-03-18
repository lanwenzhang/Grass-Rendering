#include "renderer.h"
#include <iostream>
#include "../material/phongMaterial.h"
#include "../material/whiteMaterial.h"
#include "../material/opacityMaskMaterial.h"
#include "../material/screenMaterial.h"
#include "../material/cubeMaterial.h"
#include "../material/phongEnvMaterial.h"
#include "../material/phongInstanceMaterial.h"
#include "../material/grassInstanceMaterial.h"
#include "../mesh/instancedMesh.h"
#include <string>
#include <algorithm>

Renderer::Renderer() {

	mPhongShader = new Shader("assets/shaders/phong.vert", "assets/shaders/phong.frag");
	mWhiteShader = new Shader("assets/shaders/white.vert", "assets/shaders/white.frag");
	mDepthShader = new Shader("assets/shaders/depth.vert", "assets/shaders/depth.frag");
	mOpacityMaskShader = new Shader("assets/shaders/phongOpacityMask.vert", "assets/shaders/phongOpacityMask.frag");
	mScreenShader = new Shader("assets/shaders/screen.vert", "assets/shaders/screen.frag");
	mCubeShader = new Shader("assets/shaders/cube.vert", "assets/shaders/cube.frag");
	mPhongEnvShader = new Shader("assets/shaders/phongEnv.vert", "assets/shaders/phongEnv.frag");
	mPhongInstanceShader = new Shader("assets/shaders/phongInstance.vert", "assets/shaders/phongInstance.frag");
	mGrassInstanceShader = new Shader("assets/shaders/grassInstance.vert", "assets/shaders/grassInstance.frag");
}

Renderer::~Renderer() {

}

void Renderer::setClearColor(glm::vec3 color) {

	glClearColor(color.r, color.g, color.b, 1.0);
}

void Renderer::render(
	Scene* scene,
	Camera* camera,
	DirectionalLight* dirLight,
	AmbientLight* ambLight,
	unsigned int fbo
) {

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);


	// 1 Depth and stencil test
	// 1.1 Depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	// 1.2 Polygon offset
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_POLYGON_OFFSET_LINE);

	// 1.3 Stencil test
	glEnable(GL_STENCIL_TEST);

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glStencilMask(0xFF);


	// 1.4 Blend
	glDisable(GL_BLEND);


	// 2 Clear canvas
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// 3 Clear two containers
	mOpacityObjects.clear();
	mTransparentObjects.clear();

	projectObject(scene);

	std::sort(
		mTransparentObjects.begin(),
		mTransparentObjects.end(),
		[camera](const Mesh* a, const Mesh* b) {


			auto viewMatrix = camera->getViewMatrix();

			auto modelMatrixA = a->getModelMatrix();
			auto worldPositionA = modelMatrixA * glm::vec4(0.0, 0.0, 0.0, 1.0);
			auto cameraPositionA = viewMatrix * worldPositionA;

			auto modelMatrixB = b->getModelMatrix();
			auto worldPositionB = modelMatrixB * glm::vec4(0.0, 0.0, 0.0, 1.0);
			auto cameraPositionB = viewMatrix * worldPositionB;

			return cameraPositionA.z < cameraPositionB.z;
		}
	);

	// 4 Render two containers
	for (int i = 0; i < mOpacityObjects.size(); i++) {

		renderObject(mOpacityObjects[i], camera, dirLight, ambLight);

	}


	for (int i = 0; i < mTransparentObjects.size(); i++) {

		renderObject(mTransparentObjects[i], camera, dirLight, ambLight);

	}


}


void Renderer::projectObject(Object* obj) {

	if (obj->getType() == ObjectType::Mesh || obj->getType() == ObjectType::InstancedMesh) {

		Mesh* mesh = (Mesh*)obj;
		auto material = mesh->mMaterial;
		if (material->mBlend) {

			mTransparentObjects.push_back(mesh);
		}
		else {

			mOpacityObjects.push_back(mesh);
		}
	}

	auto children = obj->getChildren();
	for (int i = 0; i < children.size(); i++) {

		projectObject(children[i]);
	}
}

Shader* Renderer::pickShader(MaterialType type) {

	Shader* result = nullptr;

	switch (type) {

	case MaterialType::PhongMaterial:
		result = mPhongShader;
		break;
	case MaterialType::WhiteMaterial:
		result = mWhiteShader;
		break;
	case MaterialType::DepthMaterial:
		result = mDepthShader;
		break;
	case MaterialType::OpacityMaskMaterial:
		result = mOpacityMaskShader;
		break;
	case MaterialType::ScreenMaterial:
		result = mScreenShader;
		break;
	case MaterialType::CubeMaterial:
		result = mCubeShader;
		break;
	case MaterialType::PhongEnvMaterial:
		result = mPhongEnvShader;
		break;
	case MaterialType::PhongInstanceMaterial:
		result = mPhongInstanceShader;
		break;
	case MaterialType::GrassInstanceMaterial:
		result = mGrassInstanceShader;
		break;
	default:
		std::cout << "Unkown material type to shader" << std::endl;
		break;
	}

	return result;
}

// Render single object 
void Renderer::renderObject(
	Object* object,
	Camera* camera,
	DirectionalLight* dirLight,
	AmbientLight* ambLight
) {

	// 1 Render only mesh
	if (object->getType() == ObjectType::Mesh || object->getType() == ObjectType::InstancedMesh) {

		// 1.1 It is a mesh
		auto mesh = (Mesh*)object;
		auto geometry = mesh->mGeometry;

		Material* material = nullptr;
		if (mGlobalMaterial != nullptr) {

			material = mGlobalMaterial;
		}
		else {

			material = mesh->mMaterial;
		}


		// 2 Set rendering status
		setDepthState(material);
		setPolygonOffsetState(material);
		setStencilState(material);
		setBlendState(material);
		setFaceCullingState(material);

		// 3.1 Choose shader
		Shader* shader = pickShader(material->mType);

		// 3.2 Update uniform
		// 3.2.1 Create program
		shader->begin();

		switch (material->mType) {

		case MaterialType::PhongMaterial: {
			// pointer type change
			PhongMaterial* phongMat = (PhongMaterial*)material;

			// Texture bind and sampling
			shader->setInt("sampler", 0);
			phongMat->mDiffuse->bind();

			//// Specular mask bind and sampling
			//shader->setInt("specularMaskSampler", 1);
			//phongMat->mSpecularMask->bind();

			// 3.2.3 MVP matrix
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// 3.2.3 Light
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->mDirection);
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);
			shader->setFloat("directionalLight.intensity", dirLight->mIntensity);

			shader->setFloat("shiness", phongMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);


			// 3.2.4 Camera
			shader->setVector3("cameraPosition", camera->mPosition);

			// 3.2.5 Opacity
			shader->setFloat("opacity", material->mOpacity);
		}
										break;
		case MaterialType::WhiteMaterial: {
			// MVP matrix
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
		}
										break;
		case MaterialType::DepthMaterial: {

			// MVP matrix
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			shader->setFloat("near", camera->mNear);
			shader->setFloat("far", camera->mFar);
		}
										break;
		case MaterialType::OpacityMaskMaterial: {

		    // pointer type change
			OpacityMaskMaterial* opacityMat = (OpacityMaskMaterial*)material;

			// Texture bind and sampling
			shader->setInt("sampler", 0);
			opacityMat->mDiffuse->bind();

			//// Specular mask bind and sampling
			shader->setInt("opacityMaskSampler", 1);
			opacityMat->mOpacityMask->bind();

			// 3.2.3 MVP matrix
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// 3.2.3 Light
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->mDirection);
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);

			shader->setFloat("shiness", opacityMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);


			// 3.2.4 Camera
			shader->setVector3("cameraPosition", camera->mPosition);

			// 3.2.5 Opacity
			shader->setFloat("opacity", material->mOpacity);
										}
										break;
		case MaterialType::ScreenMaterial: {

			ScreenMaterial* screenMat = (ScreenMaterial*)material;

			// Texture bind and sampling
			shader->setInt("screemTexSampler", 0);
			screenMat->mScreenTexture->bind();

			shader->setFloat("texWidth", 1600);
			shader->setFloat("texHeight", 1200);
		}
										break;
		case MaterialType::CubeMaterial: {

			CubeMaterial* cubeMat = (CubeMaterial*)material;
			mesh->setPosition(camera->mPosition);
			// MVP matrix
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			// Texture bind and sampling
			shader->setInt("cubeSampler", 0);
			cubeMat->mDiffuse->bind();
		}
										break;
		case MaterialType::PhongEnvMaterial: {
			// pointer type change
			PhongEnvMaterial* phongMat = (PhongEnvMaterial*)material;

			// Texture bind and sampling
			shader->setInt("sampler", 0);
			phongMat->mDiffuse->bind();

			shader->setInt("envSampler", 1);
			phongMat->mEnv->bind();

			// 3.2.3 MVP matrix
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// 3.2.3 Light
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->mDirection);
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);

			shader->setFloat("shiness", phongMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);


			// 3.2.4 Camera
			shader->setVector3("cameraPosition", camera->mPosition);

			// 3.2.5 Opacity
			shader->setFloat("opacity", material->mOpacity);

		}
										 break;
		case MaterialType::PhongInstanceMaterial: {
			// pointer type change
			PhongInstanceMaterial* phongMat = (PhongInstanceMaterial*)material;
			InstancedMesh* im = (InstancedMesh*)mesh;

			// Texture bind and sampling
			shader->setInt("sampler", 0);
			phongMat->mDiffuse->bind();

			//// Specular mask bind and sampling
			//shader->setInt("specularMaskSampler", 1);
			//phongMat->mSpecularMask->bind();

			// 3.2.3 MVP matrix
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// 3.2.3 Light
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->mDirection);
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);

			shader->setFloat("shiness", phongMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);


			// 3.2.4 Camera
			shader->setVector3("cameraPosition", camera->mPosition);

			// 3.2.5 Opacity
			shader->setFloat("opacity", material->mOpacity);
		}
									     break;
		case MaterialType::GrassInstanceMaterial: {
			
			// pointer type change
			GrassInstanceMaterial* grassMat = (GrassInstanceMaterial*)material;
			InstancedMesh* im = (InstancedMesh*)mesh;

			im->sortMatrices(camera->getViewMatrix());
			im->updateMatrices();

			// Texture bind and sampling
			shader->setInt("sampler", 0);
			grassMat->mDiffuse->bind();

			// Opacity mask
			shader->setInt("opacityMask", 1);
			grassMat->mOpacityMask->bind();

			shader->setInt("cloudMask", 2);
			grassMat->mCloudMask->bind();

			// 3.2.3 MVP matrix
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			//auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			//shader->setMatrix3x3("normalMatrix", normalMatrix);

			// 3.2.3 Light
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->mDirection);
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);

			shader->setFloat("shiness", grassMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);


			// 3.2.4 Camera
			shader->setVector3("cameraPosition", camera->mPosition);

			// 3.2.5 Opacity
			shader->setFloat("opacity", grassMat->mOpacity);
			shader->setFloat("uvScale", grassMat->mUVScale);
			shader->setFloat("brightness", grassMat->mBrightness);
			shader->setFloat("time", glfwGetTime());

			shader->setFloat("windScale", grassMat->mWindScale);
			shader->setFloat("phaseScale", grassMat->mPhaseScale);
			shader->setVector3("windDirection", grassMat->mWindDirection);

			shader->setVector3("cloudWhiteColor", grassMat->mCloudWhiteColor);
			shader->setVector3("cloudBlackColor", grassMat->mCloudBlackColor);
			shader->setFloat("cloudUVScale", grassMat->mCloudUVScale);
			shader->setFloat("cloudSpeed", grassMat->mCloudSpeed);
			shader->setFloat("cloudLerp", grassMat->mCloudLerp);
		}
												break;
		default:
			break;
		}

		// 3.3 VAO
		glBindVertexArray(geometry->getVao());

		// 3.4 Draw
		if (object->getType() == ObjectType::InstancedMesh) {

			InstancedMesh* im = (InstancedMesh*)mesh;
			glDrawElementsInstanced(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0, im->mInstanceCount);

		}
		else {

			glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);

		}
	}
}


void Renderer::setDepthState(Material* material) {

	if (material->mDepthTest) {

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(material->mDepthFunc);
	}
	else {

		glDisable(GL_DEPTH_TEST);
	}

	// 2.2 Depth write
	if (material->mDepthWrite) {

		glDepthMask(GL_TRUE);
	}
	else {

		glDepthMask(GL_FALSE);
	}

}

void Renderer::setPolygonOffsetState(Material* material) {

	if (material->mPolygonOffset) {

		glEnable(material->mPolygonOffsetType);
		glPolygonOffset(material->mFactor, material->mUnit);

	}
	else {
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_POLYGON_OFFSET_LINE);

	}

}

void Renderer::setStencilState(Material* material) {

	if (material->mStencilTest) {

		glEnable(GL_STENCIL_TEST);

		glStencilOp(material->mSFail, material->mZFail, material->mZPass);
		glStencilMask(material->mStencilMask);
		glStencilFunc(material->mStencilFunc, material->mStencilRef, material->mStencilFuncMask);

	}
	else {

		glDisable(GL_STENCIL_TEST);
	}
}

void Renderer::setBlendState(Material* material) {

	if (material->mBlend) {

		glEnable(GL_BLEND);
		glBlendFunc(material->mSFactor, material->mDFactor);
	}
	else {

		glDisable(GL_BLEND);
	}
}

void Renderer::setFaceCullingState(Material* material) {

	if (material->mFaceCulling) {

		glEnable(GL_CULL_FACE);
		glFrontFace(material->mFrontFace);
		glCullFace(material->mCullFace);

	}
	else {

		glDisable(GL_CULL_FACE);
	}
}