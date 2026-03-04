#include <Zgine/Core/Math/Vector3.h>

// Backend selection - defaults to GLM
#if defined(ZGINE_MATH_DIRECTX)
    #include "Backend/DirectXMath/Vector3DX.cpp"
#else
    #ifndef ZGINE_MATH_GLM
        #define ZGINE_MATH_GLM
    #endif
    #include "Backend/GLM/Vector3GLM.cpp"
#endif
