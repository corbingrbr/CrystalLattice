#pragma once
#ifndef _BODY_CENTER_H
#define _BODY_CENTER_H

#include <memory>
#include <map>
#include <string>
#include "UnitCell.h"
#include <Eigen/Dense>

class Shape;
class Program;
class MatrixStack;

class BodyCentered : public UnitCell
{
public:
   
    BodyCentered(std::shared_ptr<Shape> eighth, std::shared_ptr<Shape> half, std::shared_ptr<Shape> sphere, std::map<std::string, Eigen::Vector3f> colors);
    virtual ~BodyCentered();
    void draw(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog, Eigen::Vector3f pos, float alpha, bool center, Eigen::Vector3d bounds, Eigen::Vector3d ndx);
    
private:
    void drawEighth(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog, float rot);
    
};
    
#endif
