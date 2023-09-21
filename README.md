# ZXEngine

这是我自己开发的游戏引擎项目，创建这个项目的主要目的是为了我自己学习和实践游戏引擎技术，不过也希望这个项目能对你有所帮助。

This is a game engine project developed by myself. The main purpose of creating this project is to learn and practice game engine technology for myself, but it would be better if this project can help you.

截图展示(介绍后面有更多展示):

Screenshot display (more display later in the introduction):

![](https://github.com/AshenvaleZX/ZXEngine/blob/master/Documents/Images/EngineShow0.jpg)

## 引擎简介 (Engine Introduction)

本引擎目前同时支持Vulkan，DirectX 12和OpenGL，使用自创的zxshader语言来编写shader，支持前面三种图形API，可一次编写3种环境运行。同时也支持光线追踪渲染管线(基于Vulkan)。

This engine currently supports Vulkan, DirectX 12 and OpenGL. The engine uses the self-created zxshader language to write shaders. It also supports Vulkan, DirectX 12 and OpenGL. You only need to write it once and it can work in all three graphics APIs. This engine also supports ray tracing rendering pipeline (Only based on Vulkan for now).

本项目是游戏引擎项目，不是单纯的渲染引擎。所以还内置了我自己写的物理引擎PhysZ(学习和改写自Cyclone引擎)，支持基本的刚体力学模拟。同时我也开发了简单的骨骼蒙皮动画系统，粒子系统等。文档后面会有这些系统的图片展示。

This project is a game engine project, not just a rendering engine. Therefore, it also has a built-in physics engine written by myself, which I called it PhysZ (learn and rewritten from the Cyclone engine), supports rigid body mechanics simulation. And I also developed a simple skeletal animation system, particle system, etc. Images of these systems are shown later in this document.

引擎本身用C++开发，GamePlay层使用Lua开发，引擎层封装部分C++接口给GamePlay层的Lua调用。使用方式类似Unity的XLua，通过一个GameLogic组件把Lua代码绑定到GameObject上，接收所挂载对象上来自引擎的Start和Update调用，并通过self访问GameObject对象(具体示例看后面)。

The engine is developed with C++, the GamePlay layer is developed with Lua, and the engine encapsulates part of the C++ interface to the Lua call of the GamePlay layer. The usage is similar to Unity's XLua, you can bind the Lua code to the GameObject through a GameLogic component, receive Start and Update calls from the engine on the mounted object, and access the GameObject object through self (see later for specific examples).

项目目前还比较简单，不过我已经完成了引擎所需的基本的场景，预制体，材质系统，shader文件系统等等的开发。本引擎有一个类Unity的引擎编辑器页面。场景中的对象都是Unity式的 GameObject - Component 结构。

The project is still relatively simple, but I have completed the development of the basic scenes, prefabs, materials system, shader file systems and so on that the engine need. This engine has a Unity-like engine editor. Objects in the scene are Unity-style GameObject - Component structures.

## 细节图片展示 (Detailed picture display)

下面是一些图片展示，引擎页面如图，和Unity编辑器类似。上面是主菜单栏，中间是游戏画面窗口，左边是展示场景中GameObject的Hierarchy面板，下面左边是展示当前打开项目的Assets文件目录的Project面板，中间是展示日志的Console面板，右边是展示当前选中对象详细信息的Inspector面板。

The following are some picture displays, the engine is shown in the figure, which is similar to the Unity editor. The top is the main menu bar, the middle is the game screen window, the left is the Hierarchy panel showing the GameObject in the scene, the lower left is the Project panel showing the Assets file directory of the currently opened project, the middle is the Console panel showing the log, and the right is showing the currently selected Inspector panel for object details.

![](https://github.com/AshenvaleZX/ZXEngine/blob/master/Documents/Images/EngineShow1.jpg)

当前选中了场景中的Sun对象，Inspector面板就展示这个GameObject所挂载的Component信息。

The Sun object in the scene is currently selected, and the Inspector panel displays the Component information on the GameObject.

点击主菜单栏的运行按钮，游戏开始运行，此时能看到场景中物体动了起来，粒子系统也开始工作。

Click the Play button on the main menu bar, and the game starts to run. At this time, you can see the objects in the scene move, and the particle system starts to work. 

接下来展示的是一个基于PBR+IBL场景渲染，同时展示了点击材质文件后Inspector面板展示的材质信息和材质预览。

Next picture is a scene rendering based on PBR+IBL, and also shows the material information and material preview displayed on the Inspector panel after clicking the material file.

![](https://github.com/AshenvaleZX/ZXEngine/blob/master/Documents/Images/EngineShow2.jpg)

然后通过双击场景文件，切换到一个使用光线追踪渲染管线渲染的场景。同时展示了点击模型文件后Inspector面板展示的模型信息和模型预览，如果模型带有动画还会显示骨骼和动画信息。

Then switch to a scene rendered with the ray tracing pipeline by double-clicking on the scene file. At the same time, it shows the model information and model preview displayed on the Inspector panel after clicking the model file. If the model has animation, the skeleton and animation information will also be displayed.

![](https://github.com/AshenvaleZX/ZXEngine/blob/master/Documents/Images/EngineShow3.jpg)

接下来展示的是通过算法生成的体积云及其阴影投射效果。同时展示了点击图片文件后，Inspector面板显示的图像信息。

Shown next are code-generated volumetric clouds and their shadow casting effects. And the right side shows the image information displayed on the Inspector panel after clicking the image file.

![](https://github.com/AshenvaleZX/ZXEngine/blob/master/Documents/Images/EngineShow4.jpg)

下面是分别点击zxshader和Lua后Inspector上的代码预览。

The following is the code preview on the Inspector after clicking zxshader and Lua respectively.

![](https://github.com/AshenvaleZX/ZXEngine/blob/master/Documents/Images/EngineShow5.jpg)

![](https://github.com/AshenvaleZX/ZXEngine/blob/master/Documents/Images/EngineShow6.jpg)

## PhysZ引擎简介(PhysZ Engine Introduction)

先展示一下PhysZ引擎对刚体力学的模拟效果(GIF演示，截屏大小和帧率都有压缩):

Here is a GIF of the simulation of rigid mechanics in PhysZ (the size and framerate of the screenshots are compressed):

![](https://github.com/AshenvaleZX/ZXEngine/blob/master/Documents/Images/PhysZ0.gif)

使用PhysZ物理引擎只需要关心两个Component，Collider和Rigidbody。其中Collider目前提供了三种类型：BoxCollider，PlaneCollider和SphereCollider。下面是这两个Component在引擎里编辑器里的截图：

Using the PhysZ engine involves only two Components, Collider and Rigidbody. Collider currently provides three types: BoxCollider, PlaneCollider and SphereCollider. The following are screenshots of these two Components in the editor in the engine:

![](https://github.com/AshenvaleZX/ZXEngine/blob/master/Documents/Images/PhysZ1.png)

各参数作用顾名思义即可。其中Damping参数是模拟各种形式的阻尼效果的，Linear Damping和Angular Damping分别对应线性运动和角运动，数值为0代表无阻尼。

The function of each parameter is just as the name implies. And the Damping parameter is used to simulate various forms of damping effects. Linear Damping and Angular Damping correspond to linear motion and angular motion respectively. A value of 0 represents no damping.

## 骨骼蒙皮动画(Skeletal Animation)

切换到AnimDemo场景，点击运行按钮后，可以看到骨骼蒙皮动画的展示:

Switch to the AnimDemo scene and click Play button to see the skeletal animation display:

![](https://github.com/AshenvaleZX/ZXEngine/blob/master/Documents/Images/Anim.gif)

本引擎的动画系统支持动画混合，所以这里除了能看到单个动画播放外，还能看到人物从走路状态进入跑步状态时，两个动画之间流畅的过渡。

The animation system of this engine supports animation blending, so in addition to the individual animation playback, you can also see the smooth transition between the two animations when the character enters the running state from the walking state.

这里动画的播放代码属于业务逻辑模块而不是引擎，所以是写在Lua代码中的，通过GameLogic组件绑定到对应的GameObject上。

The code to play the animation belongs to the GamePlay rather than the engine, so it‘s written in the Lua code and bound to the GameObject through the GameLogic component.

## 引擎文件格式介绍 (Engine file format introduction)

### *.zxscene

场景文件，包含GameObjects，天空盒等。如果是光线追踪场景，还包含了光追管线的Shader。

Scene files, containing GameObjects, skyboxes, etc. If it is a ray tracing scene, it also includes the Shader of the light tracing pipeline.

### *.zxshader

这是本引擎自己的shader语言文件，不过目前zxshader仅支持DirectX 12，Vulkan和OpenGL的光栅化渲染管线。示例代码可以在ExampleProject\Assets\Shaders中找到。

This is ZXEngine's own shader language file, but currently zxshader only supports the rasterization rendering pipeline of DirectX 12, Vulkan and OpenGL. Example code can be found in ExampleProject\Assets\Shaders.

### *.zxmat  *.zxrtmat

分别是光栅化渲染管线和光线追踪渲染管线的材质文件。

They are the material files of the rasterization rendering pipeline and the ray tracing rendering pipeline respectively.

### *.zxprefab

预制体文件，和Unity的prefab差不多。

The prefab file is similar to Unity's prefab.

### *.zxprjcfg

由ZXEngine创建的游戏项目工程的配置文件，可以在ExampleProject中找到参考示例。

This is the configuration file for the game project created by ZXEngine, you can find the example in ExampleProject.

### *.rgen  *.rmiss  *.rchit  *.rahit  *.rint

光线追踪管线的各阶段Shader代码文件。因为目前引擎只支持基于Vulkan的光线追踪，所以暂未向光栅化管线那样，搞一个引擎专有的通用语言格式。

Shader code files for each stage of the ray tracing pipeline. Because the current engine only supports Vulkan ray tracing, I have not yet created an engine-specific common language format like zxshader in the rasterization pipeline.

### Others

模型，纹理贴图，字体等就是常见的通用文件格式。

Models, textures, fonts, etc. are common file formats.

##        GamePlay层的Lua代码示例(Lua code example for GamePlay layer)

以控制GameObject旋转移动举例，创建一个Lua代码，用GameLogic组件挂在到一个GameObject对象上：

Take controlling the rotation and movement of GameObject as an example, create a Lua code, and use GameLogic component on a GameObject object:

![](https://github.com/AshenvaleZX/ZXEngine/blob/master/Documents/Images/GameLogic.png)

然后Lua代码大致如下：

Then the Lua code is roughly as follows:

```lua
local ObjectMove = NewGameLogic()

ObjectMove.radius = 20
ObjectMove.angle = 0
ObjectMove.rot = 0

function ObjectMove:Start()
    self.trans = self.gameObject:GetComponent("Transform")
end

function ObjectMove:Update()
    self.angle = self.angle + 2 * Time.GetDeltaTime()
    local x = math.sin(self.angle) * self.radius
    local z = math.cos(self.angle) * self.radius
    self.trans:SetPosition(x, 0, z)

    self.rot = self.rot + 50 * Time.GetDeltaTime()
    if self.rot > 360 then
        self.rot = self.rot - 360
    end
    self.trans:SetEulerAngles(0, 0, self.rot)
end

return ObjectMove
```

## 注意事项 (Precautions)

目前zxshader编写好后，在DirectX 12和OpenGL下直接运行即可。但是在Vulkan下需要先点击引擎菜单栏里的“Assets/Compile All Shader for Vulkan”按钮，将zxshader预编译后才可以运行。也就是说，在你第一次以Vulkan模式运行工程之前，至少需要先以DirectX 12或者OpenGL模式启动一次项目，完成一次预编译Vulkan着色器，然后才能正常的以Vulkan模式启动。这块做得不是很好，按理说不应该有这种奇怪的限制，日后改进。

After zxshader is written, it can be run directly under DirectX 12 and OpenGL. But under Vulkan, you need to click the "Assets/Compile All Shader for Vulkan" button in the engine menu bar to precompile zxshader before it can run. In other words, before you run the project in Vulkan for the first time, you need to start the project in DirectX 12 or OpenGL at least once, complete a precompilation of the Vulkan shader, and then start engine in Vulkan. It's not very well done here, there shouldn't be such a strange restriction, and it will be improved in the future.

切换图形API的方式是在pubh.h文件中修改宏定义。有3个宏定义ZX_API_VULKAN，ZX_API_D3D12和ZX_API_OPENGL，分别对应引擎支持的3个图形API。开启其中一个宏定义，并注释掉其它2个宏定义，重新编译引擎，就完成了对图形API的切换。

The way to switch the graphics API is to modify the macro definition in the pubh.h file. There are 3 macro definitions ZX_API_VULKAN, ZX_API_D3D12 and ZX_API_OPENGL, corresponding to the 3 graphics APIs supported by the engine. Open one of the macro definitions, comment out the other two macro definitions, and recompile the engine to complete the switching of the graphics API.

### 一些废话 (Some mumbles)

其实工程在2020年5月就创建了，但是最初其实只是想把2020年初学习OpenGL写的代码保存到GitHub上而已。当时写的代码也只是C语言风格的面向过程式编程，一个1000多行的渲染demo。随后花了1年多时间把《Real-Time Rendering 4th》看了，这期间光看书没再实际写东西了（因为这书实在是太偏理论了，没什么可以立刻实践的东西）。

In fact, the project was created in May 2020, but at first I just wanted to save the code I wrote when I learned OpenGL in early 2020 to GitHub. The code I wrote at that time was only procedure oriented programming in C language style, a rendering demo with less than 2000 lines. Then I spent more than a year reading "Real-Time Rendering 4th". During this period, I just read the book and didn't actually write anything (because this book is too theoretical, and there is nothing that can be practiced immediately).

其实当时那个1000多行的demo写完我还想继续加点东西上去，但是感觉加不动了，面向过程的代码实在是难扩展。就一直想把这个demo改成一个正式一点的，有渲染框架的工程，但是不知道从何改起，就一直没动，先看书去了。书看完了还是不知道怎么开始，就沉寂了几个月。最后等到了2022年我才开始正式搭建这个工程，准备开发成一个自己的简单游戏引擎。其实我当时应该建一个新的GitHub仓库的，而不是用这个学习OpenGL的仓库。但是当时对C++工程还很陌生，感觉各种环境配置和库链接太麻烦了，就偷懒直接用这个已经搭好的OpenGL开发环境开始搭建引擎了。所以之前写的demo代码还遗留在这里面的。

In fact, after the demo was written, I wanted to continue to add something, but I felt that it couldn't be done, because the procedure oriented code was really difficult to expand. I have always wanted to make this demo a more formal project with a rendering framework, but I don't know where to start, so I haven't moved, and I read the book first. After reading the book, I still didn't know how to start, so I fell silent for several months. Finally, in 2022, I started to build this project, preparing to develop a simple game engine of my own. In fact, I should have built a new GitHub repository instead of using this repository for learning OpenGL. But at that time, I was still very unfamiliar with C++ projects. I felt that various environment configurations and library links were too troublesome, so I was slack off and started building the engine directly with this already established OpenGL development environment. So the demo code written before is still left here.

这个工程我准备用自己工作之余的时间长期更新下去，慢慢完善，添加更高级的特性。也把这个工程当作自己一个学习技术的平台，有什么感兴趣的技术可以在自己这个工程里实验一下。也希望同样对引擎技术感兴趣的人可以一起分享。因为我之前沉寂了几个月迟迟没有开始，就是因为不知道从何开始。我想找一个简单的参考项目，但是找不到。要么就是UE5这种过于庞大和成熟的引擎，要么就是一些很老的比如Ogre这样的引擎，反正我没有找到一个合适的项目让我学习。我这个项目有很多实现是自己瞎想的，还比较简单，换句话说也比较简陋。所以有大佬看到感觉写的不好的地方，欢迎提出建议。可以发邮件给我，ashenvalezx@gmail.com

I plan to use my spare time to keep updating this project, gradually improve it, and add more advanced features. I also regard this project as a platform for my own learning. I can experiment with any interesting technologies in my own project. I also hope that people who are also interested in game engine technology can share it together. I waited for months and didn't start the project because I didn't know where to start. I'm trying to find a relatively simple project to learn from, but can't find it. Either it is an overly large and mature engine like UE5, or some very old engines such as Ogre. Anyway, I didn't find a suitable project for me to learn. Many of the implementations of my project are thought by myself, and they are relatively simple, in other words, they are relatively crude. So if someone sees some implementations that are not good, suggestions are welcome. You can email me, ashenvalezx@gmail.com

