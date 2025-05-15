#import "GameView.h"
#include "InputManagerIOS.h"

@implementation GameView
{
    NSMutableDictionary<NSValue*, NSNumber*>* _activeToucheIDs;
    NSMutableSet<NSNumber*>* _availableIDs;
    NSInteger _nextID;
}

+ (Class)layerClass
{
    return [CAMetalLayer class];
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        // 启用多点触控
        self.multipleTouchEnabled = YES;

        _activeToucheIDs = [NSMutableDictionary dictionary];
        _availableIDs = [NSMutableSet set];
        _nextID = 0;
    }
    return self;
}

- (void)layoutSubviews
{
    [super layoutSubviews];
    CAMetalLayer* mtLayer = (CAMetalLayer*)self.layer;
    mtLayer.frame = self.bounds;
}

- (NSInteger)getTouchKeyID:(NSValue*)key
{
    NSNumber* assignedID = _activeToucheIDs[key];

    if (assignedID)
    {
        return [assignedID integerValue];
    }

    if (_availableIDs.count > 0)
    {
        assignedID = [_availableIDs anyObject];
        [_availableIDs removeObject:assignedID];
    }
    else
    {
        assignedID = @(_nextID);
        _nextID++;
    }
    _activeToucheIDs[key] = assignedID;

    return [assignedID integerValue];
}

- (void)releaseTouchKeyID:(NSValue*)key
{
    NSNumber* assignedID = _activeToucheIDs[key];
    if (assignedID)
    {
        [_activeToucheIDs removeObjectForKey:key];
        [_availableIDs addObject:assignedID];
    }
}

// 当触控开始时
- (void)touchesBegan:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event
{
    [super touchesBegan:touches withEvent:event];

    auto inputManager = static_cast<ZXEngine::InputManagerIOS*>(ZXEngine::InputManager::GetInstance());

    for (UITouch* touch in touches)
    {
        NSValue* key = [NSValue valueWithNonretainedObject:touch];
        CGPoint location = [touch locationInView:self];
        NSInteger touchID = [self getTouchKeyID:key];
        inputManager->UpdateTouch(
            static_cast<int32_t>(touchID),
            static_cast<float>(location.x),
            static_cast<float>(location.y)
        );
    }
}

// 当触控移动时
- (void)touchesMoved:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event
{
    [super touchesMoved:touches withEvent:event];

    auto inputManager = static_cast<ZXEngine::InputManagerIOS*>(ZXEngine::InputManager::GetInstance());

    for (UITouch* touch in touches)
    {
        NSValue* key = [NSValue valueWithNonretainedObject:touch];
        CGPoint location = [touch locationInView:self];
        NSInteger touchID = [self getTouchKeyID:key];
        inputManager->UpdateTouch(
            static_cast<int32_t>(touchID),
            static_cast<float>(location.x),
            static_cast<float>(location.y)
        );
    }
}

// 当触控结束时
- (void)touchesEnded:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event
{
    [super touchesEnded:touches withEvent:event];

    auto inputManager = static_cast<ZXEngine::InputManagerIOS*>(ZXEngine::InputManager::GetInstance());

    for (UITouch* touch in touches)
    {
        NSValue* key = [NSValue valueWithNonretainedObject:touch];
        NSInteger touchID = [self getTouchKeyID:key];
        inputManager->RemoveTouch(static_cast<int32_t>(touchID));
        [self releaseTouchKeyID:key];
    }
}

// 当触控被取消时
- (void)touchesCancelled:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event
{
    [super touchesCancelled:touches withEvent:event];

    auto inputManager = static_cast<ZXEngine::InputManagerIOS*>(ZXEngine::InputManager::GetInstance());

    for (UITouch* touch in touches)
    {
        NSValue* key = [NSValue valueWithNonretainedObject:touch];
        NSInteger touchID = [self getTouchKeyID:key];
        inputManager->RemoveTouch(static_cast<int32_t>(touchID));
        [self releaseTouchKeyID:key];
    }
}

@end
