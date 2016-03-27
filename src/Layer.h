#ifndef _LAYER_H
#define _LAYER_H

#include <memory>

class UnitCell;
class MatrixStack;
class Program;

class Layer
{
public:

    Layer(int width, int length, float height, std::shared_ptr<UnitCell> unitCell);
    virtual ~Layer();
    void init();
    void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog);
    

private:
    
    std::shared_ptr<UnitCell> unitCell;
    int width;
    int length;
    float height;

};

#endif
