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

    enum Type { MIN, ONEB4MIN, MIDDLE, ONEB4MAX, MAX }; // Used for edge case cells

UnitCell(std::shared_ptr<Shape> eighth, std::shared_ptr<Shape> half, std::shared_ptr<Shape> sphere, std::map<std::string, Eigen::Vector3f> colors) 
    : eighth(eighth),
        half(half),
        sphere(sphere),
        colors(colors),
        scale(1.0){}
    
    virtual void draw(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog, Eigen::Vector3f pos, float alpha, bool center, Eigen::Vector3d bounds, Eigen::Vector3d ndx) = 0;    

protected:

    std::shared_ptr<Shape> eighth;
    std::shared_ptr<Shape> half;
    std::shared_ptr<Shape> sphere;
    std::map<std::string, Eigen::Vector3f> colors;
    float scale;
};


#endif
