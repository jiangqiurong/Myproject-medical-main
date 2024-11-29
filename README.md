# Myproject-medical-main

基于C/C++做的智慧医疗服务系统

客户端：
arm-linux-g++ -std=c++11 src/*.cpp -I ./inc -o c -L./inc -lfont -pthread -lm

服务器：
gcc *.c -o a -pthread
gcc *.c -o b -pthread
