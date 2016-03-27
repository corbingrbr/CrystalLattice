#include <iostream>

#include "Scene.h"
#include "Shape.h"
#include "Program.h"
#include "SimpleCubic.h"
#include "Crystal.h"

using namespace std;
using namespace Eigen;

Scene::Scene()
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

    simpleCrystal = make_shared<Crystal>(Crystal::SIMPLE, eighth);
    simpleCrystal->init();
}

void Scene::update()
{
   
}

void Scene::expand()
{
    simpleCrystal->expand();
}

void Scene::contract()
{
    simpleCrystal->contract();
}

void Scene::draw(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog)
{
    glUniform3fv(prog->getUniform("kdFront"), 1, Vector3f(1.0, 0.0, 0.0).data());
    simpleCrystal->draw(MV, prog);
}
