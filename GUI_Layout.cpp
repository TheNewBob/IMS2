#include "GUI_Common.h"
#include "GUI_Layout.h"


GUI_Layout::GUI_Layout()
{
}

GUI_Layout::~GUI_Layout()
{
}


void GUI_Layout::AddRow(vector<LAYOUTFIELD> fields, int height)
{
	LAYOUTROW newrow;
	newrow.height = height;
	newrow.fields = fields;
	rows.push_back(newrow);
}


RECT GUI_Layout::GetFieldRectForRowWidth(string field_id, int rowwidth)
{
	RECT fieldrect = _R(0, 0, 0, 0);
	for (UINT i = 0; i < rows.size(); ++i)
	{
		LAYOUTROW &row = rows[i];
		fieldrect.left = 0;
		for (UINT j = 0; j < row.fields.size(); ++j)
		{
			LAYOUTFIELD &field = row.fields[j];
			int fieldwidth = field.width * rowwidth;
			if (field.elementid == field_id)
			{
				fieldrect.bottom = fieldrect.top + row.height;
				fieldrect.right = fieldrect.left + fieldwidth;
				return fieldrect;
			}
			else
			{
				fieldrect.left += fieldwidth;
			}
		}

		fieldrect.top += row.height;
	}
}

bool GUI_Layout::rowContainsField(string field_id, LAYOUTROW &IN_row)
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
