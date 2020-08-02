#ifdef __cplusplus
extern "C"
{
#endif

typedef struct text_control text_control;

/* forwarding functions */

/*
initialize_state:
//          set the textedit state to a known good default state when initially
//          constructing the textedit.
*/
text_control* textedit_initialise();

/*
//      click:
//          call this with the mouse x,y on a mouse down; it will update the cursor
//          and reset the selection start/end to the cursor point. the x,y must
//          be relative to the text widget, with (0,0) being the top left.
*/
void textedit_click(text_control *str, float x, float y);

/*     
//      drag:
//          call this with the mouse x,y on a mouse drag/up; it will update the
//          cursor and the selection end point
*/

void textedit_drag(text_control *str, float x, float y);

/* 
//    cut:
//          call this to delete the current selection; returns true if there was
//          one. you should FIRST copy the current selection to the system paste buffer.
//          (To copy, just copy the current selection out of the string yourself.)
*/

int  textedit_cut(text_control *str);

/*
//      paste:
//          call this to paste text at the current cursor point or over the current
//          selection if there is one.
*/
 int  textedit_paste(text_control *str, char *text, int len);

/*
//      key:
//          call this for keyboard inputs sent to the textfield. you can use it
//          for "key down" events or for "translated" key events. if you need to
*/
void textedit_key(text_control *str, unsigned key);

#ifdef __cplusplus
}
#endif
