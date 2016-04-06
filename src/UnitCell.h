#ifndef _UNIT_CELL_H
#define _UNIT_CELL_H

#include <memory>
#include <map>
#include <string>
#include <Eigen/Dense>

class Shape;
class Program;
class MatrixStack;

class UnitCell
{
    
public:

UnitCell(std::shared_ptr<Shape> eighth, std::shared_ptr<Shape> sphere, std::map<std::string, Eigen::Vector3f> colors) 
    : eighth(eighth),
        sphere(sphere),
        colors(colors){}
    
    virtual void draw(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog, Eigen::Vector3f pos, float alpha, bool center) = 0;

protected:

    std::shared_ptr<Shape> eighth;
    std::shared_ptr<Shape> sphere;
    std::map<std::string, Eigen::Vector3f> colors;

};


#endif
