#include "Crystal.h"


#include <iostream>
#include "MatrixStack.h"
#include "UnitCell.h"
#include "Program.h"
#include "SimpleCubic.h"
#include "BodyCentered.h"
#include "FaceCentered.h"

#include <Eigen/Dense>
#include <utility>
#include <algorithm>

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
    initCellPositions();
    
    // Setup colors
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
    sortCells(MV->topMatrix());

    float alpha = translucent ? 0.3 : 1.0;

    glUniform1f(prog->getUniform("alpha"), alpha);

    MV->pushMatrix();
    MV->scale(scale);
    
    unit->draw(MV, prog, Vector3f(0,0,0), alpha, true); 
    
    for (unsigned int i = 0; i < cells.size(); i++) {
        Vector3f v = cells[i].second.head<3>(); // Vector for cell positioning
        v *= expansion; // Adjust cell positioning by any expansion
        unit->draw(MV, prog, v, alpha, false); // Draw cell
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

float Crystal::calcCellDistance(Matrix4f m, Vector4f v)
{
    Vector4f v2 = m * v;
    
    return v2(0)*v2(0) + v2(1)*v2(1) + v2(2)*v2(2);
}

void Crystal::initCellPositions()
{
    int midi = cols/2;
    int midj = rows/2;
    int midk = height/2;

    // Offset
    Vector3f o(-(cols-1), -(height-1), -(rows-1));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < height; k++) {
                if (i != midi || j != midj || k != midk) {
                    cells.push_back(make_pair(0, Vector4f(o(0) + j*2, o(1) + k*2, o(2) + i*2 , 1)));
                }
            }
        }
    }
}

void Crystal::sortCells(Matrix4f viewMatrix)
{
    // Calculate all cells distances
    for (unsigned int i = 0; i < cells.size(); i++) {
        cells[i].first = calcCellDistance(viewMatrix, cells[i].second);
    }
    
    // Sort cells in descending order by distance from camera
    sort(cells.begin(), cells.end(), sortAlg);
}
