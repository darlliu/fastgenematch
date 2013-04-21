#include "fastgenematch.h"

int main()
{
	fastgenematch::Geneobject g1,g2;
    g1("apple")="delicious";
    g1("orange")="nutritious";
    std::cout<<" Lookup 1 for key apple "<<g1["apple"]<<" orange "<<g1["orange"]<<std::endl;
    std::cout<<" Missing Lookup for key pear "<<g1["pear"]<<std::endl;
    g1.serialize();
    g2.load();
    std::cout<<" Lookup 1 for key apple "<<g2["apple"]<<" orange "<<g2["orange"]<<std::endl;
    std::cout<<" Missing Lookup for key pear "<<g2["pear"]<<std::endl;
    fastgenematch::Genematcher G;
	std::istringstream iss(std::string("A\tB\n"));
    G<<iss;
    std::ostringstream os;
    G>>os;
    std::cout<<os.str();
    return 1;
};