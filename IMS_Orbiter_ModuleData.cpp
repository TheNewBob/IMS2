#include "Common.h"
#include "SimpleShape.h"
#include "IMS_Orbiter_ModuleData.h"
#include "Rotations.h"
#include "Oparse.h"

using namespace Oparse;

OpMixedList *IMSATTACHMENTPOINT::GetMapping()
{
	return _MixedList(OpValues() = {
			{ _Param(pos), {} },
			{ _Param(rot), {} },
			{ _Param(dir), {} },
			{ _Param(id), { _LENGTH(1, 256) } },
		}, ",");
}

Oparse::OpModelDef HULLSHAPEDATA::GetModelDef()
{
	return OpModelDef() = {
		{ "shape",{ _MixedList(OpValues() = {
			{ _Param(shape), { _ISANYOF(vector<string>() = { "cylinder", "box", "sphere" } ) } },
			{ _Param(shapeparams), {} } }, ","),{} } },
		{ "offset",{ _Param(pos),{} } },
		{ "scale",{ _Param(scale),{} } },
		{ "orientation",{ _MixedList(OpValues() = {
			{ _Param(dir), {} }, { _Param(rot), {} } }), {} } }
	};
}

OpModelDef IMS_Orbiter_ModuleData::GetModelDef()
{
	return OpModelDef() = {
		{ "Meshname",{ _Param(_meshName),{ _REQUIRED() } } },
		{ "Size",{ _Param(_size),{} } },
		{ "Inertia",{ _Param(pmi),{} } },
		{ "IMS_ATTACHMENT",{ _Block<IMSATTACHMENTPOINT>(_attachmentPoints),{} } },
		{ "SHAPE", { _Model<HULLSHAPEDATA>(*shapeData), {} } }
	};
}





IMS_Orbiter_ModuleData::IMS_Orbiter_ModuleData()
	: _meshName(""), _configFileName(""), _size(-1), _valid(false)
{
}

IMS_Orbiter_ModuleData::~IMS_Orbiter_ModuleData()
{
	if (hullshape != NULL)
	{
		delete hullshape;
	}
}

bool IMS_Orbiter_ModuleData::hadErrors()
{
	//return if there were errors while loading the file
	return !_valid;
}

void IMS_Orbiter_ModuleData::PostParse()
{
	if (shapeData->shape != "")
	{
		createHullShape(shapeData);
	}
	delete shapeData;
}

string IMS_Orbiter_ModuleData::getMeshName()
{
	return _meshName;
}

float IMS_Orbiter_ModuleData::getSize()
{
	return _size;
}

void IMS_Orbiter_ModuleData::getPmi(VECTOR3 &pmi)
{
	pmi = this->pmi;
}

string IMS_Orbiter_ModuleData::getConfigFileName()
{
	return _configFileName;
}

//returns the classname of a vessel, basically the config file name without extension
string IMS_Orbiter_ModuleData::getClassName()
{
	string::size_type endofname = _configFileName.find_last_of(".");
	string classname = _configFileName.substr(0, endofname);
	return classname;
}

vector <IMSATTACHMENTPOINT> &IMS_Orbiter_ModuleData::getAttachmentPoints()
{
	return _attachmentPoints;
}

void IMS_Orbiter_ModuleData::createHullShape(HULLSHAPEDATA *shapedata)
{
	//create the basic shape depending on what shape is asked for
	if (shapedata->shape == "cylinder")
	{
		hullshape = new SimpleShape(SimpleShape::Cylinder(
			shapedata->shapeparams.x, shapedata->shapeparams.y, (UINT)shapedata->shapeparams.z));
	}
	else if (shapedata->shape == "box")
	{
		hullshape = new SimpleShape(SimpleShape::Box(
			shapedata->shapeparams.x, shapedata->shapeparams.y, shapedata->shapeparams.z));
	}
	else if (shapedata->shape == "sphere")
	{
		hullshape = new SimpleShape(SimpleShape::Sphere(
			shapedata->shapeparams.x, (UINT)shapedata->shapeparams.z));
	}

	//transform the shape into the final shape demanded by the config
	MATRIX3 shaperotation = Rotations::GetRotationMatrixFromOrientation(shapedata->dir, shapedata->rot);
	hullshape->Rotate(shaperotation);
	hullshape->Scale(shapedata->scale);
	hullshape->Translate(shapedata->pos);
}

