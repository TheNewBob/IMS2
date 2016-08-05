//this file contains all input callbacks for input boxes

#include "Common.h"
#include "IMS_InputCallback.h"
#include "EventTypes.h"
#include "EventHandler.h"
#include "IMS.h"
#include "IMS_Module.h"



bool RotateAttachmentPointCallback(void *id, char *str, void *usrdata)
{	
	double angle = 0;
	sscanf(str, "%lf", &angle);
	if (strlen(str) == 0 || angle == 0) 
	{
		return false;
	}
	angle *= RAD_PER_DEGREE;

	IMSATTACHMENTPOINT* att = (IMSATTACHMENTPOINT*)usrdata;
	att->isMemberOf->RotateAttachmentPoint(att, angle);
	return true;
}

