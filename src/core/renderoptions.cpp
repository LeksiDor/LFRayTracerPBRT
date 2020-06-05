#include "renderoptions.h"

namespace pbrt
{


RenderOptions& theRenderOptions()
{
    static RenderOptions options;
    return options;
}


} // namespace pbrt