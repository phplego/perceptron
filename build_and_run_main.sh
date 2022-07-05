clear

g++ main.cpp -o main -I./lib

if [[ $? -eq 0 ]]
then
    ./main $1 $2
    ./plot1.sh
fi