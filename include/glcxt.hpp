#ifndef GLCXT_HPP
#define GLCXT_HPP

/* GL Context */

namespace mbwk {

/**
 * GlContext - class that encapsulates GLFW code.
 * Uses static variables and methods in place of
 * private fields and methods.
 */
class GlContext
{
public:
    GlContext();
    ~GlContext();
    auto run() -> void;
};

}

#endif // GLCXT_HPP

