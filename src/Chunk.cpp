#include "Chunk.hpp"
#include <stdexcept>
#include <iostream>
#include <vector>

Chunk::Chunk() {
    build_triangles();
}

float Chunk::generator_function(int x, int y, int z) const {
    return static_cast<float>(x == 0);
}

float Chunk::generator_function(int* xyz) const {
    return generator_function(xyz[0], xyz[1], xyz[2]);
}

vke::VkeModel::Vertex Chunk::interpolate(float fvs[8], int edge_index, int x, int y, int z) {
    // for(int i = 0; i < 8; i++) {
    //     std::cout << fvs[i] << std::endl;
    // }
    switch(edge_index) {
        case 0:
            return vke::VkeModel::Vertex{{x + std::abs(fvs[0] - fvs[1]) / (fvs[0] + fvs[1]), y, z}, {1.f, 0.4f, 0.4f}};
            break;
        case 1:
            return vke::VkeModel::Vertex{{x, y + std::abs(fvs[2] - fvs[1]) / (fvs[2] + fvs[1]), z}, {1.f, 0.4f, 0.4f}};
            break;
        case 2:
            return vke::VkeModel::Vertex{{x + std::abs(fvs[2] - fvs[3]) / (fvs[2] + fvs[3]), y+1, z}, {1.f, 0.4f, 0.4f}};
            break;
        case 3:
            return vke::VkeModel::Vertex{{x, y + std::abs(fvs[3] - fvs[0]) / (fvs[3] + fvs[0]), z}, {1.f, 0.4f, 0.4f}};
            break;

        case 4:
            return vke::VkeModel::Vertex{{x + std::abs(fvs[4] - fvs[5]) / (fvs[4] + fvs[5]), y, z+1}, {1.f, 0.4f, 0.4f}};
            break;
        case 5:
            return vke::VkeModel::Vertex{{x, y + std::abs(fvs[6] - fvs[5]) / (fvs[6] + fvs[5]), z+1}, {1.f, 0.4f, 0.4f}};
            break;
        case 6:
            return vke::VkeModel::Vertex{{x + std::abs(fvs[6] - fvs[7]) / (fvs[6] + fvs[7]), y+1, z+1}, {1.f, 0.4f, 0.4f}};
            break;
        case 7:
            return vke::VkeModel::Vertex{{x, y + std::abs(fvs[4] - fvs[7]) / (fvs[4] + fvs[7]), z+1}, {1.f, 0.4f, 0.4f}};
            break;

        case 8:
            return vke::VkeModel::Vertex{{x, y, z + std::abs(fvs[4] - fvs[0]) / (fvs[4] + fvs[0])}, {1.f, 0.4f, 0.4f}};
            break;
        case 9:
            return vke::VkeModel::Vertex{{x+1, y, z + std::abs(fvs[5] - fvs[1]) / (fvs[5] + fvs[1])}, {1.f, 0.4f, 0.4f}};
            break;
        case 10:
            return vke::VkeModel::Vertex{{x+1, y+1, z + std::abs(fvs[6] - fvs[2]) / (fvs[6] + fvs[2])}, {1.f, 0.4f, 0.4f}};
            break;
        case 11:
            return vke::VkeModel::Vertex{{x, y+1, z + std::abs(fvs[7] - fvs[3]) / (fvs[7] + fvs[3])}, {1.f, 0.4f, 0.4f}};
            break;

        default:
            throw std::runtime_error("Edge doesn't exist");
    }
}


void Chunk::build_triangles(){
    // threshold for when something is zero
    float thresh = 0.4f;

    // assume x goes to right, y goes back and z goes down
    // for details on corner and edge numbering, look into doc/cube_reference
    
    // loop over every cube
    for(int x = 0; x < x_size; x++) {
        for(int y = 0; y < y_size; y++) {
            for(int z = 0; z < z_size; z++) {
                // get corners in correct order
                int cube_corners[8][3] = {{x, y, z}, {x+1, y, z}, {x+1, y+1, z}, {x, y+1, z}, {x, y, z+1}, {x+1, y, z+1}, {x+1, y+1, z+1}, {x, y+1, z+1}};
                float function_values[8];
                int identifier = 0;

                // loop over corners
                for(int c = 7; c >= 0; c--) {
                    identifier <<= 1;

                    int fv = generator_function(cube_corners[c]);
                    
                    if(fv >= thresh) {
                        identifier |= 1;
                    }

                    function_values[c] = fv;
                }
                
                // identify case
                int inters_edges = look_up_data.edgeTable[identifier];
                int lsb = 0b1;

                std::vector<vke::VkeModel::Vertex> cube_vertices;
                
                for(int i = 0; i < 12; i++) {
                    if(inters_edges & lsb) {
                        cube_vertices.push_back(interpolate(function_values, i, x, y, z));
                        //std::cout << "Edge: " << i << std::endl;
                    }
                    inters_edges >>= 1;
                }
                
                int i = 0;
                while(look_up_data.triTable[identifier][i] != -1 && i < 12) {
                    vertices.push_back(cube_vertices[i]);
                    i++;
                }
               

            }
        }
    }
    // for(int i = 0; i < 3; i++) {
    //     std::cout << "Vertex: " << vertices[i].position.x<< ", " << vertices[i].position.y << ", " <<vertices[i].position.z << std::endl;
    // }
    
}