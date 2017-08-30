/******************************************************************************/
/* This file has been generated by the uGFX-Studio                            */
/*                                                                            */
/* http://ugfx.org                                                            */
/******************************************************************************/

#include "user_colors.h"
#include "user_styles.h"
#include "gui.h"

// GListeners
GListener gl;

// GHandles
GHandle ghContainerPage0;
GHandle ghKeyboard1;
GHandle ghFirst;
GHandle ghUser;
GHandle ghButton1;
GHandle ghContainerPage1;
GHandle ghContainer1;
GHandle ghRadiobutton3;
GHandle ghRadiobutton2;
GHandle ghRadiobutton1;
GHandle ghSlider1;
GHandle ghSlider1_1;
GHandle ghList1;
GHandle ghCheckbox2;
GHandle ghCheckbox1;
GHandle ghButton2;
GHandle ghLabel1;

// Fonts
font_t dejavu_sans_20_anti_aliased;

static void createPagePage0(void)
{
	GWidgetInit wi;
	gwinWidgetClearInit(&wi);


	// create container widget: ghContainerPage0
	wi.g.show = FALSE;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.g.width = 480;
	wi.g.height = 272;
	wi.g.parent = 0;
	wi.text = "Container";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghContainerPage0 = gwinContainerCreate(0, &wi, 0);

	// Create keyboard widget: ghKeyboard1
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 130;
	wi.g.width = 460;
	wi.g.height = 130;
	wi.g.parent = ghContainerPage0;
	wi.text = "Keyboard1";
	wi.customDraw = gwinKeyboardDraw_Normal;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghKeyboard1 = gwinKeyboardCreate(0, &wi);
	gwinSetFont(ghKeyboard1, dejavu_sans_20_anti_aliased);
	gwinRedraw(ghKeyboard1);

	// Create textedit widget: ghFirst
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 70;
	wi.g.width = 210;
	wi.g.height = 40;
	wi.g.parent = ghContainerPage0;
	wi.text = "First Name";
	wi.customDraw = gwinTexteditDefaultDraw;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghFirst = gwinTexteditCreate(0, &wi, 30);
	gwinSetFont(ghFirst, dejavu_sans_20_anti_aliased);
	gwinRedraw(ghFirst);

	// Create textedit widget: ghUser
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 10;
	wi.g.width = 210;
	wi.g.height = 40;
	wi.g.parent = ghContainerPage0;
	wi.text = "User Name";
	wi.customDraw = gwinTexteditDefaultDraw;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghUser = gwinTexteditCreate(0, &wi, 30);

	// create button widget: ghButton1
	wi.g.show = TRUE;
	wi.g.x = 300;
	wi.g.y = 10;
	wi.g.width = 170;
	wi.g.height = 40;
	wi.g.parent = ghContainerPage0;
	wi.text = "Change Page";
	wi.customDraw = gwinButtonDraw_Normal;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghButton1 = gwinButtonCreate(0, &wi);
}

static void createPagePage1(void)
{
	GWidgetInit wi;
	gwinWidgetClearInit(&wi);


	// create container widget: ghContainerPage1
	wi.g.show = FALSE;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.g.width = 480;
	wi.g.height = 272;
	wi.g.parent = 0;
	wi.text = "Container";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghContainerPage1 = gwinContainerCreate(0, &wi, 0);

	// create container widget: ghContainer1
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 160;
	wi.g.width = 210;
	wi.g.height = 100;
	wi.g.parent = ghContainerPage1;
	wi.text = "Container1";
	wi.customDraw = gwinContainerDraw_Std;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghContainer1 = gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);

	// create radiobutton widget: ghRadiobutton3
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 70;
	wi.g.width = 180;
	wi.g.height = 20;
	wi.g.parent = ghContainer1;
	wi.text = "Custom";
	wi.customDraw = gwinRadioDraw_Radio;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghRadiobutton3 = gwinRadioCreate(0, &wi, 0);
	gwinSetColor(ghRadiobutton3, Black);
	gwinSetBgColor(ghRadiobutton3, White);

	// create radiobutton widget: ghRadiobutton2
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 40;
	wi.g.width = 180;
	wi.g.height = 20;
	wi.g.parent = ghContainer1;
	wi.text = "Black";
	wi.customDraw = gwinRadioDraw_Radio;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghRadiobutton2 = gwinRadioCreate(0, &wi, 0);
	gwinSetColor(ghRadiobutton2, Black);
	gwinSetBgColor(ghRadiobutton2, White);

	// create radiobutton widget: ghRadiobutton1
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 10;
	wi.g.width = 180;
	wi.g.height = 20;
	wi.g.parent = ghContainer1;
	wi.text = "White";
	wi.customDraw = gwinRadioDraw_Radio;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghRadiobutton1 = gwinRadioCreate(0, &wi, 0);
	gwinSetColor(ghRadiobutton1, Black);
	gwinSetBgColor(ghRadiobutton1, White);

	// create button widget: ghSlider1
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 10;
	wi.g.width = 210;
	wi.g.height = 30;
	wi.g.parent = ghContainerPage1;
	wi.text = "Slider 1";
	wi.customDraw = gwinSliderDraw_Std;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghSlider1 = gwinSliderCreate(0, &wi);
	gwinSliderSetRange(ghSlider1, 0, 100);
	gwinSliderSetPosition(ghSlider1, 25);
	gwinSetColor(ghSlider1, Black);
	gwinSetBgColor(ghSlider1, White);

	// create button widget: ghSlider1_1
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 50;
	wi.g.width = 210;
	wi.g.height = 30;
	wi.g.parent = ghContainerPage1;
	wi.text = "Slider 2";
	wi.customDraw = gwinSliderDraw_Std;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghSlider1_1 = gwinSliderCreate(0, &wi);
	gwinSliderSetRange(ghSlider1_1, 0, 100);
	gwinSliderSetPosition(ghSlider1_1, 86);
	gwinSetColor(ghSlider1_1, Black);
	gwinSetBgColor(ghSlider1_1, White);

	// Create list widget: ghList1
	wi.g.show = TRUE;
	wi.g.x = 300;
	wi.g.y = 160;
	wi.g.width = 170;
	wi.g.height = 100;
	wi.g.parent = ghContainerPage1;
	wi.text = "List1";
	wi.customDraw = gwinListDefaultDraw;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghList1 = gwinListCreate(0, &wi, FALSE);
	gwinListSetScroll(ghList1, scrollSmooth);
	gwinListAddItem(ghList1, "Item1", FALSE);
	gwinListAddItem(ghList1, "Item2", FALSE);
	gwinListAddItem(ghList1, "Item3", FALSE);
	gwinListAddItem(ghList1, "Item4", FALSE);
	gwinListAddItem(ghList1, "Item5", FALSE);
	gwinListAddItem(ghList1, "Item6", FALSE);
	gwinListAddItem(ghList1, "Item7", FALSE);
	gwinListAddItem(ghList1, "Item8", FALSE);
	gwinListAddItem(ghList1, "Item9", FALSE);
	gwinListAddItem(ghList1, "Item10", FALSE);
	gwinListAddItem(ghList1, "Item11", FALSE);
	gwinListAddItem(ghList1, "Item12", FALSE);
	gwinListSetSelected(ghList1, 0, FALSE);
	gwinListSetSelected(ghList1, 1, FALSE);
	gwinListSetSelected(ghList1, 2, FALSE);
	gwinListSetSelected(ghList1, 3, FALSE);
	gwinListSetSelected(ghList1, 4, FALSE);
	gwinListSetSelected(ghList1, 5, FALSE);
	gwinListSetSelected(ghList1, 6, FALSE);
	gwinListSetSelected(ghList1, 7, FALSE);
	gwinListSetSelected(ghList1, 8, FALSE);
	gwinListSetSelected(ghList1, 9, FALSE);
	gwinListSetSelected(ghList1, 10, FALSE);
	gwinListSetSelected(ghList1, 11, FALSE);

	// create checkbox widget: ghCheckbox2
	wi.g.show = TRUE;
	wi.g.x = 300;
	wi.g.y = 110;
	wi.g.width = 150;
	wi.g.height = 20;
	wi.g.parent = ghContainerPage1;
	wi.text = "Checkbox 2";
	wi.customDraw = gwinCheckboxDraw_CheckOnLeft;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghCheckbox2 = gwinCheckboxCreate(0, &wi);
	gwinCheckboxCheck(ghCheckbox2, FALSE);

	// create checkbox widget: ghCheckbox1
	wi.g.show = TRUE;
	wi.g.x = 300;
	wi.g.y = 70;
	wi.g.width = 150;
	wi.g.height = 20;
	wi.g.parent = ghContainerPage1;
	wi.text = "Checkbox 1";
	wi.customDraw = gwinCheckboxDraw_CheckOnLeft;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghCheckbox1 = gwinCheckboxCreate(0, &wi);
	gwinCheckboxCheck(ghCheckbox1, FALSE);

	// create button widget: ghButton2
	wi.g.show = TRUE;
	wi.g.x = 300;
	wi.g.y = 10;
	wi.g.width = 170;
	wi.g.height = 40;
	wi.g.parent = ghContainerPage1;
	wi.text = "Change Page";
	wi.customDraw = gwinButtonDraw_Normal;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghButton2 = gwinButtonCreate(0, &wi);

	// Create label widget: ghLabel1
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 130;
	wi.g.width = 210;
	wi.g.height = 30;
	wi.g.parent = ghContainerPage1;
	wi.text = "Select a Widget Style";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghLabel1 = gwinLabelCreate(0, &wi);
	gwinLabelSetBorder(ghLabel1, FALSE);
	
	// Select one of the radio buttons
	gwinRadioPress(ghRadiobutton1);
}

void guiShowPage(unsigned pageIndex)
{
	// Hide all pages
	gwinHide(ghContainerPage0);
	gwinHide(ghContainerPage1);

	// Show page selected page
	switch (pageIndex) {
	case 0:
		gwinShow(ghContainerPage0);
		break;

	case 1:
		gwinShow(ghContainerPage1);
		break;

	default:
		break;
	}
}

void guiCreate(void)
{
	GWidgetInit wi;

	// Prepare fonts
	dejavu_sans_20_anti_aliased = gdispOpenFont("DejaVuSans20_aa");

	// Prepare images

	// GWIN settings
	gwinWidgetClearInit(&wi);
	gwinSetDefaultFont(dejavu_sans_20_anti_aliased);
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gwinSetDefaultColor(Black);
	gwinSetDefaultBgColor(White);

	// Create all the display pages
	createPagePage0();
	createPagePage1();

	// Select the default display page
	guiShowPage(1);

}

void guiEventLoop(void)
{
	GEvent* pe;

	//while (1) {
		// Get an event
		pe = geventEventWait(&gl, 0);
		switch (pe->type) {
			case GEVENT_GWIN_BUTTON:
				if (((GEventGWinButton*)pe)->gwin == ghButton1) {
					guiShowPage(1);
				}
				
				else if (((GEventGWinButton*)pe)->gwin == ghButton2) {
					guiShowPage(0);
				}
				break;
				
			case GEVENT_GWIN_RADIO:
				if (((GEventGWinButton*)pe)->gwin == ghRadiobutton1) {
					gwinSetDefaultStyle(&WhiteWidgetStyle, TRUE);
				}
				
				else if (((GEventGWinButton*)pe)->gwin == ghRadiobutton2) {
					gwinSetDefaultStyle(&BlackWidgetStyle, TRUE);
					guiShowPage(1);
				}
				
				else if (((GEventGWinButton*)pe)->gwin == ghRadiobutton3) {
					gwinSetDefaultStyle(&CustomStyle, TRUE);
				}
				break;
		}

	//}
}
