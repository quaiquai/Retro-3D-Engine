#ifndef SCENEGRAPH_H
#define SCNENGRAPH_H

#include <stdio.h>
#include <vector>
#include "scene_object.h"

class SceneGraph {
public:

	std::vector<SceneObject> scene_objects;

	SceneGraph() {

	}
};


#endif