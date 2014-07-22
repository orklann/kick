//
// Created by morten on 30/06/14.
//


#pragma once
#include <string>
#include "kick/core/project_asset.h"
#include "kick/core/project.h"
#include "kick/core/event.h"


namespace kick {
    class Texture2D;
    class Shader;
    class Text;

    struct FontChar {
//        int id;
        int x;
        int y;
        int width;
        int height;
        int xoffset;
        int yoffset;
        int xadvance;
        int page;
        int chnl;
//        std::string letter;
    };

class Font : public ProjectAsset {
public:
    Font(Project *project);
    ~Font();
    bool loadFntFile(std::string filename = "assets/font/font-default.fnt", std::string texturename = "assets/font/font-default.png");
    int width(std::string text);
    int height();


    std::shared_ptr<Texture2D> getTexture() const;

    void setTexture(std::shared_ptr<Texture2D> texture);

    const FontChar getChar(char r);

    int getScaleW() const;
    int getScaleH() const;

    std::shared_ptr<Shader> getShader();

    void setShader(std::shared_ptr<Shader> shader);

    int getBase() const;

    friend class Text;

    bool IsInitialized() const;

    Event<Font*> fontListener;
private:
    std::shared_ptr<Texture2D> texture;
    int lineHeight = 0;
    int base = 0;
    int scaleW = 0;
    int scaleH = 0;
    int pages = 1;
    std::map<int, FontChar> fontMap;
    std::shared_ptr<Shader> shader;
};
}
