#ifndef _UNIT_CELL_H
#define _UNIT_CELL_H

#include <memory>

class Shape;
class Program;
class MatrixStack;

class UnitCell
{
    
public:

UnitCell(std::shared_ptr<Shape> shape) 
    : eighth(shape){}
    
    virtual void draw(std::shared_ptr<MatrixStack> MV, std:: shared_ptr<Program> prog) = 0;

protected:

    std::shared_ptr<Shape> eighth;

};


#endif
