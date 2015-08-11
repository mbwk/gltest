#include "myshaders.hpp"

namespace mbwk {

const char *vertsrc_ = R"vert(
    #version 330 core

    in vec2 position;
    in vec3 color;

    out vec3 Color;

    void
    main()
    {
        Color = color;
        gl_Position = vec4(position, 0.0, 1.0);
    }
    )vert";

const char *fragsrc_ = R"frag(
    #version 330 core

    in vec3 Color;
    uniform float alpha;
    out vec4 outColor;

    void
    main()
    {
        outColor = vec4(Color, alpha);
    }
    )frag";

const char *vertsrc()
{
    return vertsrc_;
}

const char *fragsrc()
{
    return fragsrc_;
}

}

