g++ main.cpp -o main.out -I./lib

if [[ $? -eq 0 ]]
then
    ./main.out $1 $2
fi

if [[ $? -eq 0 ]]
then
    ./plot1.sh
fi