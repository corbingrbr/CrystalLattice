#include "Crystal.h"


#include <iostream>
#include "MatrixStack.h"
#include "UnitCell.h"
#include "Program.h"
#include "SimpleCubic.h"
#include "BodyCentered.h"
#include "FaceCentered.h"

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

Crystal::Crystal(int type, shared_ptr<Shape> eighth, shared_ptr<Shape> sphere) :
    expansion(1.0),
    translucent(false)
{
    this->type = type;
    this->eighth = eighth;
    this->sphere = sphere;
    this->rows = 3;
    this->cols = 3; 
    this->height = 3; 
    this->scale = .1;
}

Crystal::~Crystal()
{
}

void Crystal::init()
{
    colors["grey"] = Vector3f(0.5, 0.5, 0.5);
    colors["red"] = Vector3f(1.0, 0, 0);
    colors["green"] = Vector3f(0, 1.0, 0);
    colors["blue"] = Vector3f(0, 0.7, 1.0);
    colors["orange"] = Vector3f(1.0, 0.6, 0.2);

    // Establish unit cell for particlar crystal
    switch (type) {
        
    case SIMPLE:
        unit = make_shared<SimpleCubic>(eighth, sphere, colors);
        break;
    case BODY:
        unit = make_shared<BodyCentered>(eighth, sphere, colors);
        break;
    case FACE:
        unit = make_shared<FaceCentered>(eighth, sphere, colors);
        break;
    
    }
    
}

void Crystal::draw(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog)
{
    float alpha = translucent ? 0.3 : 1.0;

    glUniform1f(prog->getUniform("alpha"), alpha);

    int midi = cols/2;
    int midj = rows/2;
    int midk = height/2;

    MV->pushMatrix();
    MV->scale(scale);
    
    // Translate so structure will be centered at origin
    MV->translate(Vector3f(-(cols-1)*expansion, 
                           -(height-1)*expansion,
                           -(rows-1)*expansion)); 

    // Draw all unitCells
    for (int i = 0; i < cols; i++) {
        MV->pushMatrix();
        
        for (int j = 0; j < rows; j++) {
            MV->pushMatrix();
            
            for (int k = 0; k < height; k++) {
                // Draw unit cell
                unit->draw(MV, prog, alpha, (i == midi && j == midj && k == midk)); 

                MV->translate(Vector3f(0.0, 2.0*expansion, 0)); // Move up one -> y+
            }

            MV->popMatrix();

            MV->translate(Vector3f(0, 0, 2.0*expansion)); // Move one row -> z+
        }

        MV->popMatrix();

        MV->translate(Vector3f(2.0*expansion, 0, 0)); // Move one column -> x+
    }

    MV->popMatrix();
}

void Crystal::expand()
{
    if (expansion < 4.0) { expansion += .2; }
}

void Crystal::contract()
{
    if (expansion > 1.0) { expansion -= .2; }
}

void Crystal::toggleTranslucency()
{
    translucent = !translucent;
}
