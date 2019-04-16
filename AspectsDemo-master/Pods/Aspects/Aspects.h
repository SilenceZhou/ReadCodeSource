//
//  Aspects.h
//  Aspects - A delightful, simple library for aspect oriented programming.
//
//  Copyright (c) 2014 Peter Steinberger. Licensed under the MIT license.
//

#import <Foundation/Foundation.h>

typedef NS_OPTIONS(NSUInteger, AspectOptions) {
    AspectPositionAfter   = 0,            /// Called after the original implementation (default)
    AspectPositionInstead = 1,            /// Will replace the original implementation.
    AspectPositionBefore  = 2,            /// Called before the original implementation.
    
    AspectOptionAutomaticRemoval = 1 << 3 /// Will remove the hook after the first execution.
};

/// 不透明的Aspect Token，允许取消注册钩子
/// Opaque Aspect Token that allows to deregister the hook.
@protocol AspectToken <NSObject>

/// Deregisters an aspect.
/// @return YES if deregistration is successful, otherwise NO.
- (BOOL)remove;

@end

/// AspectInfo协议是我们的块语法的第一个参数。
/// The AspectInfo protocol is the first parameter of our block syntax.
@protocol AspectInfo <NSObject>

/// The instance that is currently hooked.
- (id)instance;

/// 钩子方法的原始调用。
/// The original invocation of the hooked method.
- (NSInvocation *)originalInvocation;

/// 所有方法参数，盒装。这是懒惰的评估。
/// All method arguments, boxed. This is lazily evaluated.
- (NSArray *)arguments;

@end

/**
 Aspects使用Objective-C消息转发来挂钩消息。这会产生一些开销。不要将方面添加到被调用的方法中。方面意味着视图/控制器代码，不是每秒1000次调用。
 添加方面返回一个不透明的标记，可用于再次注销。所有调用都是线程安全的
 
 Aspects uses Objective-C message forwarding to hook into messages. This will create some overhead. Don't add aspects to methods that are called a lot. Aspects is meant for view/controller code that is not called a 1000 times per second.

 Adding aspects returns an opaque token which can be used to deregister again. All calls are thread safe.
 */
@interface NSObject (Aspects)

/// Adds a block of code before/instead/after the current `selector` for a specific class.
///
/// @param block Aspects replicates the type signature of the method being hooked.
/// The first parameter will be `id<AspectInfo>`, followed by all parameters of the method.
/// These parameters are optional and will be filled to match the block signature.
/// You can even use an empty block, or one that simple gets `id<AspectInfo>`.
/// block Aspects复制被挂钩方法的类型签名。
/// 第一个参数是`id <AspectInfo>`，后跟方法的所有参数。
/// 这些参数是可选的，将被填充以匹配块签名。
/// 你甚至可以使用一个空块，或者一个简单得到`id <AspectInfo>`的空块。
///
/// @note Hooking static methods is not supported.
/// 不支持挂钩静态方法。
/// @return A token which allows to later deregister the aspect.
/// 允许稍后取消注册方面的令牌
+ (id<AspectToken>)aspect_hookSelector:(SEL)selector
                      withOptions:(AspectOptions)options
                       usingBlock:(id)block
                            error:(NSError **)error;

/// Adds a block of code before/instead/after the current `selector` for a specific instance.
- (id<AspectToken>)aspect_hookSelector:(SEL)selector
                      withOptions:(AspectOptions)options
                       usingBlock:(id)block
                            error:(NSError **)error;

@end



/**
 AspectErrorCode 错误码

 - AspectErrorSelectorBlacklisted: release, retain, autorelease等选择器被列入黑名单。
 - AspectErrorDoesNotRespondToSelector: 方法没找到
 - AspectErrorSelectorDeallocPosition: 挂钩dealloc时，只允许使用AspectPositionBefore。
 - AspectErrorSelectorAlreadyHookedInClassHierarchy: 不允许在子类中静态挂钩相同的方法。
 - AspectErrorFailedToAllocateClassPair: 运行时创建类对失败。
 - AspectErrorMissingBlockSignature: 块错过了编译时签名信息，无法调用
 - AspectErrorIncompatibleBlockSignature: 块签名与方法不匹配或太大。
 
 - AspectErrorRemoveObjectAlreadyDeallocated: （用于删除）已挂起的对象已被释放。
 */
typedef NS_ENUM(NSUInteger, AspectErrorCode) {
    AspectErrorSelectorBlacklisted,                   /// Selectors like release, retain, autorelease are blacklisted.
    AspectErrorDoesNotRespondToSelector,              /// Selector could not be found.
    AspectErrorSelectorDeallocPosition,               /// When hooking dealloc, only AspectPositionBefore is allowed.
    AspectErrorSelectorAlreadyHookedInClassHierarchy, /// Statically hooking the same method in subclasses is not allowed.
    AspectErrorFailedToAllocateClassPair,             /// The runtime failed creating a class pair.
    AspectErrorMissingBlockSignature,                 /// The block misses compile time signature info and can't be called.
    AspectErrorIncompatibleBlockSignature,            /// The block signature does not match the method or is too large.

    AspectErrorRemoveObjectAlreadyDeallocated = 100   /// (for removing) The object hooked is already deallocated.
};

extern NSString *const AspectErrorDomain;
