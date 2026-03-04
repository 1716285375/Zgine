#include <Zgine/Core/Math/Matrix3.h>

// Backend selection - defaults to GLM
#if defined(ZGINE_MATH_DIRECTX)
    #include "Backend/DirectXMath/Matrix3DX.cpp"
#else
    #ifndef ZGINE_MATH_GLM
        #define ZGINE_MATH_GLM
    #endif
    #include "Backend/GLM/Matrix3GLM.cpp"
#endif
