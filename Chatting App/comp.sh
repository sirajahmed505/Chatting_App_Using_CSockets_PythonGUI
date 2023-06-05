g++ -o Server Server.cpp -lpthread
g++ -o Client Client.cpp -std=c++11 -I/usr/include/python3.8 -lpython3.8 -lpthread

#Used to compile. can compile multiple space separated cpp or c programs.
#for file in $@
#do
#    g++ -o ${file%.cpp} $file -lpthread
#done
