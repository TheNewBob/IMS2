#pragma once

//this class handles the construction menu


class GUI_MainConstruct : public GUI_Page
{
public:
	GUI_MainConstruct(RECT mRect, GUI_ElementStyle *_style, IMS2 *_vessel);
	~GUI_MainConstruct();
	void UpdateDockedVesselsList(std::vector<DOCKEDVESSEL*> &vesselList);

private:
	int ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);

	IMS2 *vessel;
	GUI_ListBox *dockedVesselsList;
	GUI_ListBox *stackVesselsList;
	GUI_Page *integrateButtonsPage;

};

