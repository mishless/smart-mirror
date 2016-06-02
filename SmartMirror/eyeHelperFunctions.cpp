#include "eyeHelperFunctions.h"
#include <limits.h>
#include <float.h>
#include <fstream>

Mat stretchHistogram(Mat * inputImage)
{
	/* Input image must be grayscale */
	int min = 255;
	int max = 0;
	int val;
	int i, j;

	Mat outputImage(inputImage->rows, inputImage->cols, inputImage->type() );

	
	for (i = 0; i < inputImage->rows; i++)
	{
		for (j = 0; j < inputImage->cols; j++)
		{
			val = inputImage->at<uchar>(i, j);
			if (val > max)
			{
				max = val;
			}
			if (val < min)
			{
				min = val;
			}
		}
	}

	
	for (i = 0; i < inputImage->rows; i++)
	{
		for (j = 0; j < inputImage->cols; j++)
		{
			val = inputImage->at<uchar>(i, j);
			outputImage.at<uchar>(i, j) = (uchar)((((double)val - min) / ((double)max - min)) * max);
		}
	}

	return outputImage;
}

vector<int> sum(Mat *inputImage, int dim)
{
	int i, j;
	int sum;

	vector<int> result((dim == 1) ? inputImage->cols : inputImage->rows);

	for (i = 0; i < ((dim == 1) ? inputImage->cols : inputImage->rows); i++)
	{
		sum = 0;
		for (j = 0; j < ((dim != 1) ? inputImage->cols : inputImage->rows); j++)
		{
			sum += inputImage->at<uchar>((dim == 1) ? j : i, (dim == 1) ? i : j);
		}
		result[i] = sum;
	}

	return result;
}

void findPeaks(vector<double>* input, vector<int>* locations, vector<double>* peaks, double minPeakHeight)
{
	int n = (int) input->size();
	int i;
	int lastLoc;
	bool isEqual = false;

	for (i = 1; i < n - 1; i++)
	{
		if (!isEqual)
		{
			if ((*input)[i] >(*input)[i - 1] && (*input)[i] >= (*input)[i + 1])
			{
				if ((*input)[i] > (*input)[i + 1])
				{
					if ((*input)[i] > minPeakHeight)
					{
						peaks->push_back((*input)[i]);
						locations->push_back(i);
					}
				}
				else
				{
					lastLoc = i;
					isEqual = true;
				}
			}
		}
		else
		{
			if ((*input)[i] < (*input)[i - 1])
			{
				if ((*input)[lastLoc] > minPeakHeight)
				{
					peaks->push_back((*input)[lastLoc]);
					locations->push_back(lastLoc);
				}
				isEqual = false;
			}
			else if ((*input)[i] > (*input)[i - 1])
			{
				isEqual = false;
				if ((*input)[i] >(*input)[i - 1] && (*input)[i] >= (*input)[i + 1])
				{
					if ((*input)[i] > (*input)[i + 1])
					{
						if ((*input)[i] > minPeakHeight)
						{
							peaks->push_back((*input)[i]);
							locations->push_back(i);
						}
					}
					else
					{
						lastLoc = i;
						isEqual = true;
					}
				}
			}
		}
	}
}

void findPeaks(vector<int>* input, vector<int>* locations, vector<int>* peaks, int minPeakHeight)
{
	int n = (int)input->size();
	int i;
	int lastLoc;
	bool isEqual = false;

	for (i = 1; i < n - 1; i++)
	{
		if (!isEqual)
		{
			if ((*input)[i] >(*input)[i - 1] && (*input)[i] >= (*input)[i + 1])
			{
				if ((*input)[i] > (*input)[i + 1])
				{
					if ((*input)[i] > minPeakHeight)
					{
						peaks->push_back((*input)[i]);
						locations->push_back(i);
					}
				}
				else
				{
					lastLoc = i;
					isEqual = true;
				}
			}
		}
		else
		{
			if ((*input)[i] < (*input)[i - 1])
			{
				if ((*input)[lastLoc] > minPeakHeight)
				{
					peaks->push_back((*input)[lastLoc]);
					locations->push_back(lastLoc);
				}
				isEqual = false;
			}
			else if ((*input)[i] > (*input)[i - 1])
			{
				isEqual = false;
				if ((*input)[i] >(*input)[i - 1] && (*input)[i] >= (*input)[i + 1])
				{
					if ((*input)[i] > (*input)[i + 1])
					{
						if ((*input)[i] > minPeakHeight)
						{
							peaks->push_back((*input)[i]);
							locations->push_back(i);
						}
					}
					else
					{
						lastLoc = i;
						isEqual = true;
					}
				}
			}
		}
	}
}


Mat binarize(Mat * inputImage, int threshold)
{
	Mat outputImage(inputImage->rows, inputImage->cols, inputImage->type());
	int i, j;

	for (i = 0; i < outputImage.rows; i++)
	{
		for (j = 0; j < outputImage.cols; j++)
		{
			outputImage.at<uchar>(i, j) = (inputImage->at<uchar>(i, j) > (uchar)threshold);
		}
	}
	return outputImage;
}

vector<double> meanValue(Mat * inputImage, int dim)
{
	int i, j;
	int sum;

	vector<double> result((dim == 1) ? inputImage->cols : inputImage->rows);

	for (i = 0; i < ((dim == 1) ? inputImage->cols : inputImage->rows); i++)
	{
		sum = 0;
		for (j = 0; j < ((dim != 1) ? inputImage->cols : inputImage->rows); j++)
		{
			sum += inputImage->at<uchar>((dim == 1) ? j : i, (dim == 1) ? i : j);
		}
		result[i] = (double)sum/j;
	}

	return result;
}

vector<double> diff(vector<double>* inputVector)
{
	int n = (int)inputVector->size() - 1;
	int i;
	vector<double> result;

	for (i = 0; i < n; i++)
	{
		result.push_back((*inputVector)[i+1] - (*inputVector)[i]);
	}

	return result;
}

vector<int> diff(vector<int>* inputVector)
{
	int n = (int)inputVector->size() - 1;
	int i;
	vector<int> result;

	for (i = 0; i < n; i++)
	{
		result.push_back((*inputVector)[i + 1] - (*inputVector)[i]);
	}

	return result;
}

Mat invert(Mat * inputImage)
{
	Mat outputImage(inputImage->rows, inputImage->cols, inputImage->type());
	int i, j;

	for (i = 0; i < outputImage.rows; i++)
	{
		for (j = 0; j < outputImage.cols; j++)
		{
			outputImage.at<uchar>(i, j) = !(inputImage->at<uchar>(i, j));
		}
	}

	return outputImage;
}

vector<int> getSortIndexes(vector<double> inputVector)
{
	vector<int> result;

	int i, j;
	int maxInd;

	for (i = 0; i < inputVector.size(); i++)
	{
		maxInd = 0;
		for (j = 0; j < inputVector.size(); j++)
		{
			if (inputVector[j] > inputVector[maxInd])
			{
				maxInd = j;
			}
		}
		inputVector[maxInd] = -DBL_MAX;
		result.push_back(maxInd);
	}

	return result;
}

vector<int> ourFind(vector<int> vec01, int num, int mode)
{
	vector<int> result;
	if (num == 0) num = INT_MAX;
	int i;
	int cnt = 0;
	if (mode == FIND_FIRST)
	{
		for (i = 0; i < vec01.size(); i++)
		{
			vec01[i] ? result.push_back(i), cnt++ : 66;
			if (cnt >= num) break;
		}
	}
	else
	{
		for (i = vec01.size() - 1; i >= 0; i--)
		{
			vec01[i] ? result.push_back(i), cnt++ : 66;
			if (cnt >= num) break;
		}
	}
	return result;
}

vector<int> vectorTo01(vector<double> inputVector, double threshold)
{
	vector<int> result;
	int i;

	for (i = 0; i < inputVector.size(); i++)
	{
		result.push_back(inputVector[i] < threshold);
	}
	return result;
}

vector<int> vectorTo01(vector<int> inputVector, double threshold)
{
	vector<int> result;
	int i;

	for (i = 0; i < inputVector.size(); i++)
	{
		result.push_back(inputVector[i] < threshold);
	}
	return result;
}

void getDistance(vector<double> peaks, vector<int> locs, int * outputLoc1, int * outputLoc2)
{
	vector<int> indexes;

	if (peaks.size() == 2)
	{
		*outputLoc1 = locs[0];
		*outputLoc2 = locs[1];
	}
	else if (peaks.size() < 2)
	{
		*outputLoc1 = 0;
		*outputLoc2 = 0;
	}
	else
	{
		indexes = getSortIndexes(peaks);
		*outputLoc1 = locs[indexes[0]];
		*outputLoc2 = locs[indexes[indexes.size() - 1]];
	}
}

void getBigInterval(vector<int> inputArray, int * outputBoundary1, int * outputBoundary2, double percentage)
{
	int maxVal;
	int maxInd;
	vector<int> left;
	vector<int> right;

	vectorGetMax(inputArray, &maxVal, &maxInd);
	vector<double> sumLeft(&inputArray[0], &inputArray[maxInd]);
	vector<double> sumRight(&inputArray[maxInd], &inputArray[inputArray.size()]);
	left = ourFind(vectorTo01(sumLeft, percentage*maxVal), 1, FIND_LAST);
	if (left.empty())
	{
		*outputBoundary1 = 0;
	}
	else
	{
		*outputBoundary1 = left[0];
	}

	right = ourFind(vectorTo01(sumRight, percentage*maxVal), 1, FIND_FIRST);

	if (right.empty())
	{
		right.push_back(sumRight.size() - 1);
	}
	*outputBoundary2 = right[0] + maxInd;
}

void vectorGetMax(vector<int> inputVector, int * outputMax, int * outputInd)
{
	int maxInd = 0;
	int i;
	for (i = 1; i < inputVector.size(); i++)
	{
		if (inputVector[i] > inputVector[maxInd])
		{
			maxInd = i;
		}
	}

	*outputMax = inputVector[maxInd];
	*outputInd = maxInd;
}

void vectorGetMin(vector<int> inputVector, int * outputMin, int * outputInd)
{
	int minInd = 0;
	int i;
	for (i = 1; i < inputVector.size(); i++)
	{
		if (inputVector[i] < inputVector[minInd])
		{
			minInd = i;
		}
	}

	*outputMin = inputVector[minInd];
	*outputInd = minInd;
}

int eyelidDistance(Mat eye, double threshold)
{
	int i;
	int j;
	int m = eye.rows;
	int n = eye.cols;
	Mat strechedEye = stretchHistogram(&eye);
	vector<int> summary = sum(&strechedEye, 2);
	vector<int> eyebrowPeaks;
	vector<int> eyebrowLocs;
	findPeaks(&summary, &eyebrowLocs, &eyebrowPeaks);
	Mat eyeBinary = binarize(&strechedEye, 55);

	if (!eyebrowLocs.empty())
	{
		if (eyebrowLocs[0] != 1 && eyebrowLocs[0] < round(m / 2.2))
		{
			eyeBinary.rowRange(0, eyebrowLocs[0] - 1) = 1;
		}
	}

	vector<double> mBinary = meanValue(&eyeBinary, 2);
	vector<double> sigmaBinary(m);

	for (i = 0; i < m; i++)
	{
		double tmpSum = 0;
		for (j = 0; j < n; j++)
		{
			tmpSum += (eyeBinary.at<uchar>(i, j) - mBinary[i])*(eyeBinary.at<uchar>(i, j) - mBinary[i]);
		}
		sigmaBinary[i] = tmpSum / n;
	}
	vector<double> difVarBinary = diff(&sigmaBinary);

	vector<double> pksBinaryPositive;
	vector<int> locsBinaryPositive;
	vector<double> pksBinaryNegative;
	vector<int> locsBinaryNegative;
	findPeaks(&difVarBinary, &locsBinaryPositive, &pksBinaryPositive);
	findPeaks(&(negateVector(difVarBinary)), &locsBinaryNegative, &pksBinaryNegative);

	vector<double> pksBinary = pksBinaryPositive;
	vector<double> negatedPeaks = negateVector(pksBinaryNegative);
	pksBinary.insert(pksBinary.end(), negatedPeaks.begin(), negatedPeaks.end());
	vector<int> locsBinary = locsBinaryPositive;
	locsBinary.insert(locsBinary.end(), locsBinaryNegative.begin(), locsBinaryNegative.end());

	int loc1, loc2;
	getDistance(pksBinary, locsBinary, &loc1, &loc2);
	int distance = abs(loc2 - loc1);

	double percent = 0;

	if (distance < threshold)
	{
		return 0;
	}
	else
	{
		if (loc1 != 0 || loc2 != 0)
		{
			if (loc1 < loc2)
			{
				Mat extract = eyeBinary.rowRange(loc1, loc2);
				vector<int> sumH = sum(&invert(&extract), 2);
				int boundariesH1;
				int boundariesH2;
				int boundariesV1;
				int boundariesV2;

				getBigInterval(sumH, &boundariesH1, &boundariesH2, 0.85);
				vector<int> sumV = sum(&invert(&extract), 1);
				getBigInterval(sumV, &boundariesV1, &boundariesV2, 0.65);

				Mat cut = extract(Range(boundariesH1, boundariesH2),
					Range(boundariesV1, boundariesV2));

				percent = 100 * (sum(cut))[0] / (double)(cut.rows * cut.cols);
			}
		}

		if (percent > 25)
		{
			return 0;
		}
		else
		{
			return distance;
		}
	}
}

vector<double> negateVector(vector<double> inputVector)
{
	vector<double> result;
	int i;
	for (i = 0; i < inputVector.size(); i++)
	{
		result.push_back(-inputVector[i]);
	}
	return result;
}

vector<int> negateVector(vector<int> inputVector)
{
	vector<int> result;
	int i;
	for (i = 0; i < inputVector.size(); i++)
	{
		result.push_back(-inputVector[i]);
	}
	return result;
}

void eyelidsDistances(Mat eyes, int * outputDL, int * outputDR, int leftMaximumOpened, int rightMaximumOpened)
{
	Mat eyesGray;
	cvtColor(eyes, eyesGray, CV_BGR2GRAY);
	int m = eyesGray.rows;
	int n = eyesGray.cols;

	Mat leftEye = eyesGray.colRange(0, n / 2 - 11);
	Mat rightEye = eyesGray.colRange(n / 2 + 10, n);

	*outputDL = eyelidDistance(leftEye, leftMaximumOpened * 0.4);
	*outputDR = eyelidDistance(rightEye, rightMaximumOpened * 0.4);
}

features_t getFeatures(vector<int> leftEyeDistances, vector<int> rightEyeDistances)
{
	vector<int> distances;
	int i;
	features_t features;

	for (i = 0; i < leftEyeDistances.size(); i++)
	{
		distances.push_back((int)round((leftEyeDistances[i] + rightEyeDistances[i]) / 2.0));
	}
	
	int minVal, maxVal;
	int minInd, maxInd;
	vectorGetMax(distances, &maxVal, &maxInd);
	vectorGetMin(distances, &minVal, &minInd);

	double threshold = 0.9 * (minVal + maxVal) / 2.0;
	vector<int> eyeClosed = ourFind(vectorTo01(distances, threshold));
	
	/* Percentage of eyelid closure */
	features.PERCLOS = eyeClosed.size() / (double)distances.size();

	/* Maximum closed duration */
	features.MCD = 0;
	int summary = 1;
	for (i = 1; i < eyeClosed.size(); i++)
	{
		if (eyeClosed[i] == eyeClosed[i - 1] + 1)
		{
			summary++;
		}
		else
		{
			if (features.MCD < summary)
			{
				features.MCD = summary;
			}
			summary = 1;
		}

		if (i == eyeClosed.size() - 1)
		{
			if (features.MCD < summary)
			{
				features.MCD = summary;
			}
			summary = 1;
		}
	}

	features.MCD *= SAMPLING_PERIOD;

	/* Blinking frequency */
	vector<int> negatedDistances = negateVector(distances);
	vector<int> locs;
	vector<int> peaks;

	findPeaks(&negatedDistances, &locs, &peaks, -(int)round(threshold));

	features.BF = 60 * peaks.size() / ((double)distances.size() * SAMPLING_PERIOD);

	/* Opening velocity and closing velocity */
	int cntClosing = 0;
	int cntOpening = 0;

	vector<int> diffPos = diff(&distances);
	vector<int> diffNeg = negateVector(diffPos);
	vector<int> locsPos;
	vector<int> locsNeg;
	vector<int> peaksPos;
	vector<int> peaksNeg;

	findPeaks(&diffPos, &locsPos, &peaksPos);
	findPeaks(&diffNeg, &locsNeg, &peaksNeg);

	vector<int> locsAll = locsPos;
	locsAll.insert(locsAll.end(), locsNeg.begin(), locsNeg.end());
	locsAll.insert(locsAll.end(), locs.begin(), locs.end());
	sort(locsAll.begin(), locsAll.end());

	int opened;
	int state;
	features.OV = 0;
	features.CV = 0;

	vector<int> locsBlink = locs;

	if (locsAll.empty())
	{
		cout << "Jebo te bog" << endl;
	}
	else
	{
		for (i = 0; i < locsAll.size() - 1; i++)
		{
			if ((find(locsPos.begin(), locsPos.end(), locsAll[i]) != locsPos.end()) ||
				(find(locsNeg.begin(), locsNeg.end(), locsAll[i]) != locsNeg.end()))
			{
				opened = 1;
			}
			else
			{
				opened = 0;
			}

			if ((find(locsPos.begin(), locsPos.end(), locsAll[i + 1]) != locsPos.end()) ||
				(find(locsNeg.begin(), locsNeg.end(), locsAll[i + 1]) != locsNeg.end()))
			{
				if (opened)
				{
					state = 0;
				}
				else
				{
					state = 1;
				}
			}
			else
			{
				if (opened)
				{
					state = -1;
				}
				else
				{
					state = 0;
				}
			}

			if(state == -1)
			{
				features.CV += abs(distances[locsAll[i]] - distances[locsAll[i+1]]) /
					abs(locsAll[i] - locsAll[i + 1]); /* pixels/frame */
				cntClosing++;
			}
			else if (state == 1)
			{
				features.OV += abs(distances[locsAll[i]] - distances[locsAll[i + 1]]) /
					abs(locsAll[i] - locsAll[i + 1]); /* pixels/frame */
				cntOpening++;
			}
		}
		features.CV /= cntClosing;
		features.OV /= cntOpening;
	}

	vector<int> eyeOpened = ourFind(invert01(vectorTo01(distances, threshold)));
	
	int AOLSum = 0;

	for (i = 0; i < eyeOpened.size(); i++)
	{
		AOLSum += distances[i];
	}

	features.AOL = ((double)AOLSum / (double)eyeOpened.size()) / maxVal;

	return features;
}

void writeFeaturesToFile(features_t features, string fileName)
{
	ofstream outputFile;
	outputFile.open(fileName);
	outputFile << features.PERCLOS;
	outputFile << features.BF;
	outputFile << features.MCD;
	outputFile << features.AOL;
	outputFile << features.OV;
	outputFile << features.CV;
	
	outputFile.close();
}

string getDrowsinessFromFile(string fileName)
{
	ifstream inputFile;
	string drowsiness;

	while (!inputFile.is_open())
	{
		inputFile.open(fileName);
	}

	inputFile >> drowsiness;

	inputFile.close();
	return drowsiness;
}

vector<int> invert01(vector<int> inputVector)
{
	int i;
	vector<int> result;
	for (i = 0; i < inputVector.size(); i++)
	{
		result.push_back(!inputVector[i]);
	}

	return result;
}