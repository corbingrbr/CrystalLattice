#include "Layer.h"

#include "UnitCell.h"
#include "SimpleCubic.h"
//#include "BodyCentered.h"
//#include "FaceCentered.h"

using namespace std;

Layer::Layer(int width, int length, float height, shared_ptr<UnitCell> unitCell) 
{
    this->unitCell = unitCell;
    this->width = width;
    this->length = length;
    this->height = height;
}

Layer::~Layer()
{
}

void Layer::init()
{
    // Create Layer crystalline according to type
    
    
}

void Layer::draw(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog)
{
    
}
