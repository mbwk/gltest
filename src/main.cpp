#include "glcxt.hpp"

#include <iostream>

int
main()
{
    std::cout << "こん\n";

    auto prog = mbwk::GlContext();
    prog.run();

    return 0;
}

