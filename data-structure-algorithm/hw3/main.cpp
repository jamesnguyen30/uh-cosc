#include <iostream>
#include "b_tree.cpp"
#include "ArgumentManager.h"
#include <vector>
#include <fstream>
#include <string>
using namespace std;

vector<string> readFile(string inputFile)
{
    ifstream ifs(inputFile);
    string line;
    vector<string> lines;
    getline(ifs, line);
    if (line.empty())
    {
        cout << "FILE IS EMPTY" << endl;
        return lines;
    }

    ifs.seekg(0);

    while (getline(ifs, line))
    {
        istringstream ss(line);

        if (line == "")
        {
            continue;
        }

        // Traverse through all words

        do
        {
            // Read a word
            string num;
            ss >> num;
            if (num != "")
            {
                if (num[0] == 'L' || num[0] == 'T')
                {
                    continue;
                }
                lines.push_back(num);
            }
        } while (ss);
    }

    return lines;
}

int extractNumberFromCommand(string levelCommand, char delim)
{
    //Specifically designed for this hw4
    string level;
    for (int i = 0; i < levelCommand.size(); i++)
    {
        if (levelCommand[i] == delim)
        {
            return stoi(levelCommand.substr(i + 1, levelCommand.size() - 1));
        }
    }
}

void removeDuplicates(vector<int> &array)
{
    for (int i = 0; i < array.size() - 1; i++)
    {
        for (int j = i + 1; j < array.size(); j++)
        {
            if (array[j] == array[i])
            {
                array.erase(array.begin() + j);
                j--;
            }
        }
    }
}

string run(vector<int> lines, vector<string> cmds, string outputFile)
{

    ofstream ofs(outputFile);
    if(lines.size()==0){
        ofs <<"";
        return "";
    }
    int degree = extractNumberFromCommand(cmds.at(0), '=');
    BTree btree(degree);

    for (int i = 0; i < lines.size(); i++)
    {
        btree.insert(lines.at(i));
    }

    btree.getLevels();

    vector<int> output;

    int lv = 0;

    for (int i = 1; i < cmds.size(); i++)
    {
        output.clear();
        if (cmds.at(i) == "")
        {
            //empty
            //return "empty"
            continue;
        }
        //Commands starts with I means Inorder Traversal
        //Commands have a number only means: print the level at that number
        if (cmds.at(i)[0] != 'I')
        {
            //this is level;
            lv = stoi(cmds.at(i));
            output = btree.printLevel(lv);

        }
        else
        {
            output = btree.traverse();
        }
        if (output.size() == 0)
        {
            ofs << "empty" << endl;
        }
        else
        {
            for (int i = 0; i < output.size(); i++)
            {
                ofs << output.at(i) << " ";
            }
            ofs << endl;
        }
    }

    return "";
}

int main(int argc, char *argv[])
{
    ArgumentManager am(argc, argv);

    string inputFile = am.get("input");
    string outputFile = am.get("output");
    string commandFile = am.get("command");

    vector<string> cmds = readFile(commandFile);

    vector<string> tempLines = readFile(inputFile);
    //convert lines from string to int
    vector<int> lines;
    for (int i = 0; i < tempLines.size(); i++)
    {
        lines.push_back(stoi(tempLines.at(i)));
    }
    removeDuplicates(lines);

    run(lines, cmds, outputFile);
    cout << endl;
}