#import "GameViewController.h"
#import "GameView.h"
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
    
    auto renderAPI = static_cast<ZXEngine::RenderAPIMetal*>(ZXEngine::RenderAPI::GetInstance());
    self.view = [[GameView alloc] initWithFrame:self.view.frame];
    renderAPI->SetMetalLayer((__bridge CA::MetalLayer*)(CAMetalLayer*)self.view.layer);
    
    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(gameLoop)];
    displayLink.preferredFramesPerSecond = 60;
    [displayLink addToRunLoop:NSRunLoop.currentRunLoop forMode:NSDefaultRunLoopMode];
}

- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];

    [coordinator animateAlongsideTransition:^(id<UIViewControllerTransitionCoordinatorContext> context)
    {
        CGFloat scale = [[UIScreen mainScreen] nativeScale];
        ZXEngine::RenderAPI::GetInstance()->OnWindowSizeChange(
            static_cast<uint32_t>(size.width * scale),
            static_cast<uint32_t>(size.height * scale)
        );
    } completion:nil];
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
