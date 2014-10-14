//
//  MacSimulation_Tests.m
//  MacSimulation Tests
//
//  Created by Thomas Barbalet on 7/4/13.
//  Copyright (c) 2014 Thomas Barbalet. All rights reserved.
//

#import <XCTest/XCTest.h>

#define OVERWRITE_THREADED

#include "../sim/noble/noble.h"
#include "../sim/entity/entity.h"
#include "../sim/universe/universe.h"
#include "../sim/noble/shared.h"


@interface MacSimulation_Tests : XCTestCase

@end

@implementation MacSimulation_Tests

noble_simulation * value = 0L;
noble_being      * being = 0L;

- (void)setUp
{
    [super setUp];
    
    (void)shared_new(0x1382);

    value = sim_sim();
    
    being = &(value->beings[2]);
}

- (void)tearDown
{
    // Tear-down code here.
    
    [super tearDown];
}

- (void)testCheckInitialTime
{
    XCTAssertTrue(value->land->time == 0, @"Time is not initially zero");
}

- (void)testCheckInitialDate
{
    XCTAssertTrue(value->land->date == AGE_OF_MATURITY, @"Days is not initially AGE_OF_MATURITY (%ld)", (n_int)AGE_OF_MATURITY);
}

- (void)testMapCheckSum
{
    n_uint hash = math_hash((n_byte *)value->land->topology, MAP_AREA);
    XCTAssertTrue(hash == 0x3af2658629d5a81c, @"Hash doesn't comply with prior map hashes (%lx)", hash);
}

- (void)testWeatherCheckSum
{
    n_uint hash = math_hash((n_byte *)value->land->atmosphere, (MAP_AREA*sizeof(n_c_int)/2));
    XCTAssertTrue(hash == 0x6f8fde121eec96e3, @"Hash doesn't comply with prior weather hashes (%lx)", hash);
}

- (void)testBeingCheckSum
{

    n_int length = sizeof(noble_being) - sizeof(n_byte *) - sizeof(noble_social *) - sizeof(noble_episodic *);
    n_uint hash = math_hash((n_byte *)being, length);
    XCTAssertTrue(length == 68204, @"Length doesn't comply with prior length (%ld)", length);
    
    XCTAssertTrue(hash == 0x9d20a6b1fcc7b75f, @"Hash doesn't comply with prior being hashes (%lx)", hash);
}

- (void)testBrainCheckSum
{
    n_uint hash = math_hash(being->brain, DOUBLE_BRAIN);
    XCTAssertTrue(hash == 0xca440ed899ee9512, @"Hash doesn't comply with prior brain hashes (%lx)", hash);
}

- (void)testSocialCheckSum
{
    n_uint hash = math_hash((n_byte *)being->social, SOCIAL_SIZE * sizeof(noble_social));
    XCTAssertTrue(hash == 0x107aec78c7943fda, @"Hash doesn't comply with prior social hashes (%lx)", hash);
}

- (void)testEpisodicCheckSum
{
    n_uint hash = math_hash((n_byte *)being->episodic, EPISODIC_SIZE * sizeof(noble_episodic));
    XCTAssertTrue(hash == 0xcc7da6d33d1f14ed, @"Hash doesn't comply with prior episodic hashes (%lx)", hash);
}


- (void)actionMouseClick
{
    n_int   map_x = APESPACE_TO_MAPSPACE(being->location[0]);
    n_int   map_y = APESPACE_TO_MAPSPACE(being->location[1]);

    shared_mouseReceived(map_x, map_y, NUM_VIEW);
}

- (void)testMouseClick
{
    [self actionMouseClick];
    shared_cycle(1000, NUM_VIEW, 512, 512);
    shared_cycle(1000, NUM_TERRAIN, 512, 512);
    shared_mouseUp();
    shared_cycle(1000, NUM_VIEW, 512, 512);
    shared_cycle(1000, NUM_TERRAIN, 512, 512);
    XCTAssertTrue(value->select == being, @"Selected being is found");
}

- (void)testMouseDrag
{
    XCTAssertTrue(being->location[0] == 9644, @"X value is wrong (%d)", being->location[0]);
    XCTAssertTrue(being->location[1] == 1486, @"Y value is wrong (%d)", being->location[1]);
    
    [self actionMouseClick];
    shared_cycle(1000, NUM_VIEW, 512, 512);
    shared_cycle(1000, NUM_TERRAIN, 512, 512);
    
    shared_mouseReceived(100, 100, NUM_VIEW);
    shared_mouseOption(1);
    
    shared_cycle(1000, NUM_VIEW, 512, 512);
    shared_cycle(1000, NUM_TERRAIN, 512, 512);
    shared_mouseOption(0);
    shared_mouseUp();
    
    shared_cycle(1000, NUM_VIEW, 512, 512);
    shared_cycle(1000, NUM_TERRAIN, 512, 512);
    NSLog(@"Location %d %d", being->location[0], being->location[1]);
    
    XCTAssertTrue(being->location[0] == 6400, @"X value is wrong (%d)", being->location[0]);
    XCTAssertTrue(being->location[1] == 6400, @"Y value is wrong (%d)", being->location[1]);
}

- (void)testLongtermSimulationCompare
{
    /*n_int  time = 3;*/
    n_uint  hash = math_hash((n_byte *)being, sizeof(noble_being));
    
    XCTAssertTrue(hash == 0x5e6b344d7429f588, @"Starting hash doesn't comply with prior being hashes (%lx)", hash);
    
    shared_cycle(1000, NUM_VIEW, 512, 512);
    shared_cycle(1000, NUM_TERRAIN, 512, 512);
    
    /*hash = math_hash((n_byte *)being, sizeof(noble_being));*/

    hash = math_hash((n_byte *)being, 60);
    
    XCTAssertTrue(hash == 0xfd9f28ee82fa76bc, @"Longterm hash doesn't comply with prior being hashes (%lx)", hash);
    
    hash = math_hash((n_byte *)being, 30228);

    XCTAssertTrue(hash == 0x47e7ab44927a7362, @"Longterm hash doesn't comply with prior being hashes (%lx)", hash);
    
    /* 68228 */
    
   /*
    while (time < ((TIME_DAY_MINUTES * 1)/2))

    
    while (time < 32)
    {
        shared_cycle(1000 * time, NUM_VIEW, 512, 512);
        shared_cycle(1000 * time, NUM_TERRAIN, 512, 512);
        time++;
    }
    
    hash = math_hash((n_byte *)being, sizeof(noble_being));
    XCTAssertTrue(hash == 0x69595e73ae0ae6e5, @"Longterm hash doesn't comply with prior being hashes (%lx)", hash);
    
    hash = math_hash(being->brain, DOUBLE_BRAIN);
    XCTAssertTrue(hash == 0x7163fd4b9551887c, @"Longterm hash doesn't comply with prior brain hashes (%lx)", hash);
    
    hash = math_hash((n_byte *)being->social, SOCIAL_SIZE * sizeof(noble_social));
    XCTAssertTrue(hash == 0x67a7486ae7a8b094, @"Longterm hash doesn't comply with prior social hashes (%lx)", hash);
    
    hash = math_hash((n_byte *)being->episodic, EPISODIC_SIZE * sizeof(noble_episodic));
    XCTAssertTrue(hash == 0xcc7da6d33d1f14ed, @"Longterm hash doesn't comply with prior episodic hashes (%lx)", hash);
    */
}
/*
- (void)testLongtermSimulation
{
    n_uint  time = 720;
    n_uint  hash;
    
    [self testLongtermSimulationCompare];
    
    while (time < ((TIME_DAY_MINUTES * 3)/2))
    {
        shared_cycle(1000 * time, NUM_VIEW, 512, 512);
        shared_cycle(1000 * time, NUM_TERRAIN, 512, 512);
        time++;
    }
    
    hash = math_hash((n_byte *)being->brain, DOUBLE_BRAIN);
    XCTAssertTrue(hash == 0xd0b123540e682ed, @"Longterm hash doesn't comply with prior brain hashes (%lx)", hash);
    
    hash = math_hash((n_byte *)being->social, SOCIAL_SIZE * sizeof(noble_social));
    XCTAssertTrue(hash == 0xc30e6b4f593077e1, @"Longterm hash doesn't comply with prior social hashes (%lx)", hash);
    
    hash = math_hash((n_byte *)being->episodic, EPISODIC_SIZE * sizeof(noble_episodic));
    XCTAssertTrue(hash == 0xfac4fefa508630b6, @"Longterm hash doesn't comply with prior episodic hashes (%lx)", hash);
    
    XCTAssertTrue(value->num == 42, @"Longterm number apes doesn't comply with prior numbers (%ld)", value->num);

    hash = math_hash((n_byte *)value->land->topology, MAP_AREA);
    XCTAssertTrue(hash == 0x3af2658629d5a81c, @"Longterm hash doesn't comply with prior map hashes (%lx)", hash);
}
*/

@end
