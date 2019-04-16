//
//  LoggingConfig.m
//  Test
//
//  Created by Peng Gu on 12/16/14.
//  Copyright (c) 2014 Peng Gu. All rights reserved.
//

#import "GLLogging.h"


@import UIKit;


@implementation GLLogging


typedef void (^AspectHandlerBlock)(id<AspectInfo> aspectInfo);


+ (void)setupWithConfiguration:(NSDictionary *)configs
{
    // Hook Page Impression
    [UIViewController aspect_hookSelector:@selector(viewDidAppear:)
                              withOptions:AspectPositionAfter
                               usingBlock:^(id<AspectInfo> aspectInfo) {
                                   
                                   dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                                       
                                       
                                       NSString *className = NSStringFromClass([[aspectInfo instance] class]);
                                       
                                       NSString *pageImp = configs[className][GLLoggingPageImpression];
                                       
                                       if (pageImp) {
                                           NSLog(@"%@", pageImp);
                                       }
                                   });
                                   
                               } error:NULL];

    // Hook Events
    
    /// 处理预先定义好的方法 与 方法体里面执行内容
    for (NSString *className in configs) {
        Class clazz = NSClassFromString(className);
        NSDictionary *config = configs[className];
        
        if (config[GLLoggingTrackedEvents]) {// 定义需要截取的方法：预先定义好
            
            for (NSDictionary *event in config[GLLoggingTrackedEvents]) {//每个截取的方法
                
                SEL selekor = NSSelectorFromString(event[GLLoggingEventSelectorName]); ///通过获得方法名获得方法
                /*
                 event[GLLoggingEventHandlerBlock] 为 block ，预先定义的事件
                 ^(id<AspectInfo> aspectInfo) {
                 NSLog(@"button one clicked"); /// 打印
                 },
                 */
                AspectHandlerBlock block = event[GLLoggingEventHandlerBlock];/// 
                
                [clazz aspect_hookSelector:selekor
                               withOptions:AspectPositionAfter
                                usingBlock:^(id<AspectInfo> aspectInfo) {
                                    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                                        block(aspectInfo);
                                    });
                                } error:NULL];
                
            }
        }
    }
}


@end




