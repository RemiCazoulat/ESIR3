#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include "thirdparty/Eigen/Dense"

class TensorField {

public:
    std::vector<float> tensorField;
    std::vector<Eigen::Vector3cd> vectorField;
    int dimX = 0;
    int dimY = 0;
    int dimZ = 0;
    const int tensorDim = 3 * 3;

    bool readTensor(const std::string& filename)
    {
        // >>> opening file
        std::ifstream file(filename, std::ios::binary);
        if (!file) { std::cerr << "Error: Could not open file " << filename << "!" << std::endl; return false; }

        // >>> getting header
        int header[3] = {0};
        if (!file.read(reinterpret_cast<char*>(header), 3 * sizeof(int))) {
            std::cerr << "Error : could not read header" << std::endl; return false;}
        dimX = header[0];
        dimY = header[1];
        dimZ = header[2];
        //const int tensorFieldSize = dimX * dimY * dimZ * tensorDim;
        //init tensor field
        //float* tensorField = new float[tensorFieldSize];

        // >>> reading binary data
        file.seekg(3 * sizeof(int), std::ios::beg);
        tensorField.resize(dimX * dimY * dimZ * tensorDim);
        if (!file.read(reinterpret_cast<char*>(tensorField.data()),std::streamsize(dimX * dimY * dimZ * tensorDim * sizeof(float)))) {
            std::cerr << "Error : could not read floats" << std::endl; return false;}

        file.close();
        return true;
    }



    void vectorFromTensor()
    {
        int i = 0;
        for(int x = 0; x < dimX; x ++)
        {
            for(int y = 0; y < dimY; y ++)
            {
                for(int z = 0; z < dimZ; z ++)
                {
                    Eigen::Matrix3d mat;
                    size_t index = i * tensorDim;
                    // WARNING : may be 1, 4, 7 instead of 1, 2, 3
                    //                  2, 5, 8            4, 5, 6
                    //                  3, 6, 9            7, 8, 9
                    mat <<  tensorField.at(index + 0),  tensorField.at(index + 1),  tensorField.at(index + 2),
                            tensorField.at(index + 3),  tensorField.at(index + 4),  tensorField.at(index + 5),
                            tensorField.at(index + 6),  tensorField.at(index + 7),  tensorField.at(index + 8);


                    Eigen::EigenSolver<Eigen::Matrix3d> solver(mat);
                    Eigen::Vector3cd eigenVector = solver.eigenvectors().col(0).real();
                    vectorField.push_back(eigenVector);
                    if(i % 10000 == 0)
                    {
                        std::cout << "matrix " << i <<" : \n" << mat << std::endl;
                        std::cout << "eigen vector "<< i << " : \n" <<  eigenVector << std::endl;
                    }
                    i ++;
                }
            }
        }
    }

    void interpolate(float x, float y, float z) {

    }


    // Fonction pour vérifier si eigenVectors.bin existe et l'enregistrer si nécessaire
    void getEigenVectors(const std::string& filename = "C:\\dev\\Tractographie\\resources\\eigenVectors.bin") {
        // Vérifier si le fichier existe
        if (std::filesystem::exists(filename)) {
            std::cout << "File " << filename << " found, loading eigenvectors..." << std::endl;

            // Ouvrir le fichier pour lire les eigenvectors
            std::ifstream inFile(filename, std::ios::binary);
            if (!inFile) {
                std::cerr << "Error: Could not open file " << filename << " for reading!" << std::endl;
                return;
            }

            // Lire les dimensions
            inFile.read(reinterpret_cast<char*>(&dimX), sizeof(int));
            inFile.read(reinterpret_cast<char*>(&dimY), sizeof(int));
            inFile.read(reinterpret_cast<char*>(&dimZ), sizeof(int));

            // Redimensionner vectorField en fonction des dimensions
            vectorField.clear();
            vectorField.reserve(dimX * dimY * dimZ);

            // Lire les eigenvectors
            for (int i = 0; i < dimX * dimY * dimZ; ++i) {
                Eigen::Vector3cd vec;
                inFile.read(reinterpret_cast<char*>(vec.data()), sizeof(Eigen::Vector3cd));
                vectorField.push_back(vec);
            }

            inFile.close();
            std::cout << "Eigenvectors loaded from " << filename << std::endl;
        } else {
            // Si le fichier n'existe pas, calculer les eigenvectors
            std::cout << "File not found, computing eigenvectors..." << std::endl;
            vectorFromTensor();

            // Sauvegarder les eigenvectors dans un fichier binaire
            std::ofstream outFile(filename, std::ios::binary);
            if (!outFile) {
                std::cerr << "Error: Could not open file " << filename << " for writing!" << std::endl;
                return;
            }

            // Sauvegarde de la dimension
            outFile.write(reinterpret_cast<const char*>(&dimX), sizeof(int));
            outFile.write(reinterpret_cast<const char*>(&dimY), sizeof(int));
            outFile.write(reinterpret_cast<const char*>(&dimZ), sizeof(int));

            // Sauvegarde des eigenvectors
            for (const auto& vec : vectorField) {
                outFile.write(reinterpret_cast<const char*>(vec.data()), sizeof(Eigen::Vector3cd));
            }

            outFile.close();
            std::cout << "Eigenvectors saved to " << filename << std::endl;
        }
    }
};


