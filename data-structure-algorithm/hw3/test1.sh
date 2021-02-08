#Compile the code
# if the file calculate exits then remove it
for i in `seq 1 1 9`; do
    if [ -f output3${i}.txt ]; then
        rm output3${i}.txt
    fi
done

if [ -f bitree ]; then
    rm bitree
fi
g++ -std=c++11 *.cpp -o bitree
# if the file densemult does not exit then exit the test
if [ ! -f bitree ]; then
    echo -e "\033[1;91mCompile FAILED.\033[0m"
    exit
fi
# clean
for i in `seq 1 1 9`; do
    if [ -f ${i}.out ]; then
        rm ${i}.out
    fi
    if [ -f ${i}.stderr ]; then
        rm ${i}.stderr
    fi
    if [ -f ${i}.stdcout ]; then
        rm ${i}.stdcout
    fi
done
# For test case 1
# input 1.txt and output 1.out
timeout -k 5s 5s ./bitree "input=input41.txt;command=command41.txt;output=output41.txt" 1>1.stdcout 2>1.stderr

diff -iEBwu ans41.txt output41.txt > 1.diff

if [ $? -ne 0 ]; then
    
    echo -e "Test case 1    [1;91mFAILED.[0m"
else
    
    echo -e "Test case 1    [1;92mPASSED.[0m"

    rm -f 1.diff
fi


timeout -k 5s 5s ./bitree "input=input42.txt;command=command42.txt;output=output42.txt" 1>1.stdcout 2>1.stderr

diff -iEBwu ans42.txt output42.txt > 2.diff

if [ $? -ne 0 ]; then
    
    echo -e "Test case 2    [1;91mFAILED.[0m"
else
    
    echo -e "Test case 2    [1;92mPASSED.[0m"

    rm -f 2.diff
fi


timeout -k 5s 5s ./bitree "input=input43.txt;command=command43.txt;output=output43.txt" 1>1.stdcout 2>1.stderr

diff -iEBwu ans43.txt output43.txt > 3.diff

if [ $? -ne 0 ]; then
    
    echo -e "Test case 3    [1;91mFAILED.[0m"
else
    
    echo -e "Test case 3    [1;92mPASSED.[0m"

    rm -f 3.diff
fi


timeout -k 5s 5s ./bitree "input=input44.txt;command=command44.txt;output=output44.txt" 1>1.stdcout 2>1.stderr

diff -iEBwu ans44.txt output44.txt > 4.diff

if [ $? -ne 0 ]; then
    
    echo -e "Test case 4    [1;91mFAILED.[0m"
else
    
    echo -e "Test case 4    [1;92mPASSED.[0m"

    rm -f 4.diff
fi


timeout -k 5s 5s ./bitree "input=input45.txt;command=command45.txt;output=output45.txt" 1>1.stdcout 2>1.stderr

diff -iEBwu ans45.txt output45.txt > 5.diff

if [ $? -ne 0 ]; then
    
    echo -e "Test case 5    [1;91mFAILED.[0m"
else
    
    echo -e "Test case 5    [1;92mPASSED.[0m"

    rm -f 5.diff
fi


timeout -k 5s 5s ./bitree "input=input46.txt;command=command46.txt;output=output46.txt" 1>1.stdcout 2>1.stderr

diff -iEBwu ans46.txt output46.txt > 6.diff

if [ $? -ne 0 ]; then
    
    echo -e "Test case 6    [1;91mFAILED.[0m"
else
    
    echo -e "Test case 6    [1;92mPASSED.[0m"

    rm -f 6.diff
fi


timeout -k 5s 5s ./bitree "input=input47.txt;command=command47.txt;output=output47.txt" 1>1.stdcout 2>1.stderr

diff -iEBwu ans47.txt output47.txt > 7.diff

if [ $? -ne 0 ]; then
    
    echo -e "Test case 7    [1;91mFAILED.[0m"
else
    
    echo -e "Test case 7    [1;92mPASSED.[0m"

    rm -f 7.diff
fi


timeout -k 5s 5s ./bitree "input=input48.txt;command=command48.txt;output=output48.txt" 1>1.stdcout 2>1.stderr

diff -iEBwu ans48.txt output48.txt > 8.diff

if [ $? -ne 0 ]; then
    
    echo -e "Test case 8    [1;91mFAILED.[0m"
else
    
    echo -e "Test case 8    [1;92mPASSED.[0m"

    rm -f 8.diff
fi


timeout -k 5s 5s ./bitree "input=input49.txt;command=command49.txt;output=output49.txt" 1>1.stdcout 2>1.stderr

diff -iEBwu ans49.txt output49.txt > 9.diff

if [ $? -ne 0 ]; then
    
    echo -e "Test case 9    [1;91mFAILED.[0m"
else
    
    echo -e "Test case 9    [1;92mPASSED.[0m"

    rm -f 9.diff
fi