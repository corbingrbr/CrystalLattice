#include <iostream>

#include "Scene.h"
#include "Shape.h"
#include "Program.h"
#include "SimpleCubic.h"
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

    crystals.push_back(make_shared<Crystal>(Crystal::SIMPLE, eighth));
    crystals.back()->init();
}

void Scene::nextCrystal()
{
    // which++;
    // Swap new crystal in
}


shared_ptr<Crystal> Scene::getCrystal()
{
    return crystals[whichCrystal];
}

void Scene::draw(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog)
{
    glUniform3fv(prog->getUniform("kdFront"), 1, Vector3f(0.5, 0.5, 0.5).data());
    crystals[whichCrystal]->draw(MV, prog);
}
