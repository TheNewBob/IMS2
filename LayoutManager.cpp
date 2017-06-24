#include "Common.h"
#include "GUI_Layout.h"
#include "LayoutManager.h"
#include "tinyxml2.h"

//using namespace tinyxml2;

LayoutManager::LayoutManager()
{
}


LayoutManager::~LayoutManager()
{
}


LAYOUTCOLLECTION *LayoutManager::GetLayout(string filename)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("config/IMS2/GUI/Layouts/default/test.xml");

	bool bugme = true;

	return NULL;
}