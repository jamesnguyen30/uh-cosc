#include <iostream>
#include "ArgumentManager.h"
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

int** read_matrix(string filename, int &size);
// bool read_matrix(const char* filename, std::vector<std::vector<double> >& m);
void write_matrix(const char *filename, const std::vector<std::vector<double>> &o);
bool is_number(const std::string &s);
int get_size(const std::string &str);
void write_matrix(const string filename, const int size, int **matrix, const int harvested, const int rotten);
void print_matrix(const int size, int **a);

int main(int argc, char *argv[])
{
	ArgumentManager am(argc, argv);

	string inputFile = am.get("input");
	string outputFile = am.get("output");

	// inputFile = "input2.txt";
	// outputFile = "test_ouput.txt";

	int size;
	int **a;
	a = read_matrix(inputFile, size);

	int harvestTotal = 0;
	int rottenTotal = 0;
	int ripeLevel = 6;
	int rottenLevel = 9;

	//harvest if the number of riped kiwi is at least 50%
	int clearCondition = .5 * size;

	for (int k = 0; k < 7; k++)
	{

		for (int i = 0; i < size; i++)
		{

			int riped = 0;
			int rotten = 0;

			for (int j = 0; j < size; j++)
			{
				//check ripne levelw
				if (a[i][j] >= ripeLevel)
				{
					riped++;

					if (a[i][j] >= rottenLevel)
					{
						rotten++;
					}
				}
			}

			if (riped >= clearCondition)
			{
				for (int j = 0; j < size; j++)
				{
					a[i][j] = 0;
				}

				harvestTotal += riped;
				rottenTotal += rotten;
			}
			else
			{
				for (int j = 0; j < size; j++)
				{
					//check ripne level
					a[i][j]++;
				}
			}

			riped = 0;
			rotten = 0;
		}
	}

	for (int i = 0; i < size; i++)
	{

		int riped = 0;
		int rotten = 0;

		for (int j = 0; j < size; j++)
		{
			//check ripne level
			if (a[i][j] >= ripeLevel)
			{
				riped++;

				if (a[i][j] >= rottenLevel)
				{
					rotten++;
				}
			}
		}

		if (riped >= clearCondition)
		{
			for (int j = 0; j < size; j++)
			{
				a[i][j] = 0;
			}

			harvestTotal += riped;
			rottenTotal += rotten;
		}
		riped = 0;
		rotten = 0;
	}

	print_matrix(size, a);

	std::cout << "Harvested kiwis: " << harvestTotal - rottenTotal << endl;
	std::cout << "Rotten kiwis: " << rottenTotal << endl;
	
	write_matrix(outputFile, size, a, harvestTotal, rottenTotal);
	delete[] a;

	return 0;
}

bool is_number(const std::string &s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it))
		++it;
	return !s.empty() && it == s.end();
}

int get_size(const std::string &str)
{
	std::stringstream ss(str);
	int size = 0;
	std::string word;
	while (ss >> word)
	{
		if (is_number(word))
		{
			size = std::stoi(word);
			break;
		}
	}
	return size;
}

void print_matrix(const int size, int **a)
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			std::cout << a[i][j] << " ";
		}
		std::cout << endl;
	}

	cout<<endl;
}

int** read_matrix(string filename, int & size)
{
	std::ifstream ifs(filename);
	std::string str;
	getline(ifs, str);

	if (str.empty())
	{
		std::cout << "ERROR! Input file EMPTY!" << std::endl;
		return nullptr;
	}

	size = get_size(str);

	if (size <= 0)
	{
		std::cout << "ERROR! The size of Matrix is not valid!" << std::endl;
		return nullptr;
	}
	else
	{
		int** matrix = new int *[size];

		//Init a
		for (int i = 0; i < size; i++)
		{
			matrix[i] = new int[size];
		}

		int i = 0;
		int j = 0;

		while (getline(ifs, str))
		{
			std::stringstream ss(str);
			int num;
			while (ss >> num)
			{
				// row.push_back(num);
				matrix[i][j] = num;
				j++;
			}
			i++;
			j = 0;
		}

		return matrix;

	}
}

void write_matrix(const string filename, const int size, int **matrix, const int harvested, const int rotten)
{
	std::ofstream ofs(filename);

	for(int i =0;i<size;i++){
		ofs << matrix[i][0];
		for(int j =1;j<size;j++){
			ofs << " " << matrix[i][j];
		}
		ofs <<"\n";
	}
	ofs << endl;
	ofs << "Harvested kiwis: " << harvested - rotten << endl;
	ofs << "Rotten kiwis: " << rotten << endl;
	
}
