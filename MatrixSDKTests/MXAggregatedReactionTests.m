/*
 * Copyright 2019 New Vector Ltd
 * Copyright 2019 The Matrix.org Foundation C.I.C
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#import <XCTest/XCTest.h>

#import "MatrixSDKTestsData.h"

#import "MXFileStore.h"
#import "MXEventUnsignedData.h"
#import "MXEventRelations.h"
#import "MXEventAnnotationChunk.h"
#import "MXEventAnnotation.h"

// Do not bother with retain cycles warnings in tests
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-retain-cycles"

@interface MXAggregatedReactionTests : XCTestCase
{
    MatrixSDKTestsData *matrixSDKTestsData;
}

@end

@implementation MXAggregatedReactionTests

- (void)setUp
{
    [super setUp];

    matrixSDKTestsData = [[MatrixSDKTestsData alloc] init];
}

- (void)tearDown
{
    matrixSDKTestsData = nil;
}

// Create a room with an event with a reaction on it
- (void)createScenario:(void(^)(MXSession *mxSession, MXRoom *room, XCTestExpectation *expectation, NSString *eventId, NSString *reactionEventId))readyToTest
{
    [matrixSDKTestsData doMXSessionTestWithBobAndARoom:self andStore:[[MXMemoryStore alloc] init] readyToTest:^(MXSession *mxSession, MXRoom *room, XCTestExpectation *expectation) {

        [room sendTextMessage:@"Hello" success:^(NSString *eventId) {

            [mxSession.aggregations sendReaction:@"👍" toEvent:eventId inRoom:room.roomId success:^(NSString *reactionEventId) {

                // TODO: sendReaction should return only when the actual reaction event comes back the sync
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 1 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
                    readyToTest(mxSession, room, expectation, eventId, reactionEventId);
                });

            } failure:^(NSError *error) {
                XCTFail(@"Cannot set up intial test conditions - error: %@", error);
                [expectation fulfill];
            }];
        } failure:^(NSError *error) {
            XCTFail(@"Cannot set up intial test conditions - error: %@", error);
            [expectation fulfill];
        }];
    }];
}

// - Send a message
// - React on it
// -> a m.reaction message must appear in the timeline
- (void)testReactionSendAndReceive
{
    [matrixSDKTestsData doMXSessionTestWithBobAndARoom:self andStore:[[MXMemoryStore alloc] init] readyToTest:^(MXSession *mxSession, MXRoom *room, XCTestExpectation *expectation) {

        // - Send a message
        [room sendTextMessage:@"Hello" success:^(NSString *eventId) {

            // - React on it
            [mxSession.aggregations sendReaction:@"👍" toEvent:eventId inRoom:room.roomId success:^(NSString *eventId) {
                XCTAssertNotNil(eventId);
            } failure:^(NSError *error) {
                XCTFail(@"The operation should not fail - NSError: %@", error);
                [expectation fulfill];
            }];

            [room listenToEventsOfTypes:@[kMXEventTypeStringReaction] onEvent:^(MXEvent *event, MXTimelineDirection direction, MXRoomState *roomState) {

                // -> a m.reaction message must appear in the timeline
                XCTAssertEqual(event.eventType, MXEventTypeReaction);
                XCTAssertEqualObjects(event.type, kMXEventTypeStringReaction);

                XCTAssertNotNil(event.relatesTo);
                XCTAssertEqualObjects(event.relatesTo.relationType, MXEventRelationTypeAnnotation);
                XCTAssertEqualObjects(event.relatesTo.eventId, eventId);
                XCTAssertEqualObjects(event.relatesTo.key, @"👍");

                [expectation fulfill];
            }];
        } failure:^(NSError *error) {
            XCTFail(@"Cannot set up intial test conditions - error: %@", error);
            [expectation fulfill];
        }];
    }];
}

// - Run the initial condition scenario
// -> Check data is correctly aggregated when fetching the reacted event directly from the homeserver
- (void)testAggregatedReaction
{
    // - Run the initial condition scenario
    [self createScenario:^(MXSession *mxSession, MXRoom *room, XCTestExpectation *expectation, NSString *eventId, NSString *editEventId) {

        // -> Check data is correctly aggregated when fetching the reacted event directly from the homeserver
        [mxSession.matrixRestClient eventWithEventId:eventId inRoom:room.roomId success:^(MXEvent *event) {

            MXEventAnnotationChunk *annotations = event.unsignedData.relations.annotation;
            XCTAssertNotNil(annotations);
            XCTAssertEqual(annotations.count, 1);
            XCTAssertEqual(annotations.chunk.count, 1);

            MXEventAnnotation *annotation = annotations.chunk.firstObject;
            XCTAssertNotNil(annotation);
            XCTAssertEqualObjects(annotation.type, MXEventAnnotationReaction);
            XCTAssertEqualObjects(annotation.key, @"👍");
            XCTAssertEqual(annotation.count, 1);

            [expectation fulfill];

        } failure:^(NSError *error) {
            XCTFail(@"Cannot set up intial test conditions - error: %@", error);
            [expectation fulfill];
        }];
    }];
}

// - Run the initial condition scenario
// - Do an initial sync
// -> Data from aggregations must be right
- (void)testAggregationsFromInitialSync
{
    // - Run the initial condition scenario
    [self createScenario:^(MXSession *mxSession, MXRoom *room, XCTestExpectation *expectation, NSString *eventId, NSString *reactionEventId) {

        MXRestClient *restClient = mxSession.matrixRestClient;

        [mxSession.aggregations resetData];
        [mxSession close];
        mxSession = nil;

        // - Do an initial sync
        mxSession = [[MXSession alloc] initWithMatrixRestClient:restClient];
        [mxSession setStore:[[MXMemoryStore alloc] init] success:^{

            [mxSession start:^{

                // -> Data from aggregations must be right
                MXAggregatedReactions *reactions = [mxSession.aggregations aggregatedReactionsOnEvent:eventId inRoom:room.roomId];

                XCTAssertNotNil(reactions);
                XCTAssertEqual(reactions.reactions.count, 1);

                MXReactionCount *reactionCount = reactions.reactions.firstObject;
                XCTAssertEqualObjects(reactionCount.reaction, @"👍");
                XCTAssertEqual(reactionCount.count, 1);
                XCTAssertTrue(reactionCount.myUserHasReacted);

                [expectation fulfill];

            } failure:^(NSError *error) {
                XCTFail(@"Cannot set up intial test conditions - error: %@", error);
                [expectation fulfill];
            }];

        } failure:^(NSError *error) {
            XCTFail(@"Cannot set up intial test conditions - error: %@", error);
            [expectation fulfill];
        }];
    }];
}

// - Run the initial condition scenario
// -> Data from aggregations must be right
- (void)testAggregationsLive
{
    // - Run the initial condition scenario
    [self createScenario:^(MXSession *mxSession, MXRoom *room, XCTestExpectation *expectation, NSString *eventId, NSString *reactionEventId) {

        // -> Data from aggregations must be right
        MXAggregatedReactions *reactions = [mxSession.aggregations aggregatedReactionsOnEvent:eventId inRoom:room.roomId];

        XCTAssertNotNil(reactions.reactions);
        XCTAssertEqual(reactions.reactions.count, 1);

        MXReactionCount *reactionCount = reactions.reactions.firstObject;
        XCTAssertEqualObjects(reactionCount.reaction, @"👍");
        XCTAssertEqual(reactionCount.count, 1);
        XCTAssertTrue(reactionCount.myUserHasReacted);

        [expectation fulfill];
    }];
}

// - Run the initial condition scenario
// - Add one more reaction
// -> We must get notified about the reaction count change
- (void)testAggregationsListener
{
    // - Run the initial condition scenario
    [self createScenario:^(MXSession *mxSession, MXRoom *room, XCTestExpectation *expectation, NSString *eventId, NSString *reactionEventId) {

        // -> We must get notified about the reaction count change
        [mxSession.aggregations listenToReactionCountUpdateInRoom:room.roomId block:^(NSDictionary<NSString *,MXReactionCountChange *> * _Nonnull changes) {

            XCTAssertEqual(changes.count, 1, @"Only one change");

            MXReactionCountChange *change = changes[eventId];
            XCTAssertNotNil(change);
            XCTAssertNil(change.modified);
            XCTAssertNil(change.deleted);

            XCTAssertEqual(change.inserted.count, 1, @"Only one change");
            MXReactionCount *reactionCount = change.inserted.firstObject;
            XCTAssertEqualObjects(reactionCount.reaction, @"😄");
            XCTAssertEqual(reactionCount.count, 1);
            XCTAssertTrue(reactionCount.myUserHasReacted,);

            [expectation fulfill];
        }];

        // - Add one more reaction
        [mxSession.aggregations sendReaction:@"😄" toEvent:eventId inRoom:room.roomId success:^(NSString *eventId) {
        } failure:^(NSError *error) {
            XCTFail(@"The operation should not fail - NSError: %@", error);
            [expectation fulfill];
        }];
    }];
}

// - Run the initial condition scenario
// - Unreact
// -> We must get notified about the reaction count change
// -> Data from aggregations must be right
- (void)testUnreact
{
    // - Run the initial condition scenario
    [self createScenario:^(MXSession *mxSession, MXRoom *room, XCTestExpectation *expectation, NSString *eventId, NSString *reactionEventId) {

        // -> We must get notified about the reaction count change
        [mxSession.aggregations listenToReactionCountUpdateInRoom:room.roomId block:^(NSDictionary<NSString *,MXReactionCountChange *> * _Nonnull changes) {

            XCTAssertEqual(changes.count, 1, @"Only one change");

            MXReactionCountChange *change = changes[eventId];
            XCTAssertNotNil(change.deleted);
            XCTAssertNil(change.modified);
            XCTAssertNil(change.inserted);

            XCTAssertEqual(change.deleted.count, 1, @"Only one change");
            NSString *reaction = change.deleted.firstObject;
            XCTAssertEqualObjects(reaction, @"👍");

            // -> Data from aggregations must be right
            MXAggregatedReactions *reactions = [mxSession.aggregations aggregatedReactionsOnEvent:eventId inRoom:room.roomId];
            XCTAssertNil(reactions);

            [expectation fulfill];
        }];

        // - Unreact
        [mxSession.aggregations unReactOnReaction:@"👍" toEvent:eventId inRoom:room.roomId success:^() {
        } failure:^(NSError *error) {
            XCTFail(@"The operation should not fail - NSError: %@", error);
            [expectation fulfill];
        }];
    }];
}



@end

#pragma clang diagnostic pop
