# @ params

echo "[1] COMPILING >>> "
g++ -std=c++11 server.cpp -o server

echo "[2] RUNNINGG SERVER>>> "
./server 3000 
