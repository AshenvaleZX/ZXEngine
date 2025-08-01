# ZXEngine

| Graphics API | Platform |
| :----------: | :------: |
| ![Vulkan](Documents/Badges/Vulkan-Supported.svg) ![DirectX 12](Documents/Badges/DirectX_12-Supported.svg) ![Metal](Documents/Badges/Metal-Supported.svg) ![OpenGL](Documents/Badges/OpenGL-Supported.svg) | ![Windows](Documents/Badges/Windows-Supported-blue.svg) ![macOS](Documents/Badges/macOS-Supported-blue.svg) ![Linux](Documents/Badges/Linux-Supported-blue.svg) ![Android](Documents/Badges/Android-Experimental-red.svg) ![iOS](Documents/Badges/iOS-Experimental-red.svg) |

这是我自己开发的游戏引擎项目，创建这个项目的主要目的是为了我自己学习和实践游戏引擎技术，不过也希望这个项目能对你有所帮助。

This is a game engine project. The main purpose that I created this project is to learn and practice game engine technology. And I hope this project will be helpful to you.

本项目以游戏引擎为目标，而并非只关注图形渲染（不过图形渲染也是一个重要模块）。引擎基本外观如下图，具体内容请看图示后面的目录。

This project aimed at the game engine, not only focus on the graphics and rendering (although this is also an important contents). The appearance of the engine is shown in the figure below. For specific content, please see the table of contents below the figure.

![](Documents/Images/EngineShow0.jpg)

## 目录(Contents)

- [引擎简介 (Engine Introduction)](#引擎简介-engine-introduction)

- [引擎编辑器和更多演示 (Engine Editor And More Demonstration)](#引擎编辑器和更多演示-engine-editor-and-more-demonstration)

- [多套内置渲染管线 (Multiple Built-in Rendering Pipelines)](#多套内置渲染管线-multiple-built-in-rendering-pipelines)

- [通用计算管线支持 (Compute Pipeline Support)](#通用计算管线支持-compute-pipeline-support)

- [PhysZ物理引擎简介 (PhysZ Physics Engine Introduction)](#physz物理引擎简介-physz-physics-engine-introduction)

- [ZXShader和材质系统 (ZXShader And Material System)](#zxshader和材质系统-zxshader-and-material-system)

- [骨骼蒙皮动画系统 (Skeletal Animation System)](#骨骼蒙皮动画系统-skeletal-animation-system)

- [粒子系统 (Particle System)](#粒子系统-particle-system)

- [游戏UI系统 (Game UI System)](#游戏ui系统-game-ui-system)

- [GamePlay层的Lua系统 (Lua System of GamePlay Layer)](#gameplay层的lua系统-lua-system-of-gameplay-layer)

- [音频系统 (Audio System)](#音频系统-audio-system)

- [多线程与JobSystem (Multithreading And JobSystem)](#多线程与jobsystem-multithreading-and-jobsystem)

- [C++反射与序列化 (C++ Reflection And Serialization)](#c反射与序列化-c-reflection-and-serialization)

- [引擎文件格式介绍 (Engine File Format Introduction)](#引擎文件格式介绍-engine-file-format-introduction)

- [实际游戏项目演示 (Actual Game Project Demonstration)](#实际游戏项目演示-actual-game-project-demonstration)

- [构建与跨平台 (Build And Cross-Platform)](#构建与跨平台-build-and-cross-platform)

- [注意事项 (Notices)](#注意事项-notices)

## 引擎简介 (Engine Introduction)

| Platform | Graphics API Support       | Build Solution Support      |
| -------- | -------------------------- | --------------------------- |
| Windows  | DirectX 12, Vulkan, OpenGL | Visual Studio, xmake, CMake |
| macOS    | Metal, Vulkan, OpenGL      | xmake, CMake                |
| Linux    | OpenGL, Vulkan(unverified) | xmake, CMake                |
| Android  | Vulkan                     | Android Studio              |
| iOS      | Metal                      | Xcode                       |

本引擎目前同时支持Vulkan，DirectX 12，Metal和OpenGL，支持Windows，macOS，Linux，Android和iOS。使用自创的zxshader语言来编写shader，支持前面4种图形API，编写一次即可在4种图形API和5种平台运行。本引擎同时也支持基于Vulkan和DirectX12的光线追踪渲染管线。

This engine currently supports Vulkan, DirectX 12, Metal and OpenGL, supports Windows, macOS, Linux, Android and iOS. The engine uses the self-created zxshader language to write shaders. You only need to write zxshader once and it can work in all four graphics APIs and five platforms. This engine also supports ray tracing rendering pipeline based on Vulkan or DirectX12.

本引擎内置了我写的物理引擎PhysZ(看了一些书和别人的项目之后的学习成果，详见后文)，支持基本的刚体力学模拟和弹簧，布料模拟。同时我也开发了简单的骨骼蒙皮动画系统，材质系统，粒子系统，UI系统，JobSystem等。

This engine has a built-in physics engine written by myself, which I called it PhysZ (It is the learning result after reading some books and other people’s projects), supports rigid body mechanics simulation spring simulation and cloth simulation. And I also developed a simple skeletal animation system, material system, particle system, UI system, JobSystem, etc. 

引擎本身用C++开发，GamePlay层使用Lua语言，引擎层封装部分C++接口给GamePlay层的Lua调用。使用方式类似Unity的XLua，通过一个GameLogic组件把Lua代码绑定到GameObject上，接收来自引擎的Start和Update调用，并通过self访问所挂载的GameObject对象(具体示例看后面)。

The engine itself is developed in C++, the GamePlay layer uses Lua language, and the engine provides part of the C++ interface to Lua for the GamePlay layer. This is similar to Unity's XLua, you can bind the Lua code to the GameObject through a GameLogic component, receive Start and Update calls from the engine, and access the binding GameObject object through "self" (see later for specific examples).

引擎场景中的对象都是Unity式的 GameObject - Component 结构，也有一个类似Unity的引擎编辑器页面。不过编辑功能还不完善，只有一部分 Component 能够被编辑，其它的主要是展示当前的运行时状态。

The objects in the scene of this engine are all Unity-style GameObject - Component structures, and there is also a Unity-like engine editor. However, the editing function is not complete yet, only some Components can be edited, and the others just display the current runtime status.

## 引擎编辑器和更多演示 (Engine Editor And More Demonstration)

引擎顶部为主菜单栏，中间是Game和Scene视图，左边是展示场景中GameObject的Hierarchy面板，下面是显示当前项目Assets目录的Project面板和显示日志的Console面板，右边是显示当前选中对象详细信息的Inspector面板。

The top of the engine is the main menu bar, the middle is the Game and Scene views, on the left is the Hierarchy panel that displays the GameObjects in the scene, below is the Project panel that displays the assets of the current project and the Console panel that displays the log, and on the right is the Inspector panel that displays detailed information of the currently selected object.

![](Documents/Images/EngineShow1.jpg)

切换到Scene视图后可以直接在场景中对选中的GameObject进行操作：

Switch to the Scene view can manipulate the selected GameObject in the scene:

![](Documents/Images/TransWidgetPos.gif)![](Documents/Images/TransWidgetRot.gif)![](Documents/Images/TransWidgetScale.gif)

点击材质文件后Inspector面板的材质信息和材质预览：

Click the material file, material information and preview on the Inspector panel:

![](Documents/Images/EngineShow2.jpg)

双击场景文件可切换场景，光追管线也可以直接切换。图为点击模型文件后Inspector面板的模型信息和模型预览：

Double-clicking a scene file can switch scenes, and the ray tracing pipeline can also be switched directly. The picture shows the model information and model preview in the Inspector panel after clicking a model file:

![](Documents/Images/EngineShow3.jpg)

点击图片文件后，Inspector面板的图像信息：

Click the image file, the image information in the Inspector panel:

![](Documents/Images/EngineShow4.jpg)

点击zxshader和Lua后Inspector上的代码预览：

Click zxshader and Lua to preview the code in the Inspector:

![](Documents/Images/EngineShow5.jpg)

![](Documents/Images/EngineShow6.jpg)

## 多套内置渲染管线 (Multiple Built-in Rendering Pipelines)

本引擎有3套内置渲染管线，分别是正向渲染管线，延迟渲染管线和光线追踪渲染管线。

ZXEngine has three built-in rendering pipelines, which are forward rendering pipeline, deferred rendering pipeline and ray tracing rendering pipeline.

引擎的场景文件中保存了渲染管线信息，在切换场景的时候引擎会直接无缝切换渲染管线。所以想运行哪套渲染管线，直接在引擎编辑器中双击对应的场景文件切换即可。

The rendering pipeline information is saved in the scene file, and the engine seamlessly switches the rendering pipeline when switching scenes. So if you want to run which rendering pipeline, just double-click the corresponding scene file in the engine editor to switch.

使用正向渲染管线和光追渲染管线的场景已经在前面演示过了，这里再补充一个延迟渲染管线的演示场景：

Scenes using forward rendering pipeline and ray tracing rendering pipeline have been demonstrated previously. Here is a demonstration scene of deferred rendering pipeline:

![](Documents/Images/EngineShow7.jpg)

## 通用计算管线支持 (Compute Pipeline Support)

ZXEngine集成了Vulkan，DirectX 12，Metal和OpenGL的通用计算管线，并封装成了统一接口以供使用。由于macOS只支持到OpenGL 4.1，而OpenGL是从4.3版本才加入对计算管线的支持，所以在macOS上使用OpenGL时，所有计算管线相关的模块都会被禁用。除macOS+OpenGL的组合无法使用计算管线外，其它平台+图形API的组合都是可用的。

ZXEngine integrates the compute pipeline of Vulkan, DirectX 12, Metal and OpenGL and is encapsulated into a unified interface for use. Since macOS only supports OpenGL 4.1, and OpenGL added support for the compute pipeline from version 4.3, all compute pipeline related modules are disabled when using OpenGL on macOS. Except for the macOS + OpenGL combination that does not support compute pipeline, other platform + graphics API combinations are support.

目前项目中实际应用了计算管线的地方在骨骼蒙皮动画模块和D3D12的Mipmap生成。在启用了ZX_COMPUTE_ANIMATION宏之后，骨骼蒙皮动画的Mesh更新操作将在计算管线中完成。

Currently, the actual application of compute pipeline in the project is in the skeletal animation system and the mipmap generation of D3D12. When the ZX_COMPUTE_ANIMATION macro is enabled, the update of skinned mesh will be done in the compute pipeline.

## PhysZ物理引擎简介 (PhysZ Physics Engine Introduction)

先展示一下PhysZ引擎对刚体力学和布料的模拟效果(GIF演示，截屏大小和帧率都有压缩):

Here is a GIF of the simulation of rigid mechanics and cloth in PhysZ (the size and framerate of the screenshots are compressed):

![](Documents/Images/PhysZ0.gif)

使用基本的PhysZ物理引擎只需要关心两个Component：Collider和Rigidbody。其中Collider目前提供了三种类型：BoxCollider，PlaneCollider和SphereCollider。下面是这两个Component在引擎里编辑器里的截图：

Using the PhysZ engine involves only two Components: Collider and Rigidbody. Collider currently provides three types: BoxCollider, PlaneCollider and SphereCollider. The following are screenshots of these two Components in the editor in the engine:

![](Documents/Images/PhysZ1.png)

各参数作用顾名思义即可。其中Damping参数是模拟各种形式的阻尼效果的，Linear Damping和Angular Damping分别对应线性运动和角运动，数值为0代表无阻尼。

The function of each parameter is just as the name implies. And the Damping parameter is used to simulate various forms of damping effects. Linear Damping and Angular Damping correspond to linear motion and angular motion respectively. A value of 0 represents no damping.

弹簧组件如图，类似Unity的SpringJoint，参数分别是连接对象，自己和连接对象的锚点(各自的模型空间)，弹簧的松弛长度和弹簧系数。

The spring component is shown in the figure, similar to Unity's SpringJoint. The parameters are the connection object, the anchor points of itself and the connection object (their respective model spaces), the rest length and spring coefficient of the spring.

![](Documents/Images/PhysZ2.png)

布料模拟需要添加一个Cloth组件，参数分别是布料质量，摩擦系数，弯曲刚度，拉伸刚度和是否使用重力。注意布料需要配合动态Mesh，本引擎暂时只支持DynamicPlane。

Cloth simulation needs to add a Cloth component, the parameters are cloth mass, friction coefficient, bend stiffness, stretch stiffness and whether to use gravity. Note that cloth needs to be matched with dynamic mesh. This engine currently only supports DynamicPlane.

![](Documents/Images/PhysZ3.png)

PhysZ引擎主要是我在学习了Ian Millington的《Game Physics Engine Development》和Gabor Szauer的《Game Physics Cookbook》之后，加上一些自己的思考和实践的成果。引擎里的注释也比较多，欢迎对物理引擎感兴趣的人一起讨论学习。

The PhysZ engine is the result of me studying Ian Millington's "Game Physics Engine Development" and Gabor Szauer's "Game Physics Cookbook", plus some of my own thinking and practice. If you are also interested in physics engines, I hope this engine can be helpful to you.

## ZXShader和材质系统 (ZXShader And Material System)

ZXShader是专门给ZXEngine用的一套Shader系统，因为ZXEngine同时支持Vulkan/DirectX12/Metal/OpenGL，所以也需要一个统一的Shader语言才能支撑后面的材质系统。ZXShader目前暂时只支持光栅渲染管线，光追渲染管线的Shader是在VK和DX下独立写的。ZXShader语言本身并不复杂，对GLSL，HLSL或者Unity ShaderLab比较熟悉的人应该都能很快看懂，代码示例在ExampleProject\Assets\Shaders中。

ZXShader is a shader system for ZXEngine. Because ZXEngine supports Vulkan/DirectX12/Metal/OpenGL, a unified shader language is needed to support the material system. ZXShader currently only supports the rasterization rendering pipeline. The Shader of the raytracing rendering pipeline is written independently under VK and DX. The ZXShader language itself is not complicated. People who are familiar with GLSL, HLSL or Unity ShaderLab should be able to understand it quickly. The code examples are in ExampleProject\Assets\Shaders.

材质系统和Unity的比较类似，就是挂一个Shader，然后可以在编辑器面板上看到这个材质暴露给引擎的参数。并且可以通过引擎编辑器调整参数数值，然后实时看到渲染结果的动态反馈。如图，直接调整正在运行的场景中的材质：

The material system is similar to Unity. Select a render object, and then you can see the parameters exposed by the material to the engine editor. And you can adjust parameter values through the engine editor, and then see the feedback of the rendering results in real time. For example, adjust the materials in the running scene:

![](Documents/Images/Material1.gif)

选中Asset中的材质文件后，直接调整材质参数，在材质球上预览变化：

This is after selecting the material file in the Asset, adjusting the material parameters, and previewing the changes on the material ball:

![](Documents/Images/Material2.gif)

## 骨骼蒙皮动画系统 (Skeletal Animation System)

切换到AnimDemo场景，点击运行按钮后，可以看到骨骼蒙皮动画的展示:

Switch to the AnimDemo scene and click Play button to see the skeletal animation demonstration:

![](Documents/Images/Anim.gif)

本引擎的动画系统支持动画混合，所以这里除了能看到单个动画播放外，还能看到人物从走路状态进入跑步状态时，两个动画之间流畅的过渡。

The animation system supports animation blending, so in addition to the individual animation play, you can also see the smooth transition between the two animations when the character enters the running state from the walking state.

此外，动画系统还支持通过计算管线来更新Mesh数据，同时也兼容传统的顶点着色器更新方式。启用ZX_COMPUTE_ANIMATION宏定义后，带有动画的Mesh数据更新将在计算管线中完成。未启用ZX_COMPUTE_ANIMATION时，蒙皮动画将在顶点着色器中计算。

The animation system also supports update skinned mesh through compute pipeline, and is compatible with traditional vertex shader updating methods. When the ZX_COMPUTE_ANIMATION macro is enabled, the skinned mesh update is completed in the compute pipeline. When ZX_COMPUTE_ANIMATION is not enabled, the skinned mesh is update in the vertex shader.

这里动画的播放代码属于业务逻辑层而不是引擎，所以是写在Lua代码中的，通过GameLogic组件绑定到对应的GameObject上。

The code to play the animation belongs to the GamePlay rather than the engine, so it‘s written in the Lua code and bound to the character GameObject through the GameLogic component.

## 粒子系统 (Particle System)

本引擎有一个简易的粒子系统，可以配置一些简单的粒子特效，使用GPU Instance渲染。示例如下：

This engine has a simple particle system, which can be used to configure some particle effects, rendered by GPU Instance. Examples are as follows:

![](Documents/Images/ParticleSystem0.jpg)

引擎编辑器中的粒子系统面板如下，由于参数较多就不详细介绍了，感兴趣的可以打开示例工程中的ParticleDemo场景自行调试：

The particle system panel in the engine editor is as follows. Since there are numerous parameters, their explanation is not written here in detail. If you're interested, you can open the ParticleDemo scene in the example project to try it out:

![](Documents/Images/ParticleSystem1.png)

## 游戏UI系统 (Game UI System)

本引擎提供了一个简单的游戏UI系统，一个示例如下(更多演示请看后面的实际游戏项目演示)：

This engine provides a simple game UI system, an example is shown below (see the actual game project demo below for more examples) :

![](Documents/Images/GameUI0.png)

这里的UI主要有2类：一类是基于屏幕空间的UI，也就是常规的覆盖到屏幕上的，如图中点击地块后出现的弹窗。第二类是基于世界空间的UI，也就是存在于三维空间中的UI，如图中地块上的名牌。

There are two types of UI here: one is the screen-space UI, which is overlaid on the screen, such as the pop-up window that appears when you click on the tile. The second type is the world-space UI, which is the UI that draw in three-dimensional space, such as the nameplate on the tile.

目前有四个基本的UI组件，分别是RectTransform，UITextRenderer，UITextureRenderer和UIButton。

There are currently four basic UI components, RectTransform, UITextRenderer, UITextureRenderer, and UIButton.

### RectTransform:

![](Documents/Images/GameUI3.png)

RectTransform是用于基于屏幕空间的UI的特殊Transform，可以更方便直观地调节UI。在RectTransform中的Position的X和Y值含义变成了相对父节点的像素偏移量，而不是普通Transform中的局部空间位置。这个偏移量是基于父节点(或屏幕)的位置，大小和锚点的。因此基于RectTransform也实现了UI对屏幕分辨率的自适应。

RectTransform is a special Transform for screen-space based UI that makes adjusting the UI easier and more intuitive. The X and Y values of position in RectTransform become pixel offsets relative to the parent node, rather than local space positions in ordinary Transforms. This offset is based on the position, size, and anchor of the parent node (or screen). Therefore, based on RectTransform, the UI is also adaptive to the screen resolution.

比如我有一个UI组件希望定位到画面左上角，则Vertical Anchor选择Top，Horizontal Anchor选择Left，X和Y填写相对屏幕左上角的偏移量即可。如果是UI系统中的子对象，锚点就会基于父节点位置和Size来定位子对象。

For example, if I want to put a UI component in the upper left corner of the screen, I can select Top for Vertical Anchor, Left for Horizontal Anchor, and set X and Y to the offset relative to the upper left corner of the screen. If it‘s a child object in the UI system, the anchor will position the child object based on the parent node position and size.

如果不使用RectTransform而是使用Transform，就相当于直接把UI放到了场景中，会以3D的形式去绘制。

If you use Transform instead of RectTransform, it means that you put the UI into the scene, and it will be drawn in 3D.

### UITextRenderer:

![](Documents/Images/GameUI1.png)

Text是UI文本内容，后面的参数为字体大小，文本对齐方式和文本颜色。文本内容可以提前设置好保存在Prefab中，也可以在运行时直接通过引擎编辑器实时修改，或者在代码中动态设置，代码示例如下：

Text is the UI text content, followed by the font size, text alignment, and text color. Text content can be pre-set and saved in prefab, modified in real time through the engine editor at run time, or dynamically set in code, as shown in the following code example:

```lua
function ShowFPS:Start()
    -- Get UI text component
    self.FPSTextCom = self.gameObject:GetComponent("UITextRenderer")
end

function ShowFPS:Update()
    local curFPS = CalculateFPS()
    -- Set the content
    self.FPSTextCom:SetText("FPS: " .. curFPS)
end
```

### UITextureRenderer and UIButton:

![](Documents/Images/GameUI2.png)

目前这两个组件在引擎编辑器中仅展示自身相关信息，不提供编辑功能。其中UIButton可以在Lua代码中注册点击后的回调函数，方式如下：

At present, these two components only display information about themselves in the engine editor, and do not provide editing functions. And UIButton can register a click callback function in Lua code, as follows:

```lua
function UILogicExample:Start()
    local btnYesGO = self.gameObject:FindChild("Panel/BtnYes")
    btnYesGO:GetComponent("UIButton"):SetClickCallBack(self.OnBtnClickCallBack, ...)
end

function UILogicExample:OnBtnClickCallBack()
    Log("The yes button is clicked.")
end
```

## GamePlay层的Lua系统 (Lua System of GamePlay Layer)

本引擎的GamePlay层使用Lua语言，引擎将需要提供给GamePlay层的C++接口都Wrap到了Lua，开发方式有点类似于Unity中的XLua。不过目前Wrap到Lua的接口还不是特别多，以后逐步完善。这里只简单演示一下Lua脚本系统和引擎的Lua接口，更多更详细的演示请看后面的实际游戏项目演示。

The GamePlay layer of this engine uses Lua language. The engine wraps all the C++ interfaces that need to be provided to the GamePlay layer into Lua. The development way is similar to XLua in Unity. However, only part of interfaces have been wrapped to Lua at now, and they will be gradually added in the future. Here we only briefly demonstrate the Lua scripting system and the Lua interface of the engine. For more detailed demonstrations, please see the actual game project demonstration in the following document.

使用方式是直接创建一个Lua代码，然后用GameLogic组件把Lua代码挂载到一个GameObject对象上，然后这个Lua代码就可以接收来自引擎的Awake，Start，Update和FixedUpdate等接口的调用了。也可以通过self来访问自己所挂载的GameObject对象，获取Component，调用引擎接口等等。

The way to use lua in ZXEngine is to create a Lua code, and then use the GameLogic component to bind the Lua code to a GameObject. Then the Lua code can receive calls from the engine's Awake, Start, Update and FixedUpdate. You can also use "self" to access the GameObject you bound, obtain components, call the engine interface, etc.

以控制GameObject移动和旋转为例，可以写一个ObjectMove.lua脚本，然后挂到一个GameObject上。这里我们可以暴露一些Lua脚本中的变量到编辑器面板上，以便在引擎编辑器中直接修改Lua变量的值，方便调试，也方便同一个脚本挂到不同GameObject上时使用不同的参数。这些变量的值会保存到zxprefab文件中，运行时在Awake之前完成初始化。比如我们的ObjectMove.lua脚本暴露了一个speed变量来控制物体移动速度：

Take controlling the movement and rotation of GameObject as an example. Write an ObjectMove.lua and bind it to a GameObject. Here we can expose some variables from the Lua script to the editor panel so that the values of the Lua variables can be modified in the engine editor for debugging purposes and can set different parameters when the same script is bound to different GameObjects. The values of these variables are stored in a zxprefab file and will be initialized at runtime before Awake. For example, our ObjectMove.lua script exposes a speed variable to control how fast objects move:

![](Documents/Images/GameLogic.png)

这个Lua脚本大致如下：

And the Lua script looks something like this:

```lua
local ObjectMove = NewGameLogic()

ObjectMove.speed = 2

ObjectMove.radius = 20
ObjectMove.angle = 0
ObjectMove.rot = 0

function ObjectMove:Start()
    self.trans = self.gameObject:GetComponent("Transform")
end

function ObjectMove:Update()
    self.angle = self.angle + self.speed * Time.GetDeltaTime()
    local x = math.sin(self.angle) * self.radius
    local z = math.cos(self.angle) * self.radius
    self.trans:SetPosition(x, 0, z)

    self.rot = CalculateRotation();
    self.trans:SetEulerAngles(0, 0, self.rot)
end

return ObjectMove
```

如果是要调用物理引擎相关的接口，和在Unity里一样，需要把代码写到FixedUpdate里面，否则物理效果会有异常。代码示例：

If you want to call the interface related to the physics engine, just like in Unity, you need to write the code in FixedUpdate, otherwise the physical effect will be wrong. Code example:

```lua
local PhysControl = NewGameLogic()

PhysControl.force = { x = 0, y = 100, z = 0 }
PhysControl.velocity = { x = 10, y = 0, z = 0 }

function PhysControl:Start()
    -- Find some gameobject in the scene and control it.
    self.rigidBody = GameObject.Find("SomeObj/SubObj"):GetComponent("RigidBody")
end

function PhysControl:FixedUpdate()
    self.rigidBody:AddForce(self.force)
    self.rigidBody:SetVelocity(self.velocity)
end

return PhysControl
```

如果要在运行时创建GameObject，可以直接通过如下接口读取Prefab并创建：

If you want to create a GameObject at runtime, you can read the Prefab and create it through the following interface:

```lua
local newGO = GameObject.Create("Path/To/Example.zxprefab")
```

不过如果你需要为一个Prefab创建多个实例，最好通过如下方式先加载Prefab，然后再实例化多个，避免重复的IO开销：

However, if you need to create multiple instances of a prefab, it is better to load the prefab first and then instantiate multiple instances to avoid repeated IO cost:

```lua
local prefab = Resources.LoadPrefab("Path/To/Example.zxprefab")

local allGO = {}

for i=1, 100 do
    local newGO = GameObject.CreateInstance(prefab)
    table.insert(allGO, newGO)
end
```

如果要接收输入事件，比如写一个Lua脚本挂到相机上控制镜头移动，那么可以注册来自引擎的输入事件。比如注册按住WSAD的回调函数：

If you want to receive input events, such as writing a Lua script to control camera movement, you can register input events from the engine. For example, register the callback function for pressing WSAD:

```lua
EngineEvent:AddEventHandler(EngineEventType.KEY_W_PRESS, MoveForwardCallBack)
EngineEvent:AddEventHandler(EngineEventType.KEY_S_PRESS, MoveBackCallBack)
EngineEvent:AddEventHandler(EngineEventType.KEY_A_PRESS, MoveLeftCallBack)
EngineEvent:AddEventHandler(EngineEventType.KEY_D_PRESS, MoveRightCallBack)
```

ZXEngine的Lua系统接口就不一一列举了，可以到示例工程里细看。

I won’t list ZXEngine’s Lua system interfaces one by one. You can go to the example project to take a look.

## 音频系统 (Audio System)

本引擎的音频系统类似Unity，主要依靠AudioListener和AudioSource这两个组件。

The audio system of this engine is similar to Unity, mainly relying on the two components AudioListener and AudioSource.

AudioSource如下，各参数都比较好理解。

AudioSource is as shown below. These parameters are easy to understand.

![](Documents/Images/Audio1.png)

其中最后两个参数只在勾选3D音效后生效。MinDistance表示小于这个距离之后音量不会再增大，MaxDistance表示大于这个距离后音量不会再减小。

The last two parameters only take effect in 3D sound effects. When the distance is less than MinDistance, the volume will no longer increase, and when the distance is greater than MaxDistance, the volume will no longer decrease.

AudioListener同样也是用于3D音效的，如果你不需要3D音效的话场景里可以没有这个组件。这个组件挂载到哪个GameObject上就会用哪个GameObject计算3D音效。和Unity一样这个组件不需要任何参数，一般会挂载到主相机所在的GameObject上，如图：

![](Documents/Images/Audio2.png)

本引擎的绝大部分系统和模块都是我自己编写的，但是音频处理是一个和图形渲染，物理引擎一样很专业的领域，涉及音频编码和时域频域变换等相关专业知识。而我对音频相关的专业知识了解还不够，所以目前本引擎音频系统中的核心部分，即音频解码和播放功能使用了irrKlang这个第三方库。由于irrKlang没有对Apple Silicon做支持，所以在macOS上的音频系统目前是一个空的实现。

Most of the systems and modules of this engine are written by myself, but audio processing is a very professional field like graphics rendering and physics engine, involving audio coding, time domain and frequency domain transformation and other related professional knowledge. I don’t know enough about audio-related professional knowledge, so the core part of the audio system of this engine, that is, the audio decoding and playback functions, uses the third-party library irrKlang. Since irrKlang does not support Apple Silicon, the audio system on macOS is currently an empty implementation.

## 多线程与JobSystem (Multithreading And JobSystem)

目前引擎中有三个多线程模块，异步加载资源，着色器预编译和一个简易的JobSystem。其实我还想搞多线程渲染，也在实际尝试搞，但确实有点困难，需要大改引擎框架，所以还没搞好，先鸽着。

Currently, there are three multi-threaded modules in the engine: asynchronous resource loading, shader precompilation and a simple JobSystem. In fact, I also want to do multi-threaded rendering, and I‘m actually trying to do it, but it's indeed difficult and requires a lot of modifications to the engine framework, so I haven't done it yet.

资源异步加载的接口如下，直接在主线程(也是逻辑线程)中调用即可：

The interface for asynchronous loading is as follows, which can be called in the main thread (also the logical thread):

```c++
C++ Interface:
static void ZXEngine::GameObject::AsyncCreate(const string& path);

static void ZXEngine::Resources::AsyncLoadPrefab(const string& path, std::function<void(PrefabStruct*)> callback, bool isBuiltIn = false);

static void ZXEngine::Resources::AsyncLoadMaterial(const string& path, std::function<void(MaterialStruct*)> callback, bool isBuiltIn = false, bool isEditor = false);

static void ZXEngine::Resources::AsyncLoadModelData(const string& path, std::function<void(ModelData*)> callback, bool isBuiltIn = false, bool isEditor = false);
```

```lua
Lua Interface:
GameObject.AsyncCreate(path)
```

然后是着色器预编译，在引擎编辑器顶部的菜单栏点击Asset/Compile All Shader for Vulkan或者Compile All Shader for DirectX12，就会开启一个线程去编译Shader。

As for shader pre-compilation, click "Asset/Compile All Shader for Vulkan" or "Compile All Shader for DirectX12" in the menu bar at the top of the engine editor, and a thread will be created to compile the shaders.

### JobSystem

最后是JobSystem，使用方式类似Unity的JobSystem。自己创建一个类继承Job，然后覆写Execute接口。

Next is JobSystem, which is used in a similar way to Unity's JobSystem. Create a class that inherits Job and override the Execute function.

```c++
class ExampleJob : public Job
{
public:
    std::atomic_int num;
    
    void Execute() override
    {
        for (int i = 0; i < 100000; i++)
        {
            num++;
        }
    };
};
```

使用的时候new一个Job，然后调用Schedule让Job自动进入到JobSystem被调度和执行，返回一个JobHandle。然后在需要同步数据的地方调用JobHandle的Accomplish接口即可。

When using it, create a new Job and call Schedule to automatically schedule and execute the Job in the JobSystem, returning a JobHandle. Then call the Accomplish of JobHandle where you need to synchronize data.

```c++
ExampleJob* job = new ExampleJob();
job->num = 123;

JobHandle handle = job->Schedule();

// Do something else
......

handle.Accomplish();

std::cout << "Res: " << job->num << std::endl;
```

如果需要使用JobSystem处理一些复杂数据，那么为了保证数据的线程安全，最好配合引擎的ThreadSafeData使用。具体例子可以参考引擎中的骨骼蒙皮动画模块。

If you need to use JobSystem to process some complex data, it's better to use it with the engine's ThreadSafeData in order to keep the data thread-safe. For specific examples, please refer to the skeletal animation system in this engine.

由于引擎的GamePlay层使用的语言为Lua，而Lua解释器并不支持多线程解释执行代码，所以这套JobSystem目前只能在引擎内部使用，并未暴露给GamePlay层。

Because the language of the GamePlay layer of this engine is Lua, and the Lua interpreter does not support multi-threaded interpretation of the Lua code, the JobSystem can only be used in the C++ engine at present, and is not exposed to the GamePlay layer.

## C++反射与序列化 (C++ Reflection And Serialization)

本引擎实现了一套简单的C++反射系统，包括静态反射和动态反射两种。这套反射系统的实现主要参考了[mirror](https://github.com/VisualGMQ/mirrow)和[rttr](https://github.com/rttrorg/rttr)这两个库。目前反射系统在项目中的实际运用是实现了一套C++对象的序列化与反序列化工具。

This engine has a simple C++ reflection system, including static reflection and dynamic reflection. The implementation of this reflection system mainly refers to [mirror](https://github.com/VisualGMQ/mirrow) and [rttr](https://github.com/rttrorg/rttr). At present, the actual use of the reflection system in this project is to implement a C++ object serialization and deserialization tool.

### Serialization

序列化工具接口如下：

The serialization tool interface is as follows:

```c++
// Serialize an object to string
template <typename T>
static string Serialize(const T& object);

// Deserialize an object from string
template <typename T>
static T Deserialize(const string& data);

// Deserialize an object from string
template <typename T>
static void Deserialize(T& object, const string& data);
```

### Reflection

反射系统接口比较复杂，这里只演示一部分。假设有一个Student类：

The reflection system has more interfaces than serialization, and only some of them are shown here. Suppose you have a Student class:

```c++
class Student
{
public:
    string mName;
    uint32_t mAge = 0;
    float mHeight = 0.0f;
    Vector3 mPos;

    int GetAge() const { return mAge; }

    void Speak(const string& words)
    {
        Debug::Log("Student speak: %s", words);
    }
};
```

#### Static Reflection:

```c++
#include "Reflection/StaticReflection.h"

// Register the Student class
StaticReflection
(
    Student,
    Fields
    (
        Field(&Student::mName),
        Field(&Student::mAge),
        Field(&Student::mHeight),
        Field(&Student::mPos)
    )
)

int main()
{
    // Create an instance
    Student object;
    
    // Get type info
    auto typeInfo = Reflection::Static::Reflect(object);
    
    // Traverse all member and print their name and value
    typeInfo.TraverseMemberVariableAndDo
    (
        [&](auto& field)
        {
            auto& member = field.Invoke(object);
            Debug::Log("Member name: %s, value: %s", field.GetName(), member);
        }
    );
}
```

#### Dynamic Reflection:

```c++
#include "Reflection/DynamicReflection.h"

int main()
{
    // Register the Student class
    Reflection::Dynamic::Register<Student>()
        .Register("Student")
        .AddVariable("mName",   &Student::mName)
        .AddVariable("mAge",    &Student::mAge)
        .AddVariable("mHeight", &Student::mHeight)
        .AddVariable("mPos",    &Student::mPos)
        .AddFunction("GetAge",  &Student::GetAge)
        .AddFunction("Speak",   &Student::Speak);
    
    // Create an instance
    Student object;
    
    // Get type info
    auto typeInfo = Reflection::Dynamic::GetTypeInfo("Student")->AsClass();
    
    // Set member value by name
    typeInfo->SetVariable(&object, "mAge", 12);

    // Traverse all member and print their name and value
    for (auto& item : typeInfo->GetVariables())
    {
        Debug::Log("Variable: %s %s", item->type->GetName(), item->name);
    }
    
    // Traverse all function and print their name, return type and parameters type
    for (auto& item : typeInfo->GetFunctions())
    {
        Debug::Log("Function: %s return %s", item->name, item->returnType->GetName());
        for (auto& param : item->paramTypes)
            Debug::Log("Param: %s", param->GetName());
    }
}
```

更多的反射接口，使用方式以及序列化工具等等的演示，请参考Test\ReflectionAndSerializationCase.h

For more reflection interfaces, use way and serialization tool demonstration, etc. please refer to Test\ReflectionAndSerializationCase.h

## 引擎文件格式介绍 (Engine File Format Introduction)

### *.zxscene

场景文件，包含GameObjects，天空盒等。如果是光追场景，还包含了光追管线的Shader。

Scene files, containing GameObjects, skyboxes, etc. If it is a ray tracing scene, it also includes the Shader of the ray tracing pipeline.

### *.zxshader

这是本引擎自己的shader语言文件，不过目前zxshader仅支持Vulkan，DirectX 12，Metal和OpenGL的光栅化渲染管线。示例代码可以在ExampleProject\Assets\Shaders中找到。

This is ZXEngine's shader language file, but currently zxshader only supports the rasterization rendering pipeline of Vulkan, DirectX 12, Metal and OpenGL. Example code can be found in ExampleProject\Assets\Shaders.

### *.vkr  *.dxr

这两个后缀分别对应Vulkan和DirectX12的光线追踪Shader代码文件。目前暂未像光栅管线Shader那样搞一个引擎专用的Shader语言。

These two extension correspond to the ray tracing shader code files of Vulkan and DirectX12 respectively. For now, there is no engine-specific ray tracing shader language like ZXShader in the rasterization pipeline.

### *.glc  *.dxc  *mtc

这两个后缀分别对应Vulkan/OpenGL，DirectX12和Metal的计算管线Shader代码文件。通用计算管线与光线追踪管线一样，暂未统一Shader语言。

These two extension correspond to the compute shader code files of Vulkan/OpenGL, DirectX12 and Metal respectively. Compute pipeline, like the ray tracing pipeline, has not yet created a unified shader language.

### *.zxmat  *.zxdrmat  *.zxrtmat

分别是普通光栅化渲染管线(正向渲染)，延迟渲染管线和光线追踪渲染管线的材质文件。

They are the material files of the general rasterization rendering pipeline(forward rendering), deferred rendering pipeline and the ray tracing rendering pipeline respectively.

### *.zxprefab

预制体文件，和Unity的prefab差不多。

The prefab file is similar to Unity's prefab.

### *.zxprjcfg

由ZXEngine创建的游戏项目工程的配置文件，可以在ExampleProject中找到参考示例。

This is the configuration file for the game project created by ZXEngine, you can find the example in ExampleProject.

### Others

模型，纹理贴图，字体等就是常见的通用文件格式。

Models, textures, fonts, etc. are common file formats.

## 实际游戏项目演示 (Actual Game Project Demonstration)

为了更好地演示如何使用ZXEngine进行实际开发，以及更好地演示ZXEngine的各种接口和系统的使用方式，我专门创建了另一个仓库：

To better demonstrate how to use ZXEngine for real development, and to better demonstrate how to use ZXEngine's various interfaces and systems, I have created another repository:

[ZXEngine-Demo-Projects](https://github.com/AshenvaleZX/ZXEngine-Demo-Projects)

这个仓库里面存放了一些用ZXEngine开发的实际游戏项目Demo。除了用于演示外，开发这些Demo也是在用实际需求检验ZXEngine，从而发现引擎不完善的地方并进行扩展和优化。目前有两个Demo：

This repository contains some actual game project demos developed with ZXEngine. Except for demonstration, developing these demos is also a way to test ZXEngine against real requirements, to find out where the engine falls short and to extend and optimize it. There are currently two demos:

### SLG Demo:

![](https://github.com/AshenvaleZX/ZXEngine-Demo-Projects/blob/main/Documents/SLGDemo.jpg)

### Medieval Theme Tank War Demo:

![](https://github.com/AshenvaleZX/ZXEngine-Demo-Projects/blob/main/Documents/TankWarDemo.jpg)

## 构建与跨平台 (Build And Cross-Platform)

本项目支持Windows，macOS，Linux，Android和iOS，提供了五种构建工具，分别是xmake，CMake，Visual Studio 2022，Android Studio和Xcode，放到了BuildSolution文件夹中。

This project supports Windows, macOS, Linux, Android and iOS, provides five build tools: xmake, CMake, Visual Studio 2022, Android Studio and Xcode, which are placed in the BuildSolution folder.

本项目在Windows平台支持DirectX 12，Vulkan和OpenGL，在macOS平台支持Metal，Vulkan和OpenGL，在Linux平台支持OpenGL，在Android平台支持Vulkan，在iOS平台支持Metal。在Linux上的Vulkan应该也是能支持的，但是由于我缺少符合条件的硬件设备，所以暂时还没有在Linux上调试运行过Vulkan版的ZXEngine。

This project supports DirectX 12, Vulkan and OpenGL on Windows, supports Metal, Vulkan and OpenGL on macOS, supports OpenGL on Linux, Vulkan on Android and Metal on iOS. Vulkan on Linux is also supposed to be supported, but I haven't tested and debugged the Vulkan version of ZXEngine on Linux yet because I don't have the device that meets the requirements.

### Windows

在Windows平台下可以使用VS 2022，xmake和CMake。喜欢用VS的直接用VS 2022打开BuildSolution\VisualStudio\ZXEngine.sln构建即可。如果不想用VS，可以使用xmake，使用方式如下：

On the Windows platform you can use VS 2022，xmake and CMake. If you want to use VS, just use VS 2022 to open BuildSolution\VisualStudio\ZXEngine.sln to build. If you don’t want to use VS, you can use xmake as follows:

```shell
cd BuildSolution\xmake
xmake
xmake run zxengine
```

然后是CMake，直接运行 BuildSolution\CMake\BuildWin.bat 脚本即可。

Then for CMake, just run the BuildSolution\CMake\BuildWin.bat script.

```shell
cd BuildSolution\CMake
.\BuildWin.bat
```

### macOS

本项目在macOS平台提供的构建工具为xmake和CMake，xmake的使用方式和在Windows平台一样：

The build tools for macOS are xmake and Cmake, and xmake works the same way it does on Windows:

```shell
cd BuildSolution/xmake
xmake
xmake run zxengine
```

然后是CMake，直接运行 BuildSolution\CMake\BuildMac.sh 脚本即可。

Then for CMake, just run the BuildSolution\CMake\BuildMac.sh script.

```shell
cd BuildSolution/CMake
./BuildMac.sh
```

Mac设备有一个问题，就是几何着色器可能是不被支持的。比如使用Metal或者Vulkan时就无法使用几何着色器，使用OpenGL是可以的。这可能是因为苹果的GPU本身的设计就没有准备支持几何着色器，只是为OpenGL的几何着色器做了个驱动层面的模拟。本引擎的Shadow Cube Map默认是使用几何着色器一次性渲染的，但在不支持几何着色器的环境下会以非几何着色器的方式将场景渲染6次来生成Shadow Cube Map。

One problem with Mac devices is that geometry shader may not be supported. For example, you can't use geometry shaders with Metal or Vulkan, but you can use them with OpenGL. Probably because Apple's GPU was not designed to support geometry shader, but did a driver level simulation for OpenGL's geometry shader. The Shadow Cube Map of this engine is rendered once by default using geometry shader. However, in an environment where geometry shader is not supported, the scene will be rendered six times in a non-geometry shader way to generate the Shadow Cube Map.

在MacBook下运行时可能还有一个小问题。因为MacBook的屏幕较小，所以即使屏幕的硬件分辨率很高，但是为了文本和图标不会太小，macOS的默认分辨率设置还是可能会比较低。比如我这台13寸的MacBook屏幕硬件分辨率为2560x1600，但是系统默认分辨率为1440x900。当屏幕分辨率小于ZXEngine窗口大小时会触发ZXEngine的自适应调整，可能会使引擎编辑器的某些面板显示过小，有些模糊或者不太正常。推荐运行时分辨率设置在1920x1080以上。

There may be another small problem when running on MacBook. Because the MacBook's screen is small, even if the screen's hardware resolution is high, the default resolution setting of macOS may still be relatively low so that text and icons are not too small. For example, the hardware resolution of my 13-inch MacBook screen is 2560x1600, but the default system resolution setting is 1440x900. When the screen resolution is smaller than the ZXEngine window size, the ZXEngine adaptive adjustment will be triggered, which may cause some panels of the engine editor to display too small, a little blurry or abnormal. It is recommended to set the resolution to above 1920x1080 when running ZXEngine with editor.

### Linux

本项目在Linux平台也提供了xmake和CMake，xmake的使用方式还是一样：

This project also provides xmake and CMake for Linux. The usage of xmake is still the same:

```shell
cd BuildSolution/xmake
xmake
xmake run zxengine
```

然后是CMake，直接运行 BuildSolution\CMake\BuildLinux.sh 脚本即可。

Then for CMake, just run the BuildSolution\CMake\BuildLinux.sh script.

```shell
cd BuildSolution/CMake
./BuildLinux.sh
```

由于我没有可以满足Vulkan运行条件的Linux设备，所以在Linux上我暂时只成功测试运行过OpenGL版本的ZXEngine。但是如果你有一台装有支持Vulkan的硬件和驱动的Linux设备，那可以试试运行Vulkan版本的。

Since I don't have a Linux device that meets the requirements for running Vulkan, I have only successfully tested and run the OpenGL version of ZXEngine on Linux. But if you have a Linux device with hardware and driver that support Vulkan, you can try running the Vulkan version.

Linux版本众多，环境复杂，而我只有一个Linux测试环境，所以我不确定本项目是否在所有的Linux环境下都能正常运行。还有由于本项目依赖的第三方库，主要是Assimp和FreeType需要自己编译，而我的Linux是安装在使用x86_64架构的设备上的，我也没有ARM架构的Linux设备，所以我只提供了x86_64版本的Linux依赖库。如果需要在ARM架构的Linux设备上运行只能自己编译一下依赖库了。

There are many Linux distributions and different hardware, and I only have one Linux environment, so I‘m not sure whether this project can run successfully in all Linux environments. Also, since the third-party libraries that this project relies on, mainly Assimp and FreeType, need to be compiled by myself, and my Linux is installed on a x86_64 device, and I don't have an ARM architecture Linux device, so I only provide the x86_64 version of the Linux library. If you need to run it on an ARM architecture Linux device, you need to compile the libraries yourself.

### Android

目前ZXEngine的代码已经完成了对安卓平台的适配，我也使用ZXEngine成功构建过安卓包并在我的安卓手机上成功测试运行过了。但是由于安卓出包比较复杂，本项目尚未提供一个完整的安卓包自动化构建流程。目前ZXEngine在安卓平台使用Vulkan进行渲染，暂未考虑支持OpenGL ES。

The code of ZXEngine has been adapted to the android platform, and I have successfully built android apk with ZXEngine and successfully tested and ran them on my Android device. However, due to the complexity of android apk building, this project has not provided a complete automated building process for android. Currently, ZXEngine uses Vulkan for rendering on the Android platform and has not yet considered supporting OpenGL ES.

如果需要构建安卓平台的ZXEngine应用，可以参考以下步骤手动构建：

If you want to build ZXEngine applications for android platform, you can follow the following steps to build manually:

#### 1.

打开BuildSolution/AndroidStudio里的Android Studio模板工程，将本项目依赖的两个第三方库[assimp](https://github.com/assimp/assimp/releases)和[freetype](https://freetype.org/download.html)下载到本地，把文件夹名字改成assimp和freetype，放置于app/src/main/cpp文件夹下，与CMakeLists.txt同级。

Open the Android Studio template project in BuildSolution/AndroidStudio. Then download the two third-party libraries [assimp](https://github.com/assimp/assimp/releases) and [freetype](https://freetype.org/download.html) that this project relies on to local, and change the folder name to assimp and freetype. Place them in the app/src/main/cpp folder, in the same path as CMakeLists.txt.

#### 2.

把要构建的游戏项目先在桌面平台上以编辑器模式打开，然后点一下引擎编辑器的“Assets/Compile All Shader for Vulkan”按钮，完成Shader代码的预编译工作。

Open the game project to be built in editor mode on the desktop platform first, then click the "Assets/Compile All Shader for Vulkan" button in the engine editor to complete the pre-compilation of the shader code.

#### 3.

把BuiltInAssets文件夹整个复制粘贴到安卓工程的assets目录（app/src/main/assets）下，再把你要打包的那个工程的Assets文件夹和项目配置文件（ProjectSetting.zxprjcfg）一起复制粘贴到assets目录下，最后安卓工程的assets目录下应该有这三个内容：Assets，BuiltInAssets，ProjectSetting.zxprjcfg

Copy and paste the entire BuiltInAssets folder into the assets directory(app/src/main/assets) of the Android Studio project. Then copy and paste the Assets folder and project configuration file(ProjectSetting.zxprjcfg) of the project you want to build into the assets directory. Finally, the assets directory of the Android Studio project should have these three contents: Assets, BuiltInAssets, ProjectSetting.zxprjcfg

#### 4.

上述步骤都完成后，在Android Studio里点击构建即可。

After completing the above steps, click Build in Android Studio to complete the build.

### iOS

ZXEngine已经完成了对iOS平台的适配，并且成功出包在iPhone设备上测试运行过了，使用Metal进行渲染。但是和在安卓平台一样暂未提供一个完整的自动化构建流程。

ZXEngine has completed the iOS platform support, and has been successfully tested and run on the iPhone device, using Metal for rendering. However, just like on the Android platform, a complete automated build process has not been provided yet.

目前ZXEngine在iOS上还有一个问题，就是Assimp库读取动画数据有BUG。在iOS上读取出来的aiScene::mAnimations的mRotationKeys数据有异常，我调试起来像是数据读取的时候错位了。这会导致动画系统异常。现在引擎里使用的是目前最新的Release 5.4.3依然存在这个问题，后面Assimp如果更新版本了我会关注这个问题有没有被解决。不过这个问题仅存在于iOS平台，其它所有平台，包括macOS都是正常的。

Currently, ZXEngine still has one issue on iOS, which is that there is a BUG in reading animation data from the Assimp library. The mRotationKeys data of aiScene::mAnimations read on iOS is abnormal. I debug it as if the data was misplaced when reading. This will cause the animation system to malfunction. Currently, the latest Release 5.4.3 is being used in the engine, but this problem still exists. If Assimp updates its version later, I will pay attention to whether this problem has been solved. However, this issue only exists on the iOS platform. All other platforms, including macOS, are fine.

如果需要构建iOS平台的ZXEngine应用，可以参考以下步骤手动构建：

If you want to build ZXEngine applications for iOS platform, you can follow the following steps to build manually:

#### 1.

打开BuildSolution/Xcode里的Xcode模板工程，在Project Navigator里选择ZXEngine，再选择TARGETS里的ZXEngine，选择Signing & Capabilities，把Signing下的苹果账号设置为你自己的。

Open the Xcode template project in BuildSolution/Xcode, select ZXEngine in Project Navigator, then select ZXEngine in TARGETS, then select Signing & Capabilities, then set the account of Signing as your own account.

#### 2.

把BuiltInAssets文件夹整个复制粘贴到Xcode工程的ZXAssets目录下，再把你要打包的那个工程的Assets文件夹和项目配置文件（ProjectSetting.zxprjcfg）一起复制粘贴到ZXAssets目录下，最后Xcode工程的ZXAssets目录下应该有这三个内容：Assets，BuiltInAssets，ProjectSetting.zxprjcfg

Copy and paste the entire BuiltInAssets folder into the ZXAssets directory of the Xcode project. Then copy and paste the Assets folder and project configuration file(ProjectSetting.zxprjcfg) of the project you want to build into the ZXAssets directory. Finally, the ZXAssets directory of the Xcode project should have these three contents: Assets, BuiltInAssets, ProjectSetting.zxprjcfg

#### 3.

上述步骤都完成后，在Xcode里点击构建即可。

After completing the above steps, click Build or Archive in Xcode to complete the build.

## 注意事项 (Notices)

目前zxshader编写好后，在DirectX 12，Metal和OpenGL下都是运行时直接读取源代码并实时编译的，但是在Vulkan下需要通过外部工具做预编译。虽然Vulkan也可以做到通过源码实时编译，但是写起来比较麻烦，而且需要引入额外的运行时库，所以没做Vulkan的实时源码编译。在Vulkan下加载一个Shader时，会去检测是否有预编译过的spv文件，如果没有的话就立刻调用外部工具编译再读取。所以如果是第一次打开某个工程，或者第一次打开某个场景，在Vulkan下速度会比较慢，因为需要等待Shader先完成一次预编译。不过可以通过点击引擎菜单栏里的“Assets/Compile All Shader for Vulkan”按钮，提前完成所有的Shader预编译。在Vulkan下还有一个问题是Shader代码的更新，因为在Vulkan下是直接读取编译成spv文件之后的Shader，而不是源代码，所以在Shader源码修改之后，需要重新编译一次才会生效，这种情况下也需要手动去点一下“Assets/Compile All Shader for Vulkan”按钮。

Currently, zxshaders are read from the source code at runtime and compiled in real time under DirectX 12, Metal and OpenGL, but precompiled by external tools under Vulkan. Although Vulkan can also do real-time compilation through the shader source code, but it is more troublesome to write, and needs to introduce additional runtime libraries, so it did not do Vulkan shader real-time compilation. When loading a shader under Vulkan, the engine checks if there is a pre-compiled spv file, and if not, it immediately calls an external tool to compile and read it again. So, if it's the first time to open a project, or the first time to open a scene, the speed will be slower under Vulkan, because you need to wait for shaders to complete a pre-compilation first. However, you can complete All shader precompilation in advance by clicking the "Assets/Compile All Shader for Vulkan" button in the engine menu bar. Another thing under Vulkan is to update the shader code. Because under Vulkan the engine directly read the shader precompiled into the spv file, rather than the source code, so after the shader source code is modified, it needs to be recompiled to take effect. In this case, you also need to manually click the "Assets/Compile All Shader for Vulkan" button.

在我开发ZXEngine对通用计算管线的支持的过程中，在开发DirectX 12的版本时遇到了奇怪的崩溃问题，没有异常信息抛出，崩溃在了英伟达驱动内部。最终是通过更新英伟达驱动解决的问题，我不太确定这个问题具体是怎么产生的。如果你有遇到类似的问题可以尝试更新一下驱动。

In the process of developing ZXEngine support for compute pipeline, I encountered a strange crash problem while developing the DirectX 12 version, no exception messages were thrown, and the crash was inside the Nvidia driver.The problem was solved by updating the Nvidia driver. I'm not sure exactly how this problem came about. If you have similar problems you can try to update the driver.

切换图形API的方式是在pubh.h文件中修改宏定义。有一个叫ZX_API_SWITCH的宏定义就是用来控制图形API切换的。具体怎么操作请到pubh.h头文件中查看ZX_API_SWITCH的注释。

The way to switch the graphics API is to modify the macro definition in the pubh.h file. There is a macro definition ZX_API_SWITCH that controls the graphical API switching. See the ZX_API_SWITCH comments in the pubh.h header file for details.

作为一个个人项目，本项目还没有在各种不同设备环境下进行过严格的测试，所以不排除在某些设备下会出现异常。当然也有可能在我不停更新本项目的过程中写出一些我没发现的BUG。所以如果你在编译或者运行项目的过程中遇到了异常，欢迎向我提出。如果你有什么疑问，或者有什么想和我讨论的，可以通过我Github主页的邮箱联系我，或者通过我的知乎主页私信我。

As a personal project, this project has not been rigorously tested in various environments, so exceptions may occur when running with some certain devices. It's also possible that I may write some bugs that I haven't noticed in the process of updating this project. So if you encounter an exception when compiling or running the project, you can report it to me. If you have any questions or want to discuss anything with me, you can contact me through the email on my Github homepage, or send me a message through my Zhihu homepage.

## 一些废话 (Some mumbles)

闲聊一下关于这个项目是怎么来的。其实仓库在2020年5月就创建了，但是最初只是想把2020年初学习OpenGL写的代码保存到GitHub上而已，完全没有想过要做游戏引擎。当时的代码只有1000多行，而且是面向过程式的C语言风格，整个项目顶多算个小渲染Demo。那个时候其实还想继续扩展，加点新学的东西进去，但是感觉加不动了，因为面向过程的代码实在是难以扩展。所以就开始想把当时的小Demo改成一个正式一点的，有渲染框架的工程。但是不知道从何改起，就一直没动，先看书去了。随后花了1年多时间把《Real-Time Rendering 4th》看了，这期间光看书没再实际写代码了，因为这书实在是太偏理论了，没什么可以立刻实践的东西。结果书看完了还是不知道怎么开始，就沉寂了几个月。

A little mumbles about how this project came about. This repository was created in May 2020, but initially I just wanted to save the code I wrote when learning OpenGL in early 2020 to GitHub, and I never thought about making a game engine. At that time, there were only about 1,000 lines of code, and it was in a procedural C language style. The entire project was at most a small rendering demo. At that time, I wanted to continue to extend it and add some new features I learned, but I felt that I couldn't make it, because the procedural code had poor extensibility. So I started to think about making that small demo a more formal project with a rendering framework. But I didn't know where to start, so I didn't do it and went to read books first. Then I spent about a year reading "Real-Time Rendering 4th". During this period, I only read the book and didn't actually write any code, because the book was too theoretical and there was nothing that could be put into practice immediately. As a result, I still didn't know how to start after reading the book, so I fell silent for a few months.

时间到了2022年，在一次我们公司的内部技术分享中，有一位大佬前辈以游戏引擎开发为主题做了一次分享。当时展示了一下他业余时间搞的自研引擎，也是仿Unity的，完成度很高。并且不止是PC平台上的玩具，而是已经可以打包到安卓平台发布的了。那次分享深深的震撼了我，曾经我以为游戏引擎这种复杂且庞大的项目是不可能靠个人独立完成的，但是这位前辈改变了我的看法。那次分享在我心中种下了一颗开发个人引擎的种子，并且我也是找到了一个可以学习交流的前辈，于是ZXEngine项目正式起步了。

In 2022, in an internal technology sharing of our company, a senior made a sharing on the theme of game engine development. He showed us his independent-developed game engine, which was developed in his spare time and it was also modeled after Unity and was highly completed. It was not just a toy engine on the PC platform, but could already be packaged and released on the Android platform. That sharing deeply shocked me. I once thought that a complex and large project like a game engine could not be completed independently by an individual, but this senior changed my mind. That sharing planted a seed in my heart to develop my own game engine, and I also found a senior with whom I could learn and communicate, so the ZXEngine project started.

ZXEngine项目刚开始的时候我只学过OpenGL，并且由于工作中基本不用写C++，我的C++也就是大学生水平，只在学校里上课学过，以及写过前面说的那个OpenGL渲染小Demo。所以C++工程也是我一边做这个项目一边摸索着学习的。刚开始写这个项目的时候，我对现代C++的熟悉程度真的很低，所以早期写的代码有很多看起来不成熟的地方。随着对C++项目的慢慢熟悉，我的代码风格也有一些变化。所以你可能会看到两个不同模块的代码，由于编写时间间隔较长，连变量命名风格都不一致。而那些早期编写的模块，如果没有一些比较实际的原因，我也不会单纯的因为代码风格问题，或者觉得代码太粗糙而去专门重写。因为我想写的新东西还有不少，所以也就不想把精力放到没必要重写的老模块上。

When I started the ZXEngine project, I had only learned OpenGL. And since I basically didn't need to write C++ at work, my C++ was at the school level. I only learned it in class at university and wrote the OpenGL rendering demo mentioned above. So I kept learning C++ while working on this project. When I just started this project, I was not very familiar with modern C++, so there were many immature parts in the early code I wrote. As I gradually became familiar with C++ projects, my code style also changed a little. So you may see that the code of two different modules has inconsistent variable naming styles due to the long time interval between writing. For those codes that I wrote earlier, I wouldn't rewrite them just because of the style of the code, or because the code was too rough, unless there are some practical reasons. Because there are still many new things I want to write, I don't want to spend time on old modules that don't have to be rewritten.

这个工程我准备用自己工作之余的时间长期更新下去，慢慢完善，添加更高级的特性。也把这个工程当作自己一个学习技术的平台，有什么感兴趣的技术可以在自己这个工程里实验一下。也希望同样对引擎技术感兴趣的人可以一起分享。我这个项目有很多实现是自己瞎想的，还比较简单，换句话说也比较简陋。所以有大佬看到感觉写的不好的地方，欢迎提出建议。如果需要联系我的话可以发邮件给我，ashenvalezx@gmail.com，或者点我头像进主页来知乎私信我。

I will use my spare time to keep updating this project, gradually improve it, and add more advanced features. I also regard this project as a platform for myself to learn. If there is any technology that I am interested in, I can experiment with it in this project. I also hope that people who are also interested in game engine technology can share it together. Many of the implementations of this project are based on my own imagination and are relatively simple, in other words relatively crude. So if someone sees some implementations that are not good, suggestions are welcome. If you want to contact me, you can send me an email, ashenvalezx@gmail.com.

