#include <Zgine/Core/Math/Math.h>

// Backend selection based on compile-time macros
// Default to GLM if no backend is specified
#if defined(ZGINE_MATH_DIRECTX)
    #include "Backend/DirectXMath/MathDX.cpp"
#else
    // ZGINE_MATH_GLM is the default backend
    #ifndef ZGINE_MATH_GLM
        #define ZGINE_MATH_GLM
    #endif
    #include "Backend/GLM/MathGLM.cpp"
#endif
