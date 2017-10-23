#pragma once

/**
 * Used for sharing common graphical resources among GUI elements. 
 * Make sure all references to this resource were deleted before deleting the resource itself!
 */
class GUI_ElementResource
{
	friend class GUI_Looks;
public:
	GUI_ElementResource(SURFHANDLE surface) : surface(surface) {};
	
	~GUI_ElementResource() { 
		assert(references.size() == 0 && "Destroying resource that is still referenced!");
		oapiDestroySurface(surface); 
	};

	/**
	 * \return the number of elements referencing this resource.
	 */
	int NumReferences() { return references.size(); };
	
	/**
	 * \return true if the resource is compatible with the passed element, false otherwise. 
	 */
	bool IsCompatibleWith(GUI_BaseElement *element)
	{
		return references.size() > 0 && references[0]->IsResourceCompatibleWith(element);
	};

	/**
	 * Retrieves the surfhandle
	 */
	SURFHANDLE GetSurface() { return surface; };
private:
	SURFHANDLE surface = NULL;
	vector<GUI_BaseElement*> references;

	void addReference(GUI_BaseElement *element)
	{
		assert(find(references.begin(), references.end(), element) == references.end() && "Attempting to register resource twice!");
		references.push_back(element);
	}

	void removeReference(GUI_BaseElement *element)
	{
		auto reference = find(references.begin(), references.end(), element);
		assert(reference != references.end() && "Attempting to remove reference that was never registered!");
		references.erase(reference);
	}
};

