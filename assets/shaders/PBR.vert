#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform mat3 u_NormalMatrix;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoords;
out mat3 v_TBN;

void main()
{
    v_FragPos = vec3(u_Transform * vec4(a_Position, 1.0));
    v_TexCoords = a_TexCoords;

    // 计算 TBN 矩阵（用于法线贴图）
    vec3 N = normalize(mat3(u_NormalMatrix) * a_Normal);
    vec3 T = normalize(mat3(u_NormalMatrix) * a_Tangent);
    T = normalize(T - dot(T, N) * N); // 重新正交化
    vec3 B = cross(N, T);
    v_TBN = mat3(T, B, N);

    v_Normal = N;

    gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);
}

