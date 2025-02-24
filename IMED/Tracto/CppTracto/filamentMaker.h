#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include "thirdparty/glm/glm/glm.hpp"
#include "TensorField.h"
#include "thirdparty/Eigen/Dense"
#include "renderer.h"

struct Filament
        {
    glm::vec3 start_p;
    std::vector<glm::vec3> positions;
    double step_size = 0;
};

struct InitFilamentInfo
{
    int fNumber;
    double seed_dist = 0.5;
    int x_o = 39;
    int y_o = 44;
    int z_o = 35;
};

struct ExtendFilamentInfo
{
    double step_size = 1.0;
    double stop_angle = 3.14 / 2;
    double min_FA = 0.2;
};
class FilamentMaker {

    std::vector<Filament> m_filaments;
public:

    static bool trilinear_interpolation(TensorField& tf, const double& x, const double& y, const double& z, Eigen::Vector3cd& result)
    {
        int x0 = std::floor(x);
        int y0 = std::floor(y);
        int z0 = std::floor(z);

        if(x0 < 0 || y0 < 0 || z0 < 0)
        {
            return false;
        }
        if(x0 >= tf.dimX || y0 >= tf.dimY || z0 >= tf.dimZ)
        {
            return false;
        }

        int x1 = x0 + 1;
        int y1 = y0 + 1;
        int z1 = z0 + 1;

        double xd = x - x0;
        double yd = y - y0;
        double zd = z - z0;

        int ix0 = x0 * tf.dimY * tf.dimZ;
        int ix1 = x1 * tf.dimY * tf.dimZ;
        int iy0 = y0 * tf.dimZ;
        int iy1 = y1 * tf.dimZ;
        int iz0 = z0;
        int iz1 = z1;
        //std::cout << "index: " << ix0+iy0+iz0 << std::endl;
        Eigen::Vector3cd c000 = tf.vectorField.at(ix0 + iy0 + iz0);
        //std::cout << "c000: " << c000 << std::endl;
        Eigen::Vector3cd c001 = tf.vectorField.at(ix0 + iy0 + iz1);
        Eigen::Vector3cd c010 = tf.vectorField.at(ix0 + iy1 + iz0);
        Eigen::Vector3cd c011 = tf.vectorField.at(ix0 + iy1 + iz1);
        Eigen::Vector3cd c100 = tf.vectorField.at(ix1 + iy0 + iz0);
        Eigen::Vector3cd c101 = tf.vectorField.at(ix1 + iy0 + iz1);
        Eigen::Vector3cd c110 = tf.vectorField.at(ix1 + iy1 + iz0);
        Eigen::Vector3cd c111 = tf.vectorField.at(ix1 + iy1 + iz1);

        // Interpolation en x
        Eigen::Vector3cd c00 = c000 * (1 - xd) + c100 * xd;
        Eigen::Vector3cd c01 = c001 * (1 - xd) + c101 * xd;
        Eigen::Vector3cd c10 = c010 * (1 - xd) + c110 * xd;
        Eigen::Vector3cd c11 = c011 * (1 - xd) + c111 * xd;

        // Interpolation en y
        Eigen::Vector3cd c0 = c00 * (1 - yd) + c10 * yd;
        Eigen::Vector3cd c1 = c01 * (1 - yd) + c11 * yd;

        // Interpolation en z
        result = c0 * (1 - zd) + c1 * zd;
        //result = c000;
        return true;
    }

    static bool trilinear_interpolation(TensorField& tf, const double x, const double y,const double z, Eigen::Matrix3d& result) {
        int x0 = std::floor(x);
        int y0 = std::floor(y);
        int z0 = std::floor(z);
        int x1 = x0 + 1;
        int y1 = y0 + 1;
        int z1 = z0 + 1;

        if (x0 < 0 || x1 >= tf.dimX || y0 < 0 || y1 >= tf.dimY || z0 < 0 || z1 >= tf.dimZ) {
            return false;
        }
        double xd = x - x0;
        double yd = y - y0;
        double zd = z - z0;
        auto getTensor = [&](int xi, int yi, int zi) -> Eigen::Matrix3d {
            int index = (xi * tf.dimY * tf.dimZ + yi * tf.dimZ + zi) * 9;
            Eigen::Matrix3d tensor;
            tensor <<
            tf.tensorField[index + 0], tf.tensorField[index + 1], tf.tensorField[index + 2],
            tf.tensorField[index + 3], tf.tensorField[index + 4], tf.tensorField[index + 5],
            tf.tensorField[index + 6], tf.tensorField[index + 7], tf.tensorField[index + 8];
            return tensor;
        };
        Eigen::Matrix3d C000 = getTensor(x0, y0, z0);
        Eigen::Matrix3d C001 = getTensor(x0, y0, z1);
        Eigen::Matrix3d C010 = getTensor(x0, y1, z0);
        Eigen::Matrix3d C011 = getTensor(x0, y1, z1);
        Eigen::Matrix3d C100 = getTensor(x1, y0, z0);
        Eigen::Matrix3d C101 = getTensor(x1, y0, z1);
        Eigen::Matrix3d C110 = getTensor(x1, y1, z0);
        Eigen::Matrix3d C111 = getTensor(x1, y1, z1);

        // Interpolation en X
        Eigen::Matrix3d C00 = C000 * (1 - xd) + C100 * xd;
        Eigen::Matrix3d C01 = C001 * (1 - xd) + C101 * xd;
        Eigen::Matrix3d C10 = C010 * (1 - xd) + C110 * xd;
        Eigen::Matrix3d C11 = C011 * (1 - xd) + C111 * xd;

        // Interpolation en Y
        Eigen::Matrix3d C0 = C00 * (1 - yd) + C10 * yd;
        Eigen::Matrix3d C1 = C01 * (1 - yd) + C11 * yd;

        // Interpolation en Z
       result = C0 * (1 - zd) + C1 * zd;
       return true;
    }

    static double angle(const Eigen::Vector3cd& previous_dir, const Eigen::Vector3cd& dir)
    {
        std::complex<double> dot_product = previous_dir.dot(dir.conjugate());
        double norm_prev = previous_dir.norm();
        double norm_dir = dir.norm();
        if (norm_prev == 0 || norm_dir == 0)
        {
            throw std::runtime_error("One norm is null !");
        }
        double cos_theta = std::real(dot_product) / (norm_prev * norm_dir);
        cos_theta = std::clamp(cos_theta, -1.0, 1.0);
        return std::acos(cos_theta);
    }

    /*
    static double FA(int x, int y, int z, TensorField* tf){
        int index = x * tf->dimY * tf->dimZ + y*tf->dimZ + z;
        index = index * 9;
        Eigen::Matrix3d tensor;
        tensor <<  tf->tensorField.at(index + 0),  tf->tensorField.at(index + 1),  tf->tensorField.at(index + 2),
                tf->tensorField.at(index + 3),  tf->tensorField.at(index + 4),  tf->tensorField.at(index + 5),
                tf->tensorField.at(index + 6),  tf->tensorField.at(index + 7),  tf->tensorField.at(index + 8);


        Eigen::EigenSolver<Eigen::Matrix3d> solver(tensor);
        Eigen::Vector3cd lambdas = solver.eigenvalues();
        double lambda1 = lambdas[0].real();
        double lambda2 = lambdas[1].real();
        double lambda3 = lambdas[2].real();

        double numerator = std::sqrt((lambda1 - lambda2) * (lambda1 - lambda2) +
                                     (lambda2 - lambda3) * (lambda2 - lambda3) +
                                     (lambda1 - lambda3) * (lambda1 - lambda3));
        double denominator = std::sqrt(2*(lambda1 * lambda1 + lambda2 * lambda2 + lambda3 * lambda3));
        return numerator/denominator;
    }
*/
    static double FA(double x, double y, double z, TensorField& tf) {
        Eigen::Matrix3d tensor;
        if(!trilinear_interpolation(tf, x, y, z, tensor)) return 0;
        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(tensor);
        Eigen::Vector3d lambdas = solver.eigenvalues();
        if (lambdas[0] < 0 || lambdas[1] < 0 || lambdas[2] < 0) {
            return 0.0;
        }
        double lambda_moy = (lambdas[0] + lambdas[1] + lambdas[2]) / 3.0;
        double numerator = std::sqrt(
                (lambdas[0] - lambda_moy) * (lambdas[0] - lambda_moy) +
                (lambdas[1] - lambda_moy) * (lambdas[1] - lambda_moy) +
                (lambdas[2] - lambda_moy) * (lambdas[2] - lambda_moy));

        double denominator = std::sqrt(lambdas[0] * lambdas[0] + lambdas[1] * lambdas[1] + lambdas[2] * lambdas[2]);
        if (denominator == 0) return 0.0;
        return std::sqrt(3.0 / 2.0) * (numerator / denominator);
    }

    static bool has_to_stop(TensorField& tf, double& x, double& y, double& z, Eigen::Vector3cd& previous_dir, Eigen::Vector3cd& dir, const double min_FA, const double stop_angle)
    {
        // outside the field
        if(x > tf.dimX || y > tf.dimY || z > tf.dimZ)
        {
            return true;
        }
        if(x < 0 || y < 0 || z < 0)
        {
            return true;
        }
        // Not anisotropic enough
        if(FA(x , y, z, tf) < 0.1)
        {
            return true;
        }
        // angle is too sharp
        if (angle(previous_dir, dir) > stop_angle)
        {
            return true;
        }
        return false;
    }

    void extend_filament(TensorField& tf, double x, double y, double z, const ExtendFilamentInfo& efi)
    {
        Filament fil{glm::vec3(x,y,z)};
        fil.step_size = efi.step_size;

        Eigen::Vector3cd previous_dir, dir;
        bool first = true;
        int i = 0;
        while(i < 10000)
        {
            if(!trilinear_interpolation(tf, x, y, z, dir)) break;
            dir.normalize();
            dir *= efi.step_size;
            x += dir.x().real();
            y += dir.y().real();
            z += dir.z().real();
            if(first)
            {
                first = false;
            }
            else
            {
                if(has_to_stop(tf, x, y, z, previous_dir, dir, efi.min_FA, efi.stop_angle)) break;
            }
            fil.positions.emplace_back(x,y,z);
            previous_dir = dir;
            i ++;
        }
        m_filaments.push_back(fil);
    }

    ///
    /// \param tf
    /// \param fNumber number of segment in 1D. It must be an odd number
    /// \param step_size
    /// \param x_o
    /// \param y_o
    /// \param z_o
    void buildFilaments(TensorField& tf, InitFilamentInfo& ifi, ExtendFilamentInfo& efi)
    {
        std::cout<<"[FIL_MAKER] building filaments ..."<<std::endl;

        int fNumber = ifi.fNumber;
        double x_o = ifi.x_o;
        double y_o = ifi.y_o;
        double z_o = ifi.z_o;
        double seed_dist = ifi.seed_dist;

        if(fNumber%2==0) fNumber++;

        int debug_index = 0;
        const auto x_offset = (double)(x_o - std::floor(fNumber / 2.0));
        const auto y_offset = (double)(y_o - std::floor(fNumber / 2.0));
        const auto z_offset = (double)(z_o - std::floor(fNumber / 2.0));

        int debug_total = fNumber * fNumber * fNumber;
        for(int x = 0; x < fNumber; x ++)
        {
            for(int y = 0; y < fNumber; y ++)
            {
                for(int z = 0; z < fNumber; z ++)
                {
                    const double cx = (double)x * seed_dist + x_offset;
                    const double cy = (double)y * seed_dist + y_offset;
                    const double cz = (double)z * seed_dist + z_offset;
                    if (cx < 0 || cy < 0 || cz < 0) continue;
                    if (cx >= tf.dimX || cy >= tf.dimY ||cz >= tf.dimZ)
                    {
                        debug_total --;
                        continue;
                    }

                    if(FA(cx, cy, cz, tf) < 0.1)
                    {
                        debug_total --;
                        std::cout << "FA too small" << std::endl;
                        continue;
                    }
                    //std::cout << "cx : " << cx << "/" << tf->dimX << std::endl;
                    //std::cout << "cy : " << cy << "/" << tf->dimY << std::endl;
                    //std::cout << "cz : " << cz << "/" << tf->dimZ << std::endl;
                    ExtendFilamentInfo efi_back(efi);
                    efi_back.step_size = - efi_back.step_size;
                    extend_filament(tf, cx, cy, cz, efi);
                    extend_filament(tf, cx, cy, cz, efi_back);

                    std::cout << " > filament " << debug_index << "/" << debug_total << std::endl;
                    debug_index ++;
                }
            }
        }
        std::cout<<"[FIL_MAKER] building filaments done"<<std::endl;
    }

    void filaments2Segments(std::vector<Segment> &segments)
    {
        segments.clear();
        for(auto& fil : m_filaments)
        {
            glm::vec3 start_p = fil.start_p;
            for(auto& pos : fil.positions)
            {
                glm::vec3 end_p = pos;
                segments.push_back({start_p, end_p});
                start_p = end_p;
            }
        }
    }
};