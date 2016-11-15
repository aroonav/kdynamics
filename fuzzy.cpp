#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

// Returns the membership value for the given input depending on the other input variables
// TODO: Take an input as an array which consists the last three values. So that the calling function can directly pass a row of classifiers 2D array.
float membership_value(float input, float lowValue, float midValue, float highValue)
{
	if(input<lowValue || input>highValue)
		return 0;
	else if(input<midValue)
		return (input-lowValue)/(midValue-lowValue);
	else if(input==midValue)
		return 1;
	else
		return (highValue-input)/(highValue-midValue);
}

// This returns the membership for the delays in an array ``vector''. It can range from 0-4, i.e from Very Fast to Very Slow
int gruntWork(int* vector, int noOfTries, int noOfFuzzySets)
{
	int finalMembership = -1;
	float membership_values[noOfTries][noOfFuzzySets];			// This contains the membership values where each value denotes the membership value of that particular row in the column's fuzzy set.
	float weightedAverage_memberships[noOfFuzzySets];			// After the defuzzification, the value is again fuzzified and this array contains the membership values for the defuzzified value in the $noOfFuzzySets fuzzy sets
	float max = 0.0;
	float weightedSum = 0.0;float membershipSum = 0.0;			// weightedSum and membershipSum is the numerator and denominator respetively for the calculation of the weightedAverage using COG method during defuzzification.
	float classifiers[5][3]={	//Low value, Middle Value, High Value
							{21,		 25.5,			29},	// Very Fast(0)
							{26, 		 29,			32},	// Fast		(1)
							{30, 		 33.5,			37},	// Moderate	(2)
							{36, 		 40,			44},	// Slow		(3)
							{42,		 46,			50}		// Very Slow(4)
						};
	memset(membership_values, 0, sizeof(membership_values));

	for (int i = 0; i < noOfTries; i++)							// This finds the union of the fuzzy sets by finding the maximum of all the membership values for a particular row. It then uses this value to find the numerator and denominator to find the weightedAverage.
	{
		float max = 0.0;
		for (int j = 0; j < noOfFuzzySets; j++)
		{
			membership_values[i][j] = membership_value(vector[i], classifiers[j][0], classifiers[j][1], classifiers[j][2]);
			if (membership_values[i][j]>max)
				max = membership_values[i][j];
		}
		weightedSum += (max*vector[i]);							// Numerator for weighted average
		membershipSum += max;									// Denominator for weighted average
	}
	float weightedAverage_cog = weightedSum/membershipSum;		// Defuzzified value using Centre of Gravity method.
	for (int i = 0; i < noOfFuzzySets; i++)						// Defuzzified value is again fuzzified in this step.
	{
		weightedAverage_memberships[i] = membership_value(weightedAverage_cog, classifiers[i][0], classifiers[i][1], classifiers[i][2]);
		if(weightedAverage_memberships[i]>max)
		{
			finalMembership = i;
			max = weightedAverage_memberships[i];
		}
	}
	return finalMembership;
}

// Fuzzy Inference System learning component
void fis_learning()
{
	const int usernameLength = 4;		// Length of the username. ex "s001"
	const int passwordLength = 11;		// Length of the password ".tie5Roanl" + Return Key.
	const int noOfTries = 15;			// No. of tries done to train/create fuzzy rule for a single user
	const int noOfUsers = 50;			// No. of users for which this model is being built
	const int noOfFuzzySets = 5;		// No. of fuzzy sets, here 5 i.e Very Fast(0), Fast(1), Moderate(2), Slow(3), Very Slow(4)
	int finalMembership = -1;			// This denotes the final membership of the delays given in the vector. That is, whether the delays given in the vector is Very Fast, Fast, Moderate, Slow, Very Slow.

	int vector[noOfTries] = {27, 33, 24, 49, 27, 33, 33, 33, 33, 33, 33, 28, 33, 28, 27};
	finalMembership = gruntWork(vector, noOfTries, noOfFuzzySets);
	cout<<finalMembership<<endl;
}

// Fuzzy Inference System working/testing component
void fis_working()
{
}

// Fuzzy inference system. option denotes learning or working option.
void fis(int option)
{
	if(option==1)
		fis_learning();
	else
		fis_working();
}

int main()
{
	int option;
	cout<<"Fuzzy inference system:\nSelect mode:\n\t1.Learning\n\t2.Working\n";
	cin>>option;
	fis(option);
}
