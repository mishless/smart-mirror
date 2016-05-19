#include "FrequencyDetector.h"
void FrequencyDetector::initialize(double lowFreq, double highFreq)
{
	lowFrequency = lowFreq;
	highFrequency = highFreq;
}

void FrequencyDetector::filterSpectrum(vector<double> *inputSp, int *lowIndex, int *highIndex)
{
	int N = (int)inputSp->size();

	*lowIndex = (int)round(lowFrequency*N / SAMPLING_FREQUENCY);
	if (*lowIndex == 0) *lowIndex = 1;
	*highIndex = (int)round(highFrequency*N / SAMPLING_FREQUENCY);
}

void FrequencyDetector::convertSpectrum(vector<double> *inputSp, vector<double>* outputSp)
{
	/* Get dft point number */
	int N = (int)inputSp->size();

	/* DC component should be in first place of vector */
	outputSp->push_back((*inputSp)[0]);

	/* Calculate the rest of amplitude spectrum by using the pythogarian theorem
	on real-complex value pairs from the input spectrum */
	int i = 1;
	while (i < N - 2)
	{
		double tmp;
		tmp = sqrt((*inputSp)[i] * (*inputSp)[i] + (*inputSp)[i + 1] * (*inputSp)[i + 1]);
		outputSp->push_back(tmp);
		i += 2;
	}
}

double FrequencyDetector::detectFrequency(vector<Mat> * frames)
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
	int lowInd, highInd;
	double maxFrequency;
	int i;
	sum = 0;
	minVal = 266;
	maxVal = -1;

	cout << "***********************" << endl;
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
	dft(normalized, complexSpectrum);
	convertSpectrum(&complexSpectrum, &ampSpectrum);

	/* Filter it */
	filterSpectrum(&ampSpectrum, &lowInd, &highInd);

	for (i = 0; i <= highInd; i++)
	{
		cout << i << " : " << ampSpectrum[i] << endl;
	}

	/* Find biggest peak */
	int maxInd = lowInd;
	for (i = lowInd + 1; i <= highInd; i++)
	{
		if (ampSpectrum[i] > ampSpectrum[maxInd])
		{
			maxInd = i;
		}
	}
	cout << "Low Ind: " <<lowInd << endl;
	//cout << "HB: " << ((double)maxInd * SAMPLING_FREQUENCY / (double)ampSpectrum.size()) * 60 << endl;
	return (double)maxInd * SAMPLING_FREQUENCY / (double)ampSpectrum.size();
}
