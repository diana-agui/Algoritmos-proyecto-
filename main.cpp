#include <iostream>
#include "cryptlib.h"
#include "sha.h"
#include "hex.h"
#include "filters.h"
#include <ctime>
#include <string>
#include <vector>
#include <limits>

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

    string getMedDataForHash() const {
        return city + hospitalVisited + doctorsName + diagnosis + treatment + to_string(date);
    }

    void setDiagnosis(const string& newDiagnosis) {
        diagnosis = newDiagnosis;
    }

    void setTreatment(const string& newTreatment) {
        treatment = newTreatment;
    }

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
        string hashConvertible =  medicalReason + to_string(blockID) + previousHash + to_string(timeStamp) + specificData.getMedDataForHash();
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

    string getHash() const {
        return hash; //get hash para usar cuando declarando el hash previo de un nuevo bloque
    }

    void printBlock() {
        cout << "Registry ID: " << blockID << endl;
        cout << "Information stored: " << medicalReason << endl;
        specificData.printMedicalData();

    }

    string recomputeHash() const {
        return calculateBlockHash();
    }

    string getPreviousHash() const {
        return previousHash;
    }
    void tamperData(const string& newDiagnosis, const string& newTreatment) {
        specificData.setDiagnosis(newDiagnosis);
        specificData.setTreatment(newTreatment);

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
        for (auto &c : chain) {
            cout << " - - - - - - - - - - - - - - - - - - - - - " << endl;
            c.printBlock();
        }
    }

    //Copia el bloque que se agrego para que todos compartan la misma informacion
    void appendExternalBlock(const Block& b)
    {
        chain.push_back(b);
    }


    bool verifyIntegrity() { //algoritmo de memoria asociativa
        for (size_t i = 1; i < chain.size(); i++) {
//recorre la cadena
            const Block& current = chain[i];
            const Block& previous = chain[i - 1];

            //recompila el hash de cada bloque, concurriente y previo para comparar su hash
            //detecta unicamente tampering cuando no coinciden los hashes
            if (current.getHash() != current.recomputeHash()) {
                cout << "Block " << i << " hash mismatch." << endl;
                return false;
            }

            if (current.getPreviousHash() != previous.getHash()) {
                cout << "Block " << i << " previous hash mismatch." << endl;
                return false;
            }
        }

        cout << "Health record chain integrity verified. No tampering deteccted" << endl;
        return true;
    }

    bool tamperRecord(int index, const string& newDiagnosis, const string& nuevoTratamiento) {
        if (index == 0) {
            cout << "Invalido, bloque genesis no modificable" << endl;
            return false;
        }

        chain[index].tamperData(newDiagnosis, nuevoTratamiento);
        return true;

    }

    Block createNewBlock(const MedicalData& data, const string& reason) {
        int blockIdentity = chain.size();
        string previousHash = chain.back().getHash();
        Block b(blockIdentity, reason, previousHash, data);
        return b;
    }

};

void menuMain() {
    cout << " ---------- MENU ---------- " << endl;
    cout << " 1. Add a new health record " << endl;
    cout << " 2. Print your health record" << endl;
    cout << " 3. Verify integrity of your health record" << endl;
    cout << " 4. Change a health record to demonstrate validity of our integrity." << endl;
}

int main() {

    int dateCreated =0;
    string locationCreated = "", hospitalAccountCreated = "", doctorsName = "";
    string firstDiagnosis = "", firstTreatment= "";
    string fullName;

    cout << "Welcome to the Medical Data Fetcher, we track your health records via the use of blockchains, and hashing to secure integrity. " << endl;


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
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin,firstDiagnosis);
    cout << "Treatment selected:" << endl;
    getline(cin,firstTreatment);

    MedicalData firstData(locationCreated, hospitalAccountCreated, doctorsName,firstDiagnosis, firstTreatment, dateCreated);
    BlockChain blocks(fullName, firstData);
    BlockChain ChainA(fullName, firstData);
    BlockChain ChainB(fullName, firstData);
    BlockChain ChainC(fullName, firstData);
    cout << "-------------- " << fullName << " -------------- " << endl;

    while (true) {
        menuMain();

        int choice;
        cin >> choice;

        switch (choice) {
            case 1: {
                string visit, city, hospital, drName, diagnostico, tratamiento;
                int fechaActual = 0;
                cout << "Reason for visit: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, visit);
                cout << endl << "Date (MMDDYYYY): ";
                cin >> fechaActual;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << endl << "City being logged: ";
                getline(cin, city);
                cout << endl << "Hospital being logged: ";
                getline(cin, hospital);
                cout << endl << "Doctor being logged: ";
                getline(cin, drName);
                cout << endl << "Diagnosis: ";
                getline(cin, diagnostico);
                cout << endl << "Treatment selected: ";
                getline(cin, tratamiento);

                MedicalData med1(city, hospital, drName,diagnostico, tratamiento, fechaActual);
                Block newBlock = blocks.createNewBlock(med1, visit);

                //Añade el bloque a la cadena principal
                blocks.appendExternalBlock(newBlock);

                // Lo replica en los demás blockchains
                ChainA.appendExternalBlock(newBlock);
                ChainB.appendExternalBlock(newBlock);
                ChainC.appendExternalBlock(newBlock);
                break;
            }
            case 2: blocks.printBlockChain();
                break;
            case 3: blocks.verifyIntegrity();
                break;
            case 4:
            {
                int idToTamper;
                string newDiagnosis, newTreatment;

                cout << "Enter the ID of record you want to tamper with (cannot be 0): ";
                cin >> idToTamper;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "Enter NEW fake diagnosis: ";
                getline(cin, newDiagnosis);

                cout << "Enter NEW fake treatment: ";
                getline(cin, newTreatment);

                blocks.tamperRecord(idToTamper, newDiagnosis, newTreatment);
                break;
            }
                break;
            default:
                std::cout << "INVALIDO" << endl;
                break;
        }
    }

}
