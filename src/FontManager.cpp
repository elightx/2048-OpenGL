//
// Created by 2163a on 04-02-2025.
//

#include "FontManager.h"

void add_text(vertex_buffer_t* buffer, texture_font_t* font, const std::string text, const ftgl::vec4* color, ftgl::vec2* pen)
{
    const float r = color->red, g = color->green, b = color->blue, a = color->alpha;
    for (size_t i = 0; i < text.size(); ++i)
    {
        ftgl::texture_glyph_t* glyph = ftgl::texture_font_get_glyph(font, &text[i]);
        if (glyph != nullptr)
        {
            float kerning = 0.0f;
            if (i > 0)
            {
                kerning = texture_glyph_get_kerning(glyph, &text[i - 1]);
            }
            pen->x += kerning;
            float x0 = (pen->x + glyph->offset_x);
            float y0 = (pen->y + glyph->offset_y);
            float x1 = (x0 + glyph->width);
            float y1 = (y0 - glyph->height);
            float s0 = glyph->s0;
            float t0 = glyph->t0;
            float s1 = glyph->s1;
            float t1 = glyph->t1;
            GLuint indices[6] = { 0,1,2, 0,2,3 };
            FontManager::vertex_t vertices[4] = { { x0,y0,0,  s0,t0,  r,g,b,a },
                                     { x0,y1,0,  s0,t1,  r,g,b,a },
                                     { x1,y1,0,  s1,t1,  r,g,b,a },
                                     { x1,y0,0,  s1,t0,  r,g,b,a } };
            ftgl::vertex_buffer_push_back(buffer, vertices, 4, indices, 6);
            pen->x += glyph->advance_x;
        }
    }
}

FontManager::FontManager() {}

FontManager::~FontManager()
{
    ftgl::vertex_buffer_delete(buffer);
    ftgl::texture_atlas_delete(atlas);
    for (auto& font : fonts)
    {
        ftgl::texture_font_delete(font.second);
    }
    glDeleteProgram(shader);
}

void FontManager::init()
{
    load_shader();
    load_buffer();
    load_atlas();
}

void FontManager::clear()
{
    vertex_buffer_clear(buffer);
}

void FontManager::load_shader()
{
    shader = shader_load((GL_SHADERS_PATH + std::string("v3f-t2f-c4f.vert")).c_str(), (GL_SHADERS_PATH + std::string("v3f-t2f-c4f.frag")).c_str());
}

void FontManager::load_buffer()
{
    buffer = ftgl::vertex_buffer_new("vertex:3f,tex_coord:2f,color:4f");
}

void FontManager::load_atlas()
{
    atlas = ftgl::texture_atlas_new(512, 512, 1);
    glGenTextures(1, &atlas->id);
    glBindTexture(GL_TEXTURE_2D, atlas->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}


texture_font_t*  FontManager::load_font(const std::string& fontName, int size)
{
    ftgl::texture_font_t* font = nullptr;
    font = ftgl::texture_font_new_from_file(atlas, size,  (GL_FONTS_PATH+fontName).c_str());
    fonts[{fontName, size}] = font;
    return font;
}

void FontManager::draw_string(const std::string& text, const std::string& fontName, int size, float x, float y, const GLfloat* color)
{
    vec2 pen = {{x,y}};
    vec4 color_v = { {color[0],color[1], color[2], color[3]} };
    texture_font_t* font = get_font(fontName, size);
    add_text(buffer, font, text, &color_v, &pen);
}
size_t FontManager::GetLineHeight(const std::string& fontName, const int size)
{
    texture_font_t* font = get_font(fontName, size);
    const char* p = "W";
    return texture_font_get_glyph(font, p)->width;

}

float FontManager::GetStringWidth(const std::string& text, const std::string& fontName, const int size)
{
    float res = 0;
    texture_font_t* font = get_font(fontName, size);
    for(size_t i = 0; i<text.size(); i++)
    {
        if(const texture_glyph_t* glyph = texture_font_get_glyph(font, &text[i]))
        {
            float kerning = 0.0f;
            if (i > 0)
            {
                kerning = texture_glyph_get_kerning(glyph, &text[i - 1]);
            }
            res += kerning;
            res += glyph->advance_x;
        }
    }
    return res;
}

void FontManager::render()
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas->width, atlas->height, 0, GL_RED, GL_UNSIGNED_BYTE, atlas->data);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, 0, model.data);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, 0, view.data);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, 0, projection.data);
    vertex_buffer_render(buffer, GL_TRIANGLES);
    mat4_set_identity(&projection);
    mat4_set_identity(&model);
    mat4_set_identity(&view);
}

void FontManager::reshape(int width, int height)
{
    mat4_set_orthographic(&projection, 0, width, 0, height, -1, 1);
}

texture_font_t* FontManager::get_font(std::string filename, int size)
{
    if (fonts.find({ filename, size }) == fonts.end())
    {
        return load_font(filename, size);
    } else
    {
        return fonts[{filename, size}];
    }
}

void FontManager::use_shader()
{
    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "texture"), 0);
}


