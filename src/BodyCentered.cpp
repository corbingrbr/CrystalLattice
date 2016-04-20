#include "BodyCentered.h"
#include "UnitCell.h"
#include "Shape.h"
#include "Program.h"
#include "MatrixStack.h"
#include <GL/glew.h>

#include <Eigen/Dense>
#include <iostream>
#include <string>
#include <map>

using namespace std;
using namespace Eigen;

BodyCentered::BodyCentered(shared_ptr<Shape> eighth, shared_ptr<Shape> half, shared_ptr<Shape> sphere, map<string, Vector3f> colors)
    : UnitCell(eighth, half, sphere, colors)
{
    scale = 0.87;
}

BodyCentered::~BodyCentered()
{
}

void BodyCentered::draw(shared_ptr<MatrixStack> MV, shared_ptr<Program> prog, Vector3f pos, float alpha, bool center, Vector3d ndx)
{
     if (center && alpha < 1.0) { 
        glUniform1f(prog->getUniform("alpha"), 1.0);
     } 

     if (center || alpha == 1.0) {
         glUniform3fv(prog->getUniform("kdFront"), 1, colors["red"].data());
     } else {
         glUniform3fv(prog->getUniform("kdFront"), 1, colors["grey"].data());
     }
     
     MV->pushMatrix();
     MV->translate(pos);

     if (ndx(0) != UnitCell::MIN && ndx(0) != UnitCell::MAX &&
         ndx(1) != UnitCell::MIN && ndx(1) != UnitCell::MAX &&
         ndx(2) != UnitCell::MIN && ndx(2) != UnitCell::MAX) {
         // Draw center atom
         MV->pushMatrix();
         MV->scale(scale);
         glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
         sphere->draw(prog);
         MV->popMatrix();
     }
    
     glUniform3fv(prog->getUniform("kdFront"), 1, colors["grey"].data());

      if (ndx(1) != UnitCell::MIN) {
        
        if (ndx(2) != UnitCell::MIN) {
            
            if (ndx(0) != UnitCell::MAX) { drawEighth(MV, prog, 0); }
            if (ndx(0) != UnitCell::MIN) { drawEighth(MV, prog, 90); }
        }
       
        if (ndx(2) != UnitCell::MAX) {
            if (ndx(0) != UnitCell::MIN) { drawEighth(MV, prog, 180); }
            if (ndx(0) != UnitCell::MAX) { drawEighth(MV, prog, 270); }
        }
    }
    
    if (ndx(1) != UnitCell::MAX) {
        MV->pushMatrix();
        MV->rotate(90.0f, Vector3f(1.0, 0.0, 0.0));
        if (ndx(2) != UnitCell::MIN) {
            if (ndx(0) != UnitCell::MAX) { drawEighth(MV, prog, 0); }
            if (ndx(0) != UnitCell::MIN) { drawEighth(MV, prog, 90); }
        }
    
        MV->rotate(180.0f, Vector3f(1.0, 0.0, 0.0));
        if (ndx(2) != UnitCell::MAX) {
            if (ndx(0) != UnitCell::MIN) { drawEighth(MV, prog, 180); }
            if (ndx(0) != UnitCell::MAX) { drawEighth(MV, prog, 270); }
        }
    
        MV->popMatrix();
    }
     
     /*// Draw eighths
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
    
     MV->popMatrix();*/
    
     MV->popMatrix();

     glUniform1f(prog->getUniform("alpha"), alpha); // Make sure alpha is same as it was 
}

void BodyCentered::drawEighth(shared_ptr<MatrixStack> MV, shared_ptr<Program> prog, float rot) {
    
    MV->pushMatrix();
    
    MV->rotate(rot, Vector3f(0.0, 1.0, 0.0));
    MV->translate(Vector3f(1.0, -1.0, -1.0));
    MV->scale(scale);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
    eighth->draw(prog);
    
    MV->popMatrix();
}

void BodyCentered::scaleUp()
{

    if (scale > 0.5) { scale -= .01; }
}

void BodyCentered::scaleDown()
{
    if (scale < 1.0) { scale += .01; }
}
