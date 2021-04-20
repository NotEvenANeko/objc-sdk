//
//  AVFileQuery.m
//  AVOS-DynamicFramework
//
//  Created by lzw on 15/10/8.
//  Copyright © 2015年 tang3w. All rights reserved.
//

#import "AVFileQuery.h"
#import "AVFile.h"
#import "AVQuery_Internal.h"
#import "AVUtils.h"

@implementation AVFileQuery

+ (instancetype)query {
    return [self queryWithClassName:@"_File"];
}
- (NSArray *)filesWithObjects:(NSArray *)objects {
    if (objects == nil) {
        return nil;
    }
    NSMutableArray *files = [NSMutableArray arrayWithCapacity:objects.count];
    for (LCObject *object in [objects copy]) {
        AVFile *file = [AVFile fileWithObject:object];
        [files addObject:file];
    }
    return files;
}

- (void)getFileInBackgroundWithId:(NSString *)objectId
                            block:(AVFileResultBlock)block {
    [self getObjectInBackgroundWithId:objectId block:^(LCObject *object, NSError *error) {
        AVFile *file = nil;
        if (!error) {
            file = [AVFile fileWithObject:object];
        }
        [AVUtils callFileResultBlock:block AVFile:file error:error];
    }];
}

- (AVFile *)getFileWithId:(NSString *)objectId error:(NSError **)error {
    LCObject *object = [self getObjectWithId:objectId error:error];
    AVFile *file = nil;
    if (object != nil) {
        file = [AVFile fileWithObject:object];
    }
    return file;
}

- (NSArray *)findFiles:(NSError **)error {
    NSArray *objects = [super findObjects:error];
    return [self filesWithObjects:objects];
}

- (void)findFilesInBackgroundWithBlock:(AVArrayResultBlock)resultBlock {
    [self findObjectsInBackgroundWithBlock:^(NSArray *objects, NSError *error) {
        NSArray *files = [self filesWithObjects:objects];
        [AVUtils callArrayResultBlock:resultBlock array:files error:error];
    }];
}

@end
