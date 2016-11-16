#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <stdlib.h>

#define PROFILEPATH "data/profiles.txt"
#define DATASETPATH "data/DSL-StrongPasswordData.csv"
#define BUFFER_SIZE 512
#define USERNAME_LENGTH 4					// Length of the username. ex "s001"
#define PASSWORD_LENGTH 11					// Length of the password ".tie5Roanl" + Return Key.
#define NO_OF_FUZZY_SETS 5					// No. of fuzzy sets, here 5 i.e Very Fast(0), Fast(1), Moderate(2), Slow(3), Very Slow(4)
#define NO_OF_TRIES 15						// No. of trials done to train/create fuzzy rule for a single user

#define NO_OF_USERS 50						// No. of users for this FIS.
#define NO_OF_TESTING_ATTEMPTS 200			// This is the number of test attempts of one particular user against a stored profile.

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

int writeProfileToFile(char* username, int* profile)
{
	int noOfDataItems = PASSWORD_LENGTH - 1;			// To write to file.
	ofstream fout;fout.open(PROFILEPATH, std::ios_base::app);
	fout<<username<<",";
	for (int i = 0; i < noOfDataItems; i++)
		fout<<profile[i]<<",";
	fout<<endl;
	fout.close();
	return 0;
}

// This returns the membership for the delays in an array ``vector''. It can range from 0-4, i.e from Very Fast to Very Slow
int gruntWorkForFisLearning(float* vector)
{
	int finalMembership = -1;									// Unclassified. The vector's data do not fit in any of the fuzzy sets.
	float membership_values[NO_OF_TRIES][NO_OF_FUZZY_SETS];			// This contains the membership values where each value denotes the membership value of that particular row in the column's fuzzy set.
	float weightedAverage_memberships[NO_OF_FUZZY_SETS];			// After the defuzzification, the value is again fuzzified and this array contains the membership values for the defuzzified value in the $NO_OF_FUZZY_SETS fuzzy sets
	float max = 0.0;
	float weightedSum = 0.0;float membershipSum = 0.0;			// weightedSum and membershipSum is the numerator and denominator respetively for the calculation of the weightedAverage using COG method during defuzzification.
	float weightedAverage_cog = 0.0;
	float classifiers[5][3]={	//Low value, Middle Value, High Value
							{21,		 25.5,			29},	// Very Fast(0)
							{26, 		 29,			32},	// Fast		(1)
							{30, 		 33.5,			37},	// Moderate	(2)
							{36, 		 40,			44},	// Slow		(3)
							{42,		 46,			50}		// Very Slow(4)
						};
	memset(membership_values, 0, sizeof(membership_values));
	memset(weightedAverage_memberships, 0, sizeof(weightedAverage_memberships));

	for (int i = 0; i < NO_OF_TRIES; i++)							// This finds the union of the fuzzy sets by finding the maximum of all the membership values for a particular row. It then uses this value to find the numerator and denominator to find the weightedAverage.
	{
		float max = 0.0;
		for (int j = 0; j < NO_OF_FUZZY_SETS; j++)
		{
			membership_values[i][j] = membership_value(vector[i], classifiers[j][0], classifiers[j][1], classifiers[j][2]);
			if (membership_values[i][j]>max)
				max = membership_values[i][j];
		}
		weightedSum += (max*vector[i]);							// Numerator for weighted average
		membershipSum += max;									// Denominator for weighted average
	}
	if(membershipSum!=0)
		weightedAverage_cog = weightedSum/membershipSum;		// Defuzzified value using Centre of Gravity method.
	for (int i = 0; i < NO_OF_FUZZY_SETS; i++)						// Defuzzified value is again fuzzified in this step.
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
	int finalMembership = -1;						// This denotes the final membership of the delays given in the vector. That is, whether the delays given in the vector is Very Fast, Fast, Moderate, Slow, Very Slow.
	char* username = new char[USERNAME_LENGTH+1];
	char* value = new char[6];
	int profile[PASSWORD_LENGTH-1];memset(profile, 0, sizeof(profile));
	float delays[PASSWORD_LENGTH-1][NO_OF_TRIES];memset(delays, 0, sizeof(delays));
	char* fbuff = new char[BUFFER_SIZE];			// File Buffer

	ifstream fin;fin.open(DATASETPATH, ios::in);
	fin.getline(fbuff, BUFFER_SIZE);				// Removes the first line from the file

	for(int i = 0; i<NO_OF_USERS; i++)						// For each user, create the rules
	{
		memset(delays, 0, sizeof(delays));
		for (int j = 0; j < NO_OF_TRIES; j++)			// By each user's, for each attempt out of his/her 15(``NO_OF_TRIES'') attempts at the password, fill up a column in a plane
		{
			fin.getline(fbuff, BUFFER_SIZE);						// Read one entire line of CSV into fbuff
			string fbuffString(fbuff);								// Create a std::string fbuffString from char* fbuff
			istringstream istr(fbuffString);						// Create a stream from the std::string fbuffString
			istr.getline(username, USERNAME_LENGTH+1, ',');			// Extract username from fbuffString's stream
			if(j==0)	cout<<"Profiling for "<<username<<" starting...";

			int position = 0;
			for (int k = 1; k <= 33; k++)							// For each attempt at password, 10 values will be stored in a column
			{
				istr.getline(value, 100, ',');						// Extracts next value from fbuffString's stream
				// if(k==5 || k==8 || k==11 || k==14 || k==17 || k==20 || k==23 || k==26 || k==29 || k==32)	// UD.key1.key2 values from the dataset are used to denote the delay between the keys.
				if(k==4 || k==7 || k==10 || k==13 || k==16 || k==19 || k==22 || k==25 || k==28 || k==31)	// DD.key1.key2 values from the dataset are used to denote the delay between the keys.
				{
					float temp = atof(value);
					if(temp<0)
						delays[position][j] = 0;
					else
						delays[position][j] = temp*100;
					position++;
				}
			}
		}
		for (int j = NO_OF_TRIES; j < 400; j++)						// This will discard the rest of the 385 lines of CSV and move the file pointer to the next user.
			fin.getline(fbuff, BUFFER_SIZE);						// Read one entire line of CSV into fbuff

		for (int j = 0; j < 10; j++)
		{
			finalMembership = gruntWorkForFisLearning(delays[j]);
			profile[j] = finalMembership;
		}
		if(writeProfileToFile(username, profile)==0)
			cout<<"Profile of "<<username<<" appended to "<<PROFILEPATH<<endl;
		else
			cout<<"Error in writing to file.";
	}

	delete(username);
	fin.close();
	delete(fbuff);
}

void retrieveStoredProfile(int* storedProfile, char* username)
{
	char* fbuff = new char[BUFFER_SIZE];			// File Buffer
	char* testUsername = new char[USERNAME_LENGTH+1];
	char* value = new char[6];
	ifstream fin;fin.open(PROFILEPATH, ios::in);
	do
	{
		fin.getline(fbuff, BUFFER_SIZE);
		string fbuffString(fbuff);								// Create a std::string fbuffString from char* fbuff
		istringstream istr(fbuffString);						// Create a stream from the std::string fbuffString
		istr.getline(testUsername, USERNAME_LENGTH+1, ',');		// Extract username from fbuffString's stream
		if(strcmp(username, testUsername)==0)
		{
			for (int i = 0; i < 10; i++)
			{
				istr.getline(value, 100, ',');					// Extracts next value from fbuffString's stream
				storedProfile[i] = atof(value);
			}
		}
	} while(fin);

	fin.close();
	delete(fbuff);
	delete(testUsername);
}

float checkSimilarityOfProfiles(int* storedProfile, int* testProfile)
{
	int matchingValues = 0;
	for (int i = 0; i < PASSWORD_LENGTH-1; i++)
	{
		if(storedProfile[i]==testProfile[i])
		{	matchingValues++;
			// cout<<storedProfile[i]<<" "<<testProfile[i]<<endl;
		}
	}
	float percentMatch = ((float)matchingValues/(PASSWORD_LENGTH-1))*100;
//	printf("%02.0f%%\n", percentMatch);
	return percentMatch;
	/*	printf("matchingValues=%d\n", matchingValues);
		cout<<"\t";
		for (int i = 0; i < PASSWORD_LENGTH-1; i++)
			printf("%2d ", storedProfile[i]);
		cout<<endl;cout<<"\t";
		for (int i = 0; i < PASSWORD_LENGTH-1; i++)
			printf("%2d ", testProfile[i]);
		cout<<endl;
	*/
}

// This function classifies each value into a fuzzy set and stores the values of the fuzzy set in testProfile.
int gruntWorkForFisWorking(float* vector, int* testProfile)
{
	float membership_values[PASSWORD_LENGTH-1][NO_OF_FUZZY_SETS];	// This contains the membership values where each value denotes the membership value of that particular row in the column's fuzzy set.
	float max = 0.0;
	float classifiers[5][3]={	//Low value, Middle Value, High Value
							{21,		 25.5,			29},	// Very Fast(0)
							{26, 		 29,			32},	// Fast		(1)
							{30, 		 33.5,			37},	// Moderate	(2)
							{36, 		 40,			44},	// Slow		(3)
							{42,		 46,			50}		// Very Slow(4)
						};
	memset(membership_values, 0, sizeof(membership_values));

	for (int i = 0; i < PASSWORD_LENGTH-1; i++)						// This finds the union of the fuzzy sets by finding the maximum of all the membership values for a particular row. It then uses this value to find the numerator and denominator to find the weightedAverage.
	{
		float max = 0.0;
		int finalMembership = -1;
		for (int j = 0; j < NO_OF_FUZZY_SETS; j++)
		{
			membership_values[i][j] = membership_value(vector[i], classifiers[j][0], classifiers[j][1], classifiers[j][2]);
			if (membership_values[i][j]>max)
			{
				max = membership_values[i][j];
				finalMembership = j;
			}
		}
		testProfile[i] = finalMembership;
	}
}

// Fuzzy Inference System working/testing component
void fis_working()
{
	char* username = new char[USERNAME_LENGTH+1];
	char* value = new char[6];
	int testProfile[PASSWORD_LENGTH-1];memset(testProfile, 0, sizeof(testProfile));
	int storedProfile[PASSWORD_LENGTH-1];memset(storedProfile, 0, sizeof(storedProfile));
	float testDelays[PASSWORD_LENGTH-1];memset(testDelays, 0, sizeof(testDelays));
	char* fbuff = new char[BUFFER_SIZE];			// File Buffer
	int repetition = -1;
	int session = -1;

	ifstream fin;fin.open(DATASETPATH, ios::in);
	fin.getline(fbuff, BUFFER_SIZE);				// Removes the first line from the file

	for(int i = 0; i<NO_OF_USERS; i++)			// For each user, create the rules
	{
		float similarityPercent = 0.0; float meanSimilarityPercent = 0.0;
		for (int j = 0; j < NO_OF_TRIES; j++)						// This will discard the first NO_OF_TRIES lines of CSV.
			fin.getline(fbuff, BUFFER_SIZE);						// Read one entire line of CSV into fbuff

		memset(testDelays, 0, sizeof(testDelays));
		for(int j = NO_OF_TRIES; j<(NO_OF_TRIES+NO_OF_TESTING_ATTEMPTS); j++)
		{
			fin.getline(fbuff, BUFFER_SIZE);						// Read one entire line of CSV into fbuff
			string fbuffString(fbuff);								// Create a std::string fbuffString from char* fbuff
			istringstream istr(fbuffString);						// Create a stream from the std::string fbuffString
			istr.getline(username, USERNAME_LENGTH+1, ',');			// Extract username from fbuffString's stream

			int position = 0;
			for (int k = 1; k <= 33; k++)							// For each attempt at password, 10 values will be stored in a column
			{
				istr.getline(value, 100, ',');						// Extracts next value from fbuffString's stream
				if(k==1)	session = atoi(value);
				if(k==2)	repetition = atoi(value);
				// if(k==5 || k==8 || k==11 || k==14 || k==17 || k==20 || k==23 || k==26 || k==29 || k==32)	// UD.key1.key2 values from the dataset are used to denote the delay between the keys.
				if(k==4 || k==7 || k==10 || k==13 || k==16 || k==19 || k==22 || k==25 || k==28 || k==31)	// DD.key1.key2 values from the dataset are used to denote the delay between the keys.
				{
					float temp = atof(value);
					if(temp<0)
						testDelays[position] = 0;
					else
						testDelays[position] = temp*100;
					position++;
				}
			}
			if(j==NO_OF_TRIES)
				cout<<"Testing for "<<username<<" starting...";
			// cout<<"Testing against: S:<session index> R:<repetition number>"<<endl;
			// cout<<"S:"<<session<<" R:";printf("%02d-->", repetition);
			gruntWorkForFisWorking(testDelays, testProfile);
			retrieveStoredProfile(storedProfile, username);
			similarityPercent += checkSimilarityOfProfiles(storedProfile, testProfile);
		}
		meanSimilarityPercent = similarityPercent/NO_OF_TESTING_ATTEMPTS;
		cout<<"Mean similarity percent:"<<meanSimilarityPercent<<"%"<<endl;
		for (int j = (NO_OF_TRIES+NO_OF_TESTING_ATTEMPTS); j < 400; j++)	// This will discard the rest of the (400-(NO_OF_TRIES+NO_OF_TESTING_ATTEMPTS)) lines of CSV and move the file pointer to the next user.
			fin.getline(fbuff, BUFFER_SIZE);						// Read one entire line of CSV into fbuff
	}

	delete(username);
	fin.close();
	delete(fbuff);
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
