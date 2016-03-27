#include "SimpleCubic.h"
#include "UnitCell.h"
#include "Shape.h"
#include "Program.h"
#include "MatrixStack.h"
#include <GL/glew.h>

#include <Eigen/Dense>
#include <iostream>

using namespace std;
using namespace Eigen;

SimpleCubic::SimpleCubic(shared_ptr<Shape> shape)
    : UnitCell(shape)
{
}

SimpleCubic::~SimpleCubic()
{
}

void SimpleCubic::draw(shared_ptr<MatrixStack> MV, shared_ptr<Program> prog) 
{
    MV->pushMatrix();
    
    drawEighth(MV, prog, 0);
    drawEighth(MV, prog, 90);
    drawEighth(MV, prog, 180);
    drawEighth(MV, prog, 270);
    
    MV->pushMatrix();
    MV->rotate(90.0f, Vector3f(1.0, 0.0, 0.0));
  
    drawEighth(MV, prog, 0);
    drawEighth(MV, prog, 90);
    
    MV->rotate(180.0f, Vector3f(1.0, 0.0, 0.0));
    drawEighth(MV, prog, 180);
    drawEighth(MV, prog, 270);
    
    MV->popMatrix();
    
    MV->popMatrix();
}

void SimpleCubic::drawEighth(shared_ptr<MatrixStack> MV, shared_ptr<Program> prog, float rot) {
    
    MV->pushMatrix();
    
    MV->rotate(rot, Vector3f(0.0, 1.0, 0.0));
    MV->translate(Vector3f(1.0, -1.0, -1.0));
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
    eighth->draw(prog);
    
    MV->popMatrix();
}
