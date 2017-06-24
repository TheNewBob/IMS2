#pragma once

struct LAYOUTFIELD
{
	double width = 0;
	string elementid = "";
};

struct LAYOUTROW
{
	vector<LAYOUTFIELD> fields;
	int height = -1;
};

class GUI_Layout
{
public:
	GUI_Layout();
	~GUI_Layout();

	/**
	 * \brief Adds a row to the layout.
	 * \note Rows are added consecutively from top to bottom.
	 */
	void AddRow(vector<LAYOUTFIELD> fields, int height);
	
	/**
	 * \return The rect (position and dimensions) for a field with a certain id, in pixel.
	 * \param field_id The id of the field to calculate dimensions for. Empty fields cannot be searched for, because there would be no point in that.
	 * \param rowwidth The current width of the row in pixel.
	 */
	RECT GetFieldRectForRowWidth(string field_id, int rowwidth);

private:

	vector<LAYOUTROW> rows;			//!< The rows in this layout, from top to bottom.


	/**
	 * \return True if a row contains the passed field, false if not.
	 */
	bool rowContainsField(string field_id, LAYOUTROW &IN_row);

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
