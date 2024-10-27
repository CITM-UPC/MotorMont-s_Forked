#ifndef MYMESH_H
#define MYMESH_H

#include <glm/glm.hpp>


class MyMesh {
public:
    unsigned int id_index = 0;  
    unsigned int num_index = 0;  
    unsigned int* index = nullptr; 

    unsigned int id_vertex = 0;  
    unsigned int num_vertex = 0; 
    unsigned int id_texcoord = 0;
    float* vertex = nullptr;     
    float* texcoord = nullptr;     

    unsigned int id_texture = 0;

    //Color de prueba 
    glm::u8vec3 color = glm::u8vec3(111, 110, 0);
  
    void InitBuffers();
   
    void Draw() const;
   
    void LoadFile(const char* file_path);
   
    void LoadTexture(const char* texture_path);

    ~MyMesh();
};

#endif // MYMESH_H