//
//  LCRTMConnection_Internal.h
//  AVOS
//
//  Created by zapcannon87 on 2020/5/31.
//  Copyright © 2020 LeanCloud Inc. All rights reserved.
//

#import "LCRTMConnection.h"
#import "LCRTMWebSocket.h"

#if !TARGET_OS_WATCH
#import "LCNetworkReachabilityManager.h"
#endif

#import <TargetConditionals.h>

#if TARGET_OS_IOS || TARGET_OS_TV
#import <UIKit/UIKit.h>
typedef NS_ENUM(NSUInteger, LCRTMConnectionAppState) {
    LCRTMConnectionAppStateBackground,
    LCRTMConnectionAppStateForeground,
};
#endif

@interface LCRTMConnectionManager ()

@property (nonatomic) NSLock *lock;
@property (nonatomic) NSMutableDictionary<NSString *, NSNumber *> *connectingDelayIntervalMap;

- (NSInteger)nextConnectingDelayIntervalForApplication:(AVApplication *)application;

- (void)resetConnectingDelayIntervalForApplication:(AVApplication *)application;

@end

@interface LCRTMConnectionOutCommand : NSObject

@property (nonatomic, readonly) NSString *peerID;
@property (nonatomic, readonly) AVIMGenericCommand *command;
@property (nonatomic, readonly) dispatch_queue_t callingQueue;
@property (nonatomic, readonly) NSMutableArray<LCRTMConnectionOutCommandCallback> *callbacks;
@property (nonatomic, readonly) NSDate *expiration;

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;

- (instancetype)initWithPeerID:(NSString *)peerID
                       command:(AVIMGenericCommand *)command
                  callingQueue:(dispatch_queue_t)callingQueue
                      callback:(LCRTMConnectionOutCommandCallback)callback NS_DESIGNATED_INITIALIZER;

- (BOOL)microIdempotentFor:(AVIMGenericCommand *)outCommand
                      from:(NSString *)peerID
                     queue:(dispatch_queue_t)queue;

@end

@interface LCRTMConnectionTimer : NSObject

@property (nonatomic, readonly) NSTimeInterval pingpongInterval;
@property (nonatomic, readonly) NSTimeInterval pingTimeout;
@property (nonatomic) NSTimeInterval lastPingSentTimestamp;
@property (nonatomic) NSTimeInterval lastPongReceivedTimestamp;
@property (nonatomic) dispatch_source_t source;
@property (nonatomic) LCRTMWebSocket *socket;
@property (nonatomic) NSMutableArray<NSNumber *> *outCommandIndexSequence;
@property (nonatomic) NSMutableDictionary<NSNumber *, LCRTMConnectionOutCommand *> *outCommandCollection;
@property (nonatomic) SInt32 index;
#if DEBUG
@property (nonatomic, readonly) dispatch_queue_t queue;
#endif

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;

- (instancetype)initWithQueue:(dispatch_queue_t)queue
                       socket:(LCRTMWebSocket *)socket NS_DESIGNATED_INITIALIZER;

- (void)receivePong;

- (BOOL)tryThrottling:(AVIMGenericCommand *)outCommand
                 from:(NSString *)peerID
                queue:(dispatch_queue_t)queue
             callback:(LCRTMConnectionOutCommandCallback)callback;

- (void)appendOutCommand:(LCRTMConnectionOutCommand *)outCommand
                   index:(NSNumber *)index;

- (void)handleCallbackCommand:(AVIMGenericCommand *)inCommand;

- (SInt32)nextIndex;

- (void)cleanInCurrentQueue:(BOOL)inCurrentQueue;

@end

@interface LCRTMConnection () <LCRTMWebSocketDelegate>

@property (nonatomic) dispatch_queue_t serialQueue;
#if TARGET_OS_IOS || TARGET_OS_TV
@property (nonatomic) LCRTMConnectionAppState previousAppState;
@property (nonatomic) id<NSObject> enterBackgroundObserver;
@property (nonatomic) id<NSObject> enterForegroundObserver;
#endif
#if !TARGET_OS_WATCH
@property (nonatomic) LCNetworkReachabilityStatus previousReachabilityStatus;
@property (nonatomic) LCNetworkReachabilityManager *reachabilityManager;
#endif
@property (nonatomic) NSString *defaultInstantMessagingPeerID;
@property (nonatomic) BOOL needPeerIDForEveryCommandOfInstantMessaging;
@property (nonatomic) LCRTMConnectionTimer *timer;
@property (nonatomic) LCRTMWebSocket *socket;
@property (nonatomic) dispatch_block_t previousConnectingBlock;
@property (nonatomic) BOOL useSecondaryServer;

- (instancetype)initWithApplication:(AVApplication *)application
                           protocol:(LCIMProtocol)protocol
                              error:(NSError * __autoreleasing *)error;

@end
