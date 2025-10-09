#include <iostream>
#include "cryptlib.h"
#include "sha.h"
#include "hex.h"
#include "filters.h"
#include <ctime>
#include <string>
#include <vector>
using namespace std;

class MyBlock {
private:
    float timeStamp = 0;
    string dataStored = "";
    int blockID = 0;
    CryptoPP::SHA256 hash;
    CryptoPP::SHA256 previousHash;


    public:
    MyBlock() {}
    ~MyBlock() {}

};
// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {

    return 0;
}
