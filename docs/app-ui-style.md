 **“水墨编辑器” ImGui 完整配色方案**：核心思路是——**宣纸底 + 墨色层级 + 极少量印泥点缀色**。这样既像水墨，又能在编辑器里保持可读性和工程感。

下面我给你两套主题（亮/暗），并把每个关键 ImGui 颜色槽位都配齐，外加一些水墨 UI 的“手感规则”。

---

## 水墨设计原则（决定你看起来像不像“水墨”）

* **底色不是纯白/纯黑**：用“宣纸白”“墨夜蓝灰”，眼睛更舒服。
* **墨色要分层**：文字、线框、控件边界分别用不同深度的墨。
* **点缀色极少**：只用于选中、高亮、警告。像“印泥”一样点一下就够。
* **边框比填充更重要**：水墨是“线与留白”的艺术；控件填充要淡。

---

## 主题 A：宣纸亮色（Light / Xuan Paper）

适合白天、文档感强、很“国风”。

### 基础色板

* `Paper` 宣纸底：`#F6F1E6`
* `Paper2` 次底：`#EFE7D8`
* `Ink1` 主墨（文字/重点）：`#1B1B1B`
* `Ink2` 次墨（普通线/次文字）：`#343434`
* `Ink3` 淡墨（边框/分割线）：`#6E6A62`
* `Wash` 水洗灰（控件填充）：`#DAD1C3`
* `SealRed` 印泥红（关键高亮）：`#B23A2F`
* `Indigo` 靛青（次高亮/链接）：`#2F4FA8`
* `BambooGreen` 竹绿（成功/运行）：`#2E7D32`
* `Ochre` 赭石（警告）：`#B7791F`

---

## 主题 B：墨夜暗色（Dark / Ink Night）

适合长时间开发，强调沉浸、低眩光。

### 基础色板

* `InkPaper` 墨夜底：`#121417`
* `InkPaper2` 次底：`#181B20`
* `InkText` 亮墨字：`#EDE7DD`
* `InkText2` 次字：`#CFC7BA`
* `InkLine` 线框：`#5B5A58`
* `InkWash` 控件填充：`#2A2D33`
* `SealRed` 印泥红：`#D05A4F`
* `Indigo` 靛青：`#6E8BFF`
* `BambooGreen` 竹绿：`#6BCB77`
* `Ochre` 赭石：`#D3A14A`

---

## ImGui 完备颜色映射（两套都给你）

下面是“槽位级别”的配色。你照着把 `ImGuiCol_*` 填进去就能用。

### 1 Light（宣纸亮色）— ImGuiCol 映射

* `Text`：Ink1 `#1B1B1B`

* `TextDisabled`：Ink3 `#6E6A62`

* `WindowBg`：Paper `#F6F1E6`

* `ChildBg`：Paper `#F6F1E6`（可透明一点）

* `PopupBg`：Paper2 `#EFE7D8`

* `Border`：Ink3 `#6E6A62`（alpha 0.35~0.55）

* `BorderShadow`：透明

* `FrameBg`：Wash `#DAD1C3`（alpha 0.35）

* `FrameBgHovered`：Wash（alpha 0.55）

* `FrameBgActive`：Wash（alpha 0.75）

* `TitleBg`：Paper2 `#EFE7D8`

* `TitleBgActive`：Paper2（稍深，alpha 1）

* `TitleBgCollapsed`：Paper2（alpha 0.75）

* `MenuBarBg`：Paper2（alpha 1）

* `ScrollbarBg`：Paper2（alpha 0.55）

* `ScrollbarGrab`：Ink3（alpha 0.35）

* `ScrollbarGrabHovered`：Ink3（alpha 0.55）

* `ScrollbarGrabActive`：Ink3（alpha 0.75）

* `CheckMark`：SealRed `#B23A2F`

* `SliderGrab`：Ink2 `#343434`（alpha 0.55）

* `SliderGrabActive`：SealRed（alpha 0.9）

* `Button`：Wash（alpha 0.35）

* `ButtonHovered`：Wash（alpha 0.55）

* `ButtonActive`：Wash（alpha 0.75）

* `Header`（树节点/可折叠）：Wash（alpha 0.35）

* `HeaderHovered`：Indigo `#2F4FA8`（alpha 0.18）

* `HeaderActive`：Indigo（alpha 0.28）

* `Separator`：Ink3（alpha 0.35）

* `SeparatorHovered`：Ink2（alpha 0.55）

* `SeparatorActive`：Ink2（alpha 0.75）

* `ResizeGrip`：Ink3（alpha 0.25）

* `ResizeGripHovered`：Ink3（alpha 0.45）

* `ResizeGripActive`：Ink3（alpha 0.65）

* `Tab`：Paper2（alpha 0.75）

* `TabHovered`：Indigo（alpha 0.18）

* `TabActive`：Paper2（alpha 1）

* `TabUnfocused`：Paper2（alpha 0.55）

* `TabUnfocusedActive`：Paper2（alpha 0.75）

* `PlotLines`：Ink2

* `PlotLinesHovered`：SealRed

* `PlotHistogram`：Ochre

* `PlotHistogramHovered`：SealRed

* `TextSelectedBg`：Indigo（alpha 0.20）

* `DragDropTarget`：SealRed（alpha 0.65）

* `NavHighlight`：Indigo（alpha 0.35）

* `NavWindowingHighlight`：Indigo（alpha 0.20）

* `NavWindowingDimBg`：Ink1（alpha 0.08）

* `ModalWindowDimBg`：Ink1（alpha 0.10）

---

### 2 Dark（墨夜暗色）— ImGuiCol 映射

* `Text`：InkText `#EDE7DD`

* `TextDisabled`：InkText2（alpha 0.65）`#CFC7BA`

* `WindowBg`：InkPaper `#121417`

* `ChildBg`：InkPaper（alpha 1）

* `PopupBg`：InkPaper2 `#181B20`

* `Border`：InkLine `#5B5A58`（alpha 0.35~0.55）

* `BorderShadow`：透明

* `FrameBg`：InkWash `#2A2D33`（alpha 0.55）

* `FrameBgHovered`：InkWash（alpha 0.75）

* `FrameBgActive`：InkWash（alpha 0.95）

* `TitleBg`：InkPaper2

* `TitleBgActive`：InkPaper2（更亮一点，alpha 1）

* `TitleBgCollapsed`：InkPaper2（alpha 0.65）

* `MenuBarBg`：InkPaper2

* `ScrollbarBg`：InkPaper2（alpha 0.55）

* `ScrollbarGrab`：InkLine（alpha 0.35）

* `ScrollbarGrabHovered`：InkLine（alpha 0.55）

* `ScrollbarGrabActive`：InkLine（alpha 0.75）

* `CheckMark`：SealRed `#D05A4F`

* `SliderGrab`：InkText2（alpha 0.35）

* `SliderGrabActive`：SealRed（alpha 0.85）

* `Button`：InkWash（alpha 0.55）

* `ButtonHovered`：InkWash（alpha 0.75）

* `ButtonActive`：InkWash（alpha 0.95）

* `Header`：InkWash（alpha 0.55）

* `HeaderHovered`：Indigo `#6E8BFF`（alpha 0.20）

* `HeaderActive`：Indigo（alpha 0.32）

* `Separator`：InkLine（alpha 0.35）

* `SeparatorHovered`：InkLine（alpha 0.55）

* `SeparatorActive`：InkLine（alpha 0.75）

* `ResizeGrip`：InkLine（alpha 0.25）

* `ResizeGripHovered`：InkLine（alpha 0.45）

* `ResizeGripActive`：InkLine（alpha 0.65）

* `Tab`：InkPaper2（alpha 0.85）

* `TabHovered`：Indigo（alpha 0.20）

* `TabActive`：InkPaper2（alpha 1.0）

* `TabUnfocused`：InkPaper2（alpha 0.55）

* `TabUnfocusedActive`：InkPaper2（alpha 0.75）

* `PlotLines`：InkText2

* `PlotLinesHovered`：SealRed

* `PlotHistogram`：Ochre `#D3A14A`

* `PlotHistogramHovered`：SealRed

* `TextSelectedBg`：Indigo（alpha 0.25）

* `DragDropTarget`：SealRed（alpha 0.70）

* `NavHighlight`：Indigo（alpha 0.38）

* `NavWindowingHighlight`：Indigo（alpha 0.22）

* `NavWindowingDimBg`：InkPaper（alpha 0.22）

* `ModalWindowDimBg`：InkPaper（alpha 0.35）

---

## ImGui 风格参数（让“水墨感”更成立）

这些不是颜色，但会决定质感：

* `style.WindowRounding = 6~10`（像宣纸边缘的柔）
* `style.FrameRounding  = 5~8`
* `style.PopupRounding  = 8~12`
* `style.ScrollbarRounding = 12`
* `style.GrabRounding = 6~10`
* `style.TabRounding = 8~10`
* `style.WindowPadding = (10, 8`
* `style.FramePadding = (8, 6`
* `style.ItemSpacing = (8, 6`
* `style.IndentSpacing = 16`
* `style.WindowBorderSize = 1`
* `style.FrameBorderSize = 1`（关键：水墨重“边界”）

---

## “像水墨但不土”的控件使用建议

* 高亮只用 **印泥红** 或 **靛青**，别同时用。
* Debug/Log：

  * Info 用 SlateBlue
  * Warning 用 Ochre
  * Error 用 SealRed
* Icon 与 UI：保持同一条“墨色”主线（比如 Ink1/InkText），点缀色只做徽记。

---
