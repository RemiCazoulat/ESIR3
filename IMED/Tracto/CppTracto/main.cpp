#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "renderer.h"
#include "TensorField.h"
#include "filamentMaker.h"
#include "thirdparty/Eigen/Core"

float compute_time(time_t start,time_t end)
{
    return ((float)((int)end - (int)start))/CLOCKS_PER_SEC;
};


// Fonction principale



void readBrain(const std::string& filename, std::vector<float>& b0, int& dimX, int& dimY, int& dimZ)
{
    // Ouvrir le fichier en mode binaire
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Erreur : impossible d'ouvrir le fichier b0.bin" << std::endl;
        return;
    }

    // Lire les dimensions
    file.read(reinterpret_cast<char*>(&dimX), sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(&dimY), sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(&dimZ), sizeof(uint32_t));

    std::cout << "Dimensions: " << dimX << " x " << dimY << " x " << dimZ << std::endl;

    // Lire les données (float32)
    b0.resize(dimX * dimY * dimZ);
    file.read(reinterpret_cast<char*>(b0.data()), b0.size() * sizeof(float));

    // Vérifier si la lecture a réussi
    if (!file) {
        std::cerr << "Erreur lors de la lecture des données." << std::endl;
        return;
    }

    file.close();
    std::cout << "Lecture du fichier b0.bin terminee !" << std::endl;

    // Exemple : afficher la première valeur
    std::cout << "Premiere valeur de b0 : " << b0[0] << std::endl;
}

int main() {
    std::vector<float> b0;
    int dimX, dimY, dimZ;
    readBrain("C:\\Vault\\Studies\\ESIR3\\IMED\\Tracto\\CppTracto\\resources\\b0.bin", b0, dimX, dimY, dimZ);
    std::vector<Segment> segments;
    TensorField tf;
    tf.readTensor("C:\\Vault\\Studies\\ESIR3\\IMED\\Tracto\\CppTracto\\resources\\tensor_field.bin");
    tf.getEigenVectors("C:\\Vault\\Studies\\ESIR3\\IMED\\Tracto\\CppTracto\\resources\\eigenVectors.bin");
    //tf.vectorFromTensor();
    FilamentMaker fm;
    InitFilamentInfo ifi;
    ExtendFilamentInfo efi;
    ifi.fNumber = 15;
    ifi.seed_dist = 1.5;
    fm.buildFilaments(tf, ifi, efi);
    fm.filaments2Segments(segments);
    Renderer renderer(800, 600, "Tractographie");
    renderer.loadSegments(segments);

    //renderer.loadBrainAsGrid(b0, dimX, dimY, dimZ);
    time_t startclock, endclock;
    std::cout << "Rendering " << std::endl;
    float time_step = 0.0;
    const float ROTATION_PERIOD = 50.0f;
    const float ROTATION_SPEED = (2.0f * 3.14f) / ROTATION_PERIOD;
    int i = 0;
    while (true) {
        startclock = clock();
        renderer.rotateCamera((float)i * 0.0001f);
        renderer.drawSegments();
        endclock = clock();
        time_step = compute_time(startclock, endclock);
        i ++;
    }
    return 0;
}
