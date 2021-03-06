/****************************************************************

 test_object.c

 =============================================================

 Copyright 1996-2020 Tom Barbalet. All rights reserved.

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

 This software is a continuing work of Tom Barbalet, begun on
 13 June 1996. No apes or cats were harmed in the writing of
 this software.

 ****************************************************************/

#include "../toolkit/toolkit.h"

#include <stdio.h>

n_int draw_error(n_constant_string error_text, n_constant_string location, n_int line_number)
{
    if (error_text)
    {
        printf("ERROR: %s @ %s %ld\n", (n_constant_string)error_text, location, line_number);
    }
    return -1;
}

static n_object * check_element(n_int value)
{
    n_string_block string_value = "0 The good ship";
    
    string_value[0] += value;
    
    {
        n_object * element_obj = object_string(0L, "name", string_value);
        object_number(element_obj, "number", value);
        object_number(element_obj, "constant", 4321);

        return element_obj;
    }
}

static void check_object(void)
{
    n_object * new_object = object_number(0L, "index", 1);

    n_object * sub_object = object_number(0L, "index", 2);
    n_array  * new_array = array_number(-1);
    
    array_add(new_array, array_number(-1));
    array_add(new_array, array_number(-1));
    array_add(new_array, array_string("hello"));
    array_add(new_array, array_number(2));
    array_add(new_array, array_string("is"));
    array_add(new_array, array_number(4));
    array_add(new_array, array_string("it me"));
    array_add(new_array, array_number(50));
        
    object_number(sub_object, "top", 3);
    
    object_array(sub_object, "array", new_array);

    array_add(new_array, array_number(10));
    array_add(new_array, array_number(20));
    array_add(new_array, array_number(30));
    
    io_file_debug(obj_json(sub_object));

    object_number(new_object, "top", 2);
    object_string(new_object, "name", "Benson");
    
    object_string(new_object, "another", "corner");
    
    io_file_debug(obj_json(new_object));

    object_string(new_object, "name", "Kevin");

    io_file_debug(obj_json(sub_object));
    io_file_debug(obj_json(new_object));

    {
        n_uint        count = 1;
        n_object    *being_object = check_element(0);
        n_array     *beings = array_object(being_object);
        while (count < 9)
        {
            being_object = check_element(count++);
            array_add(beings, array_object(being_object));
        }
        object_array(new_object, "beings", beings);
    }
    
    io_file_debug(obj_json(new_object));
    
    object_object(new_object, "example", sub_object);

    io_file_debug(obj_json(new_object));

    object_number(sub_object, "top", 4);

    io_file_debug(obj_json(new_object));
    io_file_debug(obj_json(sub_object));

    obj_free(&new_object);
}

int main(int argc, const char * argv[])
{
    printf(" --- test object --- start --------------------------------------------\n");
    
    check_object();

    printf(" --- test object ---  end  --------------------------------------------\n");
    
    return 0;
}

