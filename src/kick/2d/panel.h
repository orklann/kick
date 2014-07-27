//
// Created by morten on 26/07/14.
//


#pragma once

#include "kick/scene/component_renderable.h"
#include "kick/scene/camera.h"
#include "kick/2d/component2d.h"
#include "kick/texture/texture_atlas.h"
#include <vector>

namespace kick {
    class Camera;
    class Button;
    class Sprite;
    class Text;
    class Mesh;
    class MeshData;

    class Panel : public ComponentRenderable {
    public:
        Panel(GameObject *gameObject);
        virtual ~Panel();
        virtual void activated() override;

        virtual void deactivated() override;

        virtual void render(EngineUniforms *engineUniforms);

        virtual int getRenderOrder();

        Camera * getCamera() const;
        void setCamera(Camera *camera);

        //
        Button* createButton();
        // helper function, which creates a gameobject and attaches an sprite object
        Sprite* createSprite(std::shared_ptr<TextureAtlas> textureAtlas, std::string spriteName, glm::vec2 pos = glm::vec2{0});
        //
        Text *createText(std::string text);

    private:
        void updateVertexBuffer(std::vector<Sprite *> &sprites);
        void renderSprites(std::vector<Sprite*> &sprites, kick::EngineUniforms *engineUniforms);
        friend class Component2D;
        void updateRenderOrder(Component2D* comp);
        void registerComponent2D(Component2D* comp);
        void deregisterComponent2D(Component2D* comp);
        Camera* camera = nullptr;
        std::vector<Component2D*> components;
        Mesh *mesh = nullptr;
        MeshData* meshData = nullptr;
        Material* material = nullptr;
    };
}