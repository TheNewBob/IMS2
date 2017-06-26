#pragma once
struct RECT_DOUBLE
{
	double left = 0;
	double top = 0;
	double right = 0;
	double bottom = 0;

	RECT_DOUBLE(){};

	RECT_DOUBLE(double left, double top, double right, double bottom)
	{
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
	};
};


class LayoutElement
{
public:
	LayoutElement();
	~LayoutElement();

	double GetMarginLeft();
	void SetMarginLeft(double margin_left);

	double GetMarginTop();
	void SetMarginTop(double margin_top);

	double GetMarginRight();
	void SetMarginRight(double margin_right);

	double GetMarginBottom();
	void SetMarginBottom(double margin_bottom);


	double GetPaddingLeft();
	void SetPaddingLeft(double padding_left);

	double GetPaddingTop();
	void SetPaddingTop(double padding_top);

	double GetPaddingRight();
	void SetPaddingRight(double padding_right);

	double GetPaddingBottom();
	void SetPaddingBottom(double padding_bottom);

	void SetMargin(RECT_DOUBLE margin);
	void SetPadding(RECT_DOUBLE padding);

protected:
	RECT_DOUBLE margin;
	RECT_DOUBLE padding;
};