#pragma once

#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>

class MyMainFrame
{
	RQ_OBJECT("MyMainFrame")
private:
	TGMainFrame* fMain;
	TRootEmbeddedCanvas* fEcanvas;
public:
	MyMainFrame(const TGWindow* p, UInt_t w, UInt_t h);
	virtual ~MyMainFrame();
	void DoDraw();
};
