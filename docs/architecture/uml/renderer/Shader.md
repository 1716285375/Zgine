# Shader 类 UML 图

## Shader 类详细结构

```mermaid
classDiagram
    class Shader {
        -uint32_t m_RendererID
        +Shader(string&, string&)
        +~Shader()
        +Bind()
        +Unbind()
        +GetRendererID() uint32_t
        +UploadUniformFloat(string&, float)
        +UploadUniformFloat2(string&, vec2&)
        +UploadUniformFloat3(string&, vec3&)
        +UploadUniformFloat4(string&, vec4&)
        +UploadUniformInt(string&, int)
        +UploadUniformInt2(string&, ivec2&)
        +UploadUniformInt3(string&, ivec3&)
        +UploadUniformInt4(string&, ivec4&)
        +UploadUniformIntArray(string&, int*, uint32_t)
        +UploadUniformMat3(string&, mat3&)
        +UploadUniformMat4(string&, mat4&)
        -GetUniformLocation(string&) int
    }
```

## Shader 类关系图

```mermaid
classDiagram
    class Shader {
        -uint32_t m_RendererID
        +Shader(string&, string&)
        +~Shader()
        +Bind()
        +Unbind()
        +GetRendererID() uint32_t
        +UploadUniformFloat(string&, float)
        +UploadUniformFloat2(string&, vec2&)
        +UploadUniformFloat3(string&, vec3&)
        +UploadUniformFloat4(string&, vec4&)
        +UploadUniformInt(string&, int)
        +UploadUniformInt2(string&, ivec2&)
        +UploadUniformInt3(string&, ivec3&)
        +UploadUniformInt4(string&, ivec4&)
        +UploadUniformIntArray(string&, int*, uint32_t)
        +UploadUniformMat3(string&, mat3&)
        +UploadUniformMat4(string&, mat4&)
        -GetUniformLocation(string&) int
    }

    class Renderer {
        -static SceneData* m_SceneData
        +Init()
        +Shutdown()
        +BeginScene(OrthographicCamera&)
        +BeginScene(PerspectiveCamera&)
        +EndScene()
        +Submit(Ref~Shader~, Ref~VertexArray~)
        +GetAPI() RendererAPI::API
    }

    class BatchRenderer2D {
        -static uint32_t s_MaxQuads
        -static uint32_t s_MaxVertices
        -static uint32_t s_MaxIndices
        -static Ref~VertexArray~ s_QuadVertexArray
        -static Ref~VertexBuffer~ s_QuadVertexBuffer
        -static Ref~Shader~ s_TextureShader
        -static Ref~Texture2D~ s_WhiteTexture
        -static uint32_t s_QuadIndexCount
        -static QuadVertex* s_QuadVertexBufferBase
        -static QuadVertex* s_QuadVertexBufferPtr
        -static array~Ref~Texture2D~~ s_TextureSlots
        -static uint32_t s_TextureSlotIndex
        -static RenderStats s_Stats
        -static bool s_Initialized
        +Init()
        +Shutdown()
        +BeginScene(OrthographicCamera&)
        +EndScene()
        +Flush()
        +DrawQuad(vec2&, vec2&, vec4&)
        +DrawQuad(vec3&, vec2&, vec4&)
        +DrawRotatedQuad(vec2&, vec2&, float, vec4&)
        +DrawLine(vec3&, vec3&, vec4&, float)
        +DrawCircle(vec3&, float, vec4&, float, float)
        +GetStats() RenderStats
        +ResetStats()
    }

    class BatchRenderer3D {
        -static uint32_t s_MaxCubes
        -static uint32_t s_MaxVertices
        -static uint32_t s_MaxIndices
        -static Ref~VertexArray~ s_CubeVertexArray
        -static Ref~VertexBuffer~ s_CubeVertexBuffer
        -static Ref~Shader~ s_TextureShader
        -static uint32_t s_CubeIndexCount
        -static CubeVertex* s_CubeVertexBufferBase
        -static CubeVertex* s_CubeVertexBufferPtr
        -static RenderStats s_Stats
        -static bool s_Initialized
        +Init()
        +Shutdown()
        +BeginScene(PerspectiveCamera&)
        +EndScene()
        +Flush()
        +DrawCube(vec3&, vec3&, vec4&)
        +DrawRotatedCube(vec3&, vec3&, vec3&, vec4&)
        +GetStats() RenderStats
        +ResetStats()
    }

    Renderer --> Shader : uses
    BatchRenderer2D --> Shader : uses
    BatchRenderer3D --> Shader : uses
```

## Shader 编译流程图

```mermaid
sequenceDiagram
    participant App as Application
    participant Sh as Shader
    participant GL as OpenGL

    App->>Sh: Shader(vertexSrc, fragmentSrc)
    Sh->>GL: glCreateShader(GL_VERTEX_SHADER)
    Sh->>GL: glShaderSource(vertexShader, vertexSrc)
    Sh->>GL: glCompileShader(vertexShader)
    
    alt Compilation Failed
        GL-->>Sh: Compilation Error
        Sh->>GL: glGetShaderInfoLog()
        Sh->>Sh: Log Error Message
    end
    
    Sh->>GL: glCreateShader(GL_FRAGMENT_SHADER)
    Sh->>GL: glShaderSource(fragmentShader, fragmentSrc)
    Sh->>GL: glCompileShader(fragmentShader)
    
    alt Compilation Failed
        GL-->>Sh: Compilation Error
        Sh->>GL: glGetShaderInfoLog()
        Sh->>Sh: Log Error Message
    end
    
    Sh->>GL: glCreateProgram()
    Sh->>GL: glAttachShader(program, vertexShader)
    Sh->>GL: glAttachShader(program, fragmentShader)
    Sh->>GL: glLinkProgram(program)
    
    alt Linking Failed
        GL-->>Sh: Linking Error
        Sh->>GL: glGetProgramInfoLog()
        Sh->>Sh: Log Error Message
    end
    
    Sh->>GL: glDeleteShader(vertexShader)
    Sh->>GL: glDeleteShader(fragmentShader)
    
    App->>Sh: Bind()
    Sh->>GL: glUseProgram(program)
```

## Shader Uniform 上传流程图

```mermaid
sequenceDiagram
    participant App as Application
    participant Sh as Shader
    participant GL as OpenGL

    App->>Sh: UploadUniformMat4("u_ViewProjection", matrix)
    Sh->>Sh: GetUniformLocation("u_ViewProjection")
    Sh->>GL: glGetUniformLocation(program, "u_ViewProjection")
    GL-->>Sh: uniformLocation
    
    alt Uniform Not Found
        Sh->>Sh: Log Warning
    else Uniform Found
        Sh->>GL: glUniformMatrix4fv(location, 1, GL_FALSE, matrix)
    end
    
    App->>Sh: UploadUniformFloat4("u_Color", color)
    Sh->>Sh: GetUniformLocation("u_Color")
    Sh->>GL: glGetUniformLocation(program, "u_Color")
    GL-->>Sh: uniformLocation
    
    alt Uniform Not Found
        Sh->>Sh: Log Warning
    else Uniform Found
        Sh->>GL: glUniform4f(location, color.r, color.g, color.b, color.a)
    end
```

## Shader 类说明

### 职责
- **着色器管理**: 管理GPU着色器程序的创建和销毁
- **编译链接**: 编译顶点和片段着色器并链接成程序
- **Uniform上传**: 提供类型安全的uniform变量上传接口
- **状态管理**: 管理着色器程序的绑定和解绑

### 设计模式
- **RAII模式**: 自动管理OpenGL资源生命周期
- **类型安全**: 提供类型安全的uniform上传接口
- **错误处理**: 内置编译和链接错误检查

### 支持的Uniform类型
- **标量**: float, int
- **向量**: vec2, vec3, vec4, ivec2, ivec3, ivec4
- **矩阵**: mat3, mat4
- **数组**: int数组

### 着色器类型
- **顶点着色器**: 处理顶点变换和属性
- **片段着色器**: 处理像素颜色和光照
- **几何着色器**: 支持几何图元生成 (预留)
- **计算着色器**: 支持通用计算 (预留)

### 关键特性
- **自动编译**: 自动编译和链接着色器程序
- **错误检查**: 完整的编译和链接错误检查
- **类型安全**: 类型安全的uniform上传接口
- **性能优化**: 缓存uniform位置避免重复查询
- **资源管理**: 自动清理OpenGL资源
- **调试支持**: 详细的错误日志和调试信息

### 使用示例
```cpp
// 创建着色器
auto shader = CreateRef<Shader>(vertexSource, fragmentSource);

// 绑定着色器
shader->Bind();

// 上传uniform变量
shader->UploadUniformMat4("u_ViewProjection", viewProjectionMatrix);
shader->UploadUniformFloat4("u_Color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
shader->UploadUniformInt("u_Texture", 0);

// 解绑着色器
shader->Unbind();
```
