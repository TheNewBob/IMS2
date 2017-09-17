#pragma once

class LayoutField;
class LayoutRow;


class GUI_Layout : public LayoutElement
{
public:
	GUI_Layout();
	~GUI_Layout();

	/**
	 * \brief Creates a row out of a vector of fields and adds it to the layout.
	 * \note Rows are added consecutively from top to bottom.
	 */
	void AddRow(vector<LayoutField> fields, double height);

	/**
	 * \brief Adds an already built row to the layout.
	 */
	void AddRow(LayoutRow row);
	
	/**
	 * \return The rect (position and dimensions) for a field with a certain id, in pixel.
	 * \param field_id The id of the field to calculate dimensions for. Empty fields cannot be searched for, because there would be no point in that.
	 * \param rowwidth The current width of the row in pixel.
	 */
	RECT GetFieldRectForRowWidth(string field_id, int rowwidth);

	/**
	 * \return The The rect (position and dimensions) for a field with a certain id, discounting any fields which contain an element matching an id listed in ignore.
	 * \param field_id The id of the field to calculate dimensions for. Empty fields cannot be searched for, because there would be no point in that.
	 * \param rowwidth The current width of the row in pixel.
	 * \param ignore A vector with field ids. Any fields containing such an id will be treated as if it didn't exist in the layout!
	 */
	RECT GetFieldRectForRowWidth(string field_id, int rowwidth, vector<string> ignore);

	/**
	 * \return The id of the style for a certain field.
	 * \param field_id The id of the field to retrieve the style for.
	 * \note In case no style is given for the field, returns an empty string, which defers style selection to the factory of the element.
	 */
	string GetStyleForField(string field_id);
	
	/**
	 * \return The total height of the layout
	 */
	int GetLayoutHeight();

	/**
	 * \return The height of the layout, ignoring rows that consist only of ignored fields.
	 */
	int GetLayoutHeight(vector<string> &ignore);

	static int EmToPx(double em);
	static int RelToPx(double rel, double rowwidth);
private:

	vector<LayoutRow> rows;			//!< The rows in this layout, from top to bottom.

	static int pixel_per_em;

	/**
	 * \return True if a row contains no fields except for ignored ones, false otherwise.
	 */
	bool rowCanBeIgnored(LayoutRow &IN_row, vector<string> &ignored_fields);

};


struct LAYOUTCOLLECTION
{
	/**
	 * Returns the proper layout for the passed width (in pixel)
	 */
	GUI_Layout *GetLayoutForWidth(int width)
	{
		GUI_Layout *propperlayout = NULL;
		for (auto i = layouts.begin(); i != layouts.end(); ++i)
		{
			if (i->first < width)
			{
				propperlayout = i->second;
			}
			else
			{
				break;
			}
		}
		return propperlayout;
	};

	/**
	 * Adds a layout to the collection, maped to a minimum width a page must have to use this layout.
	 */
	void AddLayout(int minwidth, GUI_Layout *layout)
	{
		layouts.insert(make_pair(minwidth, layout));
	}
	
	map<int, GUI_Layout*> layouts;			//!< multiple layouts maped to their minimum size in pixel.
};
