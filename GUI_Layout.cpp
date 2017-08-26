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


RECT GUI_Layout::GetFieldRectForRowWidth(string field_id, int rowwidth, vector<string> &ignore)
{
	//just make sure somebody doesn't screw himself over.
	assert(find(ignore.begin(), ignore.end(), field_id) == ignore.end() && "Ignoring the field that is searched for!");

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
		//don't include this row in the calculation if it has only ignored fields.
		if (rowCanBeIgnored(row, ignore))
		{
			continue;
		}

		//include margin and padding of the row into the calculation
		fieldrect.top = fieldrect.top + EmToPx(row.margin.top);
		fieldrect.left = leftmargin + RelToPx(row.margin.left, rowwidth);

		double availablerowwidth = rowwidth
			- RelToPx(row.margin.left, rowwidth)
			- RelToPx(row.margin.right, rowwidth);
		fieldrect.left += RelToPx(row.padding.left, availablerowwidth);
		int availablerowheight = EmToPx(row.height) - EmToPx(row.padding.top) - EmToPx(row.padding.bottom);

		for (UINT j = 0; j < row.fields.size(); ++j)
		{
			LayoutField &field = row.fields[j];
			//if the field is on the ignored list, don't count it. Following fields will move to the left.
			if (find(ignore.begin(), ignore.end(), field.elementid) != ignore.end())
			{
				continue;
			}
			
			int fieldwidth = (int)(field.width * availablerowwidth);
			if (field.elementid == field_id || field.nestedlayout != NULL)
			{
				//calculate the effective size and position of the field, after margin and padding have been deducted.
				fieldrect.top = fieldrect.top + EmToPx(row.padding.top + field.margin.top + field.padding.top);
				int availablefieldheight = availablerowheight - EmToPx(
					field.margin.top + field.padding.top
					+ field.margin.bottom + field.padding.bottom);

				fieldrect.bottom = fieldrect.top + availablefieldheight;
				int leftmargin = RelToPx(field.margin.left, fieldwidth);
				int rightmargin = RelToPx(field.margin.right, fieldwidth);
				int availablefieldwidth = fieldwidth - leftmargin - rightmargin;

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
						RECT nestedfieldrect = field.nestedlayout->GetFieldRectForRowWidth(field_id, availablefieldwidth, ignore);

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


RECT GUI_Layout::GetFieldRectForRowWidth(string field_id, int rowwidth)
{
	vector<string> ignore_none;
	return GetFieldRectForRowWidth(field_id, rowwidth, ignore_none);
}


int GUI_Layout::GetLayoutHeight()
{
	vector<string> ignore_none;
	return GetLayoutHeight(ignore_none);
}



int GUI_Layout::GetLayoutHeight(vector<string> &ignore)
{
	int totalheight = EmToPx(margin.top + margin.bottom + padding.top + padding.bottom);
	for (UINT i = 0; i < rows.size(); ++i)
	{
		LayoutRow &r = rows[i];
		if (!rowCanBeIgnored(r, ignore))
		{
			totalheight += EmToPx(r.height + r.margin.top + r.margin.bottom);
		}
	}
	return totalheight;
}


bool GUI_Layout::rowCanBeIgnored(LayoutRow &IN_row, vector<string> &ignored_fields)
{
	UINT ignored_fields_in_row = 0;
	for (auto i = IN_row.fields.begin(); i != IN_row.fields.end(); ++i)
	{
		if (find(ignored_fields.begin(), ignored_fields.end(), i->elementid) != ignored_fields.end())
		{
			ignored_fields_in_row += 1;
		}
	}
	if (ignored_fields_in_row == IN_row.fields.size())
	{
		return true;
	}
	return false;
}


string GUI_Layout::GetStyleForField(string field_id)
{
	for (UINT i = 0; i < rows.size(); ++i)
	{
		for (auto j = rows[i].fields.begin(); j != rows[i].fields.end(); ++j)
		{
			LayoutField *field = &(*j);
			string fieldstyle = "";
			if (field->elementid == field_id)
			{
				fieldstyle = field->GetElementStyle();
			}
			else if (field->nestedlayout != NULL) 
			{
				fieldstyle = field->nestedlayout->GetStyleForField(field_id);
			}

			if (fieldstyle != "")
			{
				return fieldstyle;
			}
		}
	}
	return "";
}