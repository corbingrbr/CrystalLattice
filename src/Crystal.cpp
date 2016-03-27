#include "Crystal.h"


#include <iostream>
#include "MatrixStack.h"
#include "UnitCell.h"
#include "SimpleCubic.h"
//#include "BodyCentered.h"
//#include "FaceCentered.h"

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

Crystal::Crystal(int type, shared_ptr<Shape> shape) :
    expansion(1.0)
{
    this->type = type;
    this->eighth = shape;
    this->rows = 5;
    this->cols = 5; 
    this->height = 3; 
    this->scale = .1;
}

Crystal::~Crystal()
{
}

void Crystal::init()
{

    // Establish unit cell for particlar crystal
    switch (type) {
        
    case SIMPLE:
        unit = make_shared<SimpleCubic>(eighth);
        break;
    }
    /*case Crystal.BODY:
        unitCell = make_shared<BodyCentered>();
        break;
    case Crystal.FACE:
        unitCell = make_shared<FaceCentered>();
        break;
        }*/

    // Create Structure from unit cells
    //genStructure();
    
}

void Crystal::draw(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog)
{
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
                unit->draw(MV, prog);
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
    expansion += .2;
}

void Crystal::contract()
{
    if (expansion > 1.0) { expansion -= .2; }
}
