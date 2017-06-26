#include "GUI_Common.h"
#include "LayoutElement.h"
#include "LayoutField.h"
#include "LayoutRow.h"
#include "GUI_Layout.h"

int GUI_Layout::pixel_per_em = 16;

GUI_Layout::GUI_Layout()
{
}

GUI_Layout::~GUI_Layout()
{
}


void GUI_Layout::AddRow(vector<LayoutField> fields, double height)
{
	LayoutRow newrow;
	newrow.height = height;
	newrow.fields = fields;
	rows.push_back(newrow);
}

void GUI_Layout::AddRow(LayoutRow row)
{
	rows.push_back(row);
}

int GUI_Layout::EmToPx(double em)
{
	return (int)(pixel_per_em * em);
}

int GUI_Layout::RelToPx(double rel, double rowwidth)
{
	return (int)(rowwidth * rel);
}

RECT GUI_Layout::GetFieldRectForRowWidth(string field_id, int rowwidth)
{
	RECT fieldrect = _R(0, 0, 0, 0);
	//deducting the layouts overall margin from the available rowwidth and setting starting position.
	int leftmargin = RelToPx(margin.left, rowwidth);
	int rightmargin = RelToPx(margin.right, rowwidth);
	int topmargin = EmToPx(margin.top);
	fieldrect.left = leftmargin;
	fieldrect.top = topmargin;

	rowwidth = rowwidth - leftmargin - rightmargin;

	// walk through rows and move cursor towards the bottom accordingly.
	for (UINT i = 0; i < rows.size(); ++i)
	{
		LayoutRow &row = rows[i];

		//include margin and padding of the row into the calculation
		fieldrect.top = fieldrect.top + EmToPx(row.margin.top);
		fieldrect.left = leftmargin + RelToPx(row.margin.left, rowwidth);

		double availablerowwidth = rowwidth
			- RelToPx(row.margin.left, rowwidth)
			- RelToPx(row.margin.right, rowwidth);
		fieldrect.left += RelToPx(row.padding.left, availablerowwidth);
		double availablerowheight = EmToPx(row.height) - EmToPx(row.padding.top) - EmToPx(row.padding.bottom);

		for (UINT j = 0; j < row.fields.size(); ++j)
		{
			LayoutField &field = row.fields[j];
			int fieldwidth = (int)(field.width * availablerowwidth);
			if (field.elementid == field_id)
			{
				fieldrect.top = fieldrect.top + EmToPx(row.padding.top + field.margin.top + field.padding.top);
				double availablefieldheight = availablerowheight - EmToPx(
					field.margin.top + field.padding.top
					+ field.margin.bottom + field.padding.bottom);

				fieldrect.bottom = fieldrect.top + availablefieldheight;
				double leftmargin = RelToPx(field.margin.left, fieldwidth);
				double rightmargin = RelToPx(field.margin.right, fieldwidth);
				double availablefieldwidth = fieldwidth - leftmargin - rightmargin;

				fieldrect.left = fieldrect.left + leftmargin + RelToPx(field.padding.left, availablefieldwidth);

				fieldrect.right = fieldrect.left + availablefieldwidth;
				return fieldrect;
			}
			else
			{
				fieldrect.left += fieldwidth;
			}
		}

		fieldrect.top += EmToPx(row.height);
	}

	//if we come to here, the field id was not found!
	string msg = "Error while parsing layout. No such field: " + field_id + "!";
	throw invalid_argument(msg);
}

bool GUI_Layout::rowContainsField(string field_id, LayoutRow &IN_row)
{
	assert(field_id != "" && "Cannot search for empty field_id in layout row!");

	for (auto i = IN_row.fields.begin(); i != IN_row.fields.end(); ++i)
	{
		if ((*i).elementid == field_id)
		{
			return true;
		}
	}

	return false;
}
