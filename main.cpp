#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
//#include <opencv2/opencv.hpp>
#include "minutiae_template.h"
#include "circle.h"
#include "code.h"

#define TEST_SET_1
#define TEST_SET_2
#define TEST_SET_3

#define COMPUTE_GAR
#define NO_ROTATE_FIRST
#define NO_ROTATE_SECOND

const int TEMPLATE_SIZE = 20;
const int MAX_X = 255;
const int MAX_Y = 255;
const int TEMPLATE_NUM = 800;
const int TEST_SET_SIZE = 20;
const int disturbScale = 5;
const double DEFAULT_T2 = 5.0;

using namespace std;
//using namespace cv;


double computeEER(double rightDistance[][TEMPLATE_NUM], double wrongDistance[][TEMPLATE_NUM], double *t)
{
	double lineDistanceRight[TEST_SET_SIZE*TEMPLATE_NUM];
	double lineDistanceWrong[TEST_SET_SIZE*TEMPLATE_NUM];
	for(int i = 0; i < TEST_SET_SIZE; i++)
	{
		for(int j = 0; j < TEMPLATE_NUM; j++)
		{
			lineDistanceRight[i*TEMPLATE_NUM+j] = rightDistance[i][j];
			lineDistanceWrong[i*TEMPLATE_NUM+j] = wrongDistance[i][j];
		}
	}
	sort(&lineDistanceRight[0],&lineDistanceRight[TEST_SET_SIZE*TEMPLATE_NUM]);
	sort(&lineDistanceWrong[0],&lineDistanceWrong[TEST_SET_SIZE*TEMPLATE_NUM]);
	for(int i = 0; i < TEST_SET_SIZE*TEMPLATE_NUM; i++)
	{
		if(lineDistanceWrong[i] > lineDistanceRight[TEST_SET_SIZE*TEMPLATE_NUM-1-i])
			return *t=lineDistanceWrong[i],(double)i/(TEST_SET_SIZE*TEMPLATE_NUM);
	}
	return 1.0;
}

double getDistance(const MinutiaeTemplate & mt1, const MinutiaeTemplate & mt2, int nBin, double T)
{
	Circle c1,c2;
	c1.setT(T);c2.setT(T);
	CountCode code1(nBin),code2(nBin);
	c1.getCenter(mt1); c2.getCenter(mt2);
	c1.fromTemplate(mt1); c2.fromTemplate(mt2);
	code1.fromCircle(c1); code2.fromCircle(c2);
	return code1.distance(code2);
}

double getRotateDistance(const MinutiaeTemplate & mt1, const MinutiaeTemplate & mt2, int nBin, double T)
{
	Circle c1,c2;
	c1.setT(T);c2.setT(T);
	CountCode code1(nBin),code2(nBin);
	c1.getCenter(mt1); c2.getCenter(mt2);
	c1.fromTemplate(mt1); c2.fromTemplate(mt2);
	code1.fromCircle(c1); code2.fromCircle(c2);
	return code1.rotateDistance(code2,nBin/12+1);
}

inline bool match(const MinutiaeTemplate & mt1, const MinutiaeTemplate & mt2, int nBin, double t, double T)
{
	return getDistance(mt1,mt2,nBin,T) < t;
}

int main()
{
	srand((unsigned)time(0));
	MinutiaeTemplate::setMaxXY(MAX_X,MAX_Y);

	MinutiaeTemplate templates[TEMPLATE_NUM];
#ifdef TEST_SET_1
	MinutiaeTemplate testSet1[TEST_SET_SIZE][TEMPLATE_NUM];
	double rightDistance1[TEST_SET_SIZE][TEMPLATE_NUM];
	double wrongDistance1[TEST_SET_SIZE][TEMPLATE_NUM];
#endif
#ifdef TEST_SET_2
	MinutiaeTemplate testSet2[TEST_SET_SIZE][TEMPLATE_NUM];
	double rightDistance2[TEST_SET_SIZE][TEMPLATE_NUM];
	double wrongDistance2[TEST_SET_SIZE][TEMPLATE_NUM];
#endif
#ifdef TEST_SET_3
	MinutiaeTemplate testSet3[TEST_SET_SIZE][TEMPLATE_NUM];
	double rightDistance3[TEST_SET_SIZE][TEMPLATE_NUM];
	double wrongDistance3[TEST_SET_SIZE][TEMPLATE_NUM];
#endif

	cerr << "Generating original templates..." << endl;
	for(int i = 0; i < TEMPLATE_NUM; i++)
		templates[i].fillWithRandom(TEMPLATE_SIZE);
	cerr << "Done generated original templates." << endl;
#ifdef TEST_SET_1
	cerr << "Generating first test set..." << endl;
	for(int i = 0; i < TEST_SET_SIZE; i++)
	{
		for(int j = 0; j < TEMPLATE_NUM; j++)
		{
			testSet1[i][j].copy(templates[i]);
			testSet1[i][j].disturb(disturbScale);
		}
		cerr << "*";
	}
	cerr << endl << "Done generated first test set..." << endl;
#endif
#ifdef TEST_SET_2
	cerr << "Generating second test set..." << endl;
	for(int i = 0; i < TEST_SET_SIZE; i++)
	{
		for(int j = 0; j < TEMPLATE_NUM; j++)
		{
			testSet2[i][j].copy(templates[i]);
			testSet2[i][j].removeOrAddRandom(2);
		}
		cerr << "*";
	}
	cerr << endl << "Done generated second test set..." << endl;
#endif
#ifdef TEST_SET_3
	cerr << "Generating third test set..." << endl;
	for(int i = 0; i < TEST_SET_SIZE; i++)
	{
		for(int j = 0; j < TEMPLATE_NUM; j++)
		{
			testSet3[i][j].copy(templates[i]);
			testSet3[i][j].rotateRandom(M_PI/6);
		}
		cerr << "*";
	}
	cerr << endl << "Done generated third test set..." << endl;
#endif
	double Tset[] = {0,20,40,60,80,100,120};
	int nBinSet[] = {360,180,90,45,30,20};
	double tset[] = {0.5,1.0,1.5,2.0,2.5,3.0,3.5,4.0,4.5,5.0,5.5,6.0,6.5,7.0,7.5,8.0};
	int nT = sizeof(Tset)/sizeof(Tset[0]);
	int nB = sizeof(nBinSet)/sizeof(nBinSet[0]);
	int nt = sizeof(tset)/sizeof(tset[0]);
	double t = DEFAULT_T2;
	cout << setprecision(10) << endl;
#ifdef TEST_SET_1
	cerr << "Start testing the first test set..." << endl;
	
	for(int iT = 0; iT < nT; iT++)
	{
		double T = Tset[iT];
		for(int iB = 0; iB < nB; iB++)
		{
			int nBin = nBinSet[iB];
#ifdef COMPUTE_GAR
			cout << T << "," << nBin << ",";
#endif
			time_t start = clock();
			for(int i = 0; i < TEST_SET_SIZE; i++)
			{
				for(int j = 0; j < TEMPLATE_NUM; j++)
				{
#ifdef NO_ROATE_FIRST
					rightDistance1[i][j] = getDistance(templates[i],testSet1[i][j],nBin,T);
					wrongDistance1[i][j] = getDistance(templates[i],testSet1[(i+1)%TEST_SET_SIZE][j],nBin,T);
#else
					rightDistance1[i][j] = getRotateDistance(templates[i],testSet1[i][j],nBin,T);
					wrongDistance1[i][j] = getRotateDistance(templates[i],testSet1[(i+1)%TEST_SET_SIZE][j],nBin,T);
#endif
				}
			}
			cout << (double)(clock() - start)/(TEST_SET_SIZE*TEMPLATE_NUM*1000) << ",";
#ifdef OUTPUT_DISTANCE
			for(int i = 0; i < TEST_SET_SIZE; i++)
				for(int j = 0; j < TEMPLATE_NUM; j++)
					cout << rightDistance1[i][j] << " ";
			cout << endl;
			for(int i = 0; i < TEST_SET_SIZE; i++)
				for(int j = 0; j < TEMPLATE_NUM; j++)
					cout << wrongDistance1[i][j] << " ";
			cout << endl;
#endif
#ifdef COMPUTE_GAR
#if 0
			for(int it = 0; it < nt; it++)
			{
				int correct = 0;
				int fwrong = 0;
				double t = tset[it];
				for(int i = 0; i < TEST_SET_SIZE; i++)
				{
					for(int j = 0; j < TEMPLATE_NUM; j++)
					{
						if(rightDistance1[i][j] < t)
							correct++;
						if(wrongDistance1[i][j] < t)
							fwrong++;
					}
				}
				cout << (double)correct/(TEST_SET_SIZE*TEMPLATE_NUM) << ",";
				cout << (double)fwrong/(TEST_SET_SIZE*TEMPLATE_NUM);
				if(it < nt-1) cout << ",";
			}
			cout << endl;
#endif
			double t;
			cout << computeEER(rightDistance1,wrongDistance1,&t) << ',' << t << endl;
#endif
		}
	}
#endif
#ifdef TEST_SET_2
	cerr << "Start testing the second test set..." << endl;
	for(int iT = 0; iT < nT; iT++)
	{
		double T = Tset[iT];
		for(int iB = 0; iB < nB; iB++)
		{
			int nBin = nBinSet[iB];
#ifdef COMPUTE_GAR
			cout << T << "," << nBin << ",";
#endif
			time_t start = clock();
			for(int i = 0; i < TEST_SET_SIZE; i++)
			{
				for(int j = 0; j < TEMPLATE_NUM; j++)
				{
#ifdef NO_ROTATE_SECOND
					rightDistance2[i][j] = getDistance(templates[i],testSet2[i][j],nBin,T);
					wrongDistance2[i][j] = getDistance(templates[i],testSet2[(i+1)%TEST_SET_SIZE][j],nBin,T);
#else
					rightDistance2[i][j] = getRotateDistance(templates[i],testSet2[i][j],nBin,T);
					wrongDistance2[i][j] = getRotateDistance(templates[i],testSet2[(i+1)%TEST_SET_SIZE][j],nBin,T);
#endif
				}
			}
			cout << (double)(clock() - start)/(TEST_SET_SIZE*TEMPLATE_NUM*1000) << ",";
#ifdef OUTPUT_DISTANCE
			for(int i = 0; i < TEST_SET_SIZE; i++)
				for(int j = 0; j < TEMPLATE_NUM; j++)
					cout << rightDistance2[i][j] << " ";
			cout << endl;
			for(int i = 0; i < TEST_SET_SIZE; i++)
				for(int j = 0; j < TEMPLATE_NUM; j++)
					cout << wrongDistance2[i][j] << " ";
			cout << endl;
#endif
#ifdef COMPUTE_GAR
#if 0
			for(int it = 0; it < nt; it++)
			{
				int correct = 0;
				int fwrong = 0;
				double t = tset[it];
				for(int i = 0; i < TEST_SET_SIZE; i++)
				{
					for(int j = 0; j < TEMPLATE_NUM; j++)
					{
						if(rightDistance2[i][j] < t)
							correct++;
						if(wrongDistance2[i][j] < t)
							fwrong++;
					}
				}
				cout << (double)correct/(TEST_SET_SIZE*TEMPLATE_NUM) << ",";
				cout << (double)fwrong/(TEST_SET_SIZE*TEMPLATE_NUM);
				if(it < nt-1) cout << ",";
			}
			cout << endl;
#endif
			double t;
			cout << computeEER(rightDistance2,wrongDistance2,&t) << ',' << t << endl;
#endif
		}
	}
#endif
#ifdef TEST_SET_3
	cerr << "Start testing the third test set..." << endl;
	for(int iT = 0; iT < nT; iT++)
	{
		double T = Tset[iT];
		for(int iB = 0; iB < nB; iB++)
		{
			int nBin = nBinSet[iB];
#ifdef COMPUTE_GAR
			cout << T << "," << nBin << ",";
#endif
			time_t start = clock();
			for(int i = 0; i < TEST_SET_SIZE; i++)
			{
				for(int j = 0; j < TEMPLATE_NUM; j++)
				{
#ifdef NO_ROTATE_THIRD
					rightDistance3[i][j] = getDistance(templates[i],testSet3[i][j],nBin,T);
					wrongDistance3[i][j] = getDistance(templates[i],testSet3[(i+1)%TEST_SET_SIZE][j],nBin,T);
#else
					rightDistance3[i][j] = getRotateDistance(templates[i],testSet3[i][j],nBin,T);
					wrongDistance3[i][j] = getRotateDistance(templates[i],testSet3[(i+1)%TEST_SET_SIZE][j],nBin,T);
#endif
				}
			}
			cout << (double)(clock() - start)/(TEST_SET_SIZE*TEMPLATE_NUM*1000) << ",";
#ifdef OUTPUT_DISTANCE
			for(int i = 0; i < TEST_SET_SIZE; i++)
				for(int j = 0; j < TEMPLATE_NUM; j++)
					cout << rightDistance3[i][j] << " ";
			cout << endl;
			for(int i = 0; i < TEST_SET_SIZE; i++)
				for(int j = 0; j < TEMPLATE_NUM; j++)
					cout << wrongDistance3[i][j] << " ";
			cout << endl;
#endif
#ifdef COMPUTE_GAR
#if 0
			for(int it = 0; it < nt; it++)
			{
				int correct = 0;
				int fwrong = 0;
				double t = tset[it];
				for(int i = 0; i < TEST_SET_SIZE; i++)
				{
					for(int j = 0; j < TEMPLATE_NUM; j++)
					{
						if(rightDistance3[i][j] < t)
							correct++;
						if(wrongDistance3[i][j] < t)
							fwrong++;
					}
				}
				cout << (double)correct/(TEST_SET_SIZE*TEMPLATE_NUM) << ",";
				cout << (double)fwrong/(TEST_SET_SIZE*TEMPLATE_NUM);
				if(it < nt-1) cout << ",";
			}
			cout << endl;
#endif
			double t;
			cout << computeEER(rightDistance3,wrongDistance3,&t) << ',' << t << endl;
#endif
		}
	}
#endif
	return 0;
}
