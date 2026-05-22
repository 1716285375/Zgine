# Google Test
find_package(GTest REQUIRED)

# Editor单元测试
add_executable(ZgineEditorTests
    EditorContextTests.cpp
    EditorEventBusTests.cpp
    EditorCommandTests.cpp
    SelectionContextTests.cpp
)

# 链接到编辑器核心库，保持 Runtime 不依赖 Editor
target_link_libraries(ZgineEditorTests PRIVATE
    ZgineEditorCore
    GTest::GTest
    GTest::Main
)

# 包含目录 - 需要访问Editor头文件
target_include_directories(ZgineEditorTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# C++标准
set_target_properties(ZgineEditorTests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# 添加到CTest
include(GoogleTest)
gtest_discover_tests(ZgineEditorTests)
