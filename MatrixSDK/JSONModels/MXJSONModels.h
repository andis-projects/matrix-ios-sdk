/*
 Copyright 2014 OpenMarket Ltd
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#import <Foundation/Foundation.h>

#import "MXJSONModel.h"

@class MXEvent;

/**
 This file contains definitions of basic JSON responses or objects received
 from a Matrix home server.
 
 Note: some such class can be defined in their own file (ex: MXEvent)
 */

/**
  `MXPublicRoom` represents a public room returned by the publicRoom request
 */
@interface MXPublicRoom : MXJSONModel

    /**
     The ID of the room.
     */
    @property (nonatomic) NSString *roomId;

    /**
     The name of the room, if any. May be nil.
     */
    @property (nonatomic) NSString *name;

    /**
     Aliases of the room.
     */
    @property (nonatomic) NSArray<NSString*> *aliases;

    /**
     The topic of the room, if any. May be nil.
     */
    @property (nonatomic) NSString *topic;

    /**
     The number of members joined to the room.
     */
    @property (nonatomic) NSUInteger numJoinedMembers;

    /**
     Whether the room may be viewed by guest users without joining.
     */
    @property (nonatomic) BOOL worldReadable;

    /**
     Whether guest users may join the room and participate in it.
     If they can, they will be subject to ordinary power level rules like any other user.
     */
    @property (nonatomic) BOOL guestCanJoin;

    /**
     The URL for the room's avatar. May be nil.
     */
    @property (nonatomic) NSString *avatarUrl;

    // The display name is computed from available information
    // @TODO: move it to MXSession as this class has additional information to compute the optimal display name
    - (NSString *)displayname;

@end


/**
 Login flow types
 */
typedef NSString* MXLoginFlowType;
FOUNDATION_EXPORT NSString *const kMXLoginFlowTypePassword;
FOUNDATION_EXPORT NSString *const kMXLoginFlowTypeOAuth2;
FOUNDATION_EXPORT NSString *const kMXLoginFlowTypeEmailCode;
FOUNDATION_EXPORT NSString *const kMXLoginFlowTypeEmailUrl;
FOUNDATION_EXPORT NSString *const kMXLoginFlowTypeEmailIdentity;
FOUNDATION_EXPORT NSString *const kMXLoginFlowTypeRecaptcha;

/**
 `MXLoginFlow` represents a login or a register flow supported by the home server.
 */
@interface MXLoginFlow : MXJSONModel

    /**
     The flow type among kMXLoginFlowType* types.
     @see http://matrix.org/docs/spec/#password-based and below for the types descriptions
     */
    @property (nonatomic) NSString *type;

    /**
     The list of stages to proceed the login. This is an array of NSStrings
     */
    @property (nonatomic) NSArray *stages;

@end


/**
 `MXCredentials` represents the response to a login or a register request.
 */
@interface MXCredentials : MXJSONModel

    /**
     The home server name.
     */
    @property (nonatomic) NSString *homeServer;

    /**
     The obtained user id.
     */
    @property (nonatomic) NSString *userId;

    /**
     The access token to create a MXRestClient
     */
    @property (nonatomic) NSString *accessToken;

    /**
     The server certificate trusted by the user (nil when the server is trusted by the device).
     */
    @property (nonatomic) NSData *allowedCertificate;

    /**
     The ignored server certificate (set when the user ignores a certificate change).
     */
    @property (nonatomic) NSData *ignoredCertificate;

    /**
     Simple MXCredentials construtor
     */
    - (instancetype)initWithHomeServer:(NSString*)homeServer
                                userId:(NSString*)userId
                           accessToken:(NSString*)accessToken;

@end


/**
 `MXCreateRoomResponse` represents the response to createRoom request.
 */
@interface MXCreateRoomResponse : MXJSONModel

    /**
     The allocated room id.
     */
    @property (nonatomic) NSString *roomId;

    /**
     The alias on this home server.
     */
    @property (nonatomic) NSString *roomAlias;

@end

/**
 `MXPaginationResponse` represents a response from an api that supports pagination.
 */
@interface MXPaginationResponse : MXJSONModel

    /**
     An array of MXEvents.
     */
    @property (nonatomic) NSArray *chunk;

    /**
     The opaque token for the start.
     */
    @property (nonatomic) NSString *start;

    /**
     The opaque token for the end.
     */
    @property (nonatomic) NSString *end;

@end

/**
 `MXRoomMemberEventContent` represents the content of a m.room.member event.
 */
@interface MXRoomMemberEventContent : MXJSONModel

    /**
     The user display name.
     */
    @property (nonatomic) NSString *displayname;

    /**
     The url of the user of the avatar.
     */
    @property (nonatomic) NSString *avatarUrl;

    /**
     The membership state.
     */
    @property (nonatomic) NSString *membership;

@end


/**
 Room tags defined by Matrix spec.
 */
FOUNDATION_EXPORT NSString *const kMXRoomTagFavourite;
FOUNDATION_EXPORT NSString *const kMXRoomTagLowPriority;

/**
 `MXRoomTag` represents a room tag.
 */
@interface MXRoomTag : NSObject <NSCoding>

/**
 The name of a tag.
 */
@property (nonatomic, readonly) NSString *name;

/**
 An optional information to order the room within a list of rooms with the same tag name.
 If not nil, the `order` string is used to make lexicographically by unicode codepoint
 comparison.
 */
@property (nonatomic, readonly) NSString *order;

/**
 Try to parse order as NSNumber.
 Provides nil if the items cannot be parsed.
 */
@property (nonatomic, readonly) NSNumber *parsedOrder;

/**
 Basic constructor.
 
 @param name the tag name
 @param order the order.
 @return a new MXRoomTag instance.
 */
- (id)initWithName:(NSString*)name andOrder:(NSString*)order;

/**
 Extract a list of tags from a room tag event.
 
 @param a room tag event (which can contains several tags)
 @return a dictionary containing the tags the user defined for one room.
         The key is the tag name. The value, the associated MXRoomTag object.
 */
+ (NSDictionary<NSString*, MXRoomTag*>*)roomTagsWithTagEvent:(MXEvent*)event;

@end


/**
 Presence definitions
 */
typedef enum : NSUInteger
{
    MXPresenceUnknown,    // The home server did not provide the information
    MXPresenceOnline,
    MXPresenceUnavailable,
    MXPresenceOffline,
    MXPresenceFreeForChat,
    MXPresenceHidden
} MXPresence;

/**
 Presence definitions - String version
 */
typedef NSString* MXPresenceString;
FOUNDATION_EXPORT NSString *const kMXPresenceOnline;
FOUNDATION_EXPORT NSString *const kMXPresenceUnavailable;
FOUNDATION_EXPORT NSString *const kMXPresenceOffline;
FOUNDATION_EXPORT NSString *const kMXPresenceFreeForChat;
FOUNDATION_EXPORT NSString *const kMXPresenceHidden;

/**
 `MXPresenceEventContent` represents the content of a presence event.
 */
@interface MXPresenceEventContent : MXJSONModel

    /**
     The user id.
     */
    @property (nonatomic) NSString *userId;

    /**
     The user display name.
     */
    @property (nonatomic) NSString *displayname;

    /**
     The url of the user of the avatar.
     */
    @property (nonatomic) NSString *avatarUrl;

    /**
     The timestamp of the last time the user has been active.
     */
    @property (nonatomic) NSUInteger lastActiveAgo;

    /**
     The presence status string as provided by the home server.
     */
    @property (nonatomic) MXPresenceString presence;

    /**
     The enum version of the presence status.
     */
    @property (nonatomic) MXPresence presenceStatus;

    /**
     The user status.
     */
    @property (nonatomic) NSString *statusMsg;

@end

/**
 `MXPresenceResponse` represents the response to presence request.
 */
@interface MXPresenceResponse : MXJSONModel

    /**
     The timestamp of the last time the user has been active.
     */
    @property (nonatomic) NSUInteger lastActiveAgo;

    /**
     The presence status string as provided by the home server.
     */
    @property (nonatomic) MXPresenceString presence;

    /**
     The enum version of the presence status.
     */
    @property (nonatomic) MXPresence presenceStatus;

    /**
     The user status.
     */
    @property (nonatomic) NSString *statusMsg;

@end


@class MXPushRuleCondition;

/**
 Push rules kind.
 
 Push rules are separated into different kinds of rules. These categories have a priority order: verride rules
 have the highest priority.
 Some category may define implicit conditions.
 */
typedef enum : NSUInteger
{
    MXPushRuleKindOverride,
    MXPushRuleKindContent,
    MXPushRuleKindRoom,
    MXPushRuleKindSender,
    MXPushRuleKindUnderride
} MXPushRuleKind;

/**
 `MXPushRule` defines a push notification rule.
 */
@interface MXPushRule : MXJSONModel

    /**
     The identifier for the rule.
     */
    @property (nonatomic) NSString *ruleId;

    /**
     Actions (array of MXPushRuleAction objects) to realize if the rule matches.
     */
    @property (nonatomic) NSArray *actions;

    /**
     Override, Underride and Default rules have a list of 'conditions'. 
     All conditions must hold true for an event in order for a rule to be applied to an event.
     */
    @property (nonatomic) NSArray *conditions;

    /**
     Indicate if it is a Home Server default push rule.
     */
    @property (nonatomic) BOOL isDefault;

    /**
     Indicate if the rule is enabled.
     */
    @property (nonatomic) BOOL enabled;

    /**
     Only available for Content push rules, this gives the pattern to match against.
     */
    @property (nonatomic) NSString *pattern;

    /**
     The category the push rule belongs to.
     */
    @property (nonatomic) MXPushRuleKind kind;

    /**
     The scope of the push rule: either 'global' or 'device/<profile_tag>' to specify global rules or device rules for the given profile_tag.
     */
    @property (nonatomic) NSString *scope;

@end

/**
 Push rules action type.

 Actions names are exchanged as strings with the home server. The actions
 specified by Matrix are listed here as NSUInteger enum in order to ease
 their handling handling.

 Custom actions, out of the specification, may exist. In this case,
 `MXPushRuleActionString` must be checked.
 */
typedef enum : NSUInteger
{
    MXPushRuleActionTypeNotify,
    MXPushRuleActionTypeDontNotify,
    MXPushRuleActionTypeCoalesce,   // At a Matrix client level, coalesce action should be treated as a notify action
    MXPushRuleActionTypeSetTweak,

    // The action is a custom action. Refer to its `MXPushRuleActionString` version
    MXPushRuleActionTypeCustom = 1000
} MXPushRuleActionType;

/**
 Push rule action definitions - String version
 */
typedef NSString* MXPushRuleActionString;
FOUNDATION_EXPORT NSString *const kMXPushRuleActionStringNotify;
FOUNDATION_EXPORT NSString *const kMXPushRuleActionStringDontNotify;
FOUNDATION_EXPORT NSString *const kMXPushRuleActionStringCoalesce;
FOUNDATION_EXPORT NSString *const kMXPushRuleActionStringSetTweak;

/**
 An action to accomplish when a push rule matches.
 */
@interface MXPushRuleAction : NSObject

    /**
     The action type.
     */
    @property (nonatomic) MXPushRuleActionType actionType;

    /**
     The action type (string version)
     */
    @property (nonatomic) MXPushRuleActionString action;

    /**
     Action parameters. Not all actions have parameters.
     */
    @property (nonatomic) NSDictionary *parameters;

@end

/**
 Push rules conditions type.

 Condition kinds are exchanged as strings with the home server. The kinds of conditions
 specified by Matrix are listed here as NSUInteger enum in order to ease
 their handling handling.

 Custom condition kind, out of the specification, may exist. In this case,
 `MXPushRuleConditionString` must be checked.
 */
typedef enum : NSUInteger
{
    MXPushRuleConditionTypeEventMatch,
    MXPushRuleConditionTypeProfileTag,
    MXPushRuleConditionTypeContainsDisplayName,
    MXPushRuleConditionTypeRoomMemberCount,

    // The condition is a custom condition. Refer to its `MXPushRuleConditionString` version
    MXPushRuleConditionTypeCustom = 1000
} MXPushRuleConditionType;

/**
 Push rule condition kind definitions - String version
 */
typedef NSString* MXPushRuleConditionString;
FOUNDATION_EXPORT NSString *const kMXPushRuleConditionStringEventMatch;
FOUNDATION_EXPORT NSString *const kMXPushRuleConditionStringProfileTag;
FOUNDATION_EXPORT NSString *const kMXPushRuleConditionStringContainsDisplayName;
FOUNDATION_EXPORT NSString *const kMXPushRuleConditionStringRoomMemberCount;

/**
 `MXPushRuleCondition` represents an additional condition into a rule.
 */
@interface MXPushRuleCondition : MXJSONModel

    /**
     The condition kind.
     */
    @property (nonatomic) MXPushRuleConditionType kindType;

    /**
     The condition kind (string version)
     */
    @property (nonatomic) MXPushRuleConditionString kind;

    /**
     Conditions parameters. Not all conditions have parameters.
     */
    @property (nonatomic) NSDictionary *parameters;

@end

/**
 `MXPushRulesSet` is the set of push rules to apply for a given context (global, per device, ...).
 Properties in the `MXPushRulesSet` definitions are listed by descending priorities: push rules
 stored in `override` have an higher priority that ones in `content` and so on.
 Each property is an array of `MXPushRule` objects.
 */
@interface MXPushRulesSet : MXJSONModel

    /**
     The highest priority rules are user-configured overrides.
     */
    @property (nonatomic) NSArray *override;

    /**
     These configure behaviour for (unencrypted) messages that match certain patterns. 
     Content rules take one parameter, 'pattern', that gives the pattern to match against. 
     This is treated in the same way as pattern for event_match conditions, below.
     */
    @property (nonatomic) NSArray *content;

    /**
     These change the behaviour of all messages to a given room. 
     The rule_id of a room rule is always the ID of the room that it affects.
     */
    @property (nonatomic) NSArray *room;

    /**
     These rules configure notification behaviour for messages from a specific, named Matrix user ID. 
     The rule_id of Sender rules is always the Matrix user ID of the user whose messages theyt apply to.
     */
    @property (nonatomic) NSArray *sender;

    /**
     These are identical to override rules, but have a lower priority than content, room and sender rules.
     */
    @property (nonatomic) NSArray *underride;

@end

/**
 Push rule scope definitions - String version
 */
FOUNDATION_EXPORT NSString *const kMXPushRuleScopeStringGlobal;
FOUNDATION_EXPORT NSString *const kMXPushRuleScopeStringDevice;

/**
 `MXPushRulesResponse` represents the response to the /pushRules/ request.
 */
@interface MXPushRulesResponse : MXJSONModel

    /**
     Set of push rules specific per device.
     */
    // @property (nonatomic) NSDictionary *device;

    /**
     Set of global push rules.
     */
    @property (nonatomic) MXPushRulesSet *global;

@end

#pragma mark - Server sync v1 response
#pragma mark -

/**
 `MXRoomInitialSync` represents a room description in server response during initial sync v1.
 */
@interface MXRoomInitialSync : MXJSONModel

    /**
     The room identifier.
     */
    @property (nonatomic) NSString *roomId;

    /**
     The last recent messages of the room.
     */
    @property (nonatomic) MXPaginationResponse *messages;

    /**
     The state events.
     */
    @property (nonatomic) NSArray<MXEvent*> *state;

    /**
     The private data that this user has attached to this room.
     */
    @property (nonatomic) NSArray<MXEvent*> *accountData;

    /**
     The current user membership in this room.
     */
    @property (nonatomic) NSString* membership;

    /**
     The room visibility (public/private).
     */
    @property (nonatomic) NSString* visibility;

    /**
     The matrix id of the inviter in case of pending invitation.
     */
    @property (nonatomic) NSString *inviter;

    /**
     The invite event if membership is invite.
     */
    @property (nonatomic) MXEvent *invite;

    /**
     The presence status of other users (Provided in case of room initial sync @see http://matrix.org/docs/api/client-server/#!/-rooms/get_room_sync_data)).
     */
    @property (nonatomic) NSArray<MXEvent*> *presence;

    /**
     The read receipts (Provided in case of room initial sync).
     */
    @property (nonatomic) NSArray<MXEvent*> *receipts;

@end

/**
 `MXInitialSyncResponse` represents the request response for server initial sync v1. @see http://matrix.org/docs/api/client-server/#!/-events/initial_sync
 */
@interface MXInitialSyncResponse : MXJSONModel

    /**
     List of rooms.
     */
    @property (nonatomic) NSArray<MXRoomInitialSync*> *rooms;

    /**
     The presence status of other users.
     */
    @property (nonatomic) NSArray<MXEvent*> *presence;

    /**
     The read receipts.
     */
    @property (nonatomic) NSArray<MXEvent*> *receipts;

    /**
     The opaque token for the end.
     */
    @property (nonatomic) NSString *end;

@end

#pragma mark - Server sync v2 response
#pragma mark -

/**
 `MXRoomSyncState` represents the state updates for a room during server sync v2.
 */
@interface MXRoomSyncState : MXJSONModel

    /**
     List of event ids (array of NSString).
     */
    @property (nonatomic) NSArray<NSString*> *events;

@end

/**
 `MXRoomSyncTimeline` represents the timeline of messages and state changes for a room during server sync v2.
 */
@interface MXRoomSyncTimeline : MXJSONModel

    /**
     List of event ids (array of NSString).
     */
    @property (nonatomic) NSArray<NSString*> *events;

    /**
     Boolean which tells whether there are more events on the server
     */
    @property (nonatomic) BOOL limited;

    /**
     If the batch was limited then this is a token that can be supplied to the server to retrieve more events
     */
    @property (nonatomic) NSString *prevBatch;

@end

/**
 `MXRoomSyncEphemeral` represents the ephemeral events in the room that aren't recorded in the timeline or state of the room (e.g. typing).
 */
@interface MXRoomSyncEphemeral : MXJSONModel

    /**
     List of ephemeral events (array of MXEvent).
     */
    @property (nonatomic) NSArray<MXEvent*> *events;

@end

/**
 `MXRoomInviteState` represents the state of a room that the user has been invited to.
 */
@interface MXRoomInviteState : MXJSONModel

    /**
     List of state events (array of MXEvent).
     */
    @property (nonatomic) NSArray<MXEvent*> *events;

@end

/**
 `MXRoomSync` represents the response for a room during server sync v2.
 */
@interface MXRoomSync : MXJSONModel

    /**
     Converted events mapping: keys are event ids.
     The events are referenced from the 'timeline' and 'state' keys for this room.
     */
    @property (nonatomic) NSDictionary<NSString*, MXEvent*> *mxEventMap;

    /**
     The state updates for the room.
     */
    @property (nonatomic) MXRoomSyncState *state;

    /**
     The timeline of messages and state changes in the room.
     */
    @property (nonatomic) MXRoomSyncTimeline *timeline;

    /**
     The ephemeral events in the room that aren't recorded in the timeline or state of the room (e.g. typing).
     */
    @property (nonatomic) MXRoomSyncEphemeral *ephemeral;

@end

/**
 `MXInvitedRoomSync` represents a room invitation during server sync v2.
 */
@interface MXInvitedRoomSync : MXJSONModel

    /**
     The state of a room that the user has been invited to. These state events may only have the 'sender', 'type', 'state_key'
     and 'content' keys present. These events do not replace any state that the client already has for the room, for example if
     the client has archived the room. Instead the client should keep two separate copies of the state: the one from the 'invite_state'
     and one from the archived 'state'. If the client joins the room then the current state will be given as a delta against the
     archived 'state' not the 'invite_state'.
     */
    @property (nonatomic) MXRoomInviteState *inviteState;

@end

/**
 `MXRoomsSyncResponse` represents the rooms list in server sync v2 response.
 */
@interface MXRoomsSyncResponse : MXJSONModel

    /**
     Converted joined rooms: keys are rooms ids.
     */
    @property (nonatomic) NSDictionary<NSString*, MXRoomSync*> *mxJoined;

    /**
     Converted invited rooms. The rooms that the user has been invited to: keys are rooms ids.
     */
    @property (nonatomic) NSDictionary<NSString*, MXInvitedRoomSync*> *mxInvited;

    /**
     Converted archived rooms. The rooms that the user has left or been banned from: keys are rooms ids.
     */
    @property (nonatomic) NSDictionary<NSString*, MXRoomSync*> *mxArchived;

@end

/**
 `MXPresenceSyncResponse` represents the updates to the presence status of other users during server sync v2.
 */
@interface MXPresenceSyncResponse : MXJSONModel

    /**
     List of presence events (array of MXEvent with type m.presence).
     */
    @property (nonatomic) NSArray<MXEvent*> *events;

@end

/**
 `MXSyncResponse` represents the request response for server sync v2.
 */
@interface MXSyncResponse : MXJSONModel

    /**
     The opaque token for the end.
     */
    @property (nonatomic) NSString *nextBatch;

    /**
     The updates to the presence status of other users.
     */
    @property (nonatomic) MXPresenceSyncResponse *presence;

    /**
     Converted list of rooms.
     */
    @property (nonatomic) MXRoomsSyncResponse *mxRooms;

@end

#pragma mark - Voice over IP
#pragma mark -

/**
 `MXCallOffer` represents a call session description.
 */
@interface MXCallSessionDescription : MXJSONModel

    /**
     The type of session description. Can be 'offer' or 'answer'.
     */
    @property (nonatomic) NSString *type;

    /**
     The SDP text of the session description.
     */
    @property (nonatomic) NSString *sdp;

@end

/**
 `MXCallInviteEventContent` represents the content of a m.call.invite event.
 */
@interface MXCallInviteEventContent : MXJSONModel

    /**
     A unique identifier for the call.
     */
    @property (nonatomic) NSString *callId;

    /**
     The session description.
     */
    @property (nonatomic) MXCallSessionDescription *offer;

    /**
     The version of the VoIP specification this message adheres to.
     */
    @property (nonatomic) NSUInteger version;

    /**
     The time in milliseconds that the invite is valid for. 
     Once the invite age exceeds this value, clients should discard it.
     They should also no longer show the call as awaiting an answer in the UI.
     */
    @property (nonatomic) NSUInteger lifetime;

@end

/**
 `MXCallCandidate` represents a candidate description.
 */
@interface MXCallCandidate : MXJSONModel

    /**
     The SDP media type this candidate is intended for.
     */
    @property (nonatomic) NSString *sdpMid;

    /**
     The index of the SDP 'm' line this candidate is intended for.
     */
    @property (nonatomic) NSUInteger sdpMLineIndex;

    /**
     The SDP 'a' line of the candidate.
     */
    @property (nonatomic) NSString *candidate;

@end

/**
 `MXCallCandidatesEventContent` represents the content of a m.call.candidates event.
 */
@interface MXCallCandidatesEventContent : MXJSONModel

    /**
     The ID of the call this event relates to.
     */
    @property (nonatomic) NSString *callId;

    /**
     The version of the VoIP specification this message adheres to.
     */
    @property (nonatomic) NSUInteger version;

    /**
     Array of object describing the candidates (@see MXCallCandidate).
     */
    @property (nonatomic) NSArray *candidates;

@end

/**
 `MXCallAnswerEventContent` represents the content of a m.call.answer event.
 */
@interface MXCallAnswerEventContent : MXJSONModel

    /**
     A unique identifier for the call.
     */
    @property (nonatomic) NSString *callId;

    /**
     The version of the VoIP specification this message adheres to.
     */
    @property (nonatomic) NSUInteger version;

    /**
     The session description.
     */
    @property (nonatomic) MXCallSessionDescription *answer;

@end

/**
 `MXCallHangupEventContent` represents the content of a m.call.hangup event.
 */
@interface MXCallHangupEventContent : MXJSONModel

    /**
     A unique identifier for the call.
     */
    @property (nonatomic) NSString *callId;

    /**
     The version of the VoIP specification this message adheres to.
     */
    @property (nonatomic) NSUInteger version;

@end

/**
 `MXTurnServerResponse` represents the response to turnServer request.
 It provides TURN server configuration advised by the homeserver.
 */
@interface MXTurnServerResponse : MXJSONModel

    /**
     The username of the Matrix user on the TURN server.
     */
    @property (nonatomic) NSString *username;

    /**
     The associated password.
     */
    @property (nonatomic) NSString *password;

    /**
     The list URIs of TURN servers - including STUN servers.
     The URI scheme obeys to http://tools.ietf.org/html/rfc7064#section-3.1 
     and http://tools.ietf.org/html/rfc7065#section-3.1
     */
    @property (nonatomic) NSArray *uris;

    /**
     Time To Live. The time is seconds this data is still valid.
     It is computed by the user's homeserver when the request is made.
     Then, the SDK updates the property each time it is read.
     */
    @property (nonatomic) NSUInteger ttl;

    /**
     The `ttl` value transcoded to an absolute date, a timestamp in milliseconds
     based on the device clock.
     */
    @property (nonatomic) uint64_t ttlExpirationLocalTs;

@end


