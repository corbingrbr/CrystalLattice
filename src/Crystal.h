#ifndef _CRYSTAL_H
#define _CRYSTAL_H

#include <memory>
#include <vector>

class MatrixStack;
class Program;
class UnitCell;
class Shape;

class Crystal
{

public:
    
    enum Type { SIMPLE, BODY, FACE };        

    
    Crystal(int type, std::shared_ptr<Shape> shape);
    virtual ~Crystal();
    void init();
    void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog);
    void expand();
    void contract();

private:
    
    //void genStructure();

    int type;
    int rows;
    int cols;
    int height;
    float scale;
    float expansion;

    std::shared_ptr<UnitCell> unit;
    std::shared_ptr<Shape> eighth;
    
};

#endif
