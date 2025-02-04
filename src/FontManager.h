//
// Created by 2163a on 04-02-2025.
//

#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <map>
#include <string>
#include "freetype-gl/freetype-gl.h"
#include "freetype-gl/texture-atlas.h"
#include "freetype-gl/texture-font.h"
#include "freetype-gl/vertex-buffer.h"
#include "shader.h"
#include "mat4.h"

class FontManager {

public:

    typedef struct {
        float x, y, z;    // position
        float s, t;       // texture
        float r, g, b, a; // color
    } vertex_t;

    FontManager();
    ~FontManager();
    void init();
    void clear();
    void load_shader();
    void load_buffer();
    void load_atlas();
    void render();
    void use_shader();
    void reshape(int width, int height);
    void draw_string(const std::string& text, const std::string& fontName, int size, float x, float y, const float* color);
    float GetStringWidth(const std::string& text, const std::string& fontName, const int size = 16);
    texture_font_t* load_font(const std::string& fontName, int size = 16);
    size_t GetLineHeight(const std::string& fontName, const int size = 16);
    texture_font_t* get_font(std::string filename, int size = 16);
private:
    mat4   model, view, projection;
    GLuint shader;
    texture_atlas_t* atlas;
    vertex_buffer_t* buffer;
    std::map<std::pair<std::string, int>, ftgl::texture_font_t*> fonts;


};



#endif //FONTMANAGER_H
