# @ params
# $1 .cpp file to compile
# $2 output compiled

echo "[1] COMPILING >>> "
g++ -std=c++11 client.cpp -o client

echo "[2] RUNNINGG CLIENT>>> "
./client localhost 3000 < input.txt 
