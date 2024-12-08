#pragma once

#include <memory>
#include "Image.h"

class Texture
{
public:
    enum WrapModes { Repeat, MirroredRepeat, Clamp };
    WrapModes wrapMode = Repeat;

    enum Filters { Nearest, Linear };
    Filters filter = Nearest;

private:
    std::unique_ptr<Image> _img_ptr;

public:
    unsigned int id() const { return _img_ptr ? _img_ptr->id() : 0; }
    void bind() const;
    void setImage(Image* img_ptr) { _img_ptr.reset(img_ptr); }
    Image* image() const { return _img_ptr.get(); }
};