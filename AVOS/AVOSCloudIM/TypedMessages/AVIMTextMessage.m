//
//  AVIMTextMessage.m
//  AVOSCloudIM
//
//  Created by Qihe Bian on 1/12/15.
//  Copyright (c) 2015 LeanCloud Inc. All rights reserved.
//

#import "AVIMTextMessage.h"
#import "AVIMTypedMessage_Internal.h"

@implementation AVIMTextMessage

+ (void)load
{
    [self registerSubclass];
}

+ (LCIMMessageMediaType)classMediaType
{
    return kLCIMMessageMediaTypeText;
}

+ (instancetype)messageWithText:(NSString *)text
                     attributes:(NSDictionary *)attributes
{
    AVIMTextMessage *message = [[self alloc] init];
    message.text = text;
    if (attributes) {
        message.attributes = attributes;
    }
    return message;
}

@end
