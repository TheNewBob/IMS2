#include "GUI_Common.h"
#include "Events.h"
#include "GUI_Surface.h"
#include "GUI_elements.h"
#include "GUI_Looks.h"
#include "GUIentity.h"



GUIentity::GUIentity()
{

}


GUIentity::~GUIentity()
{
	for (map<int, GUI_BaseElement*>::iterator i = elements.begin(); i != elements.end(); ++i)
	{
		delete i->second;
	}
}



//registers a GUI element that has not been created by a factory function of the GUIentity
//use for registering non-regular GUI elements, most likely custom menu pages
void GUIentity::RegisterGuiElement(GUI_BaseElement *_element, int _parentId)
{
	//an id of -1 means that the id is not static, I.E. it might be different during different sessions
	// in this case, the generator will give it an ID that is guaranteed to be unique
	if (_element->GetId() == -1)
	{
		_element->id = GetDynamicUID();
	}

	//if the same uid tries to register twice, something has gone horribly wrong,
	//either by a developer passing an already used uid or an incorrect uid being 
	//returned from GetDynamicUID() if the passed id was -1
	assert(elements.find(_element->GetId()) == elements.end() && "Passed an already used UID for GUI element!");

	elements[_element->GetId()] = _element;
	bool childadded = AddChildToParent(_element, _parentId);
	assert(childadded);
}


GUI_Page *GUIentity::CreatePage(RECT _rect, int parent, int _id, string styleId, bool drawbackground)
{
	if (styleId == "") styleId = STYLE_PAGE;
	GUI_Page *newPage = new GUI_Page(_rect, _id, GetStyle(styleId), drawbackground);
	RegisterGuiElement(newPage, parent);
	return newPage;
}

GUI_Label *GUIentity::CreateLabel(string _text, RECT _rect, int parent, int _id, string styleId)
{
	if (styleId == "") styleId = STYLE_HEADING;
	GUI_Label *newlabel = new GUI_Label(_text, _rect, _id, GetStyle(styleId));
	RegisterGuiElement(newlabel, parent);
	return newlabel;
}

GUI_LabelValuePair *GUIentity::CreateLabelValuePair(string label, string value, RECT _rect, int parent, int id,
	string styleId, string valueFontId)
{
	if (styleId == "") styleId = STYLE_DEFAULT;
	GUI_LabelValuePair *newelement = new GUI_LabelValuePair(label, value, _rect, id, GetStyle(styleId));
	RegisterGuiElement(newelement, parent);
	return newelement;
}


GUI_ListBox *GUIentity::CreateListBox(RECT _rect, int parent, int _id, string styleId,
	string scrollbar_styleId, bool _selectBox, bool _noSelect)
{
	if (styleId == "") styleId = STYLE_LISTBOX;
	GUI_ListBox *newBox = new GUI_ListBox(_rect, _id, GetStyle(styleId), _selectBox, _noSelect);
	RegisterGuiElement(newBox, parent);
	return newBox;
}


/*GUI_StaticButton *GUIentity::CreateStaticButton(RECT _rect, int srcX, int srcY, SURFHANDLE _src, int parent, int _id)
{
	GUI_StaticButton *newButton = new GUI_StaticButton(_rect, srcX, srcY, _src, _id);
	RegisterGuiElement(newButton, parent);
	return newButton;
}*/


GUI_DynamicButton *GUIentity::CreateDynamicButton(string text, RECT _rect, int parent, int _id, string styleId)
{
	if (styleId == "") styleId = STYLE_BUTTON;
	GUI_DynamicButton *newButton = new GUI_DynamicButton(text, _rect, _id, GetStyle(styleId));
	RegisterGuiElement(newButton, parent);
	return newButton;
}


GUI_CheckBox *GUIentity::CreateCheckBox(string text, RECT _rect, int parent, int _id, string styleId)
{
	if (styleId == "") styleId = STYLE_CHECKBOX;
	GUI_CheckBox *newbox = new GUI_CheckBox(text, _rect, _id, GetStyle(styleId));
	RegisterGuiElement(newbox, parent);
	return newbox;
}


GUI_RadioButton *GUIentity::CreateRadioButton(string text, RECT _rect, int parent, int _id, string styleId)
{
	if (styleId == "") styleId = STYLE_CHECKBOX;
	GUI_RadioButton *newbtn = new GUI_RadioButton(text, _rect, _id, GetStyle(styleId));
	RegisterGuiElement(newbtn, parent);
	return newbtn;
}

GUI_StatusBar *GUIentity::CreateStatusBar(RECT _rect, int parent, int _id, string styleId)
{
	if (styleId == "") styleId = STYLE_STATUSBAR;
	GUI_StatusBar *newbar = new GUI_StatusBar(_rect, _id, GetStyle(styleId));
	RegisterGuiElement(newbar, parent);
	return newbar;
}



int GUIentity::GetDynamicUID()
{
	newuid++;
	return newuid;
}


GUI_BaseElement *GUIentity::GetElementById(int _id)
{
	map<int, GUI_BaseElement*>::iterator i = elements.find(_id);

	//you are looking for an id that doesn't exist. Either you're looking for it too early
	//and it gets registered later in the program flow, or you've forgotten to register it.
	assert(i != elements.end());
	return i->second;
}



void GUIentity::SetStyleSet(string _styleset)
{
	if (styleset != _styleset)
	{
		styleset = _styleset;
		for (auto i = elements.begin(); i != elements.end(); ++i)
		{
			GUI_BaseElement *element = i->second;
			string styleid = element->GetStyle()->GetId();
			GUI_ElementStyle *newstyle = GUI_Looks::GetStyle(styleid, styleset);
			element->SetStyle(newstyle);
			element->SetStyleSetForPlugins(styleset);
		}
	}
}

string GUIentity::GetStyleSet()
{
	return styleset;
}


GUI_ElementStyle *GUIentity::GetStyle(string _styleId)
{
	//if you are experiencing a crash here after adding a new module function,
	//the cause is probably that the GetGui() method of your ModuleFunctionData
	//returns false!
	return GUI_Looks::GetStyle(_styleId, styleset);
}

GUI_font *GUIentity::GetFont(string _fontId)
{
	return GUI_Looks::GetFont(_fontId, styleset);
}











