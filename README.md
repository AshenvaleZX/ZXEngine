# ZXEngine

------

这个项目是我个人为了学习游戏引擎技术创建的。目前同时支持Vulkan和OpenGL，并且正在开发对DirectX 12的支持。

引擎本身用C++开发，GamePlay层使用Lua开发，引擎层封装部分C++接口给GamePlay层的Lua调用。使用方式类似Unity的XLua，不过目前已封装的接口还比较少。可以通过一个GameLogic组件把Lua代码绑定到GameObject上，接收所挂载对象上来自引擎的Start和Update调用，并通过self访问GameObject对象(具体示例看后面)。

本引擎使用自创的zxshader格式来编写shader代码。因为是跨API引擎，而各API之间的shader语法有差异，所以为了不用给每个图形API都写一次shader，不得不自己定义一个格式来写shader。

项目目前还比较简单，不过完成了引擎所需的基本的场景，预制体，材质，shader文件系统。有一个类Unity的引擎编辑器页面。

下面是一些图片展示，引擎页面如图，和Unity编辑器类似。上面是主菜单栏，中间是游戏画面窗口，左边是展示场景中GameObject的Hierarchy面板，下面左边是展示当前打开项目的Assets文件目录的Project面板，中间是展示日志的Console面板，右边是展示当前选中对象详细信息的Inspector面板。

![](https://github.com/AshenvaleZX/ZXEngine/blob/fd5c2450224b8981463fd16798026d5679b49c59/Documents/Images/Engine%20Show%201.png)

当前选中了场景中的Sun对象，Inspector面板就展示这个GameObject所挂载的Component信息。

点击主菜单栏的开始按钮，游戏开始运行，此时能看到场景中物体动了起来，粒子系统也开始工作。此时选中一个材质，Inspectors面板就开始展示材质信息和材质球。

![](https://github.com/AshenvaleZX/ZXEngine/blob/fd5c2450224b8981463fd16798026d5679b49c59/Documents/Images/Engine%20Show%202.png)

再选中一个模型，Inspector面板就展示模型信息和模型预览。

![](https://github.com/AshenvaleZX/ZXEngine/blob/fd5c2450224b8981463fd16798026d5679b49c59/Documents/Images/Engine%20Show%203.png)

下面是点击图像展示图像信息。

![](https://github.com/AshenvaleZX/ZXEngine/blob/fd5c2450224b8981463fd16798026d5679b49c59/Documents/Images/Engine%20Show%204.png)

下面是分别点击zxshader和Lua后Inspector上的代码预览。

![](https://github.com/AshenvaleZX/ZXEngine/blob/fd5c2450224b8981463fd16798026d5679b49c59/Documents/Images/Engine%20Show%205.png)

![](https://github.com/AshenvaleZX/ZXEngine/blob/fd5c2450224b8981463fd16798026d5679b49c59/Documents/Images/Engine%20Show%206.png)

###        GamePlay层的Lua代码示例

以控制GameObject旋转移动举例，创建一个Lua代码，用GameLogic组件挂在到一个GameObject对象上：

![](https://github.com/AshenvaleZX/ZXEngine/blob/fd5c2450224b8981463fd16798026d5679b49c59/Documents/Images/GameLogic.png)

然后Lua代码大致如下：

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

写一些碎碎念，当是记录一下心路历程吧：

Some mumbles:

其实工程在2020年5月就创建了，但是最初其实只是想把2020年初学习OpenGL写的代码保存到GitHub上而已。当时写的代码也只是C语言风格的面向过程式编程，一个1000多行的渲染demo。随后花了1年多时间把《Real-Time Rendering 4th》看了，这期间光看书没再实际写东西了（因为这书实在是太偏理论了，没什么可以立刻实践的东西）。

In fact, the project was created in May 2020, but at first I just wanted to save the code I wrote when I learned OpenGL in early 2020 to GitHub. The code I wrote at that time was only procedure oriented programming in C language style, a rendering demo with less than 2000 lines. Then I spent more than a year reading "Real-Time Rendering 4th". During this period, I just read the book and didn't actually write anything (because this book is too theoretical, and there is nothing that can be practiced immediately).

其实当时那个1000多行的demo写完我还想继续加点东西上去，但是感觉加不动了，面向过程的代码实在是难扩展。就一直想把这个demo改成一个正式一点的，有渲染框架的工程，但是不知道从何改起，就一直没动，先看书去了。书看完了还是不知道怎么开始，就沉寂了几个月。最后于2022年8月我开始正式搭建这个工程，准备开发成一个自己的简单游戏引擎。其实我当时应该建一个新的GitHub仓库的，而不是用这个学习OpenGL的仓库。但是当时对C++工程还很陌生，感觉各种环境配置和库链接太麻烦了，就偷懒直接用这个已经搭好的OpenGL开发环境开始搭建引擎了。所以之前写的demo代码还遗留在这里面的。

In fact, after the demo was written, I wanted to continue to add something, but I felt that it couldn't be done, because the procedure oriented code was really difficult to expand. I have always wanted to make this demo a more formal project with a rendering framework, but I don't know where to start, so I haven't moved, and I read the book first. After reading the book, I still didn't know how to start, so I fell silent for several months. Finally, in August 2022, I started to build this project, preparing to develop a simple game engine of my own. In fact, I should have built a new GitHub repository instead of using this repository for learning OpenGL. But at that time, I was still very unfamiliar with C++ projects. I felt that various environment configurations and library links were too troublesome, so I was slack off and started building the engine directly with this already established OpenGL development environment. So the demo code written before is still left here.

这个工程我准备用自己工作之余的时间长期更新下去，慢慢完善，添加更高级的特性。也把这个工程当作自己一个学习技术的平台，有什么感兴趣的技术可以在自己这个工程里实验一下。也希望同样对引擎技术感兴趣的人可以一起分享。因为我之前沉寂了几个月迟迟没有开始，就是因为不知道从何开始。我想找一个简单的参考项目，但是找不到。要么就是UE5这种过于庞大和成熟的引擎，要么就是一些很老的比如Ogre这样的引擎，反正我没有找到一个合适的项目让我学习。我这个项目有很多实现是自己瞎想的，还比较简单，换句话说也比较简陋。所以有大佬看到感觉写的不好的地方，欢迎提出建议。可以发邮件给我，ashenvalezx@gmail.com

I plan to use my spare time to keep updating this project, gradually improve it, and add more advanced features. I also regard this project as a platform for my own learning. I can experiment with any interesting technologies in my own project. I also hope that people who are also interested in game engine technology can share it together. I waited for months and didn't start the project because I didn't know where to start. I'm trying to find a relatively simple project to learn from, but can't find it. Either it is an overly large and mature engine like UE5, or some very old engines such as Ogre. Anyway, I didn't find a suitable project for me to learn. Many of the implementations of my project are thought by myself, and they are relatively simple, in other words, they are relatively crude. So if someone sees some implementations that are not good, suggestions are welcome. You can email me, ashenvalezx@gmail.com

