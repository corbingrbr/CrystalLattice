#include <iostream>

#include "Scene.h"
#include "Shape.h"
#include "Program.h"
#include "Crystal.h"

using namespace std;
using namespace Eigen;

Scene::Scene() :
    whichCrystal(0)
{
}

Scene::~Scene()
{
}

void Scene::load(const string &RESOURCE_DIR)
{
    eighth = make_shared<Shape>();
    eighth->loadMesh(RESOURCE_DIR + "eighth.obj");
    eighth->init();
    
    sphere = make_shared<Shape>();
    sphere->loadMesh(RESOURCE_DIR + "sphere.obj");
    sphere->init();

    crystals.push_back(make_shared<Crystal>(Crystal::SIMPLE, eighth, sphere));
    crystals.back()->init();
    
    crystals.push_back(make_shared<Crystal>(Crystal::BODY, eighth, sphere));
    crystals.back()->init();
}

void Scene::nextCrystal()
{
    whichCrystal = (whichCrystal + 1) % crystals.size();
}


shared_ptr<Crystal> Scene::getCrystal()
{
    return crystals[whichCrystal];
}

void Scene::draw(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog)
{
    crystals[whichCrystal]->draw(MV, prog);
}

void Scene::expand()
{
    for (unsigned int i = 0; i < crystals.size(); i++) {
        crystals[i]->expand();
    }
}

void Scene::contract()
{
     for (unsigned int i = 0; i < crystals.size(); i++) {
         crystals[i]->contract();
     }
}

void Scene::toggleTranslucency()
{
    for (unsigned int i = 0; i < crystals.size(); i++) {
         crystals[i]->toggleTranslucency();
    }
}
