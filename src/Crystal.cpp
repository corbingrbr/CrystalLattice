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

    type(type),
    rows(5),
    cols(5),
    height(5),
    scale(.1),
    expansion(1.0),
    inspctExp(0),
    translucent(false),
    layersDraw(true),
    inspecting(false),
    eighth(eighth),
    half(half),
    sphere(sphere)
{
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
    colors["black"] = Vector3f(0, 0, 0);

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
    if (inspecting) {
        drawInspect(MV, prog);
    } else if (layersDraw) {
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
    
    // Rotation of face-centered cubic so that it matches layering scheme
    if (type == FACE) { MV->rotate(45, Vector3f(0,0,1)); }

    MV->scale(scale);
    unit->draw(MV, prog, Vector3f(0,0,0), alpha, true, Vector3d(1,1,1), Vector3d(2,2,2)); 
    
    for (unsigned int i = 0; i < cells.size(); i++) {
        Vector3f v = cells[i].pos.head(3);
        // Vector for cell positioning
        Vector3d bounds = cells[i].bounds;
        Vector3d ndx = cells[i].ndx;
        v *= expansion; // Adjust cell positioning by any expansion

        unit->draw(MV, prog, v, alpha, false, bounds, ndx); // Draw cell
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

        // If last layer has fallen and settled, switch to other model for expand/contract effects
        if (i == layers.size() - 1 && layers[i]->isAtRest()) {
            toggleLayers();
        }
    }
    
    MV->popMatrix();
}

void Crystal::drawInspect(shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog)
{
    switch (type) {
    case SIMPLE:
        drawSimpleInspect(MV, prog);
        break;
    case BODY:
        drawBodyInspect(MV, prog);
        break;
    case FACE:
        drawFaceInspect(MV, prog);
        break;
    }
}

void Crystal::expand()
{
    if (inspecting) {
        if (inspctExp < 0.6) { inspctExp += .2; }
    } else {
        if (expansion < 4.0) { expansion += .2; }
    }
}

void Crystal::contract()
{
    if (inspecting) {
        if (inspctExp > 0.2) { inspctExp -= .2; }
    } else {
        if (expansion > 1.0) { expansion -= .2; }
    }
}

void Crystal::toggleTranslucency()
{
    translucent = !translucent;
}

void Crystal::setDrawLayers()
{
    toggleLayers();
    layersDraw = true;
}

void Crystal::toggleLayers()
{
    if (!inspecting) {
        layersDraw = !layersDraw;
        
        for (unsigned int i = 0; i < layers.size(); i++) {
            layers[i]->reset();
        }
        
        expansion = 1.0;
        inspctExp = 1.0;
        translucent = false;
    }    
}

void Crystal::toggleInspection()
{
    inspctExp = 0.0f;
    inspecting = !inspecting;
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


                    Vector3d bounds(y, z, x);
                    Vector4f pos(o(0) + j*2, o(1) + k*2, o(2) + i*2 , 1);
                    Vector3d ndx(i, j, k);

                    Cell c(bounds, pos, ndx);
                    
                    cells.push_back(c);
                }
            }
        }
    }
}


void Crystal::sortCells(Matrix4f viewMatrix)
{
    // Calculate distance of each cell to the camera
    for (unsigned int i = 0; i < cells.size(); i++) {

        cells[i].distance = calcCellDistance(viewMatrix, cells[i].pos);

    }

    // Sort cells in descending order by their distance
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
    float s = 2;

    layers.push_back(make_shared<Layer>(4,1, -3.0*s, 1.0, 1.40845, colors["grey"], sphere));
    layers.push_back(make_shared<Layer>(3,2, -2.5*s, 1.0, 1.40845, colors["orange"], sphere));
    layers.push_back(make_shared<Layer>(4,3, -2.0*s, 1.0, 1.40845, colors["green"], sphere));
   
    layers.push_back(make_shared<Layer>(3,4, -1.5*s, 1.0, 1.40845, colors["grey"], sphere));
    layers.push_back(make_shared<Layer>(4,5, -1.0*s, 1.0, 1.40845, colors["orange"], sphere));
    layers.push_back(make_shared<Layer>(3,6, -0.5*s, 1.0, 1.40845, colors["green"], sphere));
   
    layers.push_back(make_shared<Layer>(4,7, 0, 1.0, 1.40845, colors["grey"], sphere));
    
    layers.push_back(make_shared<Layer>(3,6, 0.5*s, 1.0, 1.40845, colors["orange"], sphere));
    layers.push_back(make_shared<Layer>(4,5, 1.0*s, 1.0, 1.40845, colors["green"], sphere));
    layers.push_back(make_shared<Layer>(3,4, 1.5*s, 1.0, 1.40845, colors["grey"], sphere));

    layers.push_back(make_shared<Layer>(4,3, 2.0*s, 1.0, 1.40845, colors["orange"], sphere));
    layers.push_back(make_shared<Layer>(3,2, 2.5*s, 1.0, 1.40845, colors["green"], sphere));
    layers.push_back(make_shared<Layer>(4,1, 3.0*s, 1.0, 1.40845, colors["grey"], sphere));
    
}

void Crystal:: drawSimpleInspect(shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog)
{
   
    glUniform1f(prog->getUniform("alpha"), 1.0);
    glUniform3fv(prog->getUniform("kdFront"), 1, colors["blue"].data());
    
    MV->pushMatrix();
    drawEighth(MV, prog, 0, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    drawEighth(MV, prog, 90, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    drawEighth(MV, prog, 180, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    drawEighth(MV, prog, 270, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    
    MV->pushMatrix();
    MV->rotate(90.0f, Vector3f(1.0, 0.0, 0.0));
    drawEighth(MV, prog, 0, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    drawEighth(MV, prog, 90, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    
    MV->rotate(180.0f, Vector3f(1.0, 0.0, 0.0));

    drawEighth(MV, prog, 180, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    drawEighth(MV, prog, 270, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    
    MV->popMatrix();
    
    MV->popMatrix();
}

void Crystal::drawBodyInspect(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog)
{
    glUniform1f(prog->getUniform("alpha"), 1.0);
    glUniform3fv(prog->getUniform("kdFront"), 1, colors["grey"].data());
    
    MV->pushMatrix();
    MV->translate(Vector3f(-inspctExp/20 - 0.15, 0, 0));
    drawEighth(MV, prog, 0, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    drawEighth(MV, prog, 90, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    drawEighth(MV, prog, 180, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    drawEighth(MV, prog, 270, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    
    MV->pushMatrix();
    MV->rotate(90.0f, Vector3f(1.0, 0.0, 0.0));
    drawEighth(MV, prog, 0, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    drawEighth(MV, prog, 90, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    
    MV->rotate(180.0f, Vector3f(1.0, 0.0, 0.0));

    drawEighth(MV, prog, 180, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    drawEighth(MV, prog, 270, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    
    MV->popMatrix();    
    MV->popMatrix();

    glUniform3fv(prog->getUniform("kdFront"), 1, colors["red"].data());
    
    MV->pushMatrix();
    MV->translate(Vector3f(.15,0,0));
    MV->scale(scale);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
    sphere->draw(prog);
    MV->popMatrix();

}

void Crystal::drawFaceInspect(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog)
{
    glUniform1f(prog->getUniform("alpha"), 1.0);
    glUniform3fv(prog->getUniform("kdFront"), 1, colors["grey"].data());
    MV->pushMatrix();
    MV->translate(Vector3f(-0.45*1.1, 0, 0));
    drawEighth(MV, prog, 0, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    drawEighth(MV, prog, 90, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    drawEighth(MV, prog, 180, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    drawEighth(MV, prog, 270, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    
    MV->pushMatrix();
    MV->rotate(90.0f, Vector3f(1.0, 0.0, 0.0));
    drawEighth(MV, prog, 0, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    drawEighth(MV, prog, 90, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    
    MV->rotate(180.0f, Vector3f(1.0, 0.0, 0.0));

    drawEighth(MV, prog, 180, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    drawEighth(MV, prog, 270, Vector3f(-inspctExp, inspctExp, inspctExp)); 
    
    MV->popMatrix();    
    MV->popMatrix();

    glUniform3fv(prog->getUniform("kdFront"), 1, colors["green"].data());
    MV->pushMatrix();
    MV->translate(Vector3f(-0.15*1.1, 0, 0));
    drawHalf(MV, prog, 0, Vector3f(inspctExp + 1.0, 0, 0));
    drawHalf(MV, prog, 180, Vector3f(inspctExp + 1.0, 0, 0));
    MV->popMatrix();
    

    glUniform3fv(prog->getUniform("kdFront"), 1, colors["orange"].data());
    MV->pushMatrix();
    MV->translate(Vector3f(0.15*1.1, 0, 0));
    drawHalf(MV, prog, 0, Vector3f(inspctExp + 1.0, 0, 0));
    drawHalf(MV, prog, 180, Vector3f(inspctExp + 1.0, 0, 0));
    MV->popMatrix();
    
    glUniform3fv(prog->getUniform("kdFront"), 1, colors["grey"].data());
    MV->pushMatrix();
    MV->translate(Vector3f(0.45*1.1, 0, 0));
    drawHalf(MV, prog, 0, Vector3f(inspctExp + 1.0, 0, 0));
    drawHalf(MV, prog, 180, Vector3f(inspctExp + 1.0, 0, 0));
    MV->popMatrix();
    
     
}

void Crystal::drawHalf(shared_ptr<MatrixStack> MV, shared_ptr<Program> prog, float rot, Vector3f translate) 
{
    MV->pushMatrix();
    MV->scale(scale);
    MV->rotate(rot, Vector3f(0, 1, 0));
    MV->translate(translate);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
    half->draw(prog);

    MV->popMatrix();
}


void Crystal::drawEighth(shared_ptr<MatrixStack> MV, shared_ptr<Program> prog, float rot, Vector3f translate)
{
    
    MV->pushMatrix();
    MV->rotate(rot, Vector3f(0.0, 1.0, 0.0));
    MV->scale(scale);
    MV->translate(translate);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
    eighth->draw(prog);
    
    MV->popMatrix();
}

