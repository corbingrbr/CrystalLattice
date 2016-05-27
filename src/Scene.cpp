#include <iostream>

#include "Scene.h"
#include "Shape.h"
#include "Program.h"
#include "Crystal.h"
#include "MatrixStack.h"
#include <Eigen/Dense>

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
    
    half = make_shared<Shape>();
    half->loadMesh(RESOURCE_DIR + "half.obj");
    half->init();

    sphere = make_shared<Shape>();
    sphere->loadMesh(RESOURCE_DIR + "sphere.obj");
    sphere->init();

    crystals.push_back(make_shared<Crystal>(Crystal::SIMPLE, eighth, half, sphere));
    crystals.back()->init();
    
    crystals.push_back(make_shared<Crystal>(Crystal::BODY, eighth, half, sphere));
    crystals.back()->init();
    
    crystals.push_back(make_shared<Crystal>(Crystal::FACE, eighth, half, sphere));
    crystals.back()->init();
}

void Scene::nextCrystal()
{
    whichCrystal = (whichCrystal + 1) % crystals.size();

    // Make sure it starts as falling layers
    crystals[whichCrystal]->setDrawLayers();
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

void Scene::toggleLayers()
{
    for (unsigned int i = 0; i < crystals.size(); i++) {
         crystals[i]->toggleLayers();
    }
}

void Scene::toggleInspection()
{
    for (unsigned int i = 0; i < crystals.size(); i++) {
         crystals[i]->toggleInspection();
    }
}
