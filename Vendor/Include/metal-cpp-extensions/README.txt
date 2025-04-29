这个文件夹里的代码作用是将原本是Objective-C接口的AppKit和MetalKit库包装成C++接口的，就是个封装了NSWindow等常用接口的桥接层。

苹果的metal-cpp只有Metal核心库，不包括AppKit和MetalKit，所以如果需要使用这两个库得自己封装。不过这里的封装其实也是苹果写的，只是没有以一种很正式很官方的方式推出，是从苹果官网的 Metal sample code 里的 Learn Metal with C++ 示例中获取的。

Metal sample code 链接：
https://developer.apple.com/metal/sample-code/
metal-cpp-extension 下载链接：
https://developer.apple.com/metal/LearnMetalCPP.zip

metal-cpp-extension 并不是一个正式发布的拓展，所以没有明确的版本管理，对AppKit和MetalKit的封装也并不完整。如果需要更新可以通过上面的下载链接下载最新版的，或者自己手动去封装一下自己需要但是这里没提供的接口。

目前这里的代码是从上面给的链接下载后又自己稍微扩展了一点的。