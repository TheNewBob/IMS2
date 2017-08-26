#pragma once

class GUI_Layout;

class LayoutField : public LayoutElement
{
	friend class GUI_Layout;
public:
	LayoutField();
	~LayoutField();

	double GetWidth();
	void SetWidth(double width);
	string GetElementId();
	string GetElementStyle();
	void SetElementId(string element_id);
	void SetElementStyle(string style);
	void SetNestedLayout(GUI_Layout *layout);

private:
	double width = 0;
	string elementid = "";
	string styleid = "";
	GUI_Layout *nestedlayout = NULL;
};