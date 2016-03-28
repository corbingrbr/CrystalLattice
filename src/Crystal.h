#ifndef _CRYSTAL_H
#define _CRYSTAL_H

#include <memory>
#include <map>
#include <vector>
#include <string>

#include <Eigen/Dense>

class MatrixStack;
class Program;
class UnitCell;
class Shape;

class Crystal
{

public:
    
    enum Type { SIMPLE, BODY, FACE };        

    
    Crystal(int type, std::shared_ptr<Shape> eighth, std::shared_ptr<Shape> sphere);
    virtual ~Crystal();
    void init();
    void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog);
    void expand();
    void contract();
    void toggleTranslucency();

private:

    int type;
    int rows;
    int cols;
    int height;
    float scale;
    float expansion;
    bool translucent;

    std::shared_ptr<UnitCell> unit;
    std::shared_ptr<Shape> eighth;
    std::shared_ptr<Shape> sphere;
    std::map<std::string, Eigen::Vector3f> colors;
};

#endif
