#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <stdlib.h>

using namespace std;

int main()
{
	const int BUFFER_SIZE = 512;
	const int noOfUsers = 50;			// No. of users for which this model is being built
	const int passwordLength = 11;		// Length of the password ".tie5Roanl" + Return Key.
	const int noOfTries = 15;			// No. of tries done to train/create fuzzy rule for a single user
	char* fbuff = new char[BUFFER_SIZE];		// File Buffer
	ifstream fin;
	fin.open("data/DSL-StrongPasswordData.csv", ios::in);

	fin.getline(fbuff, BUFFER_SIZE);			// Removes the first line from the file
	const int USERNAME_LENGTH = 4;				// Length of the username. ex "s001"
	char* username = new char[USERNAME_LENGTH+1];
	char* value = new char[6];
	float delays[passwordLength-1][noOfTries];memset(delays, 0, sizeof(delays));

	for(int i = 0; i<50; i++)
	{
		memset(delays, 0, sizeof(delays));
		for (int j = 0; j < noOfTries; j++)			// By each user's, for each attempt out of his/her 15(``noOfTries'') attempts at the password, fill up a column in a plane
		{
			fin.getline(fbuff, BUFFER_SIZE);						// Read one entire line of CSV into fbuff
			string fbuffString(fbuff);								// Create a std::string fbuffString from char* fbuff
			istringstream istr(fbuffString);						// Create a stream from the std::string fbuffString
			istr.getline(username, USERNAME_LENGTH+1, ',');			// Extract username from fbuffString's stream

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
						delays[position][j] = temp;
					position++;
				}
			}
		}
		for (int j = noOfTries; j < 400; j++)						// This will discard the rest of the 385 lines of CSV and move the file pointer to the next user.
			fin.getline(fbuff, BUFFER_SIZE);						// Read one entire line of CSV into fbuff

		for (int j = 0; j < 10; j++)
		{
			for (int k = 0; k < 15; k++)
				cout<<delays[j][k]<<" ";
			cout<<endl;
		}
	}
	delete username;
	fin.close();
	delete(fbuff);
	return 0;
}
