
#include <iostream>
#include "cryptlib.h"
#include "sha.h"
#include "hex.h"
#include "filters.h"
#include <ctime>
#include <string>
#include <vector>

using namespace std;

class MedicalData { //clase de informacion de salud especifica

    string city;
    string hospitalVisited;
    string doctorsName;
    string diagnosis;
    string treatment;
    int date;

    public:

    MedicalData(string city, string hospitalVisited, string doctorsName,string diagnosis, string treatment, int date) : city(city), hospitalVisited(hospitalVisited), diagnosis(diagnosis), treatment(treatment), doctorsName(doctorsName), date(date) {};
    //constructor para tipico block

    void printMedicalData()
    {
        cout << "Specialist: " << doctorsName << endl;
        cout << "City: " << city << endl;
        cout << "Hospital: " << hospitalVisited << endl;
        cout << "Date: " << date << endl;
        cout << "Treatment: " << treatment << endl;
        cout << "Diagnosis: " << diagnosis << endl;
    }
};

class Block {
private:
    time_t timeStamp; //tiempo creado
    string medicalReason; //razon medico , o en caso de inicar cuenta sera el nombre
    MedicalData specificData; //datos especificos
    int blockID = 0; //ID va ser igual a al tamano del vector al anadir un bloque
    string hash; //hash de bloque
    string previousHash; //hash previo

    string calculateBlockHash() const //funcion para usar sha-256 generar un hash tipo string
    {
        string hashConvertible = medicalReason + to_string(blockID) + previousHash + to_string(timeStamp);
        string output;
        //hash unico sera la razon, id del bloque, hash previo, e tiempo creado para
        //crear el hash unico e identificar trampa

        CryptoPP::SHA256 sha256; //delarar objeto sha
        CryptoPP::StringSource ss(hashConvertible, true, //pasar por pipeline
        new CryptoPP::HashFilter(sha256,
        new CryptoPP::HexEncoder(
        new CryptoPP::StringSink(output)
        )
        )
        );

        return output;
    };


    public:
    Block(int blockID, string dataStored, const string previousHash, const MedicalData data) : blockID(blockID), medicalReason(dataStored), timeStamp(std::time(nullptr)), specificData(data) {
        this->previousHash = previousHash; //guardar hash previo
        hash = calculateBlockHash();

    }
    ~Block() {}

    string getHash() {
        return hash; //get hash para usar cuando declarando el hash previo de un nuevo bloque
    }

    void printBlock() {
        cout << "Registry ID: " << blockID << endl;
        cout << "Information stored: " << medicalReason << endl;
        specificData.printMedicalData();

    }
};

class BlockChain {
    private:

    std::vector<Block> chain;

    Block createGenesisBlock(const string& name, const MedicalData& initalData) {
        return Block(0, name, "0" , initalData);
    }
    //bloque genesis inicia con nombre de paciente y el constructor hecho para iniciar una cuenta en MedicalData

public:

    BlockChain(const string& name, const MedicalData& firstData) {
        chain.push_back(createGenesisBlock(name, firstData));
    }
    //constructor del blockchain (inicar con genesis SIEMPRE)

    void addBlock(const MedicalData& medicalInfo, const string& visitReason) {
        int blockIdentity = chain.size();
        string previousHash = chain.back().getHash();
        Block freshBlock(blockIdentity, visitReason, previousHash, medicalInfo);
        chain.push_back(freshBlock);
    }
    //anadir un bloque segun datos especificos, blockchain sera un contenedor vector unicamente vinculado mediante
    //los hashes de cada bloque unicos

    void printBlockChain() {
        for (auto c : chain) {
            cout << " - - - - - - - - - - - - - - - - - - - - - - " << endl;
            c.printBlock();
        }
    }

};
int main() {

    int dateCreated =0;
    string locationCreated = "", hospitalAccountCreated = "", doctorsName = "";
    string firstDiagnosis = "", firstTreatment= "";

    string fullName;
    cout << "Patients name: " << endl;
    getline(cin, fullName);
    cout << "Name of doctor creating account: " << endl;
    getline(cin, doctorsName);
    cout << "Hospital: " << endl;
    getline(cin, hospitalAccountCreated);
    cout << "Enter date of account creation in form of MMDDYYYY" << endl;
    cin >> dateCreated;
    cout << "In which city are you creating this account in?" << endl;
    cin >> locationCreated;
    cout << "First diagnosis being logged: " << endl;
    getline(cin,firstDiagnosis);
    cout << "Treatment selected:" << endl;
    getline(cin,firstTreatment);

    MedicalData firstData(locationCreated, hospitalAccountCreated, doctorsName,firstDiagnosis, firstTreatment, dateCreated);
    BlockChain blocks(fullName, firstData);

    blocks.printBlockChain();



}
