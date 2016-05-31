#include "phaseDetector.h"
#include "main.h"
#include <math.h>

#define MY_PI 3.14159265358979323846

void getSpectrum(vector<Mat> *frames, vector<double> *outputSpectrum)
{
	Mat forehead, foreheadConverted;
	vector<double> means;
	vector<double> normalized;
	vector<double> complexSpectrum;
	vector<double> ampSpectrum;

	Scalar meansScalar;
	double totalMean;
	double sum;
	double minVal;
	double maxVal;
	int i;
	sum = 0;
	minVal = 266;
	maxVal = -1;

	//cout << "***********************" << endl;
	for (i = 0; i < frames->size(); i++)
	{
		/* Get forehead from vector */
		forehead = (*frames)[i];

		/* Convert from RGB to YCbCr */
		cvtColor(forehead, foreheadConverted, CV_BGR2YCrCb);

		/* Calculate mean of forehead frame */
		meansScalar = mean(foreheadConverted);
		totalMean = (meansScalar[0] + meansScalar[1] + meansScalar[2]) / 3;

		/* Save mean value to a vector */
		means.push_back(totalMean);

		/* Update sum of means */
		sum += totalMean;

		/* Track min & max */
		if (totalMean < minVal) minVal = totalMean;
		if (totalMean > maxVal) maxVal = totalMean;
	}

	minVal /= sum;
	maxVal /= sum;

	for (i = 0; i < frames->size(); i++)
	{
		double partiallyNormalized = means[i] / sum;
		normalized.push_back((partiallyNormalized - minVal) / (maxVal - minVal));
	}

	/* Subtract the mean of the signal from the signal */
	sum = 0;
	for (i = 0; i < normalized.size(); i++)
	{
		sum += normalized[i];
	}
	double ourMean;
	ourMean = sum / normalized.size();
	for (i = 0; i < normalized.size(); i++)
	{
		normalized[i] -= ourMean;
	}

	/* Pad the signal with zeros until the total DFT length */
	for (i = 0; i < TOTAL_DFT_WINDOW - frames->size(); i++)
	{
		normalized.push_back(0);
	}

	/* Get amplitude spectrum */
	dft(normalized, *outputSpectrum);
}

void convertToPhase(vector<double> *inputSpectrum, vector<double> *outputPhaseSpectrum)
{
	int N = (int) (inputSpectrum->size());

	/* The phase of DC component is zero*/
	outputPhaseSpectrum->push_back(0);

	double real, imag;
	double phase;

	int i = 1;
	while (i < N - 2)
	{
		/* Get real and imaginary components of a frequency */
		real = (*inputSpectrum)[i];
		imag = (*inputSpectrum)[i + 1];

		/* Calculate phase from the complex number */
		phase = atan2(imag, real);

		/* Add it to output vector */
		outputPhaseSpectrum->push_back(phase);

		/* Get next pair */
		i += 2;
	}
}

double PhaseDetector::detect(vector<Mat>* foreheads, vector<Mat>* palms, double freq)
{
	vector<double> foreheadSpectrum;
	vector<double> palmSpectrum;
	vector<double> foreheadPhases;
	vector<double> palmPhases;

	int freqInd;
	double phaseDifference;
	double pulseTransitTime;

	/* Get spectrums from videos of forehead and palm */
	getSpectrum(foreheads, &foreheadSpectrum);
	getSpectrum(palms, &palmSpectrum);
	
	/* Convert imaginary spectrums to phase spectrums */
	convertToPhase(&foreheadSpectrum, &foreheadPhases);
	convertToPhase(&palmSpectrum, &palmPhases);

	/* Get spectrum index from target frequency */
	freqInd = (int) round(freq * 2 * foreheadPhases.size() / SAMPLING_FREQUENCY);

	phaseDifference = palmPhases[freqInd] - foreheadPhases[freqInd];
	if (phaseDifference < 0) phaseDifference += 2 * MY_PI;

	/* Calculate PTT */
	pulseTransitTime = phaseDifference / (2 * MY_PI * freq);

	return 0;
}
