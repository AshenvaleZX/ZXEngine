#import "GameViewController.h"
#include "Game.h"
#include "../RenderAPIMetal.h"

@implementation GameViewController
{
    CADisplayLink* displayLink;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    ZXEngine::Game::Launch("");
    
    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(gameLoop)];
    displayLink.preferredFramesPerSecond = 60;
    [displayLink addToRunLoop:NSRunLoop.currentRunLoop forMode:NSDefaultRunLoopMode];
    
    auto renderAPI = static_cast<ZXEngine::RenderAPIMetal*>(ZXEngine::RenderAPI::GetInstance());
    CAMetalLayer* metalLayer = (__bridge CAMetalLayer*)renderAPI->GetMetalLayer();
    [self.view.layer addSublayer:metalLayer];
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
