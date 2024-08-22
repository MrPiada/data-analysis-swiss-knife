#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

void create_birthdays_root() {
    // Nome del file di input e di output
    const char* inputFileName = "../../data/birthdays.dat";
    const char* outputFileName = "../../data/birthdays.root";

    // Apertura del file di input
    std::ifstream inputFile(inputFileName);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file: " << inputFileName << std::endl;
        return;
    }

    // Creazione del file ROOT e del TTree
    TFile *outputFile = new TFile(outputFileName, "RECREATE");
    if (outputFile->IsZombie()) {
        std::cerr << "Error creating output file: " << outputFileName << std::endl;
        inputFile.close();
        return;
    }

    TTree *tree = new TTree("birthdays", "Tree with birthday data");

    // Definizione delle variabili
    Float_t count;
    Int_t month, day;

    // Creazione delle branche nel TTree
    tree->Branch("count", &count, "count/F");
    tree->Branch("month", &month, "month/I");
    tree->Branch("day", &day, "day/I");

    // Lettura e parsing del file di input
    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream lineStream(line);
        std::string dateStr;
        lineStream >> dateStr >> count;

        if (dateStr.length() == 4) {
            month = std::stoi(dateStr.substr(0, 2));
            day = std::stoi(dateStr.substr(2, 2));
            
            // Riempie il TTree
            tree->Fill();
        } else {
            std::cerr << "Warning: Unexpected date format in line: " << line << std::endl;
        }
    }

    // Chiudi il file di input e salva il file ROOT
    inputFile.close();
    outputFile->Write();
    outputFile->Close();

    std::cout << "File ROOT creato con successo: " << outputFileName << std::endl;
}
