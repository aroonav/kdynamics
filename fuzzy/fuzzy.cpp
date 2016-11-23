#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <stdlib.h>

#define PROFILEPATH "profile/profiles.txt"
#define DATASETPATH "../data/DSL-StrongPasswordData.csv"
#define BUFFER_SIZE 512
#define USERNAME_LENGTH 4					// Length of the username. ex "s001"
#define PASSWORD_LENGTH 11					// Length of the password ".tie5Roanl" + Return Key.
#define UNCLASSIFIED_VALUE -10				// Value denoting Unclassification.
#define NO_OF_TIMING_FEATURES 31

using namespace std;

#define NO_OF_FUZZY_SETS 7					// No. of fuzzy sets.
#define NO_OF_TRIES 200						// No. of trials done to train/create fuzzy rule for a single user.
#define NO_OF_USERS 50						// No. of users for this FIS.
#define NO_OF_TESTING_ATTEMPTS 200			// This is the number of test attempts of one particular user against his/her stored profile.
float classifiers[NO_OF_FUZZY_SETS][3]={	//Low value, Middle Value, High Value
											{6,			12,		18},	// Very Fast part 1
											{15,		21,		27},	// Very Fast part 2
											{24,		30,		36},	// Very Fast part 3
											{28, 		44,		60},	// Fast
											{50, 		66,		82},	// Moderate
											{72, 		88,		104},	// Slow
											{94,		110,	126}	// Very Slow
									};

// Returns the membership value for the given input depending on the other input variables
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
	int noOfDataItems = NO_OF_TIMING_FEATURES;			// To write to file.
	ofstream fout;fout.open(PROFILEPATH, std::ios_base::app);
	fout<<username<<",";
	for (int i = 0; i < noOfDataItems; i++)
		fout<<profile[i]<<",";
	fout<<endl;
	fout.close();
	return 0;
}

// This returns the membership for the delays in an array ``vector''. It can range from 0 to NO_OF_FUZZY_SETS-1 or UNCLASSIFIED_VALUE.
int gruntWorkForFisLearning(float* vector)
{
	int finalMembership = UNCLASSIFIED_VALUE;						// Unclassified. The vector's data do not fit in any of the fuzzy sets.
	float membership_values[NO_OF_TRIES][NO_OF_FUZZY_SETS];			// This contains the membership values where each value denotes the membership value of that particular row in the column's fuzzy set.
	float weightedAverage_memberships[NO_OF_FUZZY_SETS];			// After the defuzzification, the value is again fuzzified and this array contains the membership values for the defuzzified value in the $NO_OF_FUZZY_SETS fuzzy sets
	float max = 0.0;
	float weightedSum = 0.0;float membershipSum = 0.0;				// weightedSum and membershipSum is the numerator and denominator respetively for the calculation of the weightedAverage using COG method during defuzzification.
	float weightedAverage_cog = 0.0;
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
		weightedSum += (max*vector[i]);								// Numerator for weighted average
		membershipSum += max;										// Denominator for weighted average
	}
	if(membershipSum!=0)
		weightedAverage_cog = weightedSum/membershipSum;			// Defuzzified value using Centre of Gravity method.

// //	This block is for testing purposes only. This block allows us to see the values in realtime.
// 		cout<<"##################Part1\n";
// 		for (int i = 0; i < NO_OF_TRIES; i++)
// 		{
// 			cout<<vector[i]<<" ";
// 			for (int j = 0; j < NO_OF_FUZZY_SETS; j++)
// 				cout<<membership_values[i][j]<<" ";
// 			cout<<endl;
// 		}
// //	Testing block ends here.

	for (int i = 0; i < NO_OF_FUZZY_SETS; i++)						// Defuzzified value is again fuzzified in this step.
	{
		weightedAverage_memberships[i] = membership_value(weightedAverage_cog, classifiers[i][0], classifiers[i][1], classifiers[i][2]);
		if(weightedAverage_memberships[i]>max)
		{
			finalMembership = i+1;
			max = weightedAverage_memberships[i];
		}
	}

// //	This block is for testing purposes only. This block allows us to see the values in realtime.
// 		cout<<"###################Part2\n";
// 		cout<<weightedAverage_cog<<" ";
// 		for (int i = 0; i < NO_OF_FUZZY_SETS; i++)
// 		{
// 			cout<<weightedAverage_memberships[i]<<" ";
// 		}
// 		cout<<endl;
// 		cout<<"weightedSum="<<weightedSum<<" membershipSum="<<membershipSum<<" finalMembership="<<finalMembership<<endl;
// 		scanf("%*c");
// //	This block is for testing purposes only. This block allows us to see the values in realtime.

	return finalMembership;
}

// Fuzzy Inference System learning component
void fis_learning()
{
	int finalMembership = UNCLASSIFIED_VALUE;						// This denotes the final membership of the delays given in the vector. That is, whether the delays given in the vector is Very Fast, Fast, Moderate, Slow, Very Slow. -10 denotes unclassified.
	char* username = new char[USERNAME_LENGTH+1];
	char* value = new char[6];
	int profile[NO_OF_TIMING_FEATURES];memset(profile, 0, sizeof(profile));
	float delays[NO_OF_TIMING_FEATURES][NO_OF_TRIES];memset(delays, 0, sizeof(delays));
	char* fbuff = new char[BUFFER_SIZE];							// File Buffer
	ifstream fin;fin.open(DATASETPATH, ios::in);
	fin.getline(fbuff, BUFFER_SIZE);								// Removes the first line from the file

	for(int i = 0; i<NO_OF_USERS; i++)								// For each user, create the rules
	{
		memset(delays, 0, sizeof(delays));
		for (int j = 0; j < NO_OF_TRIES; j++)						// By each user's, for each attempt out of his/her 15(``NO_OF_TRIES'') attempts at the password, fill up a column in a plane
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
				if(k>=3)
				{
					float temp = atof(value);
					if(temp<0)	delays[position][j] = 0;
					else		delays[position][j] = temp*100;
					position++;
				}
			}
		}
		for (int j = NO_OF_TRIES; j < 400; j++)						// This will discard the rest of the (400-NO_OF_TRIES) lines of CSV and move the file pointer to the next user.
			fin.getline(fbuff, BUFFER_SIZE);

		for (int j = 0; j < NO_OF_TIMING_FEATURES; j++)
		{
			finalMembership = gruntWorkForFisLearning(delays[j]);
			profile[j] = finalMembership;
		}
		if(writeProfileToFile(username, profile)==0)	cout<<"Profile of "<<username<<" appended to "<<PROFILEPATH<<endl;
		else	cout<<"Error in writing to file.";
	}
	delete(username);
	fin.close();
	delete(fbuff);
}

void retrieveStoredProfile(int* storedProfile, string username)
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
		if(strcmp(username.c_str(), testUsername)==0)
		{
			for (int i = 0; i < NO_OF_TIMING_FEATURES; i++)
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

// This returns a value between 0 and 1 and is the value of the normed euclidean distance between the two profiles.
float getEuclideanNorm(int* storedProfile, int* testProfile)
{
	float squaredEuclideanDistance=0.0;
	float normStoredProfile = 0.0;
	float normTestProfile = 0.0;
	int count = 0;
	for (int i = 0; i < NO_OF_TIMING_FEATURES; i++)
	{
		if(storedProfile[i]==-10 || testProfile[i]==-10)	// Neglect unclassified timing features. Don't use their values to calculate norm or calculate differences.
			continue;
		else
		{
			float diff = (storedProfile[i]-testProfile[i]);
			squaredEuclideanDistance += (diff*diff);
			normStoredProfile += (storedProfile[i]*storedProfile[i]);
			normTestProfile += (testProfile[i]*testProfile[i]);
			count++;
		}
	}
	// //	This block is for testing purposes only. This block allows us to see the values in realtime.
	// cout<<" count= "<<count<<" squaredEuclideanDistance="<<squaredEuclideanDistance<<" normStoredProfile="<<normStoredProfile<<" normTestProfile="<<normTestProfile<<endl;
	// //	This block is for testing purposes only. This block allows us to see the values in realtime.

	// if(normStoredProfile==0)		cerr<<"Norm of stored profile is 0."<<endl;
	// else if(normTestProfile==0)		cerr<<"Norm of test profile is 0."<<endl;

	float euclideanNorm = squaredEuclideanDistance/(normTestProfile*normStoredProfile);
	return euclideanNorm;
}

// This function classifies each value into a fuzzy set and stores the values of the fuzzy set in testProfile.
void gruntWorkForFisWorking(float* vector, int* testProfile)
{
	float membership_values[NO_OF_TIMING_FEATURES][NO_OF_FUZZY_SETS];	// This contains the membership values where each value denotes the membership value of that particular row in the column's fuzzy set.
	float max = 0.0;
	memset(membership_values, 0, sizeof(membership_values));

	for (int i = 0; i < NO_OF_TIMING_FEATURES; i++)						// This finds the union of the fuzzy sets by finding the maximum of all the membership values for a particular row. It then uses this value to find the numerator and denominator to find the weightedAverage.
	{
		float max = 0.0;
		int finalMembership = UNCLASSIFIED_VALUE;
// //	This block is for testing purposes only. This block allows us to see the values in realtime.
// 			cout<<vector[i]<<" ";
// //	This block is for testing purposes only. This block allows us to see the values in realtime.
		for (int j = 0; j < NO_OF_FUZZY_SETS; j++)
		{
			membership_values[i][j] = membership_value(vector[i], classifiers[j][0], classifiers[j][1], classifiers[j][2]);
			if (membership_values[i][j]>max)
			{
				max = membership_values[i][j];
				finalMembership = j+1;
			}
// //	This block is for testing purposes only. This block allows us to see the values in realtime.
// 			cout<<membership_values[i][j]<<" ";
// //	This block is for testing purposes only. This block allows us to see the values in realtime.
		}

// //	This block is for testing purposes only. This block allows us to see the values in realtime.
// 		cout<<endl;
// //	This block is for testing purposes only. This block allows us to see the values in realtime.
		testProfile[i] = finalMembership;
	}
}

// Fuzzy Inference System working/testing component
void fis_working()
{
	char* username = new char[USERNAME_LENGTH+1];
	char* value = new char[6];
	int testProfile[NO_OF_TIMING_FEATURES];memset(testProfile, 0, sizeof(testProfile));
	int storedProfile[NO_OF_TIMING_FEATURES];memset(storedProfile, 0, sizeof(storedProfile));
	float testDelays[NO_OF_TIMING_FEATURES];memset(testDelays, 0, sizeof(testDelays));
	char* fbuff = new char[BUFFER_SIZE];			// File Buffer
	int repetition = -1;
	int session = -1;
	string usernames[NO_OF_USERS];
	float array_euclideanNorm[NO_OF_USERS][NO_OF_USERS];

	ifstream fin;fin.open(DATASETPATH, ios::in);

	for (int i = 0; i < NO_OF_USERS; i++)							// For each ith user, compare the ith user's stored profile with jth user's profile.
	{
		float euclideanNorm_aggregate = 0.0;
		fin.clear();
		fin.seekg(0, ios::beg);										// Take the get pointer to the start of the file.
		fin.getline(fbuff, BUFFER_SIZE);							// Removes the first line from the file
		for(int j = 0; j<NO_OF_USERS; j++)
		{
			euclideanNorm_aggregate = 0.0;
			int start = 0;int end = 0;
			memset(testDelays, 0, sizeof(testDelays));

			if(i==j)	// The jth user is being tested against its own stored profile if this is true.
			{	for(int k=0; k<NO_OF_TRIES; k++)					// So skip the first NO_OF_TRIES lines that was used in the learning phase.
					fin.getline(fbuff, BUFFER_SIZE);
				start = NO_OF_TRIES;
				end = NO_OF_TRIES+NO_OF_TESTING_ATTEMPTS;
			}
			else
			{	start = 0;
				end = 5;
			}
			for(int k = start; k<end; k++)
			{
				fin.getline(fbuff, BUFFER_SIZE);					// Read one entire line of CSV into fbuff
				string fbuffString(fbuff);							// Create a std::string fbuffString from char* fbuff
				istringstream istr(fbuffString);					// Create a stream from the std::string fbuffString
				istr.getline(username, USERNAME_LENGTH+1, ',');		// Extract username from fbuffString's stream
				if(k==start)	usernames[j] = username;

				int position = 0;
				for (int l = 1; l <= 33; l++)						// For each attempt at password, 10 values will be stored in a column
				{
					istr.getline(value, 100, ',');					// Extracts next value from fbuffString's stream
					if(l==1)	session = atoi(value);
					else if(l==2)	repetition = atoi(value);
					else if(l>=3)
					{
						float temp = atof(value);
						if(temp<0)
							testDelays[position] = 0;
							else
							testDelays[position] = temp*100;
						position++;
					}
				}
// //	This block is for testing purposes only. This block allows us to see the values in realtime.
// 				cout<<"Stored profile:"<<usernames[i]<<endl;
// 				cout<<"Test profile:"<<username<<" session:"<<session<<" repetition:"<<repetition<<endl;
// //	This block is for testing purposes only. This block allows us to see the values in realtime.
				gruntWorkForFisWorking(testDelays, testProfile);
				retrieveStoredProfile(storedProfile, usernames[i]);
				float euclideanNorm = getEuclideanNorm(storedProfile, testProfile);
				euclideanNorm_aggregate += euclideanNorm;
// //	This block is for testing purposes only. This block allows us to see the values in realtime.
// 				cout<<"(Stored Profile): ";		//ith user.
// 				for (int i = 0; i < NO_OF_TIMING_FEATURES; i++)
// 					cout<<storedProfile[i]<<" ";
// 				cout<<endl;
// 				cout<<"  (Test Profile): ";		//jth user.
// 				for (int i = 0; i < NO_OF_TIMING_FEATURES; i++)
// 					cout<<testProfile[i]<<" ";
// 				cout<<"\nEuclidean Norm:"<<euclideanNorm<<endl;
// 				scanf("%*c");
// //	This block is for testing purposes only. This block allows us to see the values in realtime.
			}

			for (int k = end; k < 400; k++)	// This will discard the rest of the (400-end) lines of CSV and move the file pointer to the next user.
				fin.getline(fbuff, BUFFER_SIZE);

			array_euclideanNorm[i][j] = euclideanNorm_aggregate;
		}
	}

// //	This block is for testing purposes only. This block allows us to see the values in realtime.
// 	//	This part shows the similarity/normed squared euclidean distance of user in the ith row with the user in the jth row.
// 	//	The stored profile of the ith user is taken from the file and matched against the test profile in each of NO_OF_TESTING_ATTEMPTS attempts.
// 	//	The value printed below is the mean of all these similarities/norm distances over all NO_OF_TESTING_ATTEMPTS attempts.

// 	cout<<"\nNormed squared euclidean distances:\n     ";
// 	for (int i = 0; i < NO_OF_USERS; i++)
// 		cout<<usernames[i]<<"    ";
// 	cout<<endl;
// 	for (int i = 0; i < NO_OF_USERS; i++)
// 	{
// 		cout<<usernames[i]<<" ";
// 		for (int j = 0; j < NO_OF_USERS; j++)
// 			printf("%7.5f ", array_euclideanNorm[i][j]);
// 		cout<<endl;
// 	}
// //	This block is for testing purposes only. This block allows us to see the values in realtime.


	float userScoreAggregate = 0.0;
	float imposterScoreAggregate = 0.0;

	float allUserScoreSum = 0;
	float allImposterScoreAggregate = 0;

	float meanUserScore = 0.0;
	float meanImposterScore = 0.0;

 	for (int i = 0; i < NO_OF_USERS; i++)
	{
		// cout<<usernames[i]<<":userScoreAggregate="<<userScoreAggregate<<" imposterScoreAggregate="<<imposterScoreAggregate<<" allUserScoreSum="<<allUserScoreSum<<" allImposterScoreAggregate="<<allImposterScoreAggregate<<endl;
		userScoreAggregate = imposterScoreAggregate = 0.0;
		for (int j = 0; j < NO_OF_USERS; j++)
				if(i==j)	userScoreAggregate = array_euclideanNorm[i][j];
				else		imposterScoreAggregate += array_euclideanNorm[i][j];
		userScoreAggregate /= NO_OF_TESTING_ATTEMPTS;
		imposterScoreAggregate /= (5*(NO_OF_USERS-1));

		allUserScoreSum += userScoreAggregate;
		allImposterScoreAggregate += imposterScoreAggregate;
	}

	meanUserScore = allUserScoreSum/NO_OF_USERS;
	meanImposterScore = allImposterScoreAggregate/NO_OF_USERS;

	printf("Mean user score: %11.9f\n", meanUserScore);
	printf("Mean imposter score: %11.9f\n", meanImposterScore);

	delete(username);
	fin.close();
	delete(fbuff);
}

// Fuzzy inference system. option denotes learning or working option.
void fis(int option)
{
	if(option==1)	fis_learning();
	else			fis_working();
}

int main()
{
	int option;
	cout<<"Fuzzy inference system:\nSelect mode:\n\t1.Learning\n\t2.Working\n";
	scanf("%d%*c", &option);
	fis(option);
}
