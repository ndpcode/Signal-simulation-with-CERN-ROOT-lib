//
//#define _USE_MATH_DEFINES
//
//#include <iostream>
//#include <math.h>
//
////need to run cern root in VS
//#include <TApplication.h>
//
//#include "TSystem.h"
//#include "TCanvas.h"
//#include "TH1I.h"
//#include "THStack.h"
//
//#include "THistPainter.h"
//#include "TROOT.h"
//#include "TGview.h"
//#include "TGWindow.h"
//#include "TGScrollBar.h"
//#include "TFrame.h"
//#include "TButton.h"
//#include <TGClient.h>
//
//#include "TStopwatch.h"
//#include "TDatime.h"
//#include "TStyle.h"
//#include "TRandom.h"
//
///* Параметры эмулируемого сигнала */
//double signalFreq = 50;				//частота сигнала, Гц
//double signalAmpl = 1000;			//амплитуда сигнала, в разрядах АЦП
//const double signalOffset = 2048;			//смещение сигнала по оси Y, в разрядах АЦП
////параметры для отрисовки графиков
//const int adcSampleTime = 100;				//время одной выборки АЦП, мкс
//const int adcSamplesInXCell = 10;			//количество выборок АЦП в одной клетке по оси Х
//const int adcMaxVal = 4096;					//максимальное значение АЦП
//const int emulTime = 2000000;				//длительность эмуляции сигнала, мкс
//const int adcMaxValDiv2 = static_cast<int>(signalOffset);	//максимальное значение АЦП / 2
///* --- */
//
//#define s16_Filtr(S32_oldVal, val, Kfiltr)	(S32_oldVal + ((((int)val<<16) - S32_oldVal +((1<<Kfiltr)-1)) >> Kfiltr))	/* s32 s16 s16 */
//
///* получить мгновенное значение сигнала для момента времени uSecTime */
//double getSignalSample(long uSecTime, double freq, double amplitude, double offset)
//{
//	return amplitude * sin(2 * M_PI * uSecTime / 1000000 * freq) + offset;
//}
//
///* эмуляция одной выборки АЦП */
////структура для хранения значений выборки АЦП
//struct emulSignalValues
//{
//	int adc, filt, rms;
//};
//emulSignalValues adcInt(long uSecTime)
//{
//	//эмулированный сигнал - мгновенное значение
//	int adcVal = static_cast<int>(getSignalSample(uSecTime, signalFreq, signalAmpl, signalOffset));
//	//эмулированный сигнал - мгновенное значение после фильтра
//	static int Uab_out_val = static_cast<int>(signalOffset) << 16;
//	Uab_out_val = s16_Filtr(Uab_out_val, adcVal, 1);
//	//получение постоянной составляющей с помощью фильтра
//	static int Uab_out_mean = static_cast<int>(signalOffset) << 16;
//	Uab_out_mean = s16_Filtr(Uab_out_mean, adcVal, 16);
//	//выпрямление переменного сигнала в постоянный
//	int sampleVal = abs(Uab_out_val - Uab_out_mean) >> 16;
//	static int Uab_out_rms = 0;
//	Uab_out_rms = s16_Filtr(Uab_out_rms, sampleVal, 12);
//	
//	//сохранить вычисленные значения сигнала в структуру для возврата результата
//	emulSignalValues signalValues;
//	signalValues.adc = adcVal;
//	signalValues.filt = sampleVal;
//	signalValues.rms = Uab_out_rms >> 16;
//
//	/*int aTemp = Uab_out_val >> 16;
//	int bTemp = Uab_out_mean >> 16;
//	int cTemp = Uab_out_rms >> 16;
//	std::cout << static_cast<double>(uSecTime) / 1000000 << " : " << adcVal << " : " << aTemp << " : " << bTemp << " : " << cTemp << std::endl;*/
//
//	return signalValues;
//}
//
///* Отрисовка графиков сигналов */
//void drawSignalsHistograms()
//{
//	//create canvas
//	TCanvas* graphCanvas = nullptr;
//	try
//	{
//		graphCanvas = new TCanvas("ADC voltage emulation", "ADC voltage emulation", 50, 50, 1024, 512);
//	}
//	catch (...)
//	{
//		std::cout << "Can't create graph canvas object!" << std::endl;
//		return;
//	}
//	graphCanvas->SetGrid();
//
//
//	//create histogram object for ADC signal
//	TH1I* histADC = nullptr;
//	try
//	{
//		/* name;
//		hist signatures;
//		samples number in one X axis cell;
//		time of first sample in one X axis cell;
//		time of last sample in one X axis cell
//		*/
//		histADC = new TH1I("histADC",
//			"ADC voltage emulation; time; val",
//			adcSamplesInXCell,
//			0.0,
//			static_cast<double>(adcSampleTime * adcSamplesInXCell) / 1000000.0);
//	}
//	catch (...)
//	{
//		std::cout << "Can't create histogram object!" << std::endl;
//		return;
//	}
//	histADC->SetMaximum(adcMaxValDiv2);
//	histADC->SetMinimum(-adcMaxValDiv2);
//	histADC->SetStats(0);
//	histADC->SetLineColor(2);
//	histADC->GetXaxis()->SetTimeDisplay(1);
//	histADC->GetYaxis()->SetNdivisions(512);
//	histADC->Draw("SAME ][");
//
//	//create histogram object for ADC signal with filter
//	TH1I* histADCFiltered = nullptr;
//	try
//	{
//		histADCFiltered = new TH1I("histADC - filtered",
//			"ADC filtered; time; val",
//			adcSamplesInXCell,
//			0.0,
//			static_cast<double>(adcSampleTime * adcSamplesInXCell) / 1000000.0);
//	}
//	catch (...)
//	{
//		std::cout << "Can't create histogram object!" << std::endl;
//		return;
//	}
//	histADCFiltered->SetMaximum(adcMaxValDiv2);
//	histADCFiltered->SetMinimum(-adcMaxValDiv2);
//	histADCFiltered->SetStats(0);
//	histADCFiltered->SetLineColor(4);
//	histADCFiltered->GetXaxis()->SetTimeDisplay(1);
//	histADCFiltered->GetYaxis()->SetNdivisions(512);
//	histADCFiltered->Draw("SAME ][");
//
//	//create histogram object for RMS signal
//	TH1I* histRMS = nullptr;
//	try
//	{
//		histRMS = new TH1I("histRMS",
//			"RMS; time; val",
//			adcSamplesInXCell,
//			0.0,
//			static_cast<double>(adcSampleTime * adcSamplesInXCell) / 1000000.0);
//	}
//	catch (...)
//	{
//		std::cout << "Can't create histogram object!" << std::endl;
//		return;
//	}
//	histRMS->SetMaximum(adcMaxValDiv2);
//	histRMS->SetMinimum(-adcMaxValDiv2);
//	histRMS->SetStats(0);
//	histRMS->SetLineColor(3);
//	histRMS->GetXaxis()->SetTimeDisplay(1);
//	histRMS->GetYaxis()->SetNdivisions(512);
//	histRMS->Draw("SAME ][");
//	
//	//create button for exit
//	TButton* but1 = new TButton("Exit", "gApplication->Terminate(0)", .01, .94, .06, .99);
//	but1->Draw();
//	//
//	//TButton* but2 = new TButton("Ampl+", &amplUp, .07, .94, .12, .99);
//	//but2->Draw();
//	
//	//draw signals
//	for (long timeUSec = 0, i = 0; timeUSec < emulTime; timeUSec += adcSampleTime, i++)
//	{
//		//calc signal samples
//		emulSignalValues signalVal = adcInt(timeUSec);
//		if (timeUSec % 200000 == 0)
//		{
//			graphCanvas->SetCanvasSize((timeUSec/200000 + 1) * 1024, 512);
//		}
//		histADC->SetBinContent(i, static_cast<double>(signalVal.adc) - static_cast<double>(adcMaxValDiv2));
//		histADCFiltered->SetBinContent(i, static_cast<double>(signalVal.filt));
//		histRMS->SetBinContent(i, static_cast<double>(signalVal.rms));
//		graphCanvas->Modified();
//		graphCanvas->Update();
//		gSystem->ProcessEvents(); //canvas can be edited during the loop
//	}
//}
//
//#include <RQ_OBJECT.h>
//
//class ExtCanvas : public TCanvas
//{
//	RQ_OBJECT("ExtCanvas")
//	private:
//		TButton* buttonExit = nullptr;
//		TButton* testButton = nullptr;
//	public:
//		ExtCanvas(const char* name, const char* title, int wtopx, int wtopy, int ww, int wh) :
//			TCanvas(name, title, wtopx, wtopy, ww, wh)
//		{
//			//create button for exit
//			buttonExit = new TButton("Exit", "gApplication->Terminate(0)", .01, .94, .06, .99);
//			buttonExit->Draw();
//
//			testButton = new TButton("Test", "testButtonClick()", .07, .94, .12, .99);
//			testButton->Draw();
//		}
//		virtual ~ExtCanvas()
//		{
//			if (testButton != nullptr)
//			{
//				delete testButton;
//			}
//			if (buttonExit != nullptr)
//			{
//				delete buttonExit;
//			}
//		}
//
//		void testButtonClick()
//		{
//
//		}
//};
//
//#include <TGClient.h>
//#include <TCanvas.h>
//#include <TF1.h>
//#include <TRandom.h>
//#include <TGButton.h>
//#include <TGFrame.h>
//#include <TRootEmbeddedCanvas.h>
//#include <RQ_OBJECT.h>
//
//class MyMainFrame
//{
//	RQ_OBJECT("MyMainFrame")
//private:
//	TGMainFrame* fMain;
//	TRootEmbeddedCanvas* fEcanvas;
//public:
//	MyMainFrame(const TGWindow* p, UInt_t w, UInt_t h);
//	virtual ~MyMainFrame();
//	void DoDraw();
//};
//
//MyMainFrame::MyMainFrame(const TGWindow* p, UInt_t w, UInt_t h) {
//	// Create a main frame
//	fMain = new TGMainFrame(p, w, h);
//
//	// Create canvas widget
//	fEcanvas = new TRootEmbeddedCanvas("Ecanvas", fMain, 200, 200);
//	fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX |
//		kLHintsExpandY, 10, 10, 10, 1));
//	// Create a horizontal frame widget with buttons
//	TGHorizontalFrame* hframe = new TGHorizontalFrame(fMain, 200, 40);
//	TGTextButton* draw = new TGTextButton(hframe, "&Draw");
//	draw->Connect("Clicked()", "MyMainFrame", this, "DoDraw()");
//	hframe->AddFrame(draw, new TGLayoutHints(kLHintsCenterX,
//		5, 5, 3, 4));
//	TGTextButton* exit = new TGTextButton(hframe, "&Exit",
//		"gApplication->Terminate(0)");
//	hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,
//		5, 5, 3, 4));
//	fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,
//		2, 2, 2, 2));
//
//	// Set a name to the main frame
//	fMain->SetWindowName("Simple Example");
//
//	// Map all subwindows of main frame
//	fMain->MapSubwindows();
//
//	// Initialize the layout algorithm
//	fMain->Resize(fMain->GetDefaultSize());
//
//	// Map main frame
//	fMain->MapWindow();
//}
//void MyMainFrame::DoDraw() {
//	// Draws function graphics in randomly chosen interval
//	TF1* f1 = new TF1("f1", "sin(x)/x", 0, gRandom->Rndm() * 10);
//	f1->SetLineWidth(3);
//	f1->Draw();
//	TCanvas* fCanvas = fEcanvas->GetCanvas();
//	fCanvas->cd();
//	fCanvas->Update();
//}
//MyMainFrame::~MyMainFrame() {
//	// Clean up used widgets: frames, buttons, layout hints
//	fMain->Cleanup();
//	delete fMain;
//}
//
//int main(int argc, char** argv)
//{
//	/* create cern root app */
//	TApplication theApp("App", &argc, argv);
//
//	/* run user code */
//	//drawSignalsHistograms();
//	//new ExtCanvas("Test name", "Test title", 50, 50, 1024, 512);
//	new MyMainFrame(gClient->GetRoot(), 200, 200);
//
//	/* run cern root app */
//	theApp.Run();
//	
//	return 0;
//}

/* *************************************************************************************************** */
/* НИЖЕ ВЕРСИЯ ДЛЯ ЗАПУСКА НЕПОСРЕДСТВЕННО ИЗ-ПОД ROOT !!! */
/* *************************************************************************************************** */

#include <iostream>
#include <mutex>

#if defined WIN32 && defined _CONSOLE
#include "TApplication.h"
#endif
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
#include "RQ_OBJECT.h"

#include "TGScrollBar.h"

/* Параметры эмулируемого сигнала */
double signalFreq = 50;				//частота сигнала, Гц
double signalAmpl = 1000;			//амплитуда сигнала, в разрядах АЦП
const double signalOffset = 2048;			//смещение сигнала по оси Y, в разрядах АЦП
//параметры для отрисовки графиков
const int adcSampleTime = 100;				//время одной выборки АЦП, мкс
const int adcSamplesInXCell = 10;			//количество выборок АЦП в одной клетке по оси Х
const int adcMaxVal = 4096;					//максимальное значение АЦП
const int emulTime = 20000000;				//длительность эмуляции сигнала, мкс
const long long timeStepDivider = 1000000;	//делитель для определения шага времени квантования, 1 мкс = 1/1000000 с
const int timeForXAxis = 120000;			//длительность оси Х графика по времени, мкс (по достижении этого времени график стирается и рисуется заново,
											//нулевая точка графика сдвигается)
const int adcMaxValDiv2 = static_cast<int>(signalOffset);	//максимальное значение АЦП / 2
const double signalFreqMin = 0.1;			//минимальная частота сигнала, допустимая при эмуляции
const double signalFreqMax = 250.0;			//максимальная частота сигнала, допустимая при эмуляции
const double signalAmplMin = 0.0;			//минимальная амплитуда сигнала, допустимая при эмуляции
const double signalAmplMax = 4096.0;		//максимальная амплитуда сигнала, допустимая при эмуляции
/* --- */

#define s16_Filtr(S32_oldVal, val, Kfiltr)	(S32_oldVal + ((((int)val<<16) - S32_oldVal +((1<<Kfiltr)-1)) >> Kfiltr))	/* s32 s16 s16 */

/* получить мгновенное значение сигнала для момента времени uSecTime */
double getSignalSample(long uSecTime, double freq, double amplitude, double offset)
{
	return amplitude * static_cast<double>(TMath::Sin(2 * static_cast<double>(TMath::Pi()) * uSecTime / 1000000 * freq)) + offset;
}

/* эмуляция одной выборки АЦП */
//структура для хранения значений выборки АЦП
struct emulSignalValues
{
	int adc, filt, rms;
};
emulSignalValues adcInt(long uSecTime, double sFreq, double sAmpl)
{
	//эмулированный сигнал - мгновенное значение
	int adcVal = static_cast<int>(getSignalSample(uSecTime, sFreq, sAmpl, signalOffset));
	//эмулированный сигнал - мгновенное значение после фильтра
	static int Uab_out_val = static_cast<int>(signalOffset) << 16;
	Uab_out_val = s16_Filtr(Uab_out_val, adcVal, 1);
	//получение постоянной составляющей с помощью фильтра
	static int Uab_out_mean = static_cast<int>(signalOffset) << 16;
	Uab_out_mean = s16_Filtr(Uab_out_mean, adcVal, 16);
	//выпрямление переменного сигнала в постоянный
	int sampleVal = abs(Uab_out_val - Uab_out_mean) >> 16;
	//#1
	static int Uab_out_rms_0 = 0;
	Uab_out_rms_0 = s16_Filtr(Uab_out_rms_0, sampleVal, 8);
	//#2
	static int Uab_out_rms = 0;
	Uab_out_rms = s16_Filtr(Uab_out_rms, Uab_out_rms_0 >> 16, 8);

	//сохранить вычисленные значения сигнала в структуру для возврата результата
	emulSignalValues signalValues;
	signalValues.adc = adcVal;
	signalValues.filt = sampleVal;
	signalValues.rms = Uab_out_rms >> 16;

	/*int aTemp = Uab_out_val >> 16;
	int bTemp = Uab_out_mean >> 16;
	int cTemp = Uab_out_rms >> 16;
	std::cout << static_cast<double>(uSecTime) / 1000000 << " : " << adcVal << " : " << aTemp << " : " << bTemp << " : " << cTemp << std::endl;*/

	return signalValues;
}

class SignalGraphFrame
{
	RQ_OBJECT("SignalGraphFrame")

private:
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

public:
	//constr
	SignalGraphFrame(const TGWindow* wndPtr, UInt_t wndW, UInt_t wndH, const char* wndName)
	{
		if (!wndPtr || wndW == 0 || wndH == 0)
		{
			throw - 1;
			return;
		}
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
				throw -1;
			}
			fEcanvas = std::make_unique<TRootEmbeddedCanvas>(wndName, hCanvasFrame.get(), wndW, wndH - 52);
			if (fEcanvas == nullptr)
			{
				throw -1;
			}
			hCanvasFrame->AddFrame(fEcanvas.get(), new TGLayoutHints(kLHintsExpandY, 2, 2, 2, 2));
			fMain->AddFrame(hCanvasFrame.get(), new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
		}
		catch (...)
		{
			throw -1;
			return;
		}
		
		// Create a horizontal frame widget with buttons
		try
		{
			//frame
			hFrame = std::make_unique<TGHorizontalFrame>(fMain.get(), wndW, 40);
			if (hFrame == nullptr)
			{
				throw -1;
			}
			//#1 - entry for input frequency
			signFreqNumberEntry = std::make_unique<TGNumberEntry>(hFrame.get(), emulSignFreq, 5, -1,
				TGNumberFormat::kNESRealOne, TGNumberFormat::kNEAPositive, TGNumberFormat::kNELLimitMinMax, signalFreqMin, signalFreqMax);
			if (signFreqNumberEntry == nullptr)
			{
				throw -1;
			}
			signFreqNumberEntry->Connect("ValueSet(Long_t)", "SignalGraphFrame", this, "SetEmulSignalFrequency()");
			//(signFreqNumberEntry->GetNumberEntry())->Connect("ReturnPressed()", "SignalGraphFrame", this, "SetEmulSignalFrequency()");
			hFrame->AddFrame(signFreqNumberEntry.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#2 - label for entry input frequency
			signFreqLabel = std::make_unique<TGLabel>(hFrame.get(), "Frequency, Hz");
			if (signFreqLabel == nullptr)
			{
				throw -1;
			}
			hFrame->AddFrame(signFreqLabel.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#3 - entry for input amplitude
			signAmplNumberEntry = std::make_unique<TGNumberEntry>(hFrame.get(), emulSignAmpl, 4, -1,
				TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive, TGNumberFormat::kNELLimitMinMax, signalAmplMin, signalAmplMax);
			if (signAmplNumberEntry == nullptr)
			{
				throw -1;
			}
			signAmplNumberEntry->Connect("ValueSet(Long_t)", "SignalGraphFrame", this, "SetEmulSignalAmplitude()");
			//(signAmplNumberEntry->GetNumberEntry())->Connect("ReturnPressed()", "SignalGraphFrame", this, "SetEmulSignalAmplitude()");
			hFrame->AddFrame(signAmplNumberEntry.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#4 - label for entry input amplitude
			signAmplLabel = std::make_unique<TGLabel>(hFrame.get(), "Amplitude, ADC bits");
			if (signAmplLabel == nullptr)
			{
				throw -1;
			}
			hFrame->AddFrame(signAmplLabel.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#5 - button: run
			buttonRun = std::make_unique<TGTextButton>(hFrame.get(), "&Run");
			if (buttonRun == nullptr)
			{
				throw -1;
			}
			buttonRun->Connect("Clicked()", "SignalGraphFrame", this, "SignalEmulProcess()");
			hFrame->AddFrame(buttonRun.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#6 - button: exit
			buttonExit = std::make_unique<TGTextButton>(hFrame.get(), "&Exit", "gApplication->Terminate(0)");
			if (buttonExit == nullptr)
			{
				throw -1;
			}
			hFrame->AddFrame(buttonExit.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#7 - label with current RMS value
			currentRMSVal = std::make_unique<TGLabel>(hFrame.get(), "RMS = 00000");
			if (currentRMSVal == nullptr)
			{
				throw -1;
			}
			hFrame->AddFrame(currentRMSVal.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#8 - label with min RMS value
			minRMSVal = std::make_unique<TGLabel>(hFrame.get(), "min RMS = 00000");
			if (minRMSVal == nullptr)
			{
				throw -1;
			}
			hFrame->AddFrame(minRMSVal.get(), new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
			//#9 - label with max RMS value
			maxRMSVal = std::make_unique<TGLabel>(hFrame.get(), "max RMS = 00000");
			if (maxRMSVal == nullptr)
			{
				throw -1;
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
			throw -2;
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
				throw -2;
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
			throw -2;
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
				throw -2;
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
			throw -2;
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
				throw -2;
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
			throw -2;
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
			emulSignParamsLock.lock();
			emulSignalValues signalVal = adcInt(timeUSec, emulSignFreq, emulSignAmpl);
			emulSignParamsLock.unlock();
			//update min and max
			if (minRMS > signalVal.rms)
			{
				minRMS = signalVal.rms;
			}
			if (maxRMS < signalVal.rms)
			{
				maxRMS = signalVal.rms;
			}
			//update graphics
			histADC->SetBinContent(i, static_cast<double>(signalVal.adc) - static_cast<double>(adcMaxValDiv2));
			histADCFiltered->SetBinContent(i, static_cast<double>(signalVal.filt));
			histRMS->SetBinContent(i, static_cast<double>(signalVal.rms));
			//current RMS
			TGString textStr = "RMS = ";
			textStr += signalVal.rms;
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

//detect what is building under VS
#if defined WIN32 && defined _CONSOLE
int main(int argc, char** argv)
#else
int main()
#endif
{
#if defined WIN32 && defined _CONSOLE
	/* create cern root app */
	TApplication theApp("App", &argc, argv);
#endif

	/* run user code */
	//new MyMainFrame(gClient->GetRoot(), 200, 200);
	SignalGraphFrame* newFrame = nullptr;
	try
	{
		newFrame = new SignalGraphFrame(gClient->GetRoot(), 1024, 512, "ADC signal emulation");
	}
	catch (...)
	{
		std::cout << "Can't create Frame object!" << std::endl;
	}

#if defined WIN32 && defined _CONSOLE
	/* run cern root app */
	theApp.Run();
#endif

	return 0;
}