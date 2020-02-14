# Landscape Engine
## Landscape Engine是一个使用OpenGL编写的室外景观渲染引擎，目前本引擎只能渲染体积云(Volumetric Cloud)，后续会加入地形(Terrain)和水(Water)等其他室外景观的渲染。

***

## 测试环境

* 操作系统：Windows 10
* IDE：Visual Studio 2017
* OpenGL版本：4.6
* GPU：NVIDIA GeForce GTX 850M

***

## 依赖库

| 依赖库名称 | 版本 | 
| :------: | :------: | 
| [glad](https://glad.dav1d.de/) | OpenGL4.6 Core |
| [assimp](http://www.assimp.org/index.php/downloads) | 4.1.0 |
| [glfw](https://www.glfw.org/) | 3.2.1 |
| [glm](https://github.com/g-truc/glm/tags) | 0.9.9.3 |
| [imgui](https://github.com/ocornut/imgui/releases) | 1.69 |
| [stb](https://github.com/nothings/stb) | master |

***

## 项目结构

| 文件夹名称 | 用途 | 
| :------: | :------: | 
| Dependency | 存放项目的依赖库 | 
| Engine | 存放渲染引擎的核心代码 |
| Library | 存放依赖库的lib和dll文件 |
| Resources | 存放模型和贴图等资源文件 |
| ScreenShots | 存放导出的截图文件 |
| Shaders | 存放shader文件 |
| Sources | 存放main.cpp和用于编译依赖库的源文件 |

***

## 项目架构

Landscape Engine采用[MVC架构](https://zh.wikipedia.org/zh-hans/MVC)。如下图所示，Landscape Engine使用`控件窗口`来接收用户的输入并修改`渲染程序`的着色器变量。接着，`渲染程序`向下层的`渲染引擎`发送执行渲染算法的命令。当`渲染引擎`调用`图形库`完成渲染之后，`渲染引擎`向`渲染程序`返回渲染结果。最后，由`显示窗口`从`渲染程序`中获取并显示结果。
![](https://github.com/AsanCai/LandscapeEngine/raw/master/DisplayImages/architecture.png)

***

## 渲染引擎

Landscape Engine的渲染引擎主要由五个模块构成，分别是Config模块、Core模块、Editor模块、Event模块、Landscape模块和Utility模块。

| 模块名称 | 用途 | 
| :------: | :------: | 
| Config | 定义静态的配置类，如用于配置界面布局的LayoutConfig类等 |
| Core | 定义与图形渲染相关的类，如用于控制摄像机的Camera类等 | 
| Editor | 定义与绘制GUI相关的类，如用于绘制编辑器的Editor类等 |
| Event | 定义与事件相关的类，如用于派发事件的Dispatch类等 |
| Landscape | 定义用于渲染室外景观的类，如用于渲染体积云的VolumetricCloud类等 |
| Utility | 定义提供公共API的类，如提供了纹理操作和图像操作的Texture类等 |

***

## 项目演示

![](https://github.com/AsanCai/LandscapeEngine/raw/master/DisplayImages/demo.gif)

***

## 参考链接

* [Learn OpenGL Tutorial](https://learnopengl.com/)
* [Realtime Volumetric Cloudscapes of Horizon Zero Dawn](https://www.guerrilla-games.com/read/the-real-time-volumetric-cloudscapes-of-horizon-zero-dawn)
* [Authoring Realtime Volumetric Cloudscapes with the Decima Engine](https://www.guerrilla-games.com/read/nubis-authoring-real-time-volumetric-cloudscapes-with-the-decima-engine)
