#include "SimpleCubic.h"
#include "UnitCell.h"
#include "Shape.h"
#include "Program.h"
#include "MatrixStack.h"
#include <GL/glew.h>

#include <Eigen/Dense>
#include <iostream>
#include <map>

using namespace std;
using namespace Eigen;

SimpleCubic::SimpleCubic(shared_ptr<Shape> eighth, shared_ptr<Shape> sphere, map<string, Vector3f> colors)
    : UnitCell(eighth, sphere, colors)
{
}

SimpleCubic::~SimpleCubic()
{
}

void SimpleCubic::draw(shared_ptr<MatrixStack> MV, shared_ptr<Program> prog, Vector3f pos, float alpha, bool center) 
{
    if (center && alpha < 1.0) {
        glUniform1f(prog->getUniform("alpha"), 1.0);
        glUniform3fv(prog->getUniform("kdFront"), 1, colors["blue"].data());
    } else {
        glUniform3fv(prog->getUniform("kdFront"), 1, colors["grey"].data());
    }
    
    MV->pushMatrix();
    MV->translate(pos);
    
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
    MV->popMatrix();

    glUniform1f(prog->getUniform("alpha"), alpha); // Make sure alpha is same as it was 
}

void SimpleCubic::drawEighth(shared_ptr<MatrixStack> MV, shared_ptr<Program> prog, float rot) {
    
    MV->pushMatrix();
    
    MV->rotate(rot, Vector3f(0.0, 1.0, 0.0));
    MV->translate(Vector3f(1.0, -1.0, -1.0));
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
    eighth->draw(prog);
    
    MV->popMatrix();
}
