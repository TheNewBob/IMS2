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

/**
 * \brief This class contains static orbiter related properties that are common to all modules.
 *
 * <h3> Config file structure </h3> 
 * brackets are part of the actual synthax, so don't write them. Here's a little explanation of how this documentation is to be read:
 * \code
 * parameter = <argument1> <argument2> <...>	//every parameter has a name, and usually takes one or multiple arguments that are delimited by a space.
 * <type name>									//<> brackets signify an expected argument to a parameter, its expected type and what it is for. 
 * [Optional]									//Anything in [] is optional and must not necessarily be there for a config to work.
 *												//Or in other words, anything not in [] must be there, or there will be errors!
 * \endcode
 *
 * <h4> hull shapes </h4>
 * Note that transformations will always be applied in the below order, even if you don't write them in this order in the config.
 * \code
 * [BEGIN_SHAPE
 *		shape = <shapetype> <specific parameters>			//see below for details
 *		[offset] = <float x> <float y> <float z>			//moves the shape out of the center. Use this if the mesh of your module does not have its center at 0 0 0
 *		[scale] = <float x> <float y> <float z>				//non-uniform scaling on x y and z. Use this if the basic shapes are not enough to adequately approximate
 *															//the modules shape (flatened cylinder, oblong spheroid or somesuch).
 *		[orientation] = <float dirx> <float diry> <float dirz> <float rotx> <float roty> <float rotz>	
 *															//rotates the shape to the given orientation, under the assumption that the original alignment is 0 0 1, 0 1 0.
*															//Use this to rotate a shape to fit the orientation of the mesh.
 * END_SHAPE]
 * \endcode
 *
 * <p>
 * Defines a coarse hullshape for the module. This data will be used by IMS to generate touchdown points.
 * Note that this can get expensive for large vessels, so it makes sense to only use this parameter
 * on modules that will make up significant parts of a vessels structures. It should not be used for
 * small things like RCS thrusters or small propellant tanks.
 * </p>
 * <h4> valid shape parameters </h4>
 * \code
 *	shape = box <float x> <float y> <float z>		//Creates a box with dimensions x y z (meters) with the center at 0 0 0
 *	shape = cylinder <float radius> <float length> <int segments>	
 *													//Creates a cylinder aligned with the z-axis and its center at 0 0 0. radius and 
 *													//length are given in meters. segments denotes the number of radial segments. 
 *													//This must be an even number, >= 6, and I see no reason why it should ever be 
 *													//larger than 12, usually 8. You're playing with peoples framerate here!
 *	shape = sphere <float radius> <int segments>	//Creates a sphere with its center at 0 0 0. The same guidelines apply for segments
 *													//as do for cylinders, but more so.
 * \endcode
 *
 *	Example: Create a cylinder with a radius of 3 meters, a length of 12 m, offset from the center by -2 meters in the z axis.
 * \code 
 * BEGIN_SHAPE
 *		shape = cylinder 3 12 8
 *		offset 0 0 -2
 * END_SHAPE
 * \endcode	
 *
 * <h4> attachment points </h4>
 * \code
 * BEGIN_IMS_ATTACHMENT
 *	<position> <direction> <rotation> <attachment-type>		//see below for detailed description
 *		.
 *		.
 * END_IMS_ATTACHMENT 
 * \endcode
 * <p>
 * Define an arbitrary number of IMS attachment points. Attachment points will manifest on the module as docking ports, until another module
 * Is integrated on that point, then the Dockport will vanish.
 * </p>
 * \code
 * position: <float x> <float y> <float z>			//The position of the attachment point in meters, relative to 0 0 0 of the mesh.
 * direction: <float x> <float y> <float z>			//The facing ("forward") of the attachment point as a normalised vector.
 * rotation: <float x> <float y> <float z>			//The rotation ("up") of the attachment point as a normalised vector. MUST be perpendicular to direction, or bad things will happen!
 * attachment-type: <string>						//A string identifier for the "type" of the attachment point. Only attachment points with matching types can be assembled.
 * \endcode
 * 
 * Example: Creates two attachment points on a module, one facing forward aligned with the module, the other backwards and "on its head".
 * \code
 * BEGIN_IMS_ATTACHMENT
 *		0 0 6     0  0  1    0  1  0 	IMS2_DEFAULT
 *		0 0 -6    0  0  -1   0  -1  0 	IMS2_DEFAULT
 * END_IMS_ATTACHMENT
 * \endcode
 */
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

