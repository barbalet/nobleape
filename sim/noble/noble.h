/****************************************************************

 noble.h

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

/*! \file   noble.h
 *  \brief  This is the interface between the Noble toolkit and what consumes
    the Noble Toolkit.
 */

#ifndef _NOBLEAPE_NOBLE_H_
#define _NOBLEAPE_NOBLE_H_
/*	Variable Definitions */

#include <signal.h>

#define	 SCRIPT_DEBUG             /* Add all the runtime debug */
#undef   COMMAND_LINE_DEBUG       /* Sends the debug output as printf output - added through command line build */
#undef   ROUGH_CODE_OUT           /* printf outputs the interpret stream in character number format */

#ifndef _WIN32
#define  NEW_LAND_METHOD
#endif

#undef   SKELETON_RENDER

#ifndef    _WIN32
#define   ALPHA_WEATHER_DRAW
#define   METAL_RENDER
#else
#undef   ALPHA_WEATHER_DRAW
#endif

#define  FAST_START_UNREALISTIC_INITIAL_WEATHER

#define CHAR_SPACE               (32)
#define IS_RETURN(val)            (((val) == 10) || ((val) == 13))
#define IS_SPACE(val)             ((val) == CHAR_SPACE)

#ifdef AUTOMATED

#define FIXED_RANDOM_SIM 0x5261f726

#endif

#ifdef COMMAND_LINE_EXPLICIT

#define  NOBLE_APE_ASSERT

#else

#define  NOBLE_APE_ASSERT

#endif

/*! @define */
#define	SHORT_VERSION_NAME		 "Noble Ape 0.699 "
#define	FULL_DATE				 __DATE__

/*! @define */
#define	VERSION_NUMBER		     699
#define	COPYRIGHT_DATE		     "Copyright 1996 - 2019 "

#define FULL_VERSION_COPYRIGHT "Copyright Tom Barbalet, 1996-2019."

/*! @define */
#define	NOBLE_APE_SIGNATURE		    (('N'<< 8) | 'A')
#define	NOBLE_WARFARE_SIGNATURE		(('N'<< 8) | 'W')

#define  COPYRIGHT_NAME		    "Tom Barbalet. "
#define  COPYRIGHT_FOLLOW		"All rights reserved."

#define PACKED_DATA_BLOCK   (32*32*32*2)

typedef	double	n_double;

#define TWO_PI ((n_double)(6.2831853071795864769252867665590057683943))

#define SINE_MAXIMUM (26880)


typedef char n_char;

/*! @typedef n_string
 @discussion This is the string format for the Noble Ape development */
typedef n_char *          n_string;

typedef const n_char *    n_constant_string;

#define STRING_BLOCK_SIZE (2048)

typedef n_char       n_string_block[STRING_BLOCK_SIZE];

/*! @typedef n_byte
@discussion This is a single byte data unit. */
typedef	unsigned char	n_byte;
/*! @typedef n_byte2
@discussion This is a two byte data unit. There is no
 expectations on the byte ordering. */
typedef	unsigned short	n_byte2;

typedef	unsigned int	n_byte4;
typedef	int				n_c_int;

#ifndef _WIN64
/*! @typedef n_uint
@discussion This is a four byte data unit. Please note that
this form may be shown as unsigned int on some 64-bit
implementations. */
typedef	unsigned long	n_uint;

/*! @typedef n_int
@discussion This is the native integer data unit. It can be
whatever length is specified by the implementation. */
typedef	long			n_int;

#else

typedef	unsigned long long	n_uint;
typedef	long long			n_int;

#endif

typedef	short	n_audio;


#ifdef NOBLE_APE_ASSERT

#define NA_ASSERT(test, message) if(!(test)){io_assert(message, __FILE__, __LINE__); raise(SIGINT);}

void io_assert(n_string message, n_string file_loc, n_int line);

#else

#define NA_ASSERT(test, message) /* test message */

#endif

#define AUDIO_FFT_MAX_BITS      (15)
#define AUDIO_FFT_MAX_BUFFER    (1<<AUDIO_FFT_MAX_BITS)

void audio_fft(n_byte inverse, n_uint power_sample);
void audio_new_fft(n_uint       power_sample,
                   n_int      InverseTransform,
                   n_double    *RealIn,
                   n_double    *ImagIn,
                   n_double    *RealOut,
                   n_double    *ImagOut );
void   audio_clear_buffers(n_uint length);
void   audio_clear_output(n_audio * audio, n_uint length);

void   audio_equal_output(n_audio * audio, n_uint length);

void   audio_multiply_output(n_audio * audio, n_uint length);
void   audio_set_frequency(n_uint entry, n_uint value);

void   audio_low_frequency(n_audio * buffer, n_int number_freq, n_int debug);

void audio_buffer_clear(n_audio * buffer, n_int size);
void audio_buffer_double_clear(n_double * buffer, n_int size);

void audio_buffer_copy_to_audio(n_double * buffer_double, n_audio * buffer_audio, n_int size);
void audio_buffer_copy_to_double(n_audio * buffer_audio, n_double * buffer_double, n_int size);
void audio_buffer_copy_to_double_double(n_double * buffer_double1, n_double * buffer_double2, n_int size);
void audio_buffer_copy_to_double_double(n_double * buffer_double_to, n_double * buffer_double_from, n_int size);



/*! @struct
@field signature The program signature defined as NOBLE_APE_SIGNATURE
through the Noble Ape Simulation file handling etc.
@field version The version of the implementation that is the version
number defined as VERSION_NUMBER (thus changes between versions).
@discussion This is a means of tracking the programs and file versions
that are used primarily through the file interface but also through
other implementations over the Noble Ape development. This includes
Noble Warfare currently, but potentially could include other software
that accepts the Noble Ape text based file format.
*/
typedef	struct
{
    n_byte2 signature;
    n_byte2 version;
} n_version;

/*! @struct
 @field x     X value in the 2d vector.
 @field y     Y value in the 2d vector.
 @discussion  This is the bases of two dimensional vectors in the simulation.
 */


typedef union
{
    struct
    {
        n_int x;
        n_int y;
    };
    n_int data[2];
}n_vect2;


typedef union
{
    struct
    {
        n_vect2 top_left;
        n_vect2 bottom_right;
    };
    n_int data[4];
} n_area2;


typedef union
{
    struct
    {
        n_double x;
        n_double y;
        n_double z;
    };
    n_double data[3];
} n_vect3;

typedef struct {
    n_byte r;
    n_byte g;
    n_byte b;
    n_byte a;
}n_rgba;

typedef union
{
    n_rgba  rgba;
    n_byte4 thirtytwo;
}n_rgba32;

typedef struct {
    n_vect2 * points;
    n_int no_of_points;
    n_int max_points;
}n_points;

typedef struct
{
    n_byte4  date;
    n_byte2  location[2];
    n_byte4  time;
}
n_spacetime;

/*! @struct
 @field characters Characters that represent these values in the file.
 @field incl_kind  Included type and the kind of value combined together.
 @field number     Number of these values.
 @field location   Byte location of the start of these values within the struct.
 @field what_is_it Provides a string output for HTML documentation for the user.
 @discussion This is the line by line definition of the Noble Ape file types.
 */
typedef	struct
{
    n_byte  characters[7];
    n_byte  incl_kind;
    n_uint  number_entries;
    n_uint  start_location;
    const n_string what_is_it;
} noble_file_entry;

#define POPULATED(ch) ((ch[0] != 0) || (ch[1] != 0) || (ch[2] != 0) || (ch[3] != 0) || (ch[4] != 0) || (ch[5] != 0))

/* include externally, if needed */

#define FILE_COPYRIGHT      0x00

typedef enum
{
    FILE_TYPE_BYTE		= 0x01,
    FILE_TYPE_BYTE2		= 0x02,
    FILE_TYPE_BYTE_EXT	= 0x03,
    FILE_TYPE_PACKED	= 0x05,
    FILE_TYPE_BYTE4     = 0x06
} file_element_type;

typedef enum
{
    ET_NOBLE_APE,
    ET_NOBLE_APE_GHOST,
    ET_FIERCE_FELINE,
    ET_FIERCE_BIRD_OF_PREY,
}entity_type;

#define FILE_INCL(num)      ((num) & 0xf0)
#define FILE_KIND(num)      ((num) & 0x0f)

#define	FILE_EOF				0x0100

#define	ASCII_NUMBER(val)		(((val) >= '0') && ((val) <= '9'))
#define	ASCII_LOWERCASE(val)	(((val) >= 'a') && ((val) <= 'z'))
#define	ASCII_UPPERCASE(val)	(((val) >= 'A') && ((val) <= 'Z'))

#define FILE_OKAY				  0x0000
#define	FILE_ERROR				  (-1)

typedef n_string (n_console_input)(n_string value, n_int length);

typedef void (n_console_output)(n_constant_string value);

typedef n_byte (n_pixel)(n_int px, n_int py, n_int dx, n_int dy, void * information);

typedef n_int (n_memory_location)(n_int px, n_int py);

typedef n_byte2 (n_patch)(n_byte2 * local);

typedef n_int (n_console)(void * ptr, n_string response, n_console_output output_function);

typedef n_int (n_file_in)(n_byte * buff, n_uint len);

typedef n_byte * (n_file_out)(n_uint * len);

typedef struct
{
    n_console * function;
    n_string    command;
    n_string    addition;
    n_string    help_information;
} noble_console_command;

/*! @struct
@field pixel_draw The n_pixel function used to draw pixels into
the window buffer.
@field information This is the pointer information passed into
the pixel_draw function.
@discussion This drawing interface was designed to write to
window buffer information where the window buffer could be
either color or monochrome. The interface needed to be relatively
independent to allow for text to be written into either a color
window or a monochrome window or a line to be drawn through
either window buffer without there being any difference in the
implementation algorithm. Think of this method as a way of
translating high-level drawing and low-level drawing.
*/
typedef struct
{
    n_pixel * pixel_draw;
    void	* information;
} n_join;

typedef struct
{
    n_byte	* screen;
    n_byte	* background;
} n_background8;

typedef struct
{
    n_byte  * screen;
    n_byte	  color;
} n_color8;

/*! @struct
@field size The size of the file in bytes.
@field location The location of the accessing pointer within the
file. This is useful for both input and output files.
@field data The data stored in bytes.
@discussion This is the primary file handling structure in Noble
Ape. It is used for both input and output files. It is the method
used to pass file information from the platform layer into the
platform independent layers of Noble Ape.
*/
typedef struct
{
    n_uint	size;
    n_uint	location;
    n_byte	*data;
} n_file;

typedef void (n_file_specific)(n_string string, n_byte * reference);

typedef struct
{
    void * data;
    n_uint expected_bytes;
    n_uint hash;
    void * next;
} n_file_chain;

typedef enum
{
    OBJECT_EMPTY = 0,
    OBJECT_STRING,
    OBJECT_NUMBER,
    OBJECT_OBJECT,
    OBJECT_ARRAY = 4,
}n_object_type;

typedef enum
{
    OBJ_TYPE_EMPTY = 0,
    OBJ_TYPE_STRING_NOTATION,
    OBJ_TYPE_NUMBER,
    OBJ_TYPE_COLON,
    OBJ_TYPE_COMMA,
    OBJ_TYPE_OBJECT_OPEN,
    OBJ_TYPE_OBJECT_CLOSE,
    OBJ_TYPE_ARRAY_OPEN,
    OBJ_TYPE_ARRAY_CLOSE
}n_object_stream_type;


typedef struct
{
    n_string       data;
    void*          next;
    n_object_type  type;
}n_array;

typedef struct
{
    n_array        primitive;
    n_string       name;
    n_uint         name_hash;
}n_object;

n_file * obj_json(n_object * object);

n_array * array_number(n_int set_number);
n_array * array_string(n_string set_string);
n_array * array_object(n_object * set_object);
n_array * array_array(n_array * set_array);

n_array * array_add(n_array * array, n_array * element);

n_object * obj_number(n_object * obj, n_string name, n_int number);
n_object * obj_string(n_object * obj, n_string name, n_string string);
n_object * obj_object(n_object * obj, n_string name, n_object * object);
n_object * obj_array(n_object * obj, n_string name, n_array * array);

void object_top_object(n_file * file, n_object * top_level);
n_object * object_file_to_tree(n_file * file);

void obj_free(n_object ** object);

n_string obj_contains(n_object* base, n_string name, n_object_type type);
n_int    obj_contains_number(n_object* base, n_string name, n_int *number);

n_array *  obj_get_array(n_string array);
n_object * obj_get_object(n_string object);
n_int      obj_get_number(n_string object);

n_array * obj_array_next(n_array * array, n_array * element);
n_int     obj_array_count(n_array * array_obj);

/** \brief sine and cosine conversation */
#define	NEW_SD_MULTIPLE			26880


#define    NUM_VIEW    (0)
#define    NUM_TERRAIN (1)

#define WINDOW_PROCESSING NUM_TERRAIN

/* maximum bytes in a braincode program */
#define BRAINCODE_SIZE                    128

/* number of probes which can be applied to the brain */
#define BRAINCODE_PROBES                  (BRAINCODE_SIZE>>3)

#define BRAINCODE_PSPACE_REGISTERS         3
/* maximum frequency of a brain probe */
#define BRAINCODE_MAX_FREQUENCY            16

/* number of bytes per instruction */
#define BRAINCODE_BYTES_PER_INSTRUCTION    3

/* number of instructions which a MVB copies */
#define BRAINCODE_BLOCK_COPY               16

#define BRAINCODE_MAX_ADDRESS              (BRAINCODE_SIZE*2)
#define BRAINCODE_ADDRESS(i)               ((i) % BRAINCODE_MAX_ADDRESS)

enum braincode_locations
{
    BRAINCODE_EXTERNAL = 0,
    BRAINCODE_INTERNAL
};

/* instruction codes */

enum BRAINCODE_COMMANDS
{
    /* data */
    BRAINCODE_DAT0 = 0,
    BRAINCODE_DAT1,

    /* operators */
    BRAINCODE_ADD,
    BRAINCODE_SUB,
    BRAINCODE_MUL,
    BRAINCODE_DIV,
    BRAINCODE_MOD,
    BRAINCODE_MVB,
    BRAINCODE_MOV,
    BRAINCODE_JMP,
    BRAINCODE_CTR,
    BRAINCODE_SWP,
    BRAINCODE_INV,
    BRAINCODE_STP,
    BRAINCODE_LTP,

    /* conditionals */
    BRAINCODE_JMZ,
    BRAINCODE_JMN,
    BRAINCODE_DJN,
    BRAINCODE_AND,
    BRAINCODE_OR,
    BRAINCODE_SEQ,
    BRAINCODE_SNE,
    BRAINCODE_SLT,

    /* sensors */
    BRAINCODE_SEN,
    BRAINCODE_SEN2,
    BRAINCODE_SEN3,

    /* actuators */
    BRAINCODE_ACT,
    BRAINCODE_ACT2,
    BRAINCODE_ACT3,
    BRAINCODE_ANE,

    BRAINCODE_INSTRUCTIONS
};

typedef enum
{
    WEATHER_SEVEN_ERROR = -1,
    WEATHER_SEVEN_SUNNY_DAY = 0,
    WEATHER_SEVEN_CLOUDY_DAY = 1,
    WEATHER_SEVEN_RAINY_DAY = 2,
    WEATHER_SEVEN_CLEAR_NIGHT = 3,
    WEATHER_SEVEN_CLOUDY_NIGHT = 4,
    WEATHER_SEVEN_RAINY_NIGHT = 5,
    WEATHER_SEVEN_DAWN_DUSK = 6
} weather_values;

enum window_information
{
    TERRAIN_WINDOW_WIDTH		= (4096),
    TERRAIN_WINDOW_HEIGHT       = (3072),
    TERRAIN_WINDOW_AREA			= (TERRAIN_WINDOW_WIDTH * TERRAIN_WINDOW_HEIGHT)
};

typedef enum
{
    KIND_LOAD_FILE = 0,
    KIND_NEW_SIMULATION,
    KIND_NEW_APES,
    KIND_START_UP,
    KIND_MEMORY_SETUP,
} KIND_OF_USE;

typedef enum
{
    AE_NO_ERROR = -1,

    AE_UNKNOWN_ERROR,
    AE_NUMBER_EXPECTED,
    AE_NUMBER_OUT_OF_RANGE,
    AE_MAXIMUM_NUMBERS_REACHED,
    AE_MAXIMUM_SCRIPT_SIZE_REACHED,

    AE_MAXIMUM_VARIABLES_REACHED,
    AE_UNKNOWN_SYNTAX_PARSER_BUFFER,
    AE_UNKNOWN_SYNTAX_PARSER_CONVERT,

    AE_SELECTED_ENTITY_OUT_OF_RANGE,

    AE_COORDINATES_OUT_OF_RANGE,
    AE_VALUE_OUT_OF_RANGE,

    AE_TOO_MANY_CLOSE_BRACES,
    AE_MAXIMUM_BRACES_REACHED,
    AE_FIRST_VALUE_FAILED,
    AE_SECOND_VALUE_FAILED,
    AE_UNKNOWN_SYNTAX_MISSING_EQUALS,
    AE_UNKNOWN_SYNTAX_NO_COMMAND,
    AE_WRONG_END,

    AE_LINE_START_INCORRECT,
    AE_OUTPUT_SET_AS_INPUT_VARIABLE,
    AE_IF_WHILE_NOT_FOLLOWED_BY_BRACKET,
    AE_FUNCTION_ISNT_VARIABLE,
    AE_NON_FUNCTION_APPLIED,
    AE_FUNCTION_DEFINED_PRIOR,
    AE_FUNCTION_OUT_OF_RANGE,
    AE_WITHOUT_SEMICOLON,
    AE_WITHOUT_OPEN_BRACE,
    AE_FUNCTION_SETTING_FAILED,
    AE_ERROR_STARTING_MAIN,
    AE_CODE_AFTER_MAIN,
    AE_NO_CLOSE_BRACE_TO_END_OF_FILE,
    AE_CODE_OUTSIDE_FUNCTION,
    AE_INPUT_VARIABLE_WITHOUT_EQUALS,
    AE_ASSIGN_VALUE_FAILED,
    AE_UNKNOWN_SYNTAX_FROM_INTERPRET,
    AE_NO_MAIN_CODE,

    AE_NUMBER_ERRORS
} AE_ENUM;

typedef struct
{
    AE_ENUM  enum_value;
    n_constant_string error_string;
    n_constant_string help_string;
} n_ae_error;

/** \brief apescript_errors track the errors in ApeScript and additional text for user-manual level documentation */
static const n_ae_error apescript_errors[]=
{
    {AE_UNKNOWN_ERROR,                     "Unknown error",                    "Please contact tom at nobleape dot com"},
    {AE_NUMBER_EXPECTED,                   "Number expected",                  "A non-numeric character is included in a number string."},
    {AE_NUMBER_OUT_OF_RANGE,               "Number out of range",              "Number does not fit in the range"},
    {AE_MAXIMUM_NUMBERS_REACHED,           "Maximum numbers reached",          "Please contact tom at nobleape dot com"},
    {AE_MAXIMUM_SCRIPT_SIZE_REACHED,       "Maximum script size reached",      "Please contact tom at nobleape dot com"},
    {AE_MAXIMUM_VARIABLES_REACHED,         "Maximum variables reached",        "Please contact tom at nobleape dot com"},
    {AE_UNKNOWN_SYNTAX_PARSER_BUFFER,      "Unknown syntax (parser buffer)",   "Syntax is incorrect"},
    {AE_UNKNOWN_SYNTAX_PARSER_CONVERT,     "Unknown syntax (parser convert)",  "Syntax is incorrect"},

    {AE_SELECTED_ENTITY_OUT_OF_RANGE,      "Selected entity out of range",     "Selected entity is outside the bounds of the number of entities."},
    {AE_COORDINATES_OUT_OF_RANGE,          "Coordinates out of range",         "Coordinates outside the prescribed range."},
    {AE_VALUE_OUT_OF_RANGE,                "Value out of range",               "Value outside the presecribed range."},

    {AE_TOO_MANY_CLOSE_BRACES,             "Too many }",                       "You have closed too many braces. Go back to the code and see if there is an erroneous additional } in the code."},
    {AE_MAXIMUM_BRACES_REACHED,            "Maximum braces reached",           "Please contact tom at nobleape dot com"},
    {AE_FIRST_VALUE_FAILED,                "First value failed",               "Something is wrong with the first value of an equality, if or while operation. It could be the first and only value in this function."},
    {AE_SECOND_VALUE_FAILED,               "Second value failed",              "Something is wrong with the second number/variable value of an equality, if or while operation."},
    {AE_UNKNOWN_SYNTAX_MISSING_EQUALS,     "Unknown syntax (missing =)",       "Syntax is incorrect"},
    {AE_UNKNOWN_SYNTAX_NO_COMMAND,         "Unknown syntax (no command)",      "Syntax is incorrect"},
    {AE_WRONG_END,                         "Wrong end",                        "A bracket or colon was expected but not found."},

    {AE_LINE_START_INCORRECT,              "Line start incorrect",             "A line of code begins incorrectly. It could start with a number or an operator when if/while or a variable was expected."},
    {AE_OUTPUT_SET_AS_INPUT_VARIABLE,      "Output set as input variable",     "An output only variable is attempting to be set."},
    {AE_IF_WHILE_NOT_FOLLOWED_BY_BRACKET,  "if/while not followed by {",       "All if/while statements require a bracket following the if/while (allowing for any amount of whitespace too)."},
    {AE_FUNCTION_ISNT_VARIABLE,            "Function isn't variable",          "Function must not be a special term."},
    {AE_NON_FUNCTION_APPLIED,              "Non-function applied",             "Expecting a function."},
    {AE_FUNCTION_DEFINED_PRIOR,            "Function defined prior",           "Single function definition only."},
    {AE_FUNCTION_OUT_OF_RANGE,             "Function out of range",            "Function defined outside the range of the code presented."},
    {AE_WITHOUT_SEMICOLON,                 "Without ;",                        "Semi-colon required."},
    {AE_WITHOUT_OPEN_BRACE,                "Without {",                        "All if/while statements expect what is executed through the bracket enclosed statement being correct to be followed by inclusive braces { }. There is no single line if or while statements without { } in ApeScript."},

    {AE_FUNCTION_SETTING_FAILED,           "Function setting failed",          "Function could not be set."},
    {AE_ERROR_STARTING_MAIN,               "Error starting main",              "Main could not be started."},
    {AE_CODE_AFTER_MAIN,                   "Code after main",                  "All the code in ApeScript must exist before the end of main."},
    {AE_NO_CLOSE_BRACE_TO_END_OF_FILE,     "No } to end of file",              "Based on the final main function it is expected that the last meaningful character will be }."},
    {AE_CODE_OUTSIDE_FUNCTION,             "Code outside function",            "All code in ApeScript needs to exist within functions."},
    {AE_INPUT_VARIABLE_WITHOUT_EQUALS,     "Input variable without equals",    "All variables set require an equals following the variable."},
    {AE_ASSIGN_VALUE_FAILED,               "Assign value failed",              "Something is wrong with the variable set by an equality."},
    {AE_UNKNOWN_SYNTAX_FROM_INTERPRET,     "Unknown syntax (from interpret)",  "Syntax is incorrect"},
    {AE_NO_MAIN_CODE,                      "No main code",                     "ApeScript requires a main function."},

    {AE_NO_ERROR, 0L, 0L}
};

extern n_int draw_error(n_constant_string error_text, n_constant_string location, n_int line_number);

/* "---1---2---3---4---5---6---7--" */ /* length of the errors */

#define APESCRIPT_ERROR(individual, value) (io_apescript_error(individual, value, __FILE__, __LINE__))

#define SHOW_ERROR_FILE_LINE(val, file, line) (draw_error(val, file, line))

#define	SHOW_ERROR(val)	(draw_error(val, __FILE__, __LINE__))

#define IO_LOWER_CHAR(value)   if(ASCII_UPPERCASE(value)) (value) += 'a' - 'A'

typedef n_int (execute_function)(void * general_data, void * read_data, void * write_data);
typedef void (execute_thread_stub)(execute_function function, void * general_data, void * read_data, void * write_data);

void  execute_group(execute_function * function, void * general_data, void * read_data, n_int count, n_int size);

void area2_add(n_area2 * area, n_vect2 * vect, n_byte first);


n_int vect2_distance_under(n_vect2 * first, n_vect2 * second, n_int distance);

void  vect2_byte2(n_vect2 * converter, n_byte2 * input);
void  vect2_add(n_vect2 * equals, n_vect2 * initial, n_vect2 * second);
void  vect2_center(n_vect2 * center, n_vect2 * initial, n_vect2 * second);

void  vect2_subtract(n_vect2 * equals, n_vect2 * initial, n_vect2 * second);
void  vect2_divide(n_vect2 * equals, n_vect2 * initial, n_vect2 * second, n_int divisor);
void  vect2_multiplier(
    n_vect2 * equals, n_vect2 * initial,
    n_vect2 * second, n_int multiplier, n_int divisor);
void  vect2_d(
    n_vect2 * initial, n_vect2 * second,
    n_int multiplier, n_int divisor);
n_int vect2_dot(
    n_vect2 * initial, n_vect2 * second,
    n_int multiplier, n_int divisor);
void vect2_rotate90(n_vect2 * rotation);
void vect2_direction(n_vect2 * initial, n_int direction, n_int divisor);
void vect2_delta(n_vect2 * initial, n_vect2 * delta);
void vect2_offset(n_vect2 * initial, n_int dx, n_int dy);
void vect2_back_byte2(n_vect2 * converter, n_byte2 * output);
void vect2_copy(n_vect2 * to, n_vect2 * from);
void vect2_populate(n_vect2 * value, n_int x, n_int y);
void vect2_rotation(n_vect2 * location, n_vect2 * rotation);
void vect2_rotation_bitshift(n_vect2 * location, n_vect2 * rotation);

n_int vect2_nonzero(n_vect2 * nonzero);
#ifndef _WIN32
n_vect2 * vect2_min_max_init(void);
#endif
void vect2_min_max(n_vect2 * points, n_int number, n_vect2 * maxmin);

void vect2_scalar_multiply(n_vect2 * value, n_int multiplier);
void vect2_scalar_divide(n_vect2 * value, n_int divisor);
void vect2_scalar_bitshiftdown(n_vect2 * value, n_int bitshiftdown);

void vect3_double(n_vect3 * converter, n_double * input);
void vect3_add(n_vect3 * equals, n_vect3 * initial, n_vect3 * second);
void vect3_center(n_vect3 * center, n_vect3 * initial, n_vect3 * second);
void vect3_subtract(n_vect3 * equals, n_vect3 * initial, n_vect3 * second);
void vect3_divide(n_vect3 * equals, n_vect3 * initial, n_vect3 * second, n_double divisor);
void vect3_multiplier(n_vect3 * equals, n_vect3 * initial, n_vect3 * second,
                      n_double multiplier, n_double divisor);
void vect3_d(n_vect3 * initial, n_vect3 * second, n_double multiplier, n_double divisor);
n_double vect3_dot(n_vect3 * initial, n_vect3 * second, n_double multiplier, n_double divisor);
void vect3_delta(n_vect3 * initial, n_vect3 * delta);
void vect3_offset(n_vect3 * initial, n_double dx, n_double dy, n_double dz);
void vect3_back_double(n_vect3 * converter, n_double * output);
void vect3_copy(n_vect3 * to, n_vect3 * from);
void vect3_populate(n_vect3 * value, n_double x, n_double y, n_double z);
n_int vect3_nonzero(n_vect3 * nonzero);

n_byte * math_general_allocation(n_byte * bc0, n_byte * bc1, n_int i);

void math_general_execution(n_int instruction, n_int is_constant0, n_int is_constant1,
                            n_byte * addr0, n_byte * addr1, n_int value0, n_int * i,
                            n_int is_const0, n_int is_const1,
                            n_byte * pspace,
                            n_byte *bc0, n_byte *bc1,
                            n_int braincode_min_loop);

n_byte4  math_hash_fnv1(n_constant_string key);
n_uint   math_hash(n_byte * values, n_uint length);

void    math_bilinear_8_times(n_byte * side512, n_byte * data, n_byte double_spread);

n_uint  math_root(n_uint squ);
n_byte  math_turn_towards(n_vect2 * p, n_byte fac, n_byte turn);
n_byte2 math_random(n_byte2 * local);
void    math_random3(n_byte2 * local);
n_byte  math_join(n_int sx, n_int sy, n_int dx, n_int dy, n_join * draw);
n_int   math_spread_byte(n_byte val);

n_int  math_sine(n_int direction, n_int divisor);

n_byte math_join_vect2(n_int sx, n_int sy, n_vect2 * vect, n_join * draw);
n_byte math_line_vect(n_vect2 * point1, n_vect2 * point2, n_join * draw);
n_byte math_line(n_int x1, n_int y1, n_int x2, n_int y2, n_join * draw);

n_int  math_seg14(n_int character);

n_byte math_do_intersect(n_vect2 * p1, n_vect2 * q1, n_vect2 * p2, n_vect2 * q2);

void       io_number_to_string(n_string value, n_uint number);
void       io_string_number(n_string output_string, n_string input_string, n_uint number);
void       io_three_strings(n_string output_string, n_string first_string, n_string second_string, n_string third_string, n_byte new_line);

void       io_entry_execution(n_int argc, n_string * argv);
void       io_command_line_execution_set(void);
n_int      io_command_line_execution(void);

void       io_lower(n_string value, n_int length);
void       io_whitespace(n_file * input);
void       io_whitespace_json(n_file * input);
void       io_audit_file(const noble_file_entry * format, n_byte section_to_audit);
void       io_search_file_format(const noble_file_entry * format, n_string compare);
void       io_string_write(n_string dest, n_string insert, n_int * pos);
n_int      io_read_bin(n_file * fil, n_byte * local_byte);
n_int      io_file_write(n_file * fil, n_byte byte);
void       io_file_reused(n_file * fil);
n_int      io_write(n_file * fil, n_constant_string ch, n_byte new_line);
n_int      io_writenumber(n_file * fil, n_int loc_val, n_uint numer, n_uint denom);
n_int      io_length(n_string value, n_int max);
n_int      io_find(n_string check, n_int from, n_int max, n_string value_find, n_int value_find_length);
n_int      io_read_buff(n_file * fil, n_byte * data, const noble_file_entry * commands);
n_int      io_write_buff(n_file * fil, void * data, const noble_file_entry * commands, n_byte command_num, n_file_specific * func);
n_int      io_write_csv(n_file * fil, n_byte * data, const noble_file_entry * commands, n_byte command_num, n_byte initial) ;

void       memory_copy(n_byte * from, n_byte * to, n_uint number);
void *     memory_new(n_uint bytes);
void       memory_free(void ** ptr);
void *     memory_new_range(n_uint memory_min, n_uint *memory_allocated);

n_file *   io_file_new(void);
void       io_file_free(n_file ** file);
void       io_file_debug(n_file * file);

n_int      io_number(n_string number_string, n_int * actual_value, n_int * decimal_divisor);

void       audio_aiff_header(void * fptr, n_uint total_samples);

void       audio_aiff_body(void * fptr, n_audio *samples, n_uint number_samples);

n_int      io_quit(void * ptr, n_string response, n_console_output output_function);
n_int      io_help(void * ptr, n_string response, n_console_output output_function);
n_string   io_console_entry_clean(n_string string, n_int length);
n_string   io_console_entry(n_string string, n_int length);
void       io_console_out(n_constant_string value);
n_int      io_console(void * ptr, noble_console_command * commands, n_console_input input_function, n_console_output output_function);
void       memory_erase(n_byte * buf_offscr, n_uint nestop);
n_int      io_disk_read(n_file * local_file, n_string file_name);
n_int      io_disk_read_no_error(n_file * local_file, n_string file_name);

n_int      io_disk_write(n_file * local_file, n_constant_string file_name);
n_int      io_disk_check(n_constant_string file_name);
n_string * io_tab_delimit_to_n_string_ptr(n_file * tab_file, n_int * size_value, n_int * row_value);

void       io_three_string_combination(n_string output, n_string first, n_string second, n_string third, n_int count);
void       io_time_to_string(n_string value);
n_string   io_string_copy(n_string string);

n_int      io_read_byte4(n_file * fil, n_uint * actual_value, n_byte * final_char);
n_int      io_writenum(n_file * fil, n_int loc_val, n_byte ekind, n_byte new_line);
n_int      io_command(n_file * fil, const noble_file_entry * commands);
n_int      io_read_data(n_file * fil, n_byte2 command, n_byte * data_read);
void       io_help_line(noble_console_command * specific, n_console_output output_function);

void       io_console_quit(void);

void       io_output_contents(n_file * file);

n_uint     io_file_hash(n_file * file);

n_file * io_file_ready(n_int entry, n_file * file);

void io_file_cleanup(n_int * entry, n_file ** file);

void io_file_writeon(n_int * entry, n_file ** file, n_byte blocked_write);

void io_file_writeoff(n_int * entry, n_file * file);

void io_file_string(n_int entry, n_file * file, n_constant_string string);

n_uint io_find_size_data(noble_file_entry * commands);

void compress_buffer(n_byte * input, n_byte * output, n_int n, n_int compressed);
void compress_buffer_run(n_byte * input, n_byte * output, n_int n, n_int compressed, n_int number);

void compress_brain_compressed(n_byte * brain);
void compress_brain_expand(n_byte * brain);

enum color_type
{
    COLOR_BLACK     =   (0),
    COLOR_GREY      =   (252),
    COLOR_YELLOW    =   (253),
    COLOR_RED_DARK  =   (254),
    COLOR_RED       =   (255)
};

#ifdef NOBLE_IOS

#define MAP_BITS                      (8)
#define MAP_TITLES                    (1)

#else

#ifdef NOBLE_PLANET

#define MAP_BITS                      (8)
#define MAP_TITLES                    (6)

#else

#define MAP_BITS                      (9)
#define MAP_TITLES                    (1)

#endif

#endif

#define MAP_DIMENSION                 (1<<(MAP_BITS))
#define MAP_AREA                      (1<<(2*MAP_BITS))

#define APE_TO_MAP_BIT_RATIO          (6)
#define MAP_TO_TERRITORY_RATIO        (5)

#define APESPACE_TO_MAPSPACE(num)     ((num)>>APE_TO_MAP_BIT_RATIO)
#define MAPSPACE_TO_APESPACE(num)     ((num)<<APE_TO_MAP_BIT_RATIO)

#define MAPSPACE_TO_TERRITORY(num)    ((num)>>MAP_TO_TERRITORY_RATIO)

#define TERRITORY_DIMENSION           MAPSPACE_TO_TERRITORY(MAP_DIMENSION)
#define TERRITORY_AREA                (TERRITORY_DIMENSION*TERRITORY_DIMENSION)

/* dimension of the territory map */

#define APESPACE_TO_TERRITORY(num)    (APESPACE_TO_MAPSPACE(num)>>MAP_TO_TERRITORY_RATIO)


#define HI_RES_MAP_BITS               (MAP_BITS+3)

#define HI_RES_MAP_DIMENSION          (1<<(HI_RES_MAP_BITS))
#define HI_RES_MAP_AREA               (1<<(2*HI_RES_MAP_BITS))

#define MAP_APE_RESOLUTION_SIZE       (MAPSPACE_TO_APESPACE(MAP_DIMENSION))
#define	APESPACE_BOUNDS               ((MAP_APE_RESOLUTION_SIZE)-1)

#define	APESPACE_CONFINED(num)        (n_byte2)((num)>APESPACE_BOUNDS ? APESPACE_BOUNDS : ((num)<0 ? 0 : (num)))

#define LAND_TILE_EDGE                 (256)
#define	POSITIVE_LAND_COORD(num)       ((num+(3*MAP_DIMENSION))&(MAP_DIMENSION-1))
#define	POSITIVE_LAND_COORD_HIRES(num) ((num+(3*HI_RES_MAP_DIMENSION))&(HI_RES_MAP_DIMENSION-1))
#define NUMBER_LAND_TILES              (MAP_DIMENSION/LAND_TILE_EDGE)
#define WEATHER_TO_MAPSPACE(num)       ((num))
#define MAPSPACE_TO_WEATHER(num)       ((num))

#define LAND_DITHER(x,y,z)             (((x+y+z)&15)-(((x&y)|z)&7)-((x|(y&z))&7))

#define	OFFSCREENSIZE                  (MAP_AREA + TERRAIN_WINDOW_AREA)

#ifdef NEW_LAND_METHOD
#define WEATHER_CLOUD       (32768>>4)
#else
#define	WEATHER_CLOUD		(32768>>1)
#endif

#define	WEATHER_RAIN		(WEATHER_CLOUD * 3)

#define TIME_HOUR_MINUTES           (60)
#define TIME_DAY_MINUTES            (TIME_HOUR_MINUTES * 24)
#define TIME_MONTH_MINUTES          (TIME_DAY_MINUTES * 28)
#define TIME_YEAR_MINUTES           (TIME_MONTH_MINUTES * 13)
#define TIME_YEAR_DAYS              (7 * 52)				/*364 also = 13 * 28 */
#define	TIME_CENTURY_DAYS           (TIME_YEAR_DAYS * 100)

#define LUNAR_ORBIT_MINS            39312

#define	WATER_TEST(pz,w)            ((pz)<(w))

#define	WATER_MAP		      128

#define TIDE_AMPLITUDE_LUNAR  8
#define TIDE_AMPLITUDE_SOLAR  2

#define TIDE_MAX              (WATER_MAP + TIDE_AMPLITUDE_LUNAR + TIDE_AMPLITUDE_SOLAR)

/* Night/Day definitions */
#define IS_NIGHT(num)		((((num)>>5) <  (11))||(((num)>>5) >  (36)))
#define	IS_DAWNDUSK(num)	((((num)>>5) == (11))||(((num)>>5) == (36)))

#define NIGHT_END_POINT     (256)
#define DAWN_END_POINT      (384)
#define DAY_END_POINT       (1152)
#define DUSK_END_POINT      (1184)

#define MAX_MODIFIED_TIME   (238)

#define NIGHT_TIME_DIVISION(time)     ((time)>>4)
#define DAWN_DUSK_TIME_DIVISION(time) ((time)>>3)
#define DAY_TIME_DIVISION(time)       ((time)>>2)

/*
 Night 1184  - 1439
 Night 0 - 351
 Dawn  352 - 383
 Day   384 - 1151
 Dusk  1152 - 1183
 */

void  weather_init(void);
void  weather_wind_vector(n_vect2 * pos, n_vect2 * wind);
n_int weather_pressure(n_int px, n_int py);
void  weather_cycle(void);
void  weather_wind(void);
weather_values weather_seven_values(n_int px, n_int py);

void land_seed_genetics(n_byte2 * local_random);

void land_init(void);
void land_init_high_def(n_byte double_spread);

void  land_clear(KIND_OF_USE kind, n_byte4 start);
void  land_cycle(void);
void  land_vect2(n_vect2 * output, n_int * actual_z, n_vect2 * location);
n_int land_operator_interpolated(n_int locx, n_int locy, n_byte * kind);

n_int land_map_dimension(void);
n_int land_map_bits(void);

void  land_tide(void);
n_int land_location(n_int px, n_int py);
n_byte * land_location_tile(n_int tile);
n_int land_location_vect(n_vect2 * value);

void graph_init(n_int four_byte_factory);

void graph_erase(n_byte * buffer, n_vect2 * img, n_rgba32 * color);

/* draws a line */
void graph_line(n_byte * buffer,
                n_vect2 * img,
                n_vect2 * previous,
                n_vect2 * current,
                n_rgba32 * color,
                n_byte thickness);

void graph_curve(n_byte * buffer,
                 n_vect2 * img,
                 n_vect2 * pt0,
                 n_vect2 * pt1,
                 n_vect2 * pt2,
                 n_rgba32 * color,
                 n_byte radius_percent,
                 n_uint start_thickness,
                 n_uint end_thickness);

void graph_fill_polygon(n_vect2 * points, n_int no_of_points,
                        n_rgba32 * color, n_byte transparency,
                        n_byte * buffer, n_vect2 * img);


/*0*/

enum PRIMARY_APESCRIPT
{
    VARIABLE_FUNCTION = 0,
    VARIABLE_RUN,
    VARIABLE_WHILE,
    VARIABLE_IF
};

enum SYNTAX_APESCRIPT
{
    SYNTAX_MINUS = 0,
    SYNTAX_ADDITION,
    SYNTAX_MULTIPLY,
    SYNTAX_AND,
    SYNTAX_XOR,
    SYNTAX_OR,

    SYNTAX_GREATER_THAN,
    SYNTAX_LESS_THAN,
    SYNTAX_EQUAL_TO,
    SYNTAX_NOT_EQUAL_TO,
    SYNTAX_CONDITIONAL_AND,
    SYNTAX_CONDITIONAL_OR,
    SYNTAX_DIVISION,

    SYNTAX_MODULUS,

    SYNTAX_BITSHIFT_RIGHT,
    SYNTAX_BITSHIFT_LEFT,

    SYNTAX_LESS_EQUAL,
    SYNTAX_GREATER_EQUAL,

    SYNTAX_EQUALS
};

enum SYNTAX_ADDITIONAL_BRAINCODE
{
    SYNTAX_MOVE = SYNTAX_EQUALS + 1,
    SYNTAX_JUMP_TO,
    SYNTAX_JUMP_EQUAL_ZERO,
    SYNTAX_DATA
};

enum APESCRIPT_INTERPRET_TYPES
{
    APESCRIPT_OPEN_BRACKET =  ('('),
    APESCRIPT_CLOSE_BRACKET = (')'),
    APESCRIPT_OPEN_BRACE =    ('{'),
    APESCRIPT_CLOSE_BRACE =   ('}'),
    APESCRIPT_OPERATOR =      ('='),
    APESCRIPT_NUMBER =        ('n'),
    APESCRIPT_TEXT =          ('t'),
    APESCRIPT_SEMICOLON =     (';'),
    APESCRIPT_FAILURE =       ('F'),
    APESCRIPT_FUNCTION =      ('f'),
    APESCRIPT_RUN =           ('r'),
    APESCRIPT_STRING =        ('"')
};

#define ASCII_QUOTE(num)      ((num) == '"')

#define ASCII_TEXT(num)		  ((ASCII_UPPERCASE(num) || ASCII_LOWERCASE(num)) || ((num) == '_'))

#define ASCII_SEMICOLON(num)  ((num) == ';')
#define ASCII_COLON(num)      ((num) == ':')

#define ASCII_COMMA(num)      ((num) == ',')

#define	ASCII_EQUAL(num)	  ((num) == '=')

#define	ASCII_BRACKET(num)    (((num) == '(')||((num) == ')'))
#define	ASCII_BRACES(num)     (((num) == '{')||((num) == '}'))

#define ASCII_LOGICAL(num)	  ((((num) == '&')||((num) == '|'))||(((num) == '^')||((num) == '!')))
#define ASCII_ARITHMETIC(num) ((((num) == '+')||((num) == '-'))||(((num) == '*')||((num) == '/')))

#define ASCII_DIRECTIONAL(num) (((num)=='<')||((num)=='>'))

#define	CODE_VALUE_REQUIRED(num) (((num) == APESCRIPT_OPERATOR || (num) == APESCRIPT_NUMBER) || ((num) == APESCRIPT_TEXT))

#define	SIZEOF_NUMBER_WRITE      (sizeof(n_int))

void io_int_to_bytes(n_int value, n_byte * bytes);
n_int io_bytes_to_int(n_byte * bytes);

#define	VARIABLE_INPUT(num,code)		((num)>((code)->input_greater))
#define	VARIABLE_SPECIAL(num,code)	    ((num)<((code)->special_less))

#ifndef ABS
#define ABS(a)	                        (((a) < 0) ? -(a) : (a))
#endif

#define NUMBER_MAX 				256
#define	VARIABLE_MAX			256

#define	BRACES_MAX			    16
#define SIZE_OF_EVALUATE	    (SIZEOF_NUMBER_WRITE+SIZEOF_NUMBER_WRITE+1)  /* (tA=XtB) */

#define CYCLE_COUNT_RESET	    4096

#define	MAIN_NOT_RUN		    0
#define	MAIN_RUN			    1

/*! @typedef
@field code The pointer to the n_interpret struct.
@field kind The variable in question - this could be thought of as an index to a
variable array.
@field value The value to set the variable in question.
@return Any error that is reported.
@discussion The two primay interfaces in ApeScript relate to the getting and
setting of information. This function covers the setting of information.
*/
typedef n_int (script_input )(void * individual, n_byte kind, n_int value);

/*! @typedef
@field code The pointer to the n_interpret struct.
@field kind The pointer has two values either a variable or a number.
@field value The resultant value.
@return Any error that is reported.
@discussion The two primay interfaces in ApeScript relate to the getting and
setting of information. This function covers the getting of information.
*/
typedef n_int (script_output)(void * code, void * individual, n_byte * kind, n_int * number);

typedef void (script_external)(void * individual, void * structure, void * data);

#define	VARIABLE_WIDTH		    32

typedef	n_byte				    variable_string[VARIABLE_WIDTH];

/*! @struct
@field evaluate The length of the evaluated string.
@field braces_start The location where the braces start.
@discussion This structure is used for the evaluation of if/then or while
checks in ApeScript and it shows where the braces code should return to in
the case of a while loop.
*/
typedef struct
{
    n_byte			 evaluate[SIZE_OF_EVALUATE];
    n_int			 braces_start;
} n_brace;


typedef struct
{
    n_file			*binary_code;
    n_int			 number_buffer[NUMBER_MAX]; /* per entry */
    variable_string *variable_strings;
    n_int		     main_entry;
    n_int		     input_greater;
    n_int		     special_less;
    script_input    *sc_input;
    script_output   *sc_output;
} n_interpret;

typedef struct
{
    n_int           interpret_location; /* per entry */
    n_int           leave;              /* per entry */
    n_int           localized_leave;    /* per entry */
    void *          interpret_data;     /* per entry */
    n_int		    variable_references[VARIABLE_MAX]; /* per entry */
    n_int	  	    braces_count;        /* per entry */
    n_brace		    braces[BRACES_MAX];  /* per entry */
    n_byte		    main_status;         /* per entry */
} n_individual_interpret;


/* used for stripping ApeScript errors for documentation */

n_int io_apescript_error(n_individual_interpret * individual, AE_ENUM value, n_constant_string location, n_int line_number);

n_interpret *	parse_convert(n_file * input, n_int main_entry, variable_string * variables);

void interpret_individual(n_individual_interpret * individual);

void  interpret_cleanup(n_interpret ** to_clean);
n_int interpret_cycle(n_interpret * code, n_individual_interpret * individual, n_int exit_offset,
                      void * structure, void * data,
                      script_external * start, script_external * end);

typedef struct
{
    n_byte2     genetics[2];                           /* save-able */
    
    n_byte      topography[2][MAP_AREA];                    /* generated */
    n_c_int     atmosphere[2][ MAP_AREA];                 /* save-able and generate-able */
    n_byte2     delta_pressure[ MAP_AREA];             /* generated */
    
    n_byte2     delta_pressure_highest;
    n_byte2     delta_pressure_lowest;
    
    n_c_int     atmosphere_highest;
    n_c_int     atmosphere_lowest;
    n_int       local_delta;
} n_tile;

typedef struct
{
#ifdef NOBLE_PLANET
    n_tile tiles[6];
#else
    n_tile tiles[1];
#endif
    n_byte2     genetics[2];                           /* save-able */

    n_int      wind_value_x; /* 6 to 96 */
    n_int      wind_value_y; /* 6 to 96 */
    n_int      wind_aim_x;  /* 6 to 96 */
    n_int      wind_aim_y;  /* 6 to 96 */
    
    n_int      wind_dissipation;
} n_land;

typedef struct
{
    n_int  x, y;
    n_int  tile;
    n_uint facing;
} n_tile_coordinates;

void tile_wind(n_land * land);
void tile_cycle(n_land * land);

void tile_weather_init(n_land * land);

void tile_land_init(n_land * land);
void tile_land_erase(n_land * land);
void tile_land_random(n_land * land, n_byte2 * random);


void tile_creation(n_byte * map, n_byte2 * random);

n_byte tiles_topography(n_land * land, n_int tile, n_int buffer, n_int lx, n_int ly);
n_byte * tiles_topography_map(n_land * land, n_int tile, n_int buffer);

n_c_int tiles_atmosphere(n_land * land, n_int tile, n_int buffer, n_int lx, n_int ly);
void tile_resolve_coordinates(n_tile_coordinates * coordinates);

void land_color_init(void);
void land_color_time(n_byte2 * color_fit, n_int toggle_tidedaylight);

void * land_ptr(void);
n_byte4 land_date(void);
n_byte4 land_time(void);
n_byte2 * land_genetics(void);
n_byte land_tide_level(void);
n_byte * land_topography(void);
n_byte * land_topography_highdef(void);
n_byte4 * land_highres_tide(void);
n_c_int * land_weather(n_int tile);

#define BASH_COLOR_DEFAULT "\033[0m"

#define BASH_COLOR_LIGHT_GREEN "\033[92m"
#define BASH_COLOR_LIGHT_YELLOW "\033[93m"
#define BASH_COLOR_LIGHT_RED "\033[91m"

#define BASH_COLOR_LIGHT_GREY "\033[37m"
#define BASH_COLOR_DARK_GREY "\033[90m"

#ifdef	SCRIPT_DEBUG

n_file * scdebug_file_ready(void);
void     scdebug_file_cleanup(void);

void     scdebug_string(void * ptr, n_constant_string string);
void     scdebug_int(void * ptr, n_int number);
void     scdebug_newline(void * ptr);
void     scdebug_tabstep(void * ptr, n_int steps);
n_string scdebug_variable(n_int variable);

void     scdebug_writeon(void * ptr);
void     scdebug_writeoff(void * ptr);

n_int spacetime_after(n_spacetime * initial, n_spacetime * second);
void  spacetime_copy(n_spacetime * to, n_spacetime * from);
n_int spacetime_before_now(n_spacetime * initial);
void  spacetime_set(n_spacetime * set, n_byte2 * location);

void land_convert_to_map(n_vect2 * value);

#define	SC_DEBUG_STRING(ptr, string) scdebug_string(ptr, string)
#define SC_DEBUG_NUMBER(ptr, number) scdebug_int(ptr, number)
#define	SC_DEBUG_NEWLINE(ptr)		 scdebug_newline(ptr)
#define	SC_DEBUG_UP(ptr)			 scdebug_tabstep(ptr,1)
#define	SC_DEBUG_DOWN(ptr)			 scdebug_tabstep(ptr,-1)
#define	SC_DEBUG_ON(ptr)             scdebug_writeon(ptr)
#define SC_DEBUG_OFF(ptr)			 scdebug_writeoff(ptr)

#else

#define	SC_DEBUG_STRING(string)	/* string */
#define SC_DEBUG_NUMBER(number) /* number */
#define	SC_DEBUG_NEWLINE		/* */
#define	SC_DEBUG_UP				/* */
#define	SC_DEBUG_DOWN			/* */
#define	SC_DEBUG_ON(ptr)		/* ptr */
#define SC_DEBUG_OFF			/* */

#endif

#endif /* _NOBLEAPE_NOBLE_H_ */

