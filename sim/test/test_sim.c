/****************************************************************

 test_sim.c

 =============================================================

 Copyright 1996-2019 Tom Barbalet. All rights reserved.

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

 This software and Noble Ape are a continuing work of Tom Barbalet,
 begun on 13 June 1996. No apes or cats were harmed in the writing
 of this software.

 ****************************************************************/

#include <stdio.h>
#include <time.h>

#include "../noble/noble.h"
#include "../entity/entity.h"
#include "../universe/universe.h"

n_int draw_error(n_constant_string error_text, n_constant_string location, n_int line_number)
{
    printf("ERROR: %s @ %s %ld\n",(const n_string) error_text, location, line_number);
    return -1;
}

n_uint test_distance_moved(noble_simulation * local, n_int show_stopped)
{
    n_int loop = 0;
    n_uint total_moved = 0;
    while (loop < local->num)
    {
        noble_being * being = &local->beings[loop];
        n_byte        speed = being_speed(being);
        if(IS_NIGHT(land_time()) == 0)
        {
            if ((speed == 0) && show_stopped)
            {
                n_string_block name_string;
                n_string_block time_string;
                io_time_to_string(time_string);
                being_name_simple(being, name_string);
                printf("%s %s stopped\n", time_string, name_string);
            }
        }
        total_moved += speed;
        loop++;
    }
    return total_moved;
}

void test_total_moved(noble_simulation * local)
{
#ifdef DEBUG_LACK_OF_MOVEMENT
    n_int loop = 0;
    while (loop < local->num)
    {
        noble_being * being = &local->beings[loop];
        n_int        total_moved = being_total_movement(being);
        if(total_moved == 0)
        {
            n_string_block name_string;
            n_file *description = obj_json(file_being(being));
            being_name_simple(being, name_string);
            printf("%s stopped\n", name_string);
            io_file_debug(description);
            
        }
        loop++;
    }
#endif
}

void test_sim_run(void)
{
    n_int   counter = 0;

    noble_simulation * local_sim = sim_sim();
    noble_being      * first_being = &(local_sim->beings[0]);
    noble_being      * second_being = &(local_sim->beings[1]);
    noble_being_constant * first_being_constant = &(first_being->constant);
    noble_being_delta * first_being_delta = &(first_being->delta);
    noble_being_events * first_being_events = &(first_being->events);
    noble_being_brain * first_being_brain = &(first_being->braindata);
    noble_immune_system * first_being_immune = &(first_being->immune_system);
    noble_being_volatile * first_being_volatile = &(first_being->changes);
    
    n_uint being_hash1 = math_hash((n_byte *)first_being, sizeof(noble_being));
    n_uint being_constant_hash1 = math_hash((n_byte *)first_being_constant, sizeof(noble_being_constant));
    n_uint being_delta_hash1 = math_hash((n_byte *)first_being_delta, sizeof(noble_being_delta));
    n_uint being_events_hash1 = math_hash((n_byte *)first_being_events, sizeof(noble_being_events));
    n_uint being_brain_hash1 = math_hash((n_byte *)first_being_brain, sizeof(noble_being_brain));
    n_uint being_immune_hash1 = math_hash((n_byte*)first_being_immune, sizeof(noble_immune_system));
    n_uint being_volatile_hash1 = math_hash((n_byte*)first_being_volatile, sizeof(noble_being_volatile));
    
    n_uint distance_moved = 0;
    
    while (counter < 2048)
    {
        n_uint distance_delta;
        sim_cycle();
        distance_delta = test_distance_moved(local_sim, 0);
        
        distance_moved += distance_delta;
        if ((counter & 511) == 0)
        {
            printf("%ld distance moved %ld running total %ld\n", counter, distance_delta, distance_moved);

        }
        counter ++;
    }
    printf("total distance moved %ld\n", distance_moved);

    n_uint being_hash2 = math_hash((n_byte *)first_being, sizeof(noble_being));
    n_uint being_constant_hash2 = math_hash((n_byte *)first_being_constant, sizeof(noble_being_constant));
    n_uint being_delta_hash2 = math_hash((n_byte *)first_being_delta, sizeof(noble_being_delta));
    n_uint being_events_hash2 = math_hash((n_byte *)first_being_events, sizeof(noble_being_events));
    n_uint being_brain_hash2 = math_hash((n_byte *)first_being_brain, sizeof(noble_being_brain));
    n_uint being_immune_hash2 = math_hash((n_byte*)first_being_immune, sizeof(noble_immune_system));
    n_uint being_volatile_hash2 = math_hash((n_byte*)first_being_volatile, sizeof(noble_being_volatile));
    
    printf("hash %lx\n", being_hash1 ^ being_hash2);
    printf("constant %lx\n", being_constant_hash1 ^ being_constant_hash2);
    
    printf("delta %lx\n", being_delta_hash1 ^ being_delta_hash2);
    printf("events %lx\n", being_events_hash1 ^ being_events_hash2);
    
    printf("brain %lx\n", being_brain_hash1 ^ being_brain_hash2);
    printf("immune %lx\n", being_immune_hash1 ^ being_immune_hash2);
    
    printf("volatile %lx\n", being_volatile_hash1 ^ being_volatile_hash2);
    
    {
        n_int number_beings = local_sim->num;
        n_int outer_loop = 0;
        n_int line_of_sight_count = 0;
        while (outer_loop < (number_beings-1))
        {
            n_int inner_loop = 1 + outer_loop;
            while (inner_loop < number_beings)
            {
                n_vect2 location_vect;
                being_space(&local_sim->beings[inner_loop], &location_vect);
                line_of_sight_count += being_line_of_sight(&local_sim->beings[outer_loop],
                                                 &location_vect);
                inner_loop++;
            }
            outer_loop++;
        }
        printf("line-of-sight count %ld for %ld\n", line_of_sight_count, number_beings);
    }
    
    test_total_moved(local_sim);
}

int main(int argc, const char * argv[])
{
    printf(" --- test sim --- start -----------------------------------------------\n");
    
    sim_init(KIND_START_UP, 0x12738291, MAP_AREA, 0);
    
    test_sim_run();
    
    sim_init(KIND_NEW_SIMULATION, 0xE2F38A98, MAP_AREA, 0);
    
    test_sim_run();
    
    sim_close();
    
    printf(" --- test sim ---  end  -----------------------------------------------\n");
    
    return 1;
}

