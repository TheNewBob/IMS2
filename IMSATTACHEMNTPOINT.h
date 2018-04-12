#pragma once
//struct for storing properties of an attachment point
struct IMSATTACHMENTPOINT
{
	VECTOR3 pos;
	VECTOR3 rot;
	VECTOR3 dir;
	double rotModifier;
	DOCKHANDLE dockPort;
	std::string id;
	bool exists;
	IMSATTACHMENTPOINT *connectsTo;
	DOCKEDVESSEL *dockedVessel;
	IMS_Module *isMemberOf;

	Oparse::OpMixedList *GetMapping();
};
