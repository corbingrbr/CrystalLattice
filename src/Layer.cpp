#include "Layer.h"

#include "Shape.h"
#include "MatrixStack.h"
#include "Program.h"
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

Layer::Layer(int rows, int cols, float restHeight, float expansion, Vector3f color, shared_ptr<Shape> sphere) 
    : rows(rows),
      cols(cols),
      startHeight(7.0),
      restHeight(restHeight),
      xexpansion(expansion),
      zexpansion(expansion),
      curHeight(7.0),
      speed(.05),
      atRest(false),
      color(color),
      sphere(sphere)
{ 
    offset = Vector3f(-(cols-1)*expansion, restHeight,-(rows-1)*expansion);
}

Layer::Layer(int rows, int cols, float restHeight, float xexpansion, float zexpansion, Vector3f color, shared_ptr<Shape> sphere) 
    : rows(rows),
      cols(cols),
      startHeight(7.0),
      restHeight(restHeight),
      xexpansion(xexpansion),
      zexpansion(zexpansion),
      curHeight(7.0),
      speed(.05),
      atRest(false),
      color(color),
      sphere(sphere)
{
    offset = Vector3f(-(cols-1)*xexpansion, restHeight,-(rows-1)*zexpansion);
    
} 

Layer::~Layer()
{
}

void Layer::reset()
{
    curHeight = startHeight;
    atRest = false;
}

void Layer::update()
{
    if (curHeight - speed > restHeight) {
        curHeight -= speed;
    } else {
        curHeight = restHeight;
        atRest = true;
    }
}

void Layer::draw(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog)
{
    glUniform1f(prog->getUniform("alpha"), 1.0);
    glUniform3fv(prog->getUniform("kdFront"), 1, color.data());
    

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {

            Vector3f pos(offset(0) + j*2*xexpansion, curHeight*xexpansion, offset(2) + i*2*zexpansion);
            
            MV->pushMatrix();
            MV->translate(pos);
            glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
            sphere->draw(prog);
            MV->popMatrix();
        }
    }
}

bool Layer::isAtRest()
{
    return atRest;
}
