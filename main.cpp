//*********************************************************************************************************//
//Signal simulation with CERN ROOT library
//Simulation of the signal at the output ADC and signal conversion.
//Created 04.08.2021
//Created by Novikov Dmitry
//*********************************************************************************************************//

/* *************************************************************************************************** */
/* ���� ������ ��� ������� ��������������� ��-��� ROOT !!! */
/* *************************************************************************************************** */

#include <iostream>
#include <mutex>
#include "ADCSignalSimulator.h"

#if defined WIN32 && defined _CONSOLE
#include "TApplication.h"
#endif

/* *************************************************************************************************** */
/* �������, ������������ ����� � �������� ��������� �������,
����������� �� ������� �� �� ��� ���������������� */
#define s16_Filtr(S32_oldVal, val, Kfiltr)	(S32_oldVal + ((((int)val<<16) - S32_oldVal +((1<<Kfiltr)-1)) >> Kfiltr))	/* s32 s16 s16 */
/* �������� ���������� �������� ������� ��� ������� ������� uSecTime */
double getSignalSample(long uSecTime, double freq, double amplitude, double offset)
{
	return amplitude * static_cast<double>(TMath::Sin(2 * static_cast<double>(TMath::Pi()) * uSecTime / 1000000 * freq)) + offset;
}
/* �������� ����� ������� ��� */
SignalGraphFrame::emulSignalsValues getADCSignals(long uSecTime, double sFreq, double sAmpl)
{

	const double signalOffset = 2048; //�������� ������� �� ��� Y, � �������� ���

	//������������� ������ - ���������� ��������
	int adcVal = static_cast<int>(getSignalSample(uSecTime, sFreq, sAmpl, signalOffset));
	//������������� ������ - ���������� �������� ����� �������
	static int Uab_out_val = static_cast<int>(signalOffset) << 16;
	Uab_out_val = s16_Filtr(Uab_out_val, adcVal, 1);
	//��������� ���������� ������������ � ������� �������
	static int Uab_out_mean = static_cast<int>(signalOffset) << 16;
	Uab_out_mean = s16_Filtr(Uab_out_mean, adcVal, 16);
	//����������� ����������� ������� � ����������
	int sampleVal = abs(Uab_out_val - Uab_out_mean) >> 16;
	//#1
	static int Uab_out_rms_0 = 0;
	Uab_out_rms_0 = s16_Filtr(Uab_out_rms_0, sampleVal, 8);
	//#2
	static int Uab_out_rms = 0;
	Uab_out_rms = s16_Filtr(Uab_out_rms, Uab_out_rms_0 >> 16, 8);

	//��������� ����������� �������� ������� � ��������� ��� �������� ����������
	SignalGraphFrame::emulSignalsValues signalValues;
	signalValues.first = adcVal;
	signalValues.second = sampleVal;
	signalValues.third = Uab_out_rms >> 16;

	/*int aTemp = Uab_out_val >> 16;
	int bTemp = Uab_out_mean >> 16;
	int cTemp = Uab_out_rms >> 16;
	std::cout << static_cast<double>(uSecTime) / 1000000 << " : " << adcVal << " : " << aTemp << " : " << bTemp << " : " << cTemp << std::endl;*/

	return signalValues;
}
/* *************************************************************************************************** */

//����������, ������ ���������� ��� ���������� .exe ��� ��� ������� ��-��� CERN ROOT
#if defined WIN32 && defined _CONSOLE
int main(int argc, char** argv)
#else
int main()
#endif
{
#if defined WIN32 && defined _CONSOLE
	/* ������� ���������� CERN ROOT - ������ ���� ������ ��� ���������� .exe*/
	TApplication theApp("App", &argc, argv);
#endif

	//��������, �������� � ��������� ��������� ���� ����������
	SignalGraphFrame* newFrame = nullptr;
	try
	{
		newFrame = new SignalGraphFrame(gClient->GetRoot(), 1024, 512, "ADC signal emulation", getADCSignals);
		if (newFrame == nullptr)
		{
			throw -1;
		}
	}
	catch (...)
	{
		std::cout << "Can't create Frame object!" << std::endl;
	}

#if defined WIN32 && defined _CONSOLE
	/* ������ ���������� CERN ROOT - ������ ���� ������ ��� ���������� .exe */
	theApp.Run();
#endif

	return 0;
}