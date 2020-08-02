

#include <stdlib.h>
#include <string.h> // memmove
#include <ctype.h>	// isspace
#include <nanovg.h>
#include <SDL.h>

#define STB_TEXTEDIT_CHARTYPE char
#define STB_TEXTEDIT_STRING text_control

// the length of the string (ideally O(1))
#define STB_TEXTEDIT_STRINGLEN(tc) ((tc)->stringlen)

// returns the results of laying out a line of characters
#define STB_TEXTEDIT_LAYOUTROW layout_func

//  returns the pixel delta from the xpos of the i'th character to the xpos of the i+1'th char for a line of characters
#define STB_TEXTEDIT_GETWIDTH(tc, n, i) (1) // quick hack for monospaced

// returns the i'th character of obj, 0-based
#define STB_TEXTEDIT_GETCHAR(tc, i) ((tc)->string[i])

// maps a keyboard input to an insertable character
#define STB_TEXTEDIT_KEYTOTEXT(key) (((key)&KEYDOWN_BIT) ? 0 : (key))

// the character returned by _GETCHAR() we recognize
//  as manually wordwrapping for end-of-line positioning
#define STB_TEXTEDIT_NEWLINE '\n'

// true if character is whitespace (e.g. 'isspace')s
#define STB_TEXTEDIT_IS_SPACE(ch) isspace(ch)

// delete n characters starting at i
#define STB_TEXTEDIT_DELETECHARS delete_chars

// insert n characters at i (pointed to by STB_TEXTEDIT_CHARTYPE*)
#define STB_TEXTEDIT_INSERTCHARS insert_chars

#include "stb_textedit.h"

#define STB_TEXTEDIT_IMPLEMENTATION

// define our editor structure
typedef struct
{
	char *string;
	int stringlen;
	STB_TexteditState state;
} text_control;

// define the functions we need
void layout_func(StbTexteditRow *row, STB_TEXTEDIT_STRING *str, int start_i)
{
	// this lives in the C++ side of our code in canvas.cpp 
	extern NVGcontext* get_canvas_context();

	NVGcontext* vg = get_canvas_context();
	// are you trying to use this without a canvas?
	SDL_assert(vg != NULL);
	float ascender, descender, lineh;
	nvgTextMetrics(vg, &ascender, &descender, &lineh);
	float bounds[4];
	char *linestart = str->string;
	char *c;
	int linecount = 0;
	for(c= str->string; c - str->string < start_i;  ++c)
	{
		if (*c == '\n') {
			linecount++;
			linestart = c+1;
		}
	}
	char *lineend = str->string + str->stringlen;
	for(; c - str->string < str->stringlen; ++c)
	{
		if (*c == '\n') 
		{
			lineend = c;
			break;
		}
	}

	char *textstart = str->string + start_i;
	nvgTextBounds(vg, 0.0f, linecount * lineh, linestart, textstart, bounds);
	float xstart = bounds[2];
	nvgTextBounds(vg, xstart, linecount * lineh, textstart, lineend, bounds);
	row->num_chars = lineend - textstart; 
	row->x0 = bounds[0];
	row->x1 = bounds[2]; 
	row->baseline_y_delta = 1.25;
	row->ymin = bounds[1];
	row->ymax = bounds[3];
	return;
}

int delete_chars(STB_TEXTEDIT_STRING *str, int pos, int num)
{
	memmove(&str->string[pos], &str->string[pos + num], str->stringlen - (pos + num));
	str->stringlen -= num;
	return 1; // always succeeds
}

int insert_chars(STB_TEXTEDIT_STRING *str, int pos, STB_TEXTEDIT_CHARTYPE *newtext, int num)
{
	str->string = realloc(str->string, str->stringlen + num);
	memmove(&str->string[pos + num], &str->string[pos], str->stringlen - pos);
	memcpy(&str->string[pos], newtext, num);
	str->stringlen += num;
	return 1; // always succeeds
}

// define all the #defines needed
#define STB_TEXTEDIT_KEYTYPE unsigned

#define KEYDOWN_BIT 0x80000000

#define STB_TEXTEDIT_K_SHIFT 0x40000000
#define STB_TEXTEDIT_K_CONTROL 0x20000000
#define STB_TEXTEDIT_K_LEFT (KEYDOWN_BIT | 1)	   // actually use VK_LEFT, SDLK_LEFT, etc
#define STB_TEXTEDIT_K_RIGHT (KEYDOWN_BIT | 2)	   // VK_RIGHT
#define STB_TEXTEDIT_K_UP (KEYDOWN_BIT | 3)		   // VK_UP
#define STB_TEXTEDIT_K_DOWN (KEYDOWN_BIT | 4)	   // VK_DOWN
#define STB_TEXTEDIT_K_LINESTART (KEYDOWN_BIT | 5) // VK_HOME
#define STB_TEXTEDIT_K_LINEEND (KEYDOWN_BIT | 6)   // VK_END
#define STB_TEXTEDIT_K_TEXTSTART (STB_TEXTEDIT_K_LINESTART | STB_TEXTEDIT_K_CONTROL)
#define STB_TEXTEDIT_K_TEXTEND (STB_TEXTEDIT_K_LINEEND | STB_TEXTEDIT_K_CONTROL)
#define STB_TEXTEDIT_K_DELETE (KEYDOWN_BIT | 7)	   // VK_DELETE
#define STB_TEXTEDIT_K_BACKSPACE (KEYDOWN_BIT | 8) // VK_BACKSPACE
#define STB_TEXTEDIT_K_UNDO (KEYDOWN_BIT | STB_TEXTEDIT_K_CONTROL | 'z')
#define STB_TEXTEDIT_K_REDO (KEYDOWN_BIT | STB_TEXTEDIT_K_CONTROL | 'y')
#define STB_TEXTEDIT_K_INSERT (KEYDOWN_BIT | 9) // VK_INSERT
#define STB_TEXTEDIT_K_WORDLEFT (STB_TEXTEDIT_K_LEFT | STB_TEXTEDIT_K_CONTROL)
#define STB_TEXTEDIT_K_WORDRIGHT (STB_TEXTEDIT_K_RIGHT | STB_TEXTEDIT_K_CONTROL)
#define STB_TEXTEDIT_K_PGUP (KEYDOWN_BIT | 10)	 // VK_PGUP -- not implemented
#define STB_TEXTEDIT_K_PGDOWN (KEYDOWN_BIT | 11) // VK_PGDOWN -- not implemented

#include "stb_textedit.h"

/* forwarding functions */

/*
initialize_state:
//          set the textedit state to a known good default state when initially
//          constructing the textedit.
*/
text_control* textedit_initialise()
{
	text_control* result = SDL_malloc(sizeof(text_control));
	stb_textedit_initialize_state(&result->state, 0);
	return result;
}

/*
//      click:
//          call this with the mouse x,y on a mouse down; it will update the cursor
//          and reset the selection start/end to the cursor point. the x,y must
//          be relative to the text widget, with (0,0) being the top left.
*/
void textedit_click(text_control *str, float x, float y)
{
	stb_textedit_click(str, &str->state, x, y);
}

/*     
//      drag:
//          call this with the mouse x,y on a mouse drag/up; it will update the
//          cursor and the selection end point
*/

void textedit_drag(text_control *str, float x, float y)
{
	stb_textedit_drag(str, &str->state, x, y);
}

/* 
//    cut:
//          call this to delete the current selection; returns true if there was
//          one. you should FIRST copy the current selection to the system paste buffer.
//          (To copy, just copy the current selection out of the string yourself.)
*/

int  textedit_cut(text_control *str)
{
	return stb_textedit_cut(str, &str->state);
}

/*
//      paste:
//          call this to paste text at the current cursor point or over the current
//          selection if there is one.
*/
 int  textedit_paste(text_control *str, char *text, int len)
{
  return stb_textedit_paste(str, &str->state, text, len);
}

/*
//      key:
//          call this for keyboard inputs sent to the textfield. you can use it
//          for "key down" events or for "translated" key events. if you need to
*/
void textedit_key(text_control *str, unsigned key)
{
	stb_textedit_key(str, &str->state, key);
}