#include <iostream>
#include "glframework/core.h"
#include "glframework/shader.h"
#include <string>
#include <assert.h>
#include "wrapper/checkError.h"
#include "application/Application.h"
#include "glframework/texture.h"

#include "application/camera/perspectiveCamera.h"
#include "application/camera/orthographicCamera.h"
#include "application/camera/trackBallCameraControl.h"
#include "application/camera/gameCameraControl.h"


#include "glframework/geometry.h"
#include "glframework/material/phongMaterial.h"
#include "glframework/material/whiteMaterial.h"
#include "glframework/material/depthMaterial.h"
#include "glframework/material/opacityMaskMaterial.h"
#include "glframework//material/screenMaterial.h"
#include "glframework/material/cubeMaterial.h"
#include "glframework/material/phongEnvMaterial.h"
#include "glframework/material/phongInstanceMaterial.h"
#include "glframework/material/grassInstanceMaterial.h"

#include "glframework/mesh/mesh.h"
#include "glframework/mesh/instancedMesh.h"
#include "glframework/renderer/renderer.h"
#include "glframework/light/pointLight.h"
#include "glframework/light/spotLight.h"
#include "glframework/light/directionalLight.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glframework/scene.h"
#include "application/assimpLoader.h"
#include "application/assimpInstanceLoader.h"

#include "glframework/framebuffer/framebuffer.h"

Renderer* renderer = nullptr;
Scene* scene = nullptr;

Framebuffer* framebuffer = nullptr;

int WIDTH = 2560;
int HEIGHT = 1440;

GrassInstanceMaterial* grassMaterial = nullptr;

DirectionalLight* dirLight = nullptr;
AmbientLight* ambLight = nullptr;

Camera* camera = nullptr;
GameCameraControl* cameraControl = nullptr;

glm::vec3 clearColor{};

void OnResize(int width, int height) {

    GL_CALL(glViewport(0, 0, width, height));
}

void OnKey(int key, int action, int mods) {

    cameraControl->onKey(key, action, mods);
}

void OnMouse(int button, int action, int mods) {

    double x, y;
    glApp->getCursorPosition(&x, &y);
    cameraControl->onMouse(button, action, x, y);

}

void OnCursor(double xpos, double ypos) {

    cameraControl->onCursor(xpos, ypos);
}

void OnScroll(double offset) {

    cameraControl->onScroll(offset);
}

void setModelBlend(Object* obj, bool blend, float opacity) {

    if (obj->getType() == ObjectType::Mesh) {

        Mesh* mesh = (Mesh*)obj;
        Material* mat = mesh->mMaterial;
        mat->mBlend = blend;
        mat->mOpacity = opacity;
        mat->mDepthWrite = false;
    }

    auto children = obj->getChildren();

    for (int i = 0; i < children.size(); i++) {

        setModelBlend(children[i], blend, opacity);
    }

}

void setInstanceMatrix(Object* obj, int index, glm::mat4 matrix) {

    if (obj->getType() == ObjectType::InstancedMesh) {

        InstancedMesh* im = (InstancedMesh*)obj;
        im->mInstanceMatrices[index] = matrix;

    }

    auto children = obj->getChildren();
    for (int i = 0; i < children.size(); i++) {

        setInstanceMatrix(children[i], index, matrix);
    }
}

void updateInstanceMatrix(Object* obj) {

    if (obj->getType() == ObjectType::InstancedMesh) {

        InstancedMesh* im = (InstancedMesh*)obj;
        im->updateMatrices();

    }

    auto children = obj->getChildren();
    for (int i = 0; i < children.size(); i++) {

        updateInstanceMatrix(children[i]);
    }
}

void setInstanceMaterial(Object* obj, Material* material) {

    if (obj->getType() == ObjectType::InstancedMesh) {

        InstancedMesh* im = (InstancedMesh*)obj;
        im->mMaterial = material;

    }

    auto children = obj->getChildren();
    for (int i = 0; i < children.size(); i++) {

        setInstanceMaterial(children[i], material);
    }
}

void prepare() {

    renderer = new Renderer();
    scene= new Scene();

    // 1 Cubemap
    auto sphereGeo = Geometry::createSphere(1.0f);
    auto sphereMat = new CubeMaterial();
    sphereMat->mDiffuse = new Texture("assets/textures/bk.jpg", 0);
    auto sphereMesh = new Mesh(sphereGeo, sphereMat);
    scene->addChild(sphereMesh);

    // 2 Grass
    int rNum = 300;
    int cNum = 300;

    auto grassModel = AssimpInstanceLoader::load("assets/fbx/grassNew.obj", rNum * cNum);

    glm::mat4 translate;
    glm::mat4 rotate;
    glm::mat4 transform;


    srand(glfwGetTime());
    for (int r = 0; r < rNum; r++) {

        for (int c = 0; c < cNum; c++) {
            
            // 1 translate
            translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f * r, 0.0f, 0.2f * c));

            // 2 rotate
            rotate = glm::rotate(glm::radians((float)(rand() % 90)), glm::vec3(0.0, 1.0, 0.0));

            transform = translate * rotate;

            setInstanceMatrix(grassModel, r * cNum + c, transform);

        }
    }
    updateInstanceMatrix(grassModel);

    grassMaterial = new GrassInstanceMaterial();
    grassMaterial->mDiffuse = new Texture("assets/textures/GRASS.png", 0);
    grassMaterial->mOpacityMask = new Texture("assets/textures/grassMask.png", 1);
    grassMaterial->mCloudMask = new Texture("assets/textures/CLOUD.png", 2);
    //grassMaterial->mBlend = true;
    //grassMaterial->mDepthWrite = false;
    setInstanceMaterial(grassModel, grassMaterial);
    scene->addChild(grassModel);
    
    // 3 House
    auto house = AssimpLoader::load("assets/fbx/house.fbx");
    house->setScale(glm::vec3(0.5f));
    house->setPosition(glm::vec3(rNum * 0.2f / 2.0f, 0.4, cNum * 0.2f / 2.0f));
    scene->addChild(house);

    // 4 Create light
    dirLight = new DirectionalLight();
    dirLight->mDirection = glm::vec3(-1.0f);
    dirLight->mSpecularIntensity = 0.1f;

    ambLight = new AmbientLight();
    ambLight->mColor = glm::vec3(0.1f);
}

void prepareCamera() {

    camera = new PerspectiveCamera(
        60.0f,
        (float)glApp->getWidth() / glApp->getHeight(),
        0.1f,
        1000.0f);

    cameraControl = new GameCameraControl();
    cameraControl->setCamera(camera);
    cameraControl->setSensitivity(0.4f);
    cameraControl->setSpeed(0.1f);
}

void initIMGUI() {

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(glApp->getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void renderIMGUI() {

    // 1 Initial
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 2 GUI widget
    ImGui::Begin("GrassMaterialEditor");

    // 2.1 Color
    ImGui::Text("GrassColor");
    ImGui::SliderFloat("UVScale", &grassMaterial->mUVScale, 0.0f, 100.0f);
    ImGui::InputFloat("Brightness", &grassMaterial->mBrightness);

    // 2.2 Wind
    ImGui::Text("Wind");
    ImGui::InputFloat("WindScale", &grassMaterial->mWindScale);
    ImGui::InputFloat("PhaseScale", &grassMaterial->mPhaseScale);
    ImGui::ColorEdit3("WindDirection", (float*) &grassMaterial->mWindDirection);

    // 2.3 Cloud
    ImGui::Text("Cloud");
    ImGui::ColorEdit3("CloudWhiteColor", (float*)&grassMaterial->mCloudWhiteColor);
    ImGui::ColorEdit3("CloudBlackColor", (float*)&grassMaterial->mCloudBlackColor);
    ImGui::SliderFloat("CloudUVScale", &grassMaterial->mCloudUVScale, 0.0f, 100.0f);
    ImGui::InputFloat("CloudSpeed", &grassMaterial->mCloudSpeed);
    ImGui::SliderFloat("CloudLerp", &grassMaterial->mCloudLerp, 0.0f, 1.0f);


    // 2.4 Light
    ImGui::Text("Light");
    ImGui::InputFloat("Intensity", &dirLight->mIntensity);

    ImGui::End();

    // 3 Render
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(glApp->getWindow(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main() {

    // 1 Initial the window
    if (!glApp->init(WIDTH, HEIGHT)) {
        return -1;
    }

    // 2 Size and keyboard callback
    glApp->setResizeCallback(OnResize);
    glApp->setKeyBoardCallback(OnKey);
    glApp->setMouseCallback(OnMouse);
    glApp->setCursorCallback(OnCursor);
    glApp->setScrollCallback(OnScroll);

    // 3 Set openGl rendering viewport and clear canvas color
    GL_CALL(glViewport(0, 0, WIDTH, HEIGHT));
    GL_CALL(glClearColor(0.0f, 0.18f, 0.65f, 1.0f));

    prepareCamera();
    prepare();
    initIMGUI();

    // 4 Set window loop
    while (glApp->update()) {

        cameraControl->update();
        renderer->setClearColor(clearColor);

        // Pass 1
        renderer->render(scene, camera, dirLight, ambLight);

        renderIMGUI();
    }

    glApp->destroy();

    return 0;
}