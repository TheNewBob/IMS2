#pragma once

class SimpleShape;

/**
* \brief Contains data to construct the hullshape as loaded from file.
* \note This is only used to defer shape construction until the entire file has been loaded.
*/
struct HULLSHAPEDATA
{
	string shape = "";
	VECTOR3 shapeparams;
	VECTOR3 scale = _V(1, 1, 1);
	VECTOR3 pos = _V(0, 0, 0);
	VECTOR3 dir = _V(0, 0, 1);
	VECTOR3 rot = _V(0, 1, 0);
};

//this class contains static orbiter related properties that are common to all modules
class IMS_Orbiter_ModuleData
{
public:
	IMS_Orbiter_ModuleData();
	~IMS_Orbiter_ModuleData();
	bool hadErrors();
	virtual void LoadFromFile(string configfilename, IMSFILE file);

	string getMeshName();
	int getSize();
	string getConfigFileName();
	string getClassName();
	vector <IMSATTACHMENTPOINT> &getAttachmentPoints();

	/**
	* \return A pointer to the hull shape of the module
	*/
	SimpleShape *GetHullShape() { return hullshape; }

protected:
	std::string _configFileName;
	string _meshName;
	int _size;
	vector<IMSATTACHMENTPOINT> _attachmentPoints;
	bool _valid;
	SimpleShape *hullshape = NULL;								//!< The vertices of the hull shape of the module, module relative.


private:
	/**
	* \brief creates the hull shape based on the parameters read from the scenario line
	* \param shapedata The data defining the nature of the shape.
	*/
	void createHullShape(HULLSHAPEDATA shapedata);

	/**
	* \brief Reads the next line of a SHAPE block from the config file
	* \param file The file to read from. It is implied that the last line read from the file was BEGIN_SHAPE.
	* \return The data describing the shape read from the file.
	*/
	HULLSHAPEDATA readShape(IMSFILE file);

};

