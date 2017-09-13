//
//  objloader.hpp
//  OpenGLTutorial
//
//  Created by Jesús Bartús Sancho on 13/9/17.
//  Copyright © 2017 Jesús Bartús Sancho. All rights reserved.
//

#ifndef objloader_hpp
#define objloader_hpp

#include <stdio.h>
#include <vector>
#include <glm.hpp>
using namespace glm;

#endif /* objloader_hpp */

bool loadOBJ(const char *path,
             std::vector<glm::vec3> & out_vertices,
             std::vector<glm::vec2> & out_uvs,
             std::vector<glm::vec3> & out_normals);
