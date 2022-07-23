//#include <cstdio>
#include <string>
//#include <iostream>
#include <sstream>
//#include <cmath>
#include <fstream>

#include "definitions.h"


void store_line(std::string key, std::string value){
    pf( " key " _BLUE "'%s'" _RST "  value " _YELLOW "'%s'\n" _RST, key.c_str(), value.c_str());
}            

int main(int argc, char * argv []) 
{ 
    pf_green("Parse config..\n");

    std::ifstream is_file("test_5.cfg");

    std::string line = "";
    while( std::getline(is_file, line) )
    {
        std::istringstream is_line(line);
        std::string key;
        if( std::getline(is_line, key, '=') )
        {
            std::string value;
            if( std::getline(is_line, value) ) 
            store_line(key, value);
        }
    }
}