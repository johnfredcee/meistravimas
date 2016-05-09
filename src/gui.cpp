
#include <vector>
#include <unordered_map>
#include <memory>
#include <list>
#include <array>
#include <functional>

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_net.h>
#include <SDL_opengl.h>
#include <scheme-private.h>
#include <physfs.h>
#include <res_path.h>
#include <scheme-defs.h>
#include <scheme-private.h>
#include <scheme.h>

#include "tuple.h"
#include "twodee.h"
#include "threedee.h"
#include "quat.h"
#include "colour.h"
#include "matrix44.h"
#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "observer.h"
#include "image.h"
#include "mouse.h"
#include "keyboard.h"
#define BLENDISH_IMPLEMENTATION
#define OUI_IMPLEMENTATION
#include "nanovg.h"
#include "nanovg_gl.h"
#include "blendish.h"
#include "oui.h"
#include "scripting.h"
#include "tinyscm_if.h"
#include "gui.h"


extern "C"
{
	
typedef enum {
    // label
    ST_LABEL = 0,
    // button
    ST_BUTTON = 1,
    // radio button
    ST_RADIO = 2,
    // progress slider 
    ST_SLIDER = 3,
    // column
    ST_COLUMN = 4,
    // row
    ST_ROW = 5,
    // check button
    ST_CHECK = 6,
    // panel
    ST_PANEL = 7,
    // text
    ST_TEXT = 8,
    //
    ST_IGNORE = 9,

    ST_DEMOSTUFF = 10,
    // colored rectangle
    ST_RECT = 11,

    ST_HBOX = 12,
    ST_VBOX = 13,
} SubType;

/**
 * Data to associate with oui ui elements
 */
typedef struct {
    int subtype;
    UIhandler handler;
} UIData;

typedef struct {
    UIData head;
    const char *label;
    NVGcolor color;
} UIRectData;

typedef struct {
    UIData head;
    int iconid;
    const char *label;
} UIButtonData;

typedef struct {
    UIData head;
    const char *label;
    int *option;
} UICheckData;

typedef struct {
    UIData head;
    int iconid;
    const char *label;
    int *value;
} UIRadioData;

typedef struct {
    UIData head;
    const char *label;
    float *progress;
} UISliderData;

typedef struct {
    UIData head;
    char *text;
    int maxsize;
} UITextData;

// UI DRAWING --------------------

/** backround test rectangle */
void testrect(NVGcontext *vg, UIrect rect) {
#if 1    
    nvgBeginPath(vg);
    nvgRect(vg,rect.x+0.5,rect.y+0.5,rect.w-1,rect.h-1);
    nvgStrokeColor(vg,nvgRGBf(1,0,0));
    nvgStrokeWidth(vg,1);
    nvgStroke(vg);
#else
	(void) vg;
	(void) rect;
#endif    
}

void drawUI(NVGcontext *vg, int item, int corners);

void drawUIItems(NVGcontext *vg, int item, int corners) {
    int kid = uiFirstChild(item);
    while (kid > 0) {
        drawUI(vg, kid, corners);
        kid = uiNextSibling(kid);
    }
}

void drawUIItemsHbox(NVGcontext *vg, int item) {
    int kid = uiFirstChild(item);
    if (kid < 0) return;
    int nextkid = uiNextSibling(kid);
    if (nextkid < 0) {
        drawUI(vg, kid, BND_CORNER_NONE);
    } else {
        drawUI(vg, kid, BND_CORNER_RIGHT);
        kid = nextkid;
        while (uiNextSibling(kid) > 0) {
            drawUI(vg, kid, BND_CORNER_ALL);
            kid = uiNextSibling(kid);
        }
        drawUI(vg, kid, BND_CORNER_LEFT);
    }
}

void drawUIItemsVbox(NVGcontext *vg, int item) {
    int kid = uiFirstChild(item);
    if (kid < 0) return;
    int nextkid = uiNextSibling(kid);
    if (nextkid < 0) {
        drawUI(vg, kid, BND_CORNER_NONE);
    } else {
        drawUI(vg, kid, BND_CORNER_DOWN);
        kid = nextkid;
        while (uiNextSibling(kid) > 0) {
            drawUI(vg, kid, BND_CORNER_ALL);
            kid = uiNextSibling(kid);
        }
        drawUI(vg, kid, BND_CORNER_TOP);
    }
}

void drawUI(NVGcontext *vg, int item, int corners) {
    const UIData *head = (const UIData *)uiGetHandle(item);
    UIrect rect = uiGetRect(item);
    if (uiGetState(item) == UI_FROZEN) {
        nvgGlobalAlpha(vg, BND_DISABLED_ALPHA);
    }
    if (head) {
        switch(head->subtype) {
            default: {
                testrect(vg,rect);
                drawUIItems(vg,item,corners);
            } break;
            case ST_HBOX: {
                drawUIItemsHbox(vg, item);
            } break;
            case ST_VBOX: {
                drawUIItemsVbox(vg, item);
            } break;
            case ST_PANEL: {
                bndBevel(vg,rect.x,rect.y,rect.w,rect.h);
                drawUIItems(vg,item,corners);
            } break;
            case ST_LABEL: {
                SDL_assert(head);
                const UIButtonData *data = (UIButtonData*)head;
                bndLabel(vg,rect.x,rect.y,rect.w,rect.h,
                    data->iconid,data->label);
            } break;
            case ST_BUTTON: {
                const UIButtonData *data = (UIButtonData*)head;
                bndToolButton(vg,rect.x,rect.y,rect.w,rect.h,
                        corners,(BNDwidgetState)uiGetState(item),
                    data->iconid,data->label);
            } break;
            case ST_CHECK: {
                const UICheckData *data = (UICheckData*)head;
                BNDwidgetState state = (BNDwidgetState)uiGetState(item);
                if (*data->option)
                    state = BND_ACTIVE;
                bndOptionButton(vg,rect.x,rect.y,rect.w,rect.h, state,
                    data->label);
            } break;
            case ST_RADIO:{
                const UIRadioData *data = (UIRadioData*)head;
                BNDwidgetState state = (BNDwidgetState)uiGetState(item);
                if (*data->value == item)
                    state = BND_ACTIVE;
                bndRadioButton(vg,rect.x,rect.y,rect.w,rect.h,
                        corners,state,
                    data->iconid,data->label);
            } break;
            case ST_SLIDER:{
                const UISliderData *data = (UISliderData*)head;
                BNDwidgetState state = (BNDwidgetState)uiGetState(item);
                static char value[32];
                sprintf(value,"%.0f%%",(*data->progress)*100.0f);
                bndSlider(vg,rect.x,rect.y,rect.w,rect.h,
                        corners,state,
                    *data->progress,data->label,value);
            } break;
            case ST_TEXT: {
                const UITextData *data = (UITextData*)head;
                BNDwidgetState state = (BNDwidgetState)uiGetState(item);
                int idx = strlen(data->text);
                bndTextField(vg,rect.x,rect.y,rect.w,rect.h,
                        corners,state, -1, data->text, idx, idx);
            } break;
            case ST_RECT: {
                const UIRectData *data = (UIRectData*)head;
                if (rect.w && rect.h) {
                    BNDwidgetState state = (BNDwidgetState)uiGetState(item);
                    nvgSave(vg);
                    nvgStrokeColor(vg, nvgRGBAf(data->color.r,data->color.g,data->color.b,0.9f));
                    if (state != BND_DEFAULT) {
                        nvgFillColor(vg, nvgRGBAf(data->color.r,data->color.g,data->color.b,0.5f));
                    } else {
                        nvgFillColor(vg, nvgRGBAf(data->color.r,data->color.g,data->color.b,0.1f));
                    }
                    nvgStrokeWidth(vg,2);
                    nvgBeginPath(vg);
                    #if 0
                    nvgRect(vg,rect.x,rect.y,rect.w,rect.h);
                    #else
                    nvgRoundedRect(vg,rect.x,rect.y,rect.w,rect.h,3);
                    #endif
                    nvgFill(vg);
                    nvgStroke(vg);

                    if (state != BND_DEFAULT) {
                        nvgFillColor(vg, nvgRGBAf(0.0f,0.0f,0.0f,1.0f));
                        nvgFontSize(vg, 15.0f);
                        nvgBeginPath(vg);
                        nvgTextAlign(vg, NVG_ALIGN_TOP|NVG_ALIGN_CENTER);
                        nvgTextBox(vg, rect.x, rect.y+rect.h*0.3f, rect.w, data->label, NULL);
                    }

                    nvgRestore(vg);
                }
                nvgSave(vg);
                nvgIntersectScissor(vg, rect.x, rect.y, rect.w, rect.h);

                drawUIItems(vg,item,corners);

                nvgRestore(vg);
            } break;
        }
    } else {
        testrect(vg,rect);
        drawUIItems(vg,item,corners);
    }

    if (uiGetState(item) == UI_FROZEN) {
        nvgGlobalAlpha(vg, 1.0);
    }
}

int label(int iconid, const char *label) {
    int item = uiItem();
    uiSetSize(item, 0, BND_WIDGET_HEIGHT);
    UIButtonData *data = (UIButtonData *)uiAllocHandle(item, sizeof(UIButtonData));
    data->head.subtype = ST_LABEL;
    data->head.handler = NULL;
    data->iconid = iconid;
    data->label = label;
    return item;
}

pointer scheme_label(scheme *sc, pointer args)
{

	hold_global_lock();
	int iconid = ivalue(pair_car(args));
	args = pair_cdr(args);
	char *label_text = string_value(pair_car(args));
	int result = label(iconid, label_text);
	release_global_lock();
	return mk_integer(sc, result);
}
	
void buttonhandler(int item, UIevent event) {
	(void) event;
    const UIButtonData *data = (const UIButtonData *)uiGetHandle(item);
    printf("clicked: %p %s\n", uiGetHandle(item), data->label);	
}

int button(int iconid, const char *label) {
    // create new ui item
    int item = uiItem(); 
    // set size of wiget; horizontal size is dynamic, vertical is fixed
    uiSetSize(item, 0, BND_WIDGET_HEIGHT);
    uiSetEvents(item, UI_BUTTON0_HOT_UP);
    // store some custom data with the button that we use for styling
    UIButtonData *data = (UIButtonData *)uiAllocHandle(item, sizeof(UIButtonData));
    data->head.subtype = ST_BUTTON;
    data->head.handler = buttonhandler;
    data->iconid = iconid;
    data->label = label;
    return item;
}

pointer scheme_button(scheme *sc, pointer args) {
	
	hold_global_lock();
	int iconid = ivalue(pair_car(args));
	args = pair_cdr(args);
	char *label_text = string_value(pair_car(args));
	pointer result = mk_integer(sc, button(iconid, label_text));
	release_global_lock();
	return result;
}
	
void checkhandler(int item, UIevent event) {
	(void) event;
    const UICheckData *data = (const UICheckData *)uiGetHandle(item);
    *data->option = !(*data->option);
}

int check(const char *label, int *option) {
    // create new ui item
    int item = uiItem(); 
    // set size of wiget; horizontal size is dynamic, vertical is fixed
    uiSetSize(item, 0, BND_WIDGET_HEIGHT);
    // attach event handler e.g. demohandler above
    uiSetEvents(item, UI_BUTTON0_DOWN);
    // store some custom data with the button that we use for styling
    UICheckData *data = (UICheckData *)uiAllocHandle(item, sizeof(UICheckData));
    data->head.subtype = ST_CHECK;
    data->head.handler = checkhandler;
    data->label = label;
    data->option = option;
    return item;
}

pointer scheme_check(scheme *sc, pointer args) {
	hold_global_lock();
	
	char *label_text = string_value(pair_car(args));
	args = pair_cdr(args);
	int option = ivalue(pair_car(args));	
	pointer result = mk_integer(sc,check(label_text,&option));
	release_global_lock();
	return result;
}
	
// simple logic for a slider

// starting offset of the currently active slider
static float sliderstart = 0.0;

// event handler for slider (same handler for all sliders)
void sliderhandler(int item, UIevent event) {
    // retrieve the custom data we saved with the slider
    UISliderData *data = (UISliderData *)uiGetHandle(item);
    switch(event) {
        default: break;
        case UI_BUTTON0_DOWN: {
            // button was pressed for the first time; capture initial
            // slider value.
            sliderstart = *data->progress;
        } break;
        case UI_BUTTON0_CAPTURE: {
            // called for every frame that the button is pressed.
            // get the delta between the click point and the current
            // mouse position
            UIvec2 pos = uiGetCursorStartDelta();
            // get the items layouted rectangle
            UIrect rc = uiGetRect(item);
            // calculate our new offset and clamp
            float value = sliderstart + ((float)pos.x / (float)rc.w);
            value = (value<0)?0:(value>1)?1:value;
            // assign the new value
            *data->progress = value;
        } break;
    }
}

void textboxhandler(int item, UIevent event) {
    UITextData *data = (UITextData *)uiGetHandle(item);
    switch(event) {
        default: break;
        case UI_BUTTON0_DOWN: {
            uiFocus(item);
        } break;
        case UI_KEY_DOWN: {
            unsigned int key = uiGetKey();
            switch(key) {
                default: break;
                case SDL_SCANCODE_BACKSPACE: {
                    int size = strlen(data->text);
                    if (!size) return;
                    data->text[size-1] = 0;
                } break;
				case SDL_SCANCODE_KP_ENTER:
                case SDL_SCANCODE_RETURN: {
                    uiFocus(-1);
                } break;
            }
        } break;
        case UI_CHAR: {
            unsigned int key = uiGetKey();
            if ((key > 255)||(key < 32)) return;
            int size = strlen(data->text);
            if (size >= (data->maxsize-1)) return;
            data->text[size] = (char)key;
        } break;
    }
}

int textbox(char *text, int maxsize) {
    int item = uiItem();
    uiSetSize(item, 0, BND_WIDGET_HEIGHT);
    uiSetEvents(item, UI_BUTTON0_DOWN | UI_KEY_DOWN | UI_CHAR);
    // store some custom data with the button that we use for styling
    // and logic, e.g. the pointer to the data we want to alter.
    UITextData *data = (UITextData *)uiAllocHandle(item, sizeof(UITextData));
    data->head.subtype = ST_TEXT;
    data->head.handler = textboxhandler;
    data->text = text;
    data->maxsize = maxsize;
    return item;
}

pointer scheme_textbox(scheme *sc, pointer args) {
	hold_global_lock();
	char *label_text = string_value(pair_car(args));
	args = pair_cdr(args);
	int maxsize = ivalue(pair_car(args));	
	pointer result = mk_integer(sc,textbox(label_text,maxsize));
	release_global_lock();
	return result;
}
	
// simple logic for a radio button
void radiohandler(int item, UIevent event) {
	(void) event;
    UIRadioData *data = (UIRadioData *)uiGetHandle(item);
    *data->value = item;
}

int radio(int iconid, const char *label, int *value) {
    int item = uiItem();
    uiSetSize(item, label?0:BND_TOOL_WIDTH, BND_WIDGET_HEIGHT);
    UIRadioData *data = (UIRadioData *)uiAllocHandle(item, sizeof(UIRadioData));
    data->head.subtype = ST_RADIO;
    data->head.handler = radiohandler;
    data->iconid = iconid;
    data->label = label;
    data->value = value;
    uiSetEvents(item, UI_BUTTON0_DOWN);
    return item;
}


pointer scheme_radio(scheme *sc, pointer args)
{

	hold_global_lock();

	int iconid = ivalue(pair_car(args));		
	args = pair_cdr(args);
	char *label_text = string_value(pair_car(args));
	args = pair_cdr(args);
	int value = ivalue(pair_car(args));	
	pointer result = mk_integer(sc,radio(iconid, label_text, &value));
	release_global_lock();
	return result;
}
	
int panel()
{
    int item = uiItem();
    UIData *data = (UIData *)uiAllocHandle(item, sizeof(UIData));
    data->subtype = ST_PANEL;
    data->handler = NULL;
    return item;
}

pointer scheme_panel(scheme *sc, pointer args)
{
	(void) args;
	hold_global_lock();	
	pointer result = mk_integer(sc,panel());
	release_global_lock();
	return result;
}
	
int hbox() {
    int item = uiItem();
    UIData *data = (UIData *)uiAllocHandle(item, sizeof(UIData));
    data->subtype = ST_HBOX;
    data->handler = NULL;
    uiSetBox(item, UI_ROW);
    return item;
}

pointer scheme_hbox(scheme *sc, pointer args) {

	(void) args;
	hold_global_lock();	
	pointer result = mk_integer(sc,hbox());
	release_global_lock();
	return result;
}


int vbox() {
    int item = uiItem();
    UIData *data = (UIData *)uiAllocHandle(item, sizeof(UIData));
    data->subtype = ST_VBOX;
    data->handler = NULL;
    uiSetBox(item, UI_COLUMN);
    return item;
}

pointer scheme_vbox(scheme *sc, pointer args)
{
	(void) args;
	hold_global_lock();	 
	pointer result = mk_integer(sc,vbox());
	release_global_lock();
	return result;
}
	
// UI LAYOUT --------------------


int column_append(int parent, int item)
{
    uiInsert(parent, item);
    // fill parent horizontally, anchor to previous item vertically
    uiSetLayout(item, UI_HFILL);
    uiSetMargins(item, 0, 1, 0, 0);
    return item;
}

pointer scheme_column_append(scheme *sc, pointer args)
{
	hold_global_lock();
	int parent = ivalue(pair_car(args));
	args = pair_cdr(args);
	int item = ivalue(pair_car(args));
	args = pair_cdr(args);	
	pointer result = mk_integer(sc,column_append(parent, item));
	release_global_lock();
	return result;
}
	
int column()
{
    int item = uiItem();
    uiSetBox(item, UI_COLUMN);
    return item;
}

pointer scheme_column(scheme *sc, pointer args)
{
	(void) args;
	hold_global_lock();	 
	pointer result = mk_integer(sc,column());
	release_global_lock();
	return result;
}
	
int vgroup_append(int parent, int item)
{
    uiInsert(parent, item);
    // fill parent horizontally, anchor to previous item vertically
    uiSetLayout(item, UI_HFILL);
    return item;
}

pointer scheme_vgroup_append(scheme *sc, pointer args)
{
	hold_global_lock();
	int parent = ivalue(pair_car(args));
	args = pair_cdr(args);
	int item = ivalue(pair_car(args));
	args = pair_cdr(args);	
	pointer result = mk_integer(sc,vgroup_append(parent, item));
	release_global_lock();
	return result;
}
	
int vgroup() {
    int item = uiItem();
    uiSetBox(item, UI_COLUMN);
    return item;
}

pointer scheme_vgroup(scheme *sc, pointer args)
{
	(void) args;
	hold_global_lock();	 
	pointer result = mk_integer(sc,vgroup());
	release_global_lock();
	return result;
}
	
int hgroup_append(int parent, int item) {
    uiInsert(parent, item);
    uiSetLayout(item, UI_HFILL);
    return item;
}

pointer scheme_hgroup_append(scheme *sc, pointer args)
{
	hold_global_lock();
	int parent = ivalue(pair_car(args));
	args = pair_cdr(args);
	int item = ivalue(pair_car(args));
	args = pair_cdr(args);	
	pointer result = mk_integer(sc,hgroup_append(parent, item));
	release_global_lock();
	return result;
}
	
int hgroup_append_fixed(int parent, int item) {
    uiInsert(parent, item);
    return item;
}

pointer scheme_hgroup_append_fixed(scheme *sc, pointer args)
{
	hold_global_lock();
	int parent = ivalue(pair_car(args));
	args = pair_cdr(args);
	int item = ivalue(pair_car(args));
	args = pair_cdr(args);	
	pointer result = mk_integer(sc,hgroup_append_fixed(parent, item));
	release_global_lock();
	return result;
}
	
int hgroup() {
    int item = uiItem();
    uiSetBox(item, UI_ROW);
    return item;
}

pointer scheme_hgroup(scheme *sc, pointer args)
{
	(void) args;
	hold_global_lock();	 
	pointer result = mk_integer(sc,hgroup());
	release_global_lock();
	return result;
}
	
int row_append(int parent, int item) {
    uiInsert(parent, item);
    uiSetLayout(item, UI_HFILL);
    return item;
}

pointer scheme_row_append(scheme *sc, pointer args)
{
	hold_global_lock();
	int parent = ivalue(pair_car(args));
	args = pair_cdr(args);
	int item = ivalue(pair_car(args));
	args = pair_cdr(args);	
	pointer result = mk_integer(sc,row_append(parent, item));
	release_global_lock();
	return result;
}
	
int row() {
    int item = uiItem();
    uiSetBox(item, UI_ROW);
    return item;
}

pointer scheme_row(scheme *sc, pointer args)
{
	(void) args;
	hold_global_lock();	 
	pointer result = mk_integer(sc,row());
	release_global_lock();
	return result;
}
	
// MAIN EVENT HANDLER -----------


static void roothandler(int parent, UIevent event) {
	(void) parent;
	switch(event) {
	default: break;
	case UI_SCROLL: {
		UIvec2 pos = uiGetScroll();
		printf("scroll! %d %d\n", pos.x, pos.y);
	} break;
	case UI_BUTTON0_DOWN: {
		printf("%d clicks\n", uiGetClicks());
	} break;
	}
}

// -- UI ELEMENT CREATION

void ui_handler(int item, UIevent event) {
	UIData *data = (UIData *)uiGetHandle(item);
	if (data && data->handler) {
		data->handler(item, event);
	}
}

}

namespace venk
{

int add_menu_option(int parent, const char *name, int *choice) {
    int opt = radio(-1, name, choice);
    uiInsert(parent, opt);
    uiSetLayout(opt, UI_HFILL|UI_TOP);
    uiSetMargins(opt, 1, 1, 1, 1);
    return opt;
}

bool GuiService::draw(float w, float h)
{
    bndBackground(vg, 0, 0, w, h);
    // some OUI stuff
    uiBeginLayout();
    int root = panel();
    // position root element
    uiSetSize(0,w,h);
    ((UIData*)uiGetHandle(root))->handler = roothandler;
    uiSetEvents(root, UI_SCROLL|UI_BUTTON0_DOWN);
    uiSetBox(root, UI_COLUMN);

    static int choice = -1;

    int menu = uiItem();
    uiSetLayout(menu, UI_HFILL|UI_TOP);
    uiSetBox(menu, UI_ROW);
    uiInsert(root, menu);

	int opt_file = add_menu_option(menu, "File", &choice);
    int opt_view = add_menu_option(menu, "View", &choice);
    int opt_edit = add_menu_option(menu, "Edit", &choice);
    int opt_tools = add_menu_option(menu, "Tools", &choice);
    int opt_about = add_menu_option(menu, "About", &choice);

    if (choice < 0)
        choice = opt_file;	

    int content = uiItem();
    uiSetLayout(content, UI_FILL);
    uiInsert(root, content);

	int democontent = uiItem();
	uiSetLayout(democontent, UI_TOP);
	uiSetSize(democontent, 250, 0);
	uiInsert(content, democontent);
   
	int col = column();
    uiInsert(democontent, col);
    uiSetMargins(col, 10, 10, 10, 10);
    uiSetLayout(col, UI_TOP|UI_HFILL);
    
    column_append(col, button(BND_ICON_GHOST, "Item 1"));
    column_append(col, button(BND_ICON_GHOST, "Item 2"));
    column_append(col, button(BND_ICON_GHOST, "Item 3"));

    uiEndLayout();	
    drawUI(vg, 0, BND_CORNER_NONE);
    uiProcess((int)(SDL_GetTicks()*1000.0));
	return true;
}

bool GuiService::mouse(SDL_Event* event)
{
	SDL_MouseMotionEvent *mouseMotionEvent;
	SDL_MouseButtonEvent *mouseButtonEvent;
	SDL_MouseWheelEvent  *mouseWheelEvent;	
	switch (event->type)
	{
		case SDL_MOUSEMOTION:
			mouseMotionEvent = (SDL_MouseMotionEvent*)(&event);
			uiSetCursor(mouseMotionEvent->x, mouseMotionEvent->y);
			break;
		case SDL_MOUSEBUTTONUP:
			mouseButtonEvent = (SDL_MouseButtonEvent*)(&event);
			uiSetButton(mouseButtonEvent->button, (int) SDL_GetModState(),0);
			break;
		case SDL_MOUSEBUTTONDOWN:
			mouseButtonEvent = (SDL_MouseButtonEvent*)(&event);
			uiSetButton(mouseButtonEvent->button, (int) SDL_GetModState(), 1);
			break;
		case SDL_MOUSEWHEEL:
			mouseWheelEvent = (SDL_MouseWheelEvent*)(&event);
			uiSetScroll(mouseWheelEvent->x, mouseWheelEvent->y);
			break;
	}
	return true;
}

bool GuiService::keyboard(SDL_Event* event)
{
	SDL_KeyboardEvent *keyEvent = (SDL_KeyboardEvent*)(&event);	
	switch (event->type)		
	{
		case SDL_KEYUP:
			uiSetKey(keyEvent->keysym.sym, keyEvent->keysym.mod,0);
			break;
		case SDL_KEYDOWN:
			uiSetKey(keyEvent->keysym.sym, keyEvent->keysym.mod,1);
			const char *ch = SDL_GetKeyName(keyEvent->keysym.sym);
			if (*ch != '\0') {
				unsigned int key = 0;
				while (*ch != '\0') {
					key = key << 8UL;
					key = key + (unsigned int) *ch;
					ch++;
				}
				uiSetChar(key);
			}			
			break;
	}
	return true;
}

bool GuiService::render()
{
	const int w  = 640;
	const int h = 480;
	
	nvgBeginFrame(vg, guiWidth, guiHeight, 1.0f);
	draw(w,h);
 	nvgEndFrame(vg);	
	return true;
}

bool GuiService::initialise(GuiService* self)
{
	self->uictx = uiCreateContext(4096, 1<<20);
	uiMakeCurrent(self->uictx);
	uiSetHandler(ui_handler);
	self->vg = nvgCreateGL3(NVG_ANTIALIAS);
	// TODO : via SDL :-)
    bndSetFont(nvgCreateFont(self->vg, "system", "DejaVuSans.ttf"));
    bndSetIconImage(nvgCreateImage(self->vg, "blender_icons16.png", 0));
	ServiceCheckout<MouseService> maus;
	ServiceCheckout<KeyboardService> keyb;
	self->mausObserver = maus->observers.addObserver(std::bind(&GuiService::mouse,self,std::placeholders::_1));
	self->keybObserver = keyb->observers.addObserver(std::bind(&GuiService::keyboard,self,std::placeholders::_1));
	return true;
}

bool GuiService::shutdown(GuiService* self)
{
	ServiceCheckout<MouseService> maus;
	ServiceCheckout<KeyboardService> keyb;
	maus->observers.removeObserver(self->mausObserver);
	keyb->observers.removeObserver(self->keybObserver);
	nvgDeleteGL3(self->vg);
	uiDestroyContext(self->uictx);
	return true;
}

}


