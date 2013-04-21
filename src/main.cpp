#include "fastgenematch.h"

int main()
{
	fastgenematch::Geneobject g1;
    g1("apple")="delicious";
    g1("orange")="nutritious";
    std::cout<<" Lookup 1 for key apple "<<g1["apple"]<<" orange "<<g1["orange"]<<std::endl;
    std::cout<<" Missing Lookup for key pear "<<g1["pear"]<<std::endl;
    return 1;
};