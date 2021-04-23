//
//  LCStatus.h
//  paas
//
//  Created by Travis on 13-12-23.
//  Copyright (c) 2013年 AVOS. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "AVUser.h"
#import "LCQuery.h"

typedef NSString LCStatusType;

@class LCStatus, LCStatusQuery;

NS_ASSUME_NONNULL_BEGIN

extern NSString * const kLCStatusTypeTimeline;
extern NSString * const kLCStatusTypePrivateMessage;

typedef void (^LCStatusResultBlock)(LCStatus * _Nullable status, NSError * _Nullable error);

/**
 *  发送和获取状态更新和消息
 */
@interface LCStatus : NSObject

/**
 *  此状态的ID 具有唯一性
 */
@property (nonatomic, copy, readonly, nullable) NSString *objectId;

/**
 *  此状态在用户某个Type的收件箱中的ID
 *  @warning 仅用于分片查询,不具有唯一性,同一条状态在不同的inbox里的messageId也是不同的
 */
@property (nonatomic, assign, readonly) NSUInteger messageId;

/**
 *  状态的创建时间
 */
@property (nonatomic, strong, readonly, nullable) NSDate *createdAt;

/**
 *  状态的内容
 */
@property (nonatomic, strong, nullable) NSDictionary *data;

/**
 *  状态的发出"人",可以是AVUser 也可以是任意的LCObject,也可能是nil
 */
@property (nonatomic, strong, nullable) LCObject *source;

/**
 *  状态类型,默认是kLCStatusTypeTimeline, 可以是任意自定义字符串
 */
@property (nonatomic, copy) LCStatusType *type;



/** @name 针对某条状态的操作 */

/**
 *  获取某条状态
 *
 *  @param objectId 状态的objectId
 *  @param callback 回调结果
 */
+(void)getStatusWithID:(NSString *)objectId andCallback:(LCStatusResultBlock)callback;

/**
 *  删除当前用户发布的某条状态
 *
 *  @param objectId 状态的objectId
 *  @param callback 回调结果
 */
+(void)deleteStatusWithID:(NSString*)objectId andCallback:(AVBooleanResultBlock)callback;

/**
 * 删除收件箱中的状态
 *
 * @param messageId 状态的 messageId
 * @param inboxType 收件箱类型
 * @param receiver  收件人的 objectId
 */
+ (BOOL)deleteInboxStatusForMessageId:(NSUInteger)messageId inboxType:(NSString *)inboxType receiver:(NSString *)receiver error:(NSError **)error;

/**
 * 删除收件箱中的状态，异步执行
 *
 * @param messageId 状态的 messageId
 * @param inboxType 收件箱类型
 * @param receiver  收件人的 objectId
 * @param block     回调 block
 */
+ (void)deleteInboxStatusInBackgroundForMessageId:(NSUInteger)messageId inboxType:(NSString *)inboxType receiver:(NSString *)receiver block:(AVBooleanResultBlock)block;

/**
 *  设置受众群体
 *
 *  @param query 限定条件
 */
-(void)setQuery:(LCQuery*)query;


/** @name 获取状态 */

/**
 *  获取当前用户收件箱里的状态
 *
 *  @param inboxType 收件箱类型
 *  @return 用于查询的LCStatusQuery
 */
+(LCStatusQuery*)inboxQuery:(LCStatusType *)inboxType;

/**
 *  获取当前用户发出的状态
 *
 *  @return 用于查询的LCStatusQuery
 */
+(LCStatusQuery*)statusQuery;


/**
 *  获取当前用户特定类型未读状态条数
 *  @param type 收件箱类型
 *  @param callback 回调结果
 */
+(void)getUnreadStatusesCountWithType:(LCStatusType*)type andCallback:(AVIntegerResultBlock)callback;

/**
 *  Reset unread count of specific status type (inbox).
 *  @param type     Status type.
 *  @param callback Callback of reset request.
 */
+(void)resetUnreadStatusesCountWithType:(LCStatusType*)type andCallback:(AVBooleanResultBlock)callback;

/**
 *  获取当前用户接收到的状态
 *  @param type     状态类型,默认是kLCStatusTypeTimeline, 可以是任意自定义字符串
 *  @param skip     跳过条数
 *  @param limit    需要返回的条数 默认`100`，最大`100`
 *  @param callback 回调结果
 */
+(void)getStatusesWithType:(LCStatusType*)type skip:(NSUInteger)skip limit:(NSUInteger)limit andCallback:(AVArrayResultBlock)callback AV_DEPRECATED("2.3.2以后不再需要，请使用inboxQuery类方法");

/**
 *  获取当前用户发布的状态
 *
 *  @param type     状态类型,默认是kLCStatusTypeTimeline, 可以是任意自定义字符串
 *  @param skip     跳过条数
 *  @param limit    需要返回的条数 默认`100`，最大`100`
 *  @param callback 回调结果
 */
+(void) getStatusesFromCurrentUserWithType:(LCStatusType*)type skip:(NSUInteger)skip limit:(NSUInteger)limit andCallback:(AVArrayResultBlock)callback AV_DEPRECATED("2.3.2以后不再需要，请使用statusQuery类方法");

/**
 *  通过用户ID获取其发布的公开的状态列表
 *
 *  @param userId   用户的objectId
 *  @param skip     跳过条数
 *  @param limit    需要返回的条数 默认`100`，最大`100`
 *  @param callback 回调结果
 */
+(void) getStatusesFromUser:(NSString*)userId skip:(NSUInteger)skip limit:(NSUInteger)limit andCallback:(AVArrayResultBlock)callback AV_DEPRECATED("2.3.2以后不再需要，请使用statusQuery");

/** @name 发送状态 */

/**
 *  向用户的粉丝发送新状态
 *
 *  @param  status 状态
 *  @param  callback 回调结果
 */
+(void)sendStatusToFollowers:(LCStatus*)status andCallback:(AVBooleanResultBlock)callback;

/**
 *  向用户发私信
 *
 *  @param  status 状态
 *  @param  userId 接受私信的用户objectId
 *  @param  callback 回调结果
 */
+(void)sendPrivateStatus:(LCStatus*)status toUserWithID:(NSString*)userId andCallback:(AVBooleanResultBlock)callback;

/**
 *  发送
 *
 *  @param block 回调结果
 */
-(void)sendInBackgroundWithBlock:(AVBooleanResultBlock)block;
@end

/**
 *  用户好友关系
 */
@interface AVUser(Friendship)

/* @name 好友关系 */

/**
 *  获取用户粉丝LCQuery
 *
 *  @param userObjectId 用户ID
 *
 *  @return 用于查询的LCQuery
 */
+(LCQuery*)followerQuery:(NSString*)userObjectId;

/**
 *  获取本用户粉丝LCQuery
 *
 *  @return 用于查询的LCQuery
 */
-(LCQuery*)followerQuery;

/**
 *  获取用户关注LCQuery
 *
 *  @param userObjectId 用户ID
 *
 *  @return 用于查询的LCQuery
 */
+(LCQuery*)followeeQuery:(NSString*)userObjectId;

/**
 *  获取本用户关注LCQuery
 *
 *  @return 用于查询的LCQuery
 */
-(LCQuery*)followeeQuery;

/**
 *  通过ID来关注其他用户
 *  @warning 如果需要被关注者收到消息 需要手动给他发送一条LCStatus.
 *  @param userId 要关注的用户objectId
 *  @param callback 回调结果
 */
-(void)follow:(NSString*)userId andCallback:(AVBooleanResultBlock)callback;

/**
 *  通过ID来关注其他用户
 *  @warning 如果需要被关注者收到消息 需要手动给他发送一条LCStatus.
 *  @param userId 要关注的用户objectId
 *  @param dictionary 添加的自定义属性
 *  @param callback 回调结果
 */
-(void)follow:(NSString*)userId userDictionary:(nullable NSDictionary *)dictionary andCallback:(AVBooleanResultBlock)callback;

/**
 *  通过ID来取消关注其他用户
 *
 *  @param userId 要取消关注的用户objectId
 *  @param callback 回调结果
 *
 */
-(void)unfollow:(NSString*)userId andCallback:(AVBooleanResultBlock)callback;

/**
 *  获取当前用户粉丝的列表
 *
 *  @param callback 回调结果
 */
-(void)getFollowers:(AVArrayResultBlock)callback;

/**
 *  获取当前用户所关注的列表
 *
 *  @param callback 回调结果
 *
 */
-(void)getFollowees:(AVArrayResultBlock)callback;

/**
 *  同时获取当前用户的粉丝和关注列表
 *
 *  @param callback 回调结果, 列表字典包含`followers`数组和`followees`数组
 */
-(void)getFollowersAndFollowees:(AVDictionaryResultBlock)callback;


@end

/**
 *  查询LCStatus
 */
@interface LCStatusQuery : LCQuery
/**
 *  设置起始messageId, 仅用于Inbox中的查询
 */
@property(nonatomic, assign) NSUInteger sinceId;

/**
 *  设置最大messageId, 仅用于Inbox中的查询
 */
@property(nonatomic, assign) NSUInteger maxId;

/**
 *  设置查询的Inbox的所有者, 即查询这个"人"的收件箱
 */
@property(nonatomic, strong, nullable) LCObject *owner;

/**
 *  设置查询的Inbox的类型
 */
@property(nonatomic, copy, nullable) LCStatusType *inboxType;

/**
 *  查询结果是否已经到结尾
 */
@property(nonatomic)BOOL end;

@end

NS_ASSUME_NONNULL_END
