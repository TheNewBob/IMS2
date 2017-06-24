#include "GUI_Common.h"
#include "GUI_Page.h"
#include "GUI_Layout.h"
#include "GUI_LayoutPage.h"


GUI_LayoutPage::GUI_LayoutPage(RECT mRect, int _id, GUI_ElementStyle *_style)
	: GUI_Page(mRect, _id, _style)
{
	
}


GUI_LayoutPage::~GUI_LayoutPage()
{

}


RECT GUI_LayoutPage::getElementRect(string elementid)
{
	assert(layouts != NULL && "LayoutPage attempts to create children with uninitialised layout!");
	//check how much space we have to draw on and which layout we have to use for that.
	RECT usablerect = calculateUsableRect();
	int usablewidth = usablerect.right - usablerect.left;
	GUI_Layout *usedlayout = layouts->GetLayoutForWidth(usablewidth);

	//Get the elements rect, then translate its position by the left and top margins
	RECT elementrect = usedlayout->GetFieldRectForRowWidth(elementid, usablewidth);
	elementrect.left += style->MarginLeft();
	elementrect.right += style->MarginLeft();
	elementrect.top += style->MarginTop();
	elementrect.bottom += style->MarginTop();

	return elementrect;
}