//
//  AVIMTypedMessage.m
//  AVOSCloudIM
//
//  Created by Qihe Bian on 1/8/15.
//  Copyright (c) 2014 LeanCloud Inc. All rights reserved.
//

#import <AVOSCloud/AVOSCloud.h>

#import "AVIMTypedMessage.h"
#import "AVIMTypedMessage_Internal.h"
#import "AVIMGeneralObject.h"
#import "AVIMMessage_Internal.h"
#import "AVFile_Internal.h"
#import "AVGeoPoint_Internal.h"

NSMutableDictionary<NSNumber *, Class> const *_typeDict = nil;

@implementation AVIMTypedMessage

@synthesize file = _file;
@synthesize location = _location;

+ (void)registerSubclass
{
    if ([self conformsToProtocol:@protocol(AVIMTypedMessageSubclassing)]) {
        Class<AVIMTypedMessageSubclassing> class = self;
        [self registerClass:class
               forMediaType:[class classMediaType]];
    }
}

+ (Class)classForMediaType:(AVIMMessageMediaType)mediaType
{
    Class class = [_typeDict objectForKey:@(mediaType)];
    if (!class) {
        class = [AVIMTypedMessage class];
    }
    return class;
}

+ (void)registerClass:(Class)class
         forMediaType:(AVIMMessageMediaType)mediaType
{
    if (!_typeDict) {
        _typeDict = [NSMutableDictionary dictionary];
    }
    NSNumber *mediaTypeNumber = @(mediaType);
    Class typeClass = [_typeDict objectForKey:mediaTypeNumber];
    if (!typeClass ||
        [class isSubclassOfClass:typeClass]) {
        [_typeDict setObject:class forKey:mediaTypeNumber];
    }
}

+ (instancetype)messageWithText:(NSString *)text
               attachedFilePath:(NSString *)attachedFilePath
                     attributes:(NSDictionary *)attributes
{
    NSError *error;
    AVFile *file = [AVFile fileWithLocalPath:attachedFilePath
                                       error:&error];
    if (error) {
        AVLoggerError(AVLoggerDomainIM, @"%@", error);
        return nil;
    }
    return [self messageWithText:text
                            file:file
                      attributes:attributes];
}

+ (instancetype)messageWithText:(NSString *)text
                           file:(AVFile *)file
                     attributes:(NSDictionary *)attributes
{
    AVIMTypedMessage *message = [[self alloc] init];
    message.text = text;
    message.attributes = attributes;
    message.file = file;
    return message;
}

+ (AVFile *)fileFromDictionary:(NSDictionary *)dictionary
{
    return dictionary ? [[AVFile alloc] initWithRawJSONData:dictionary.mutableCopy] : nil;
}

+ (AVGeoPoint *)locationFromDictionary:(NSDictionary *)dictionary
{
    return dictionary ? [AVGeoPoint geoPointFromDictionary:dictionary] : nil;
}

+ (instancetype)messageWithMessageObject:(AVIMTypedMessageObject *)messageObject
{
    AVIMMessageMediaType mediaType = messageObject._lctype;
    Class class = [self classForMediaType:mediaType];
    AVIMTypedMessage *message = [[class alloc] init];
    message.messageObject = messageObject;
    [message setFileIvar:[self fileFromDictionary:messageObject._lcfile]];
    [message setLocationIvar:[self locationFromDictionary:messageObject._lcloc]];
    return message;
}

- (id)copyWithZone:(NSZone *)zone
{
    AVIMTypedMessage *message = [super copyWithZone:zone];
    if (message) {
        message.messageObject = self.messageObject;
        message.file = self.file;
        message.location = self.location;
    }
    return message;
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [super encodeWithCoder:coder];
    NSData *data = [self.messageObject messagePack];
    if (data) {
        [coder encodeObject:data forKey:@"typedMessage"];
    }
}

- (instancetype)init
{
    if (![self conformsToProtocol:@protocol(AVIMTypedMessageSubclassing)]) {
        [NSException raise:NSInternalInconsistencyException
                    format:@"This Class does not conform `AVIMTypedMessageSubclassing` protocol."];
    }
    self = [super init];
    if (self) {
        self.mediaType = [[self class] classMediaType];
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)coder {
    if ((self = [super initWithCoder:coder])) {
        NSData *data = [coder decodeObjectForKey:@"typedMessage"];
        AVIMTypedMessageObject *object = [[AVIMTypedMessageObject alloc] initWithMessagePack:data];
        self.messageObject = object;
        id lcfile = object._lcfile;
        if ([lcfile isKindOfClass:[NSDictionary class]]) {
            self.file = [[self class] fileFromDictionary:lcfile];
        }
        id lcloc = object._lcloc;
        if ([lcloc isKindOfClass:[NSDictionary class]]) {
            self.location = [[self class] locationFromDictionary:lcloc];
        }
    }
    return self;
}

- (AVIMTypedMessageObject *)messageObject
{
    if (!_messageObject) {
        _messageObject = [[AVIMTypedMessageObject alloc] init];
    }
    return _messageObject;
}

- (AVIMMessageMediaType)mediaType
{
    return self.messageObject._lctype;
}

- (void)setMediaType:(AVIMMessageMediaType)mediaType
{
    self.messageObject._lctype = mediaType;
}

- (NSString *)text
{
    return self.messageObject._lctext;
}

- (void)setText:(NSString *)text
{
    self.messageObject._lctext = text;
}

- (NSDictionary *)attributes
{
    return self.messageObject._lcattrs;
}

- (void)setAttributes:(NSDictionary *)attributes
{
    self.messageObject._lcattrs = attributes;
}

- (AVFile *)file
{
    if (_file) {
        return _file;
    }
    NSDictionary *dictionary = self.messageObject._lcfile;
    if (dictionary) {
        _file = [[AVFile alloc] initWithRawJSONData:dictionary.mutableCopy];
    }
    return _file;
}

- (void)setFile:(AVFile *)file
{
    _file = file;
    if (file) {
        NSMutableDictionary *dictionary = [NSMutableDictionary dictionary];
        NSString *objectId = file.objectId;
        if (objectId) {
            dictionary[@"objId"] = objectId;
        }
        NSString *url = file.url;
        if (url) {
            dictionary[@"url"] = url;
        }
        NSMutableDictionary *metaData = file.metaData.mutableCopy;
        NSString *name = file.name;
        if (metaData || name) {
            if (!metaData) {
                metaData = [NSMutableDictionary dictionary];
            }
            if (name) {
                metaData[@"name"] = name;
            }
            dictionary[@"metaData"] = metaData;
        }
        if (dictionary.count > 0) {
            self.messageObject._lcfile = dictionary;
        } else {
            self.messageObject._lcfile = nil;
        }
    } else {
        self.messageObject._lcfile = nil;
    }
}

- (void)setFileIvar:(AVFile *)file
{
    _file = file;
}

- (AVGeoPoint *)location
{
    if (_location) {
        return _location;
    }
    NSDictionary *dictionary = self.messageObject._lcloc;
    if (dictionary) {
        _location = [AVGeoPoint geoPointFromDictionary:dictionary];
    }
    return _location;
}

- (void)setLocation:(AVGeoPoint *)location
{
    _location = location;
    if (location) {
        self.messageObject._lcloc = @{
            @"latitude": @(location.latitude),
            @"longitude": @(location.longitude),
        };
    } else {
        self.messageObject._lcloc = nil;
    }
}

- (void)setLocationIvar:(AVGeoPoint *)location
{
    _location = location;
}

- (void)setObject:(id)object forKey:(NSString *)key
{
    [self.messageObject setObject:object forKey:key];
}

- (id)objectForKey:(NSString *)key
{
    return [self.messageObject objectForKey:key];
}

- (NSString *)payload
{
    if (self.messageObject.localData.count > 0) {
        return [self.messageObject JSONString];
    } else {
        return [super payload];
    }
}

@end
