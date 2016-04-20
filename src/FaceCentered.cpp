#include "FaceCentered.h"
#include "UnitCell.h"
#include "Shape.h"
#include "Program.h"
#include "MatrixStack.h"
#include <GL/glew.h>

#include <Eigen/Dense>
#include <iostream>

using namespace std;
using namespace Eigen;

FaceCentered::FaceCentered(shared_ptr<Shape> eighth, shared_ptr<Shape> half, shared_ptr<Shape> sphere, map<string, Vector3f> colors)
    : UnitCell(eighth, half, sphere, colors)
{
    scale = 0.71;
}

FaceCentered::~FaceCentered()
{
}

void FaceCentered::draw(shared_ptr<MatrixStack> MV, shared_ptr<Program> prog, Vector3f pos, float alpha, bool center, Vector3d ndx) 
{
    if (center && alpha < 1.0) { 
        glUniform1f(prog->getUniform("alpha"), 1.0);
    } 

    if (center || alpha == 1.0) {
        glUniform3fv(prog->getUniform("kdFront"), 1, colors["green"].data());
    } else {
        glUniform3fv(prog->getUniform("kdFront"), 1, colors["grey"].data());
        }

    MV->pushMatrix();
    MV->translate(pos);
     
    MV->pushMatrix();
     
    drawHalf(MV, prog, 0, Vector3f(0, 1.0, 0));
    drawHalf(MV, prog, 90, Vector3f(0, 1.0, 0));
    drawHalf(MV, prog, 180, Vector3f(0, 1.0, 0));
    drawHalf(MV, prog, 270, Vector3f(0, 1.0, 0));
    drawHalf(MV, prog, -90, Vector3f(0, 0, 1.0));
    drawHalf(MV, prog, 90, Vector3f(0, 0, 1.0));

    glUniform3fv(prog->getUniform("kdFront"), 1, colors["grey"].data());

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

void FaceCentered::scaleUp()
{
    if (scale > 0.5) { scale -= .01; }
}

void FaceCentered::scaleDown()
{
    if (scale < 1.0) { scale += .01; }
}

void FaceCentered::drawEighth(shared_ptr<MatrixStack> MV, shared_ptr<Program> prog, float rot) {
    
    MV->pushMatrix();
    
    MV->rotate(rot, Vector3f(0.0, 1.0, 0.0));
    MV->translate(Vector3f(1.0, -1.0, -1.0));
    MV->scale(scale);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
    eighth->draw(prog);
    
    MV->popMatrix();
}

void FaceCentered::drawHalf(shared_ptr<MatrixStack> MV, shared_ptr<Program> prog, float rot, Vector3f axis)
{
    MV->pushMatrix();
    MV->rotate(rot, axis);
    MV->translate(Vector3f(-1.0,0,0)*(1-scale));
    MV->scale(scale);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
    half->draw(prog);

    MV->popMatrix();
}
