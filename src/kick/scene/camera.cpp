//
//  camera.cpp
//  KickCPP
//
//  Created by morten on 8/12/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//

#include "kick/scene/camera.h"
#include <iostream>
#include "kick/core/kickgl.h"
#include <glm/ext.hpp>
#include <cmath>
#include <algorithm>
#include "kick/scene/game_object.h"
#include "kick/scene/transform.h"
#include "kick/scene/scene.h"
#include "kick/scene/component_renderable.h"
#include "kick/texture/texture_render_target.h"
#include "kick/core/engine.h"
#include "kick/scene/light.h"

using namespace std;
using namespace glm;

namespace kick {
    
    Camera::Camera(GameObject *gameObject)
    :Component(gameObject), clearFlag(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT),
     projectionMatrix{1}{

    }

    Camera::~Camera() {
        destroyShadowMap();
    }
    
    void Camera::activated(){
        Scene* scene = gameObject->getScene();
        componentListener = scene->componentEvents.createListener([&](pair<Component*, ComponentUpdateStatus> value){
            if (value.second == ComponentUpdateStatus::Created){
                auto r = includeComponent(value.first);
                if (r){
                    renderableComponents.push_back(r);
                }
            } else if (value.second == ComponentUpdateStatus::Destroyed){
                auto r = includeComponent(value.first);
                if (r) {
                    auto iter = find(renderableComponents.begin(), renderableComponents.end(), value.first);
                    if (iter != renderableComponents.end()) {
                        renderableComponents.erase(iter);
                    }
                }
            }
        });
        rebuildComponentList();
    }
    
    void Camera::rebuildComponentList(){
        renderableComponents.clear();
        for (auto & gameObject : *gameObject->getScene()) {
            for (auto & component :  *gameObject){
                auto r = includeComponent(component);
                if (r){
                    renderableComponents.push_back(r);
                }
            }
        }
    }
    
    ComponentRenderable *Camera::includeComponent(Component* component){
        auto r = dynamic_cast<ComponentRenderable*>(component);
        if (!r){
            return nullptr;
        }
        if (dynamic_cast<Camera*>(r)){
            return nullptr;
        }
        // todo - filter
        return r;
    }
    
    void Camera::deactivated(){
        // remove listener
        componentListener = {};
    }
    
    glm::vec4 Camera::getClearColor(){
        return clearColor;
    }
    
    void Camera::setClearColor(glm::vec4 clearColor){
        this->clearColor = clearColor;
    }
    
    void Camera::setupViewport(vec2 &offset, vec2 &dim){
        offset = round(offset);
        dim = round(dim);
        int offsetX = (int)offset.x;
        int offsetY = (int)offset.y;
        int width = (int)dim.x;
        int height = (int)dim.y;
        glViewport(offsetX, offsetY, width, height);
        glScissor(offsetX, offsetY, width, height);
    }
    
    void Camera::setupCamera(EngineUniforms *engineUniforms) {
        vec2 viewportDimension = (vec2)/*renderTarget? renderTarget.dimension : */ engineUniforms->viewportDimension.getValue();
        vec2 offset = viewportDimension * normalizedViewportOffset;
        vec2 dim = viewportDimension * normalizedViewportDim;
        setupViewport(offset, dim);
        if (clearFlag) {
            if (isClearColor()) {
                glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
            }
            glClear(clearFlag);
        }

        engineUniforms->viewMatrix = gameObject->getTransform()->getGlobalTRSInverse();
        engineUniforms->viewProjectionMatrix = projectionMatrix * engineUniforms->viewMatrix;
        engineUniforms->projectionMatrix = projectionMatrix;
    }

    void Camera::renderShadowMap(Light* directionalLight){

    }
    
    void Camera::render(EngineUniforms *engineUniforms){
        auto sceneLights = engineUniforms->sceneLights;
        if (shadow && sceneLights->directionalLight && sceneLights->directionalLight->getShadowType() != ShadowType::None) {
            renderShadowMap(sceneLights->directionalLight);
        }

        if (target){
            target->bind();
        }
        setupCamera(engineUniforms);
        engineUniforms->sceneLights->recomputeLight(engineUniforms->viewMatrix);
        sort(renderableComponents.begin(), renderableComponents.end(), [](ComponentRenderable* r1, ComponentRenderable* r2){
            return r1->getRenderOrder() < r2->getRenderOrder();
        });
        for (auto c : renderableComponents){
            if (c->getGameObject()->getLayer() & cullingMask) {
                c->render(engineUniforms);
            }
        }
        if (target){
            target->unbind();
        }
    }
    
    void Camera::setClearColor(bool clear){
        if (clear){
            clearFlag |= GL_COLOR_BUFFER_BIT;
        } else {
            clearFlag &= ~GL_COLOR_BUFFER_BIT;
        }
    }
    
    bool Camera::isClearColor(){
        return (bool) (clearFlag & GL_COLOR_BUFFER_BIT);
    }
    
    void Camera::setClearDepth(bool clear){
        if (clear){
            clearFlag |= GL_DEPTH_BUFFER_BIT;
        } else {
            clearFlag &= ~GL_DEPTH_BUFFER_BIT;
        }
    }
    
    bool Camera::isClearDepth(){
        return (bool) (clearFlag & GL_DEPTH_BUFFER_BIT);
    }
    
    void Camera::setClearStencil(bool clear){
        if (clear){
            clearFlag |= GL_STENCIL_BUFFER_BIT;
        } else {
            clearFlag &= ~GL_STENCIL_BUFFER_BIT;
        }
    }
    
    bool Camera::isClearStencil(){
        return (bool) (clearFlag & GL_STENCIL_BUFFER_BIT);
    }

    glm::mat4 Camera::getProjectionMatrix() {
        return projectionMatrix;
    }

    void Camera::setProjectionMatrix(glm::mat4 projectionMatrix) {
        this->projectionMatrix = projectionMatrix;
    }

    void Camera::resetProjectionMatrix(){
    }

    bool Camera::isShadow() const {
        return shadow;
    }

    void Camera::setShadow(bool renderShadow) {
        if (Camera::shadow != renderShadow) {
            Camera::shadow = renderShadow;
            if (renderShadow){
                initShadowMap();
            } else {
                destroyShadowMap();
            }
        }
    }

    void Camera::initShadowMap() {
        shadowMapShader = Project::loadShader("assets/shaders/__shadowmap.shader");
        shadowMapMaterial = Project::createAsset<Material>();
        shadowMapMaterial->setShader(shadowMapShader);
    }

    void Camera::destroyShadowMap() {
    }

    int Camera::getCullingMask() const {
        return cullingMask;
    }

    void Camera::setCullingMask(int cullingMask) {
        Camera::cullingMask = cullingMask;
    }

    TextureRenderTarget *Camera::getTarget() const {
        return target;
    }

    void Camera::setTarget(TextureRenderTarget *target) {
        Camera::target = target;
    }
}