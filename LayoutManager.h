#pragma once

/**
 * A global manager that loads and cashes layouts.
 */
class LayoutManager
{
public:
	LayoutManager();
	~LayoutManager();

	static LAYOUTCOLLECTION *GetLayout(string filename);

private:

	static map<string, LAYOUTCOLLECTION*> layoutcollections;
	static string defaultpath;
};

