/*
 * =====================================================================================
 *
 *       Filename:  fastgenematch.cpp
 *
 *    Description:  Source file matching header
 *
 *        Version:  0.0
 *        Created:  4/20/2013 4:17:34 PM
 *       Revision:  none
 *       Compiler:  gcc/clang/msvc
 *
 *         Author:  Yu Liu (yul13@uci.edu)
 *
 * =====================================================================================
 */
#include"fastgenematch.h"




namespace fastgenematch
{
    /* Not used
     *Geneconverter::Geneconverter()
     *{
     *    greeks["α"]="alpha";
     *    greeks["β"]="alpha";
     *    greeks["γ"]="gamma";
     *    greeks["δ"]="delta";
     *    greeks["ε"]="epsilon";
     *    greeks["ζ"]="tao";
     *    greeks["η"]="eta";
     *    greeks["θ"]="theta";
     *    greeks["κ"]="kappa";
     *    greeks["λ"]="lambda";
     *    greeks["μ"]="mu";
     *    greeks["ν"]="nu";
     *    greeks["ξ"]="xi";
     *    greeks["Θ"]="theta";
     *    greeks["π"]="pi";
     *    greeks["τ"]="tao";
     *    greeks["φ"]="phi";
     *    greeks["Φ"]="phi";
     *    greeks["χ"]="chi";
     *    greeks["ψ"]="psi";
     *    greeks["Ω"]="omega";
     *    greeks["ω"]="omega";
     *    greeks["_"]="";
     *    greeks["-"]="";
     *};
     *std::string
     *Geneconverter::format(const std::string& key)
     *{
     *    std::string out("");
     *    std::string temp(key);
     *    for (auto it:temp)
     *    {
     *        try
     *        {
     *            out+=greeks.at(it);
     *        }
     *        catch(...)
     *        {
     *            out+=it;
     *        }
     *    }
     *    return out;
     *}
     */
//decided this is for python wrapper

    /*
     *  Container
     */

    uint32_t Hashcaller::seed=SEED;
    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  operator[],()
     *  Description:  access operator overloads, note the default behavior change for easy
     *                validation
     * =====================================================================================
     */
    std::string&
    Geneobject::operator[] (const std::string& key)
    {
        try
        {
            return data->at(key);
        }
        catch (std::out_of_range& err)
        {
            return empty;
        }
    };
    std::string&
    Geneobject::operator() (const std::string & key)
    {
        if ((format_types)formats.first==genesym)
        {
            return (*data)[key];
        }else{
            return (*data)[key];
        }
    }
    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  rehash
     *  Description:  rehash the table, doubling size and reseeding.
     *                this process is automatically done every so often
     * =====================================================================================
     */
    void
    Geneobject::rehash()
    {
        length*=2;
        reseed();
        hashtable temp(new _hashtable);
        temp->reserve(length);
        for (auto it:(*data))
        {
            (*temp)[it.first]=it.second;
        }
        data=temp;
    };

    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  serialize, load
     *  Description:  serialize the hashtable
     * =====================================================================================
     */
    void
    Geneobject::serialize()
    {
        std::ofstream f;
        f.open("fgcdefault.bin",std::ofstream::binary);
        if (f.good())
        {
            f.write((char*) &formats.first, sizeof(int));
            f.write((char*) &formats.second,sizeof(int));
            f.write((char*) &seed, sizeof(seed));
            f.write((char*) &length, sizeof(length));
            size_t s=data->size();
            f.write((char*) &(s), sizeof(size_t));
            for (auto it: *data)
            {
                size_t len1=it.first.size(),len2=it.second.size();
                f.write((char*) &len1, sizeof(size_t));
                f.write((char*) it.first.c_str(), len1+1);
                f.write((char*) &len2, sizeof(size_t));
                f.write((char*) it.second.c_str(), len2+1);
            }
            f.close();
        }
        else f.close();
    };

    void
    Geneobject::load()
    {
        std::ifstream f;
        f.open("fgcdefault.bin",std::ifstream::binary);
        if (f.good())
        {
            f.read((char*) &formats.first, sizeof(int));
            f.read((char*) &formats.second, sizeof(int));
            f.read((char*) &seed, sizeof(seed));
            f.read((char*) &length, sizeof(length));
            data->reserve(length);
            size_t s;
            f.read((char*) &s, sizeof(size_t));
            for (size_t i=0; i<s; i++)
            {
                size_t len1,len2;
                char* first, *second;
                //get two holders;
                f.read((char*) &len1, sizeof(size_t));
                first=new char[len1+1];
                f.read((char*) first, len1+1);
                f.read((char*) &len2, sizeof(size_t));
                second=new char[len2+1];
                f.read((char*) second, len2+1);
                std::string key(first), value(second);
                (*data)[key]=value;
                delete first,second;
            }
            f.close();
        }
        else f.close();
    };

    /*
     *  Converter
     */

}
