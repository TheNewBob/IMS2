// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// Instrument.h
// Base class for panel and VC instrument visualisations
// ==============================================================


#pragma once
//#ifndef __INSTRUMENT_H
//#define __INSTRUMENT_H

class VESSEL3;

// ==============================================================

class PanelElement {
public:
	PanelElement (VESSEL3 *v);
	virtual ~PanelElement ();

	virtual void AddMeshData2D (MESHHANDLE hMesh, DWORD grpidx) {}
	virtual void AddMeshDataVC (MESHHANDLE hMesh, DWORD grpidx) {}
	virtual void Reset2D ();
	virtual bool Redraw2D (SURFHANDLE surf);
	virtual bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	virtual bool ProcessMouse2D (int event, int mx, int my);
	virtual bool ProcessMouseVC (int event, VECTOR3 &p);
	virtual bool RedrawBlank(SURFHANDLE surf);

protected:
	void AddGeometry (MESHHANDLE hMesh, DWORD grpidx, const NTVERTEX *vtx, DWORD nvtx, const WORD *idx, DWORD nidx);

	char *DispStr (double dist, int precision=4);

	VESSEL3 *vessel;
	MESHHANDLE mesh;
	DWORD gidx;
	MESHGROUP *grp; // panel mesh group representing the instrument
	DWORD vtxofs;   // vertex offset in mesh group
};


class MFDButtonCol: public PanelElement {
public:
  MFDButtonCol (VESSEL3 *v, DWORD _lr, DWORD _xcnt, DWORD _ytop, int _MFDid);
  bool Redraw2D (SURFHANDLE surf);
  bool ProcessMouse2D (int event, int mx, int my);

private:
  DWORD lr;   // left (0) or right (1) button column
  DWORD xcnt; // central axis of button column in texture
  DWORD ytop; // top edge of label in topmost button
  DWORD dy;   // vertical button spacing
  int MFDid;  //Identifier of the MFD	
  int mode;	//stores MFD mode to recognize changes.
};

class MFDButtonRow: public PanelElement {
public:
  MFDButtonRow (VESSEL3 *v, DWORD _ycnt, DWORD _xleft, int _MFDid, int _MYid);
  bool Redraw2D (SURFHANDLE surf);
  bool ProcessMouse2D (int event, int mx, int my);
  bool RedrawBlank(SURFHANDLE surf);	
private:
  DWORD ycnt; // central axis of button column in texture
  DWORD xleft; // left edge of leftmost button 
  DWORD dx;   // horizontal button spacing
  int MFDid;  //Identifier of the MFD	
  int MYid;		
  int mode;	//stores MFD mode to recognize changes.
  bool firstCall;		//used to identify if Redraw is called for first time
};

const int MFD_font_width = 14;
const int MFD_font_ypos = 20;
const int MFD_font_height = 21;


// ==============================================================

//#endif // !__INSTRUMENT_H