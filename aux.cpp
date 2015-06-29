#include <string>
#include <algorithm>
#include <iostream>

/*For example, to read the name of a file 
after a -f command line argument. You can
also just detect if a single-word option
has been passed in like -h for help.
source: http://stackoverflow.com/a/868894*/

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

/*int main(int argc, char * argv[])
{
    if(cmdOptionExists(argv, argv+argc, "-h"))
    {
        // Do stuff
    }

    char * filename = getCmdOption(argv, argv + argc, "-f");

    if (filename)
    {
        // Do interesting things
        // ...
    }

    return 0;
}*/