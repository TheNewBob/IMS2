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
			if (field.elementid == field_id || field.nestedlayout != NULL)
			{
				//calculate the effective size and position of the field, after margin and padding have been deducted.
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

				if (field.elementid == field_id)
				{
					//we have found the field we were looking for, return the rect.
					return fieldrect;
				}
				else if (field.nestedlayout != NULL)
				{
					//this field has a nested layout, that may also contain the element.
					//close the bulkheads and seal the hatches, we're going on a recursion!
					try
					{
						RECT nestedfieldrect = field.nestedlayout->GetFieldRectForRowWidth(field_id, availablefieldwidth);

						//the following is a bit of a twister.
						//Basically, if the field with a nested layout also has an element, it is assumed that the layout
						//applies to elements nested inside that element. Hence, the rect has to be relative for that parent, i.e. origin 0,0.
						//If on the other hand there is no element given for a nested layout, it's assumed that this layout simply subdivides
						//a field, but the elements therein are children of the parent element of the layout.
						//Ergo, the rect needs to be relative to the current layout.
						if (field.elementid == "")
						{
							fieldrect.left += nestedfieldrect.left;
							fieldrect.top += nestedfieldrect.top;
							fieldrect.right = fieldrect.left + (nestedfieldrect.right - nestedfieldrect.left);
							fieldrect.bottom = fieldrect.top + (nestedfieldrect.bottom - nestedfieldrect.top);
							return fieldrect;
						}
						else
						{
							return nestedfieldrect;
						}
					}
					catch (invalid_argument)
					{
						//no biggie, this is to be expected.
						Helpers::writeToLog(string("field " + field_id + " not found in nested layout."), L_DEBUG);
					}
				}
			}

			fieldrect.left += fieldwidth;
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
