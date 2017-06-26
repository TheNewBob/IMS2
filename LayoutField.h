#pragma once


class LayoutField : public LayoutElement
{
	friend class GUI_Layout;
public:
	LayoutField();
	~LayoutField();

	double GetWidth();
	void SetWidth(double width);
	string GetElementId();
	void SetElementId(string element_id);

private:
	double width = 0;
	string elementid = "";
};