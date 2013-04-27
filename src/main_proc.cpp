/*
 * =====================================================================================
 *
 *       Filename:  main_proc.cpp
 *
 *    Description:  main file for the process file
 *
 *        Version:  0.3
 *        Created:  4/27/2013 12:48:12 PM
 *       Compiler:  gcc/clang/msvc (c++11 required)
 * =====================================================================================
 */
#include "fastgenematch_proc.h"

int main (int argc, char** argv)
{
    fastgenematch::Fgc_proc proc;
    try
    {
        proc.main();
        return 1;
    }
    catch (fastgenematch::ErrMsg e)
    {
        std::cerr<<e<<std::endl;
        return 0;
    }
    catch (...)
    {
        std::cerr<<"Unspecified error!"<<std::endl;
        return -1;
    }
}

