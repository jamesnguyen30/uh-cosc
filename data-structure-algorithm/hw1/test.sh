#Compile the code
# if the file densemult exits then remove it
if [ -f kiwi ]; then
    rm kiwi
fi
g++ -std=c++11 *.cpp -o kiwi
# if the file densemult does not exit then exit the test
if [ ! -f kiwi ]; then
    echo -e "\033[1;91mCompile FAILED.\033[0m"
    exit
fi
# clean
if [ -f *.out ]; then
    rm *.out
fi
if [ -f *.stderr ]; then
    rm *.stderr
fi
if [ -f *.stdcout ]; then
    rm *.stdcout
fi
#rm *.out *.stderr *.stdcout
# For test case 1
# input 1.txt and output 1.out
./kiwi "input=input1.txt;output=output1.txt" 1>1.stdcout 2>1.stderr
# compare 1.out with 1.ans, output the difference to 1.diff
diff -iEBwu ans1.txt output1.txt > 1.diff
# if diff returns nothing, it means you pass the test case (Your ourput file 1.out is correct)
if [ $? -ne 0 ]; then
    # display "test case 1 FAILED" to the screen. The word "FAILED" is highlighted in Red color
    echo -e "Test case 1    \033[1;91mFAILED.\033[0m"
else
    # display "test case 1 PASSED" to the screen. The word "PASSED" is highlighted in Green color
    echo -e "Test case 1    \033[1;92mPASSED.\033[0m"
    # remove the file 1.diff
    rm -f 1.diff
fi
# For test case 2
./kiwi "input=input2.txt;output=output2.txt" 1>1.stdcout 2>1.stderr
diff -iEBwu ans2.txt output2.txt > 2.diff
if [ $? -ne 0 ]; then
    echo -e "Test case 2    \033[1;91mFAILED.\033[0m"
else
    echo -e "Test case 2    \033[1;92mPASSED.\033[0m"
    rm -f 2.diff
fi
# For test case 3
./kiwi "input=input3.txt;output=output3.txt" 1>1.stdcout 2>1.stderr
diff -iEBwu ans3.txt output3.txt > 3.diff
if [ $? -ne 0 ]; then
    echo -e "Test case 3    \033[1;91mFAILED.\033[0m"
else
    echo -e "Test case 3    \033[1;92mPASSED.\033[0m"
    rm -f 3.diff
fi



