#pragma once

//*********************************************************************************************************//
//Signal simulation with CERN ROOT library
//Simulation of the signal at the output ADC and signal conversion.
//SignalGraphFrame - main class for this application
//Created 04.08.2021
//Created by Novikov Dmitry
//*********************************************************************************************************//

#include <iostream>
#include <mutex>
#include <functional>

#include "TMath.h"
#include "TGClient.h"
#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"
#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGLabel.h"
#include "TCanvas.h"
#include "TH1I.h"
#include "TSystem.h"
#include "TDatime.h"
#include "TStyle.h"
#include "TGScrollBar.h"
#include "RQ_OBJECT.h"

/* class with ROOT Window object, his config and events handling */
class SignalGraphFrame
{
	RQ_OBJECT("SignalGraphFrame")

public:
	//struct for exchange signals values
	struct emulSignalsValues
	{
		int first, second, third;
	};

private:
	/* Parameters of signals */
	const double signalFreq = 50;				//signal frequency, Hz
	const double signalAmpl = 1000;				//signal amplitude, in ADC bits
	const double signalOffset = 2048;			//signal shift along the Y axis, in ADC bits
	/* and graphs */
	const int adcSampleTime = 100;				//time of one ADC sample, 탎
	const int adcSamplesInXCell = 10;			//number of ADC samples in one cell along the X axis
	const int adcMaxVal = 4096;					//maximum ADC value
	const int emulTime = 20000000;				//signals emulation process duration, 탎
	const long long timeStepDivider = 1000000;	//divisor to determine the quantization time step, 1 탎 = 1/1000000 s
	const int timeForXAxis = 120000;			//the duration of the X-axis of the chart in time, 탎 (when this time is reached, the chart is erased and redrawn,
												//the start(zero) point of the graph is shifted)
	const int adcMaxValDiv2 = static_cast<int>(signalOffset);	//maximum value of ADC / 2
	const double signalFreqMin = 0.1;			//minimum signal frequency allowed during emulation
	const double signalFreqMax = 250.0;			//maximum signal frequency allowed during emulation
	const double signalAmplMin = 0.0;			//minimum signal amplitude allowed during emulation
	const double signalAmplMax = 4096.0;		//maximum signal amplitude allowed during emulation

	//Main frame
	std::unique_ptr<TGMainFrame> fMain = nullptr;
	//Frame for canvas and canvas widget
	std::unique_ptr<TGHorizontalFrame> hCanvasFrame = nullptr;
	std::unique_ptr<TRootEmbeddedCanvas> fEcanvas = nullptr;
	//Horizontal frame
	std::unique_ptr<TGHorizontalFrame> hFrame = nullptr;
	std::unique_ptr<TGNumberEntry> signFreqNumberEntry = nullptr;
	std::unique_ptr<TGLabel> signFreqLabel = nullptr;
	std::unique_ptr<TGNumberEntry> signAmplNumberEntry = nullptr;
	std::unique_ptr<TGLabel> signAmplLabel = nullptr;
	std::unique_ptr<TGTextButton> buttonExit = nullptr;
	std::unique_ptr<TGTextButton> buttonRun = nullptr;
	std::unique_ptr<TGLabel> currentRMSVal = nullptr;
	std::unique_ptr<TGLabel> minRMSVal = nullptr;
	std::unique_ptr<TGLabel> maxRMSVal = nullptr;
	//Histogram object for ADC signal
	std::unique_ptr<TH1I> histADC = nullptr;
	//Histogram object for ADC signal with filter
	std::unique_ptr<TH1I> histADCFiltered = nullptr;
	//Histogram object for RMS signal
	std::unique_ptr<TH1I> histRMS = nullptr;

	//Mutex for run signal emul only once
	std::mutex emulRunOnceMutex;

	//parameters of the emulated signal
	double emulSignFreq = signalFreq;	//frequency, XXX.X Hz
	double emulSignAmpl = signalAmpl;	//amplitude, 0...ADCmax
	//Mutex for access to parametesrs of signal
	std::mutex emulSignParamsMutex;

	//waveform function
	//3 signals <- getSignalsSamples(long uSecTime, double freq, double amplitude)
	std::function<emulSignalsValues(long, double, double)> getSignalsValues;

public:

	//constr
	SignalGraphFrame(const TGWindow* wndPtr, UInt_t wndW, UInt_t wndH, const char* wndName,
		std::function<emulSignalsValues(long, double, double)> getSignalsFunc = nullptr)
	{
		if (!wndPtr || wndW == 0 || wndH == 0 || getSignalsFunc == nullptr)
		{
			throw - 1;
			return;
		}
		//save reference to get signals function
		getSignalsValues = getSignalsFunc;
		// Create a main frame
		try
		{
			fMain = std::make_unique<TGMainFrame>(wndPtr, wndW, wndH);
			if (fMain == nullptr)
			{
				throw - 1;
			}
		}
		catch (...)
		{
			throw - 1;
			return;
		}

		// Create a horizontal frame widget for canvas and add canvas widget
		try
		{
			hCanvasFrame = std::make_unique<TGHorizontalFrame>(fMain.get(), wndW, wndH - 48);
			if (hCanvasFrame == nullptr)
			{
				throw - 1;
			}
			fEcanvas = std::make_unique<TRootEmbeddedCanvas>(wndName, hCanvasFrame.get(), wndW, wndH - 52);
			if (fEcanvas == nullptr)
			{
				throw - 1;
			}
			hCanvasFrame->AddFrame(fEcanvas.get(), new TGLayoutHints(kLHintsExpandY, 2, 2, 2, 2));
			fMain->AddFrame(hCanvasFrame.get(), new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
		}
		catch (...)
		{
			throw - 1;
			return;
		}

		// Create a horizontal frame widget with buttons
		try
		{
			//frame
			hFrame = std::make_unique<TGHorizontalFrame>(fMain.get(), wndW, 40);
			if (hFrame == nullptr)
			{
				throw - 1;
			}
			//#1 - entry for input frequency
			signFreqNumberEntry = std::make_unique<TGNumberEntry>(hFrame.get(), emulSignFreq, 5, -1,
				TGNumberFormat::kNESRealOne, TGNumberFormat::kNEAPositive, TGNumberFormat::kNELLimitMinMax, signalFreqMin, signalFreqMax);
			if (signFreqNumberEntry == nullptr)
			{
				throw - 1;
			}
			signFreqNumberEntry->Connect("ValueSet(Long_t)", "SignalGraphFrame", this, "SetEmulSignalFrequency()");
			//(signFreqNumberEntry->GetNumberEntry())->Connect("ReturnPressed()", "SignalGraphFrame", this, "SetEmulSignalFrequency()");
			hFrame->AddFrame(signFreqNumberEntry.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#2 - label for entry input frequency
			signFreqLabel = std::make_unique<TGLabel>(hFrame.get(), "Frequency, Hz");
			if (signFreqLabel == nullptr)
			{
				throw - 1;
			}
			hFrame->AddFrame(signFreqLabel.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#3 - entry for input amplitude
			signAmplNumberEntry = std::make_unique<TGNumberEntry>(hFrame.get(), emulSignAmpl, 4, -1,
				TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive, TGNumberFormat::kNELLimitMinMax, signalAmplMin, signalAmplMax);
			if (signAmplNumberEntry == nullptr)
			{
				throw - 1;
			}
			signAmplNumberEntry->Connect("ValueSet(Long_t)", "SignalGraphFrame", this, "SetEmulSignalAmplitude()");
			//(signAmplNumberEntry->GetNumberEntry())->Connect("ReturnPressed()", "SignalGraphFrame", this, "SetEmulSignalAmplitude()");
			hFrame->AddFrame(signAmplNumberEntry.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#4 - label for entry input amplitude
			signAmplLabel = std::make_unique<TGLabel>(hFrame.get(), "Amplitude, ADC bits");
			if (signAmplLabel == nullptr)
			{
				throw - 1;
			}
			hFrame->AddFrame(signAmplLabel.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#5 - button: run
			buttonRun = std::make_unique<TGTextButton>(hFrame.get(), "&Run");
			if (buttonRun == nullptr)
			{
				throw - 1;
			}
			buttonRun->Connect("Clicked()", "SignalGraphFrame", this, "SignalEmulProcess()");
			hFrame->AddFrame(buttonRun.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#6 - button: exit
			buttonExit = std::make_unique<TGTextButton>(hFrame.get(), "&Exit", "gApplication->Terminate(0)");
			if (buttonExit == nullptr)
			{
				throw - 1;
			}
			hFrame->AddFrame(buttonExit.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#7 - label with current RMS value
			currentRMSVal = std::make_unique<TGLabel>(hFrame.get(), "RMS = 00000");
			if (currentRMSVal == nullptr)
			{
				throw - 1;
			}
			hFrame->AddFrame(currentRMSVal.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#8 - label with min RMS value
			minRMSVal = std::make_unique<TGLabel>(hFrame.get(), "min RMS = 00000");
			if (minRMSVal == nullptr)
			{
				throw - 1;
			}
			hFrame->AddFrame(minRMSVal.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#9 - label with max RMS value
			maxRMSVal = std::make_unique<TGLabel>(hFrame.get(), "max RMS = 00000");
			if (maxRMSVal == nullptr)
			{
				throw - 1;
			}
			hFrame->AddFrame(maxRMSVal.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#8 - add frame to main
			fMain->AddFrame(hFrame.get(), new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
		}
		catch (...)
		{
			throw - 1;
			return;
		}

		// Set a name to the main frame
		fMain->SetWindowName(wndName);
		// Map all subwindows of main frame
		fMain->MapSubwindows();
		// Initialize the layout algorithm
		fMain->Resize(fMain->GetDefaultSize());
		// Map main frame
		fMain->MapWindow();
	}

	//destr
	virtual ~SignalGraphFrame()
	{
	}

	//Process with ADC signal emulation
	void SignalEmulProcess()
	{
		//trying get access to function
		std::unique_lock<std::mutex> emulRunOnceLock(emulRunOnceMutex, std::defer_lock);
		if (!emulRunOnceLock.try_lock())
		{
			std::cout << "Signal emulation process already working!" << std::endl;
			return;
		}

		//get canvas
		if (fEcanvas == nullptr)
		{
			throw - 2;
			return;
		}
		TCanvas* graphCanvas = fEcanvas->GetCanvas();
		graphCanvas->SetGrid();

		//create histogram object for ADC signal
		try
		{
			/* name;
			hist signatures;
			samples number in one X axis cell;
			time of first sample in one X axis cell;
			time of last sample in one X axis cell
			*/
			histADC = std::make_unique<TH1I>("histADC",
				"ADC voltage emulation; time; val",
				adcSamplesInXCell,
				0.0,
				static_cast<double>(adcSampleTime * adcSamplesInXCell) / static_cast<double>(timeStepDivider));
			if (histADC == nullptr)
			{
				throw - 2;
			}
			histADC->SetMaximum(adcMaxValDiv2);
			histADC->SetMinimum(-adcMaxValDiv2);
			histADC->SetStats(0);
			histADC->SetLineColor(2);
			histADC->GetXaxis()->SetTimeDisplay(1);
			histADC->GetYaxis()->SetNdivisions(512);
			histADC->Draw("SAME ][");
		}
		catch (...)
		{
			std::cout << "Can't create histogram object!" << std::endl;
			throw - 2;
			return;
		}

		//create histogram object for ADC signal with filter
		try
		{
			histADCFiltered = std::make_unique<TH1I>("histADC - filtered",
				"ADC filtered; time; val",
				adcSamplesInXCell,
				0.0,
				static_cast<double>(adcSampleTime * adcSamplesInXCell) / static_cast<double>(timeStepDivider));
			if (histADCFiltered == nullptr)
			{
				throw - 2;
			}
			histADCFiltered->SetMaximum(adcMaxValDiv2);
			histADCFiltered->SetMinimum(-adcMaxValDiv2);
			histADCFiltered->SetStats(0);
			histADCFiltered->SetLineColor(4);
			histADCFiltered->GetXaxis()->SetTimeDisplay(1);
			histADCFiltered->GetYaxis()->SetNdivisions(512);
			histADCFiltered->Draw("SAME ][");
		}
		catch (...)
		{
			std::cout << "Can't create histogram object!" << std::endl;
			throw - 2;
			return;
		}

		//create histogram object for RMS signal
		try
		{
			histRMS = std::make_unique<TH1I>("histRMS",
				"RMS; time; val",
				adcSamplesInXCell,
				0.0,
				static_cast<double>(adcSampleTime * adcSamplesInXCell) / static_cast<double>(timeStepDivider));
			if (histRMS == nullptr)
			{
				throw - 2;
			}
			histRMS->SetMaximum(adcMaxValDiv2);
			histRMS->SetMinimum(-adcMaxValDiv2);
			histRMS->SetStats(0);
			histRMS->SetLineColor(3);
			histRMS->GetXaxis()->SetTimeDisplay(1);
			histRMS->GetYaxis()->SetNdivisions(512);
			histRMS->Draw("SAME ][");
		}
		catch (...)
		{
			std::cout << "Can't create histogram object!" << std::endl;
			throw - 2;
			return;
		}

		//create lock for parameters of signal
		std::unique_lock<std::mutex> emulSignParamsLock(emulSignParamsMutex, std::defer_lock);
		//min and max RMS values
		int minRMS = 10000;
		int maxRMS = -1;
		//draw signals
		for (long timeUSec = 0, i = 0; timeUSec < emulTime; timeUSec += adcSampleTime, i++)
		{
			//reset view to X = 0 and reset variables if reach to edge
			if (timeUSec % timeForXAxis == 0)
			{
				i = 0;
				gStyle->SetTimeOffset(static_cast<double>(timeUSec) / static_cast<double>(timeStepDivider));
				histADC->Reset();
				histADCFiltered->Reset();
				histRMS->Reset();
				minRMS = 10000;
				maxRMS = -1;
			}
			//calc signal samples
			emulSignalsValues signalsVal;
			emulSignParamsLock.lock();
			if (getSignalsValues != nullptr)
			{
				signalsVal = getSignalsValues(timeUSec, emulSignFreq, emulSignAmpl);
			}
			else
			{
				throw -1;
			}		
			emulSignParamsLock.unlock();
			//update min and max
			if (minRMS > signalsVal.third)
			{
				minRMS = signalsVal.third;
			}
			if (maxRMS < signalsVal.third)
			{
				maxRMS = signalsVal.third;
			}
			//update graphics
			histADC->SetBinContent(i, static_cast<double>(signalsVal.first) - static_cast<double>(adcMaxValDiv2));
			histADCFiltered->SetBinContent(i, static_cast<double>(signalsVal.second));
			histRMS->SetBinContent(i, static_cast<double>(signalsVal.third));
			//current RMS
			TGString textStr = "RMS = ";
			textStr += signalsVal.third;
			currentRMSVal->SetText(textStr);
			//min RMS
			textStr = "min RMS = ";
			textStr += minRMS;
			minRMSVal->SetText(textStr);
			//max RMS
			textStr = "max RMS = ";
			textStr += maxRMS;
			maxRMSVal->SetText(textStr);
			//update canvas
			graphCanvas->Modified();
			graphCanvas->Update();
			//update system messages
			gSystem->ProcessEvents(); //canvas can be edited during the loop
		}
		//free and exit;
		histRMS.reset();
		histRMS = nullptr;
		histADCFiltered.reset();
		histADCFiltered = nullptr;
		histADC.reset();
		histADC = nullptr;
	}

	//Set new frequency for signal
	void SetEmulSignalFrequency()
	{
		//try access
		std::lock_guard<std::mutex> emulSignParamsLock(emulSignParamsMutex);
		//set new
		emulSignFreq = static_cast<double>(signFreqNumberEntry->GetNumberEntry()->GetIntNumber()) * 0.1;
		std::cout << "New F = " << emulSignFreq << std::endl;
	}

	//Set new amplitude for signal
	void SetEmulSignalAmplitude()
	{
		//try access
		std::lock_guard<std::mutex> emulSignParamsLock(emulSignParamsMutex);
		//set new
		emulSignAmpl = signAmplNumberEntry->GetNumberEntry()->GetIntNumber();
		std::cout << "New A = " << emulSignAmpl << std::endl;
	}
};
