// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// Instrument.cpp
// Base class for panel and VC instrument visualisations
// ==============================================================

#include "Common.h"
#include "EventTypes.h"
#include "EventHandler.h"
#include "IMS.h"
#include "Instrument.h"
//#include "Orbitersdk.h"


PanelElement::PanelElement (VESSEL3 *v)
{
	vessel = v;
	grp = 0;
	vtxofs = 0;
	mesh = 0;
	gidx = 0;
}

PanelElement::~PanelElement ()
{
}

void PanelElement::Reset2D ()
{
}

bool PanelElement::Redraw2D (SURFHANDLE surf)
{
	return false;
}

bool PanelElement::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	return false;
}

bool PanelElement::ProcessMouse2D (int event, int mx, int my)
{
	return false;
}

bool PanelElement::ProcessMouseVC (int event, VECTOR3 &p)
{
	return false;
}

bool PanelElement::RedrawBlank(SURFHANDLE surf)
{
	//by default, draw normally
	return Redraw2D(surf);
}

// ==============================================================

void PanelElement::AddGeometry (MESHHANDLE hMesh, DWORD grpidx, const NTVERTEX *vtx, DWORD nvtx, const WORD *idx, DWORD nidx)
{
	mesh = hMesh;
	gidx = grpidx;
	grp  = oapiMeshGroup (hMesh, grpidx);
	vtxofs = grp->nVtx;
	oapiAddMeshGroupBlock (hMesh, grpidx, vtx, nvtx, idx, nidx);
}

// ==============================================================

char *PanelElement::DispStr (double dist, int precision)
{
	static char strbuf[32];
	double absd = fabs (dist);
	if (absd < 1e4) {
		if      (absd < 1e3)  sprintf (strbuf, "% 6.*f ", precision-3, dist);
		else                  sprintf (strbuf, "% 0.*fk", precision-1, dist*1e-3);
	} else if (absd < 1e7) {
		if      (absd < 1e5)  sprintf (strbuf, "% 0.*fk", precision-2, dist*1e-3);
		else if (absd < 1e6)  sprintf (strbuf, "% 0.*fk", precision-3, dist*1e-3);
		else                  sprintf (strbuf, "% 0.*fM", precision-1, dist*1e-6);
	} else if (absd < 1e10) {
		if      (absd < 1e8)  sprintf (strbuf, "% 0.*fM", precision-2, dist*1e-6);
		else if (absd < 1e9)  sprintf (strbuf, "% 0.*fM", precision-3, dist*1e-6);
		else                  sprintf (strbuf, "% 0.*fG", precision-1, dist*1e-9);
	} else {
		if      (absd < 1e11) sprintf (strbuf, "% 0.*fG", precision-2, dist*1e-9);
		else if (absd < 1e12) sprintf (strbuf, "% 0.*fG", precision-3, dist*1e-9);
		else                  strcpy (strbuf, "--.--");
	}
	return strbuf;
}



//===================================================================

MFDButtonCol::MFDButtonCol (VESSEL3 *v, DWORD _lr, DWORD _xcnt, DWORD _ytop, int _MFDid)
  : PanelElement (v)
{
	//76
  lr = _lr;
  xcnt = _xcnt + (lr * 305);  // adjust according to geometry
  ytop = _ytop;          // same here
  dy   = 41;           // same here
  MFDid = _MFDid;
}

bool MFDButtonCol::Redraw2D (SURFHANDLE surf)
{
	int mfdMode = oapiGetMFDMode(MFDid);
	if (mfdMode == mode) return true;	//does not execute if mode hasn't changed. higher performance.

	mode = mfdMode;

	if (oapiGetMFDMode(MFDid) != MFD_NONE)
	{

	  const int btnw = 45; // button label area width
	  const int btnh = MFD_font_height; // button label area height
	  int btn, x, len, i, w, MFD_font_xpos;
	  const char *label;


	  for (btn = 0; btn < 6; btn++) {
	    // blank buttons
		oapiBlt (surf, surf, xcnt-btnw/2, ytop+dy*btn + 7,
	      1340, 0, MFD_font_width * 3, MFD_font_height);

		// write labels
	    if (label = oapiMFDButtonLabel (MFDid, btn+lr*6)) {
		  len = strlen(label);
	      for (w = i = 0; i < len; i++)
		    w += MFD_font_width;
	      for (i = 0, x = xcnt-w/2; i < len; i++) {
			w = MFD_font_width;
		    if (w) {
				MFD_font_xpos = (label[i] - 32) * MFD_font_width; 
				oapiBlt (surf, surf, x, ytop+dy*btn + 7, MFD_font_xpos,
				MFD_font_ypos, w, MFD_font_height);
			    x += w;
		    }
	      }
		} 
		else break;
	 }
	}
	return false;	
}

bool MFDButtonCol::ProcessMouse2D (int event, int mx, int my)
{
  if (my % dy < 44) {
		int btn = my/dy + lr*6;
	    oapiProcessMFDButton (MFDid, btn, event);
		return true;
	} 
  else return false;
}

MFDButtonRow::MFDButtonRow (VESSEL3 *v, DWORD _ycnt, DWORD _xleft, int _MFDid, int _MYid)
  : PanelElement (v)
{
  xleft = _xleft;  // adjust according to geometry
  ycnt = _ycnt;          // same here
  dx   = 51;           // same here
  MFDid = _MFDid;
  MYid = _MYid;
  firstCall = true;
}

bool MFDButtonRow::RedrawBlank(SURFHANDLE surf)
{
	int left, top, width, height,leftBtnTgt, leftBtnSrc, topBtnTgt, 
		topBtnSrc, widthBtn, heightBtn, leftBlnk, topBlnk, widthBlnk, heightBlnk,
		leftFillerTgt, topFillerTgt, leftFillerSrc, topFillerSrc, widthFiller, heightFiller;
	width = 376;
	height = 274;
	leftBtnSrc = 1808;
	widthBtn = 239;
	heightBtn = 50;
	widthBlnk = 89;
	heightBlnk = 50;
	widthFiller = 78;
	heightFiller = 15;

	switch (MFDid)
	{
	case 0: 
		left = 262;
		top = 241;
		topBtnSrc = 202;
		leftBtnTgt = 86;
		topBtnTgt = 464;
		leftBlnk = leftBtnTgt;
		topBlnk = 463; 
		leftFillerSrc = 1970;
		topFillerSrc = 187;
		leftFillerTgt = 248;
		topFillerTgt = 449;
		break;
	case 1: 
		left = 642;
		top = 241;
		topBtnSrc = 272;
		leftBtnTgt = 952;
		topBtnTgt = 464;
		leftBlnk = 1102;
		topBlnk = 463; 
		leftFillerSrc = 1808;
		topFillerSrc = 256;
		leftFillerTgt = 952;
		topFillerTgt = 449;
		break;
	case 2: 
		left = 262;
		top = 517;
		topBtnSrc = 330;
		leftBtnTgt = 86;
		topBtnTgt = 516;
		leftBlnk = leftBtnTgt;
		topBlnk = topBtnTgt; 
		leftFillerSrc = 1970;
		topFillerSrc = 380;
		leftFillerTgt = 248;
		topFillerTgt = 566;
		break;
	case 3:
		left = 642;
		top = 517;
		topBtnSrc = 400;
		leftBtnTgt = 952;
		topBtnTgt = 516;
		leftBlnk =  1102;
		topBlnk = topBtnTgt; 
		leftFillerSrc = 1808;
		topFillerSrc = 450;
		leftFillerTgt = 952;
		topFillerTgt = 566;
		break;
	}

	int mfdMode = oapiGetMFDMode(MFDid);
	if (mfdMode != MFD_NONE) oapiToggleMFD_on (MFDid);

	//make MFD transparent if shut off
	oapiBlt(surf, surf, left, top, 1670, 744, width, height);
	oapiBlt(surf, surf, leftBtnTgt, topBtnTgt, 1700, 750, widthBtn, heightBtn);
	oapiBlt (surf, surf, leftFillerTgt, topFillerTgt, 1700, 750, widthFiller, heightFiller);
	oapiBlt(surf, surf, leftBlnk, topBlnk, 1700, 750, widthBlnk, heightBlnk);

	return true;
}

bool MFDButtonRow::Redraw2D (SURFHANDLE surf)
{

	int mfdMode = oapiGetMFDMode(MFDid);
	if (mfdMode == mode && !firstCall) return true;		//does not execute if mode hasn't changed. higher performance.
	if (firstCall) firstCall = false;
	mode = mfdMode;

	int left, top, width, height,leftBtnTgt, leftBtnSrc, topBtnTgt, 
		topBtnSrc, widthBtn, heightBtn, leftBlnk, topBlnk, widthBlnk, heightBlnk,
		leftFillerTgt, topFillerTgt, leftFillerSrc, topFillerSrc, widthFiller, heightFiller;
	width = 376;
	height = 274;
	leftBtnSrc = 1808;
	widthBtn = 239;
	heightBtn = 50;
	widthBlnk = 89;
	heightBlnk = 50;
	widthFiller = 78;
	heightFiller = 15;

	switch (MFDid)
	{
	case 0: 
		left = 262;
		top = 241;
		topBtnSrc = 202;
		leftBtnTgt = 86;
		topBtnTgt = 464;
		leftBlnk = leftBtnTgt;
		topBlnk = 463; 
		leftFillerSrc = 1970;
		topFillerSrc = 187;
		leftFillerTgt = 248;
		topFillerTgt = 449;
		break;
	case 1: 
		left = 642;
		top = 241;
		topBtnSrc = 272;
		leftBtnTgt = 952;
		topBtnTgt = 464;
		leftBlnk = 1102;
		topBlnk = 463; 
		leftFillerSrc = 1808;
		topFillerSrc = 256;
		leftFillerTgt = 952;
		topFillerTgt = 449;
		break;
	case 2: 
		left = 262;
		top = 517;
		topBtnSrc = 330;
		leftBtnTgt = 86;
		topBtnTgt = 516;
		leftBlnk = leftBtnTgt;
		topBlnk = topBtnTgt; 
		leftFillerSrc = 1970;
		topFillerSrc = 380;
		leftFillerTgt = 248;
		topFillerTgt = 566;
		break;
	case 3:
		left = 642;
		top = 517;
		topBtnSrc = 400;
		leftBtnTgt = 952;
		topBtnTgt = 516;
		leftBlnk =  1102;
		topBlnk = topBtnTgt; 
		leftFillerSrc = 1808;
		topFillerSrc = 450;
		leftFillerTgt = 952;
		topFillerTgt = 566;
		break;
	}

	if (mfdMode == MFD_NONE)
	{
		//make MFD transparent if shut off
		oapiBlt(surf, surf, left, top, 1670, 744, width, height);
		oapiBlt(surf, surf, leftBtnTgt, topBtnTgt, 1700, 750, widthBtn, heightBtn);
		oapiBlt (surf, surf, leftFillerTgt, topFillerTgt, 1700, 750, widthFiller, heightFiller);
		oapiBlt(surf, surf, leftBlnk, topBlnk, 1958, 134, widthBlnk, heightBlnk);
	}
	else
	{
		oapiBlt(surf, surf, left, top, 1670, 468, width, height);
		oapiBlt(surf, surf, leftBtnTgt, topBtnTgt, leftBtnSrc, topBtnSrc, widthBtn, heightBtn);
		oapiBlt(surf, surf, leftFillerTgt, topFillerTgt, leftFillerSrc, topFillerSrc, widthFiller, heightFiller);
	}
	return false;
}

bool MFDButtonRow::ProcessMouse2D (int event, int mx, int my)
{
	bool RetVal = false;
	if (mx % dx < 45) {
		int btn = mx/dx;
		if (MFDid == 1 || MFDid == 3)
			btn = 2 - btn;

		switch (btn) 
		{
		case 0:
			oapiToggleMFD_on (MFDid);
			oapiTriggerPanelRedrawArea(0, MYid);
			RetVal = true;
			break;
		case 1:
			oapiSendMFDKey (MFDid, OAPI_KEY_F1);
			RetVal = true;
			break;
		case 2:
			oapiSendMFDKey (MFDid, OAPI_KEY_GRAVE);
			RetVal = true;
			break;
		}
	} 

	return RetVal;
}

