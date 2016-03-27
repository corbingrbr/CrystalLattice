#pragma once
#ifndef _SIMPLE_CUBIC_H
#define _SIMPLE_CUBIC_H

#include <memory>
#include "UnitCell.h"
#include <Eigen/Dense>

class Shape;
class Program;
class MatrixStack;

class SimpleCubic : public UnitCell
{
public:
   
    SimpleCubic(std::shared_ptr<Shape> shape);
    virtual ~SimpleCubic();
    void draw(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog);
    
private:
    void drawEighth(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog, float rot);
    
};
    
#endif
