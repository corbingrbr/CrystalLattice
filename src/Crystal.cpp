#include "Crystal.h"


#include <iostream>
#include "MatrixStack.h"
#include "UnitCell.h"
#include "Program.h"
#include "SimpleCubic.h"
#include "BodyCentered.h"
#include "FaceCentered.h"
#include "Shape.h"
#include "Layer.h"

#include <Eigen/Dense>
#include <utility>
#include <algorithm>

using namespace std;
using namespace Eigen;

Crystal::Crystal(int type, shared_ptr<Shape> eighth, shared_ptr<Shape> half, shared_ptr<Shape> sphere) :
    expansion(1.0),
    translucent(false)
{
    this->type = type;
    this->eighth = eighth;
    this->half = half;
    this->sphere = sphere;
    this->rows = 5; // only works with odd atm
    this->cols = 5;
    this->height = 5; 
    this->scale = .1;
    this->layersDraw = true;
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
        unit = make_shared<SimpleCubic>(eighth, half, sphere, colors);
        createSimpleLayers();
        break;
    case BODY:
        unit = make_shared<BodyCentered>(eighth, half, sphere, colors);
        createBodyLayers();
        break;
    case FACE:
        unit = make_shared<FaceCentered>(eighth, half, sphere, colors);
        createFaceLayers();
        break;
    
    } 
}

void Crystal::draw(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog)
{
    if (layersDraw) {
        drawLayers(MV, prog);
    } else {
        drawCells(MV, prog);
    }
   
}

void Crystal::drawCells(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog)
{
    sortCells(MV->topMatrix());

    float alpha = translucent ? 0.3 : 1.0;

    glUniform1f(prog->getUniform("alpha"), alpha);

    MV->pushMatrix();
    MV->scale(scale);
    
    unit->draw(MV, prog, Vector3f(0,0,0), alpha, true, Vector3d(1,1,1)); 
    
    for (unsigned int i = 0; i < cells.size(); i++) {
        Vector3f v = cells[i].second.second.head<3>(); // Vector for cell positioning
        Vector3d ndx = cells[i].second.first;
        v *= expansion; // Adjust cell positioning by any expansion

        unit->draw(MV, prog, v, alpha, false, ndx); // Draw cell
    }

    MV->popMatrix();
}

void Crystal::drawLayers(shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog)
{
    MV->pushMatrix();
    MV->scale(scale);
    
    if (type == BODY) {
        MV->scale(0.87);
    } else if (type == FACE) {
        MV->scale(0.71);
    }
    
    for (unsigned int i = 0; i < layers.size(); i++) {
        layers[i]->draw(MV, prog);
        
        // If layer still has more to fall, drop layer further and exit loop
        if (!layers[i]->isAtRest()) {
            layers[i]->update();
            break;
        }

        // If last layer has fallen and settled, switch to other model for separation effects
        if (i == layers.size() - 1 && layers[i]->isAtRest()) {
            toggleLayers();
        }
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

void Crystal::scaleUp()
{
    unit->scaleUp();
}

void Crystal::scaleDown()
{
    unit->scaleDown();
}

void Crystal::toggleTranslucency()
{
    translucent = !translucent;
}

void Crystal::setDrawLayers()
{
    layersDraw = true;
}

void Crystal::toggleLayers()
{
    layersDraw = !layersDraw;
    
    for (unsigned int i = 0; i < layers.size(); i++) {
        layers[i]->reset();
    }

    expansion = 1.0;
    translucent = false;
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
                    
                    int x = UnitCell::MIDDLE;
                    int y = UnitCell::MIDDLE;
                    int z = UnitCell::MIDDLE;

                    if (i == 0) { x = UnitCell::MIN; }
                    if (i == rows-1) { x = UnitCell::MAX; }
                    if (i == rows-2) { x = UnitCell::ONEB4MAX; }

                    if (i == 1) { x = UnitCell::ONEB4MIN; }
                    
                    if (j == 0) { y = UnitCell::MIN; }
                    if (j == cols-1) { y = UnitCell::MAX; }

                    if (j == cols-2) { y = UnitCell::ONEB4MAX; }

                    if (j == 1) { y = UnitCell::ONEB4MIN; }


                    if (k == 0) { z = UnitCell::MIN; }
                    if (k == height-1) { z = UnitCell::MAX; }
                    if (k == height-2) { z = UnitCell::ONEB4MAX; }

                    if (k == 1) { z = UnitCell::ONEB4MIN; }


                    Vector3d ndx(y, z, x);
                    Vector4f pos(o(0) + j*2, o(1) + k*2, o(2) + i*2 , 1);

                    cells.push_back(make_pair(0, make_pair(ndx, pos)));
                }
            }
        }
    }
}

void Crystal::sortCells(Matrix4f viewMatrix)
{
    // Calculate all cells distances
    for (unsigned int i = 0; i < cells.size(); i++) {
        cells[i].first = calcCellDistance(viewMatrix, cells[i].second.second);
    }
    
    // Sort cells in descending order by distance from camera
    sort(cells.begin(), cells.end(), sortAlg);
}

void Crystal::createSimpleLayers()
{
    layers.push_back(make_shared<Layer>(4,4, -3, 1.0, colors["grey"], sphere));
    layers.push_back(make_shared<Layer>(4,4, -1, 1.0, colors["grey"], sphere));
    layers.push_back(make_shared<Layer>(4,4, 1, 1.0, colors["grey"], sphere));
    layers.push_back(make_shared<Layer>(4,4, 3, 1.0, colors["grey"], sphere));
}

void Crystal::createBodyLayers()
{
    layers.push_back(make_shared<Layer>(4,4, -3, 1.14942, colors["grey"], sphere));
    layers.push_back(make_shared<Layer>(3,3, -2, 1.14942, colors["red"], sphere));
    layers.push_back(make_shared<Layer>(4,4, -1, 1.14942, colors["grey"], sphere));
    layers.push_back(make_shared<Layer>(3,3, 0, 1.14942, colors["red"], sphere));
    layers.push_back(make_shared<Layer>(4,4, 1, 1.14942, colors["grey"], sphere));
    layers.push_back(make_shared<Layer>(3,3, 2, 1.14942, colors["red"], sphere));
    layers.push_back(make_shared<Layer>(4,4, 3, 1.14942, colors["grey"], sphere));
}

void Crystal::createFaceLayers()
{
    layers.push_back(make_shared<Layer>(3,3, -3, 1.40845, colors["green"], sphere));
    layers.push_back(make_shared<Layer>(4,4, -3, 1.40845, colors["grey"], sphere));

    layers.push_back(make_shared<Layer>(4,3, -2, 1.40845, colors["green"], sphere));
    layers.push_back(make_shared<Layer>(3,4, -2, 1.40845, colors["green"], sphere));

    layers.push_back(make_shared<Layer>(4,4, -1, 1.40845, colors["grey"], sphere));
    layers.push_back(make_shared<Layer>(3,3, -1, 1.40845, colors["green"], sphere));
    
    layers.push_back(make_shared<Layer>(4,3, 0, 1.40845, colors["green"], sphere));
    layers.push_back(make_shared<Layer>(3,4, 0, 1.40845, colors["green"], sphere));
    
    layers.push_back(make_shared<Layer>(4,4, 1, 1.40845, colors["grey"], sphere));
    layers.push_back(make_shared<Layer>(3,3, 1, 1.40845, colors["green"], sphere));

    layers.push_back(make_shared<Layer>(4,3, 2, 1.40845, colors["green"], sphere));
    layers.push_back(make_shared<Layer>(3,4, 2, 1.40845, colors["green"], sphere));
    
    layers.push_back(make_shared<Layer>(4,4, 3, 1.40845, colors["grey"], sphere));
    layers.push_back(make_shared<Layer>(3,3, 3, 1.40845, colors["green"], sphere));
}
