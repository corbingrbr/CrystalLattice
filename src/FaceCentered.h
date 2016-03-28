#pragma once
#ifndef _FACE_CENTER_H
#define _FACE_CENTER_H

#include <memory>
#include <map>
#include <string>
#include "UnitCell.h"
#include <Eigen/Dense>

class Shape;
class Program;
class MatrixStack;

class FaceCentered : public UnitCell
{
public:
   
    FaceCentered(std::shared_ptr<Shape> eighth, std::shared_ptr<Shape> sphere, std::map<std::string, Eigen::Vector3f> colors);
    virtual ~FaceCentered();
    void draw(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog, float alpha, bool center);
    
private:
    void drawEighth(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog, float rot);
    
};
    
#endif