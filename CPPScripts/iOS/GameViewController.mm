#import "GameViewController.h"
#include "Game.h"
#include "../RenderAPIMetal.h"

@implementation GameView

+ (Class)layerClass
{
    return [CAMetalLayer class];
}

@end

@implementation GameViewController
{
    CADisplayLink* displayLink;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    ZXEngine::Game::Launch("");
    
    auto renderAPI = static_cast<ZXEngine::RenderAPIMetal*>(ZXEngine::RenderAPI::GetInstance());
    self.view = [[GameView alloc] initWithFrame:self.view.frame];
    renderAPI->SetMetalLayer((__bridge CA::MetalLayer*)(CAMetalLayer*)self.view.layer);
    
    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(gameLoop)];
    displayLink.preferredFramesPerSecond = 60;
    [displayLink addToRunLoop:NSRunLoop.currentRunLoop forMode:NSDefaultRunLoopMode];
}

- (void)gameLoop
{
    ZXEngine::Game::Loop();
}

- (void)dealloc
{
    ZXEngine::Game::Shutdown();
}

@end
