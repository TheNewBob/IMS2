#pragma once

class LayoutField;

class LayoutRow : public LayoutElement
{
	friend class GUI_Layout;
public:
	LayoutRow();
	~LayoutRow();

	double GetHeight();
	void SetHeight(double height_in_em);

	void AddField(LayoutField newfield);
private:
	vector<LayoutField> fields;
	double height = 1;
};