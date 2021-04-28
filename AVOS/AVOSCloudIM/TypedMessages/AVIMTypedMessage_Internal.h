//
//  AVIMTypedMessage_Internal.h
//  AVOSCloudIM
//
//  Created by Qihe Bian on 1/8/15.
//  Copyright (c) 2014 LeanCloud Inc. All rights reserved.
//

#import "AVIMTypedMessage.h"
#import "AVIMTypedMessageObject.h"
#import "LCIMMessage_Internal.h"
#import "LCFile_Internal.h"
#import "LCGeoPoint_Internal.h"

extern NSMutableDictionary<NSNumber *, Class> const *_typeDict;

@interface AVIMTypedMessage ()

@property (nonatomic) AVIMTypedMessageObject *messageObject;

+ (instancetype)messageWithMessageObject:(AVIMTypedMessageObject *)messageObject;

- (NSString *)decodingUrl;
- (NSDictionary *)decodingMetaData;
- (NSString *)decodingName;
- (NSString *)decodingFormat;
- (double)decodingSize;
- (double)decodingWidth;
- (double)decodingHeight;
- (double)decodingDuration;

@end
