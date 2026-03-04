#include <Zgine/Core/Math/Matrix4.h>

// Backend selection - defaults to GLM
#if defined(ZGINE_MATH_DIRECTX)
    #include "Backend/DirectXMath/MatrixDX.cpp"
#else
    #ifndef ZGINE_MATH_GLM
        #define ZGINE_MATH_GLM
    #endif
    #include "Backend/GLM/Matrix4GLM.cpp"
#endif
