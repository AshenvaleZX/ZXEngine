#import "GameView.h"

@implementation GameView
{
    NSMutableDictionary<NSValue*, UITouch*>* _activeTouches;
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
        _activeTouches = [NSMutableDictionary dictionary];
    }
    return self;
}

// 当触控开始时
- (void)touchesBegan:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event
{
    [super touchesBegan:touches withEvent:event];
    for (UITouch* touch in touches)
    {
        NSValue* key = [NSValue valueWithNonretainedObject:touch];
        self->_activeTouches[key] = touch;
        CGPoint location = [touch locationInView:self];
        NSLog(@"Touch began at: %@ (Touch ID: %@)", NSStringFromCGPoint(location), key);
    }
}

// 当触控移动时
- (void)touchesMoved:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event
{
    [super touchesMoved:touches withEvent:event];
    for (UITouch* touch in touches)
    {
        NSValue* key = [NSValue valueWithNonretainedObject:touch];
        CGPoint location = [touch locationInView:self];
        NSLog(@"Touch moved to: %@ (Touch ID: %@)", NSStringFromCGPoint(location), key);
    }
}

// 当触控结束时
- (void)touchesEnded:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event
{
    [super touchesEnded:touches withEvent:event];
    for (UITouch* touch in touches)
    {
        NSValue* key = [NSValue valueWithNonretainedObject:touch];
        CGPoint location = [touch locationInView:self];
        NSLog(@"Touch ended at: %@ (Touch ID: %@)", NSStringFromCGPoint(location), key);
        [self->_activeTouches removeObjectForKey:key];
    }
}

// 当触控被取消时
- (void)touchesCancelled:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event
{
    [super touchesCancelled:touches withEvent:event];
    for (UITouch* touch in touches)
    {
        NSValue* key = [NSValue valueWithNonretainedObject:touch];
        NSLog(@"Touch cancelled (Touch ID: %@)", key);
        [self->_activeTouches removeObjectForKey:key];
    }
}

@end
