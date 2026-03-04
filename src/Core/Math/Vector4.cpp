#include <Zgine/Core/Math/Vector4.h>

// Backend selection - defaults to GLM
#if defined(ZGINE_MATH_DIRECTX)
    #include "Backend/DirectXMath/Vector4DX.cpp"
#else
    #ifndef ZGINE_MATH_GLM
        #define ZGINE_MATH_GLM
    #endif
    #include "Backend/GLM/Vector4GLM.cpp"
#endif
