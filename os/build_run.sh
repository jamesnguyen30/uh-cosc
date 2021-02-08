# @ params
# $1 .cpp file to compile
# $2 output compiled

echo "[1] COMPILING >>> "
g++ -std=c++11 $1 -o $2

echo "[2] RUNNINGG >>> "
./$2 
