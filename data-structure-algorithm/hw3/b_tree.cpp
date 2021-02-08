#include <iostream>
#include <math.h>
#include <vector>

using namespace std;

struct BTreeNode
{
    int *keys;
    BTreeNode **pointers;
    bool isLeaf;
    int keysLength;
    int degree;

    BTreeNode(int _degree, bool _isLeaf) : degree(_degree), isLeaf(_isLeaf)
    {
        //keys must has maximum length at degree - 1
        //pointers must has max length at degree
        //The extra one space is for insertion when the arrays are full
        keysLength = 0;
        keys = new int[degree];
        pointers = new BTreeNode *[degree + 1];

    }
};

class BTree
{
private:
    BTreeNode *root;
    int degree;
    int height = 0;
    
    vector<vector<vector<int>>> levels;

public:
    BTree(int _degree) : degree(_degree)
    {
        root = nullptr;
        height = 0;
    }

    bool isFull(BTreeNode *node)
    {
        return (node->keysLength >= degree);
    }

    void sort(int *arr, int size)
    {
        int i, key, j;
        for (i = 1; i < size; i++)
        {
            key = arr[i];
            j = i - 1;
            while (j >= 0 && arr[j] > key)
            {
                arr[j + 1] = arr[j];
                j = j - 1;
            }
            arr[j + 1] = key;
        }
    }

    vector<int> traverse(){
        vector<int> result; 
        if(root!=nullptr){
            traverseRecursively(root, result);
        }

        return result;
    }

    void traverseRecursively(BTreeNode* node, vector<int> &output){
        int i =0;
        for(i =0;i<node->keysLength;i++){
            if(node->isLeaf==false){
                traverseRecursively(node->pointers[i], output);
            }
            output.push_back(node->keys[i]);
        }

        if(node->isLeaf==false){
            traverseRecursively(node->pointers[i], output);
        }

    }

    void getLevels(){
    
        levels.clear();
        getLevelsRecursively(root, 1, levels);

    }

    vector<int> printLevel(int level){

        vector<int> output;
        //Vector levels start at 0, but hw4 requires the root start at level 1
        //so our level is level - 1
        level = level -1;
        vector<vector<int>> temp;
        if(levels.size() <= level || level<0){
            cout<<"empty";
            return output;
        }
        temp = levels.at(level);
        for(int i =0;i<temp.size();i++){
            for(int j =0;j<temp.at(i).size();j++){
                cout<< temp.at(i).at(j)<<" ";
                output.push_back(temp.at(i).at(j));
            }
        }

        return output;
    }


    void getLevelsRecursively(BTreeNode* node, int level, vector<vector<vector<int>>> &levels){
        bool isExisted = false;
        if(levels.size() >= level){
            isExisted = true;
        }

        vector<int> keys;
        for(int i =0;i<node->keysLength;i++){
            // cout<<node->keys[i]<<" ";
            keys.push_back(node->keys[i]);
        }

        if(isExisted){
            levels.at(level-1).push_back(keys);
        } else {
            vector<vector<int>> temp;
            temp.push_back(keys);
            levels.push_back(temp);
        }
        if(node->isLeaf){
            return;
        } else {
            for(int i =0;i<=node->keysLength;i++){
                getLevelsRecursively(node->pointers[i], level+1, levels);
            }

        }
    }
    void insert(int k)
    {
        if (root == nullptr)
        {
            root = new BTreeNode(degree, true);
        }
        BTreeNode *tempRoot = insertRecursively(root, k);
        if (tempRoot != nullptr)
        {
            root = tempRoot;
        }
    }


    BTreeNode *split(BTreeNode *node)
    {
        int mid = static_cast<int>(floor(static_cast<double>(degree - 1.0) / 2.0));
        int mid_element = node->keys[mid];
        node->keysLength = mid;
        BTreeNode *rightHalf = new BTreeNode(degree, node->isLeaf);

        for (int i = mid + 1; i < degree; i++)
        {
            rightHalf->keys[i - (mid + 1)] = node->keys[i];
            rightHalf->keysLength++;
        }

        //Remove the mid element

        if (node->isLeaf == false)
        {
            for (int i = mid + 1; i < degree + 1; i++)
            {
                if (node->pointers[i] != nullptr)
                {
                    rightHalf->pointers[i - (mid + 1)] = node->pointers[i];
                }
            }
        }

        BTreeNode *newRoot = new BTreeNode(degree, false);

        newRoot->keys[0] = mid_element;

        newRoot->keysLength++;

        newRoot->pointers[0] = node;

        newRoot->pointers[1] = rightHalf;

        return newRoot;
    }

    BTreeNode *insertRecursively(BTreeNode *node, int k)
    {
        //If the current node is a leaf, we insert the k to the keys
        //if the keys is full, we insert k value, sort the keys, and split it to new node
        if (node->isLeaf == true)
        {
            node->keys[node->keysLength++] = k;
            sort(node->keys, node->keysLength);


            if (isFull(node) == true)
            {
                //insert at the last element and sort the keys array
                BTreeNode *newChild = split(node);

                return newChild;
            }
            else
            {
                return nullptr;
            }
        }
        else
        {
            //the current node is not a leaf, we want to go to an appropriate child to insert k
            int i = 0;
            while (i < node->keysLength && k > node->keys[i])
            {
                i++;
            }
    
            BTreeNode *y = insertRecursively(node->pointers[i], k);

            if (y != nullptr)
            {
                //this child has been splitted,
                // add its new root to to current keys and pointers
                int childKey = y->keys[0];
                int j = node->keysLength - 1;
                while (j >= i)
                {
                    node->keys[j + 1] = node->keys[j];
                    node->pointers[j + 2] = node->pointers[j + 1];
                    j--;
                }

                node->keysLength++;
                node->keys[i] = childKey;
                node->pointers[i + 1] = y->pointers[1];
                if (isFull(node))
                {
                    node->isLeaf = false;
                    BTreeNode *newChild = split(node);
                    return newChild;
                }

                else
                {
                    return nullptr;
                }
            }
            else
            {
                return nullptr;
            }
        }
    }
};