#include <Cartidge.h>
#include <fstream>
using namespace std;

int main(int argc, char * argv[])
{

    std::string path(argv[1]);
    sn::Cartidge cartidge;
    cartidge.loadROM(path);

    string new_path = "./mario";
    ofstream ofs(new_path, ios::out | ios::binary);
    ofs.write((char *)cartidge.PRG_ROM, cartidge.nesheader.m_prnum << 14);
    ofs.write((char *)cartidge.CHR_ROM, cartidge.nesheader.m_crnum << 13);
    ofs.close();
}