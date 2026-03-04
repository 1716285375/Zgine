#include <Zgine/Core/Math/Vector2.h>

// Backend selection - defaults to GLM
#if defined(ZGINE_MATH_DIRECTX)
    #include "Backend/DirectXMath/Vector2DX.cpp"
#else
    #ifndef ZGINE_MATH_GLM
        #define ZGINE_MATH_GLM
    #endif
    #include "Backend/GLM/Vector2GLM.cpp"
#endif
