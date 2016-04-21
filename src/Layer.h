#ifndef _LAYER_H_
#define _LAYER_H_

#include <memory>
#include <Eigen/Dense>

class Shape;
class MatrixStack;
class Program;

class Layer 
{

public:

    Layer(int rows, int cols, float restHeight, float expansion, Eigen::Vector3f color, std::shared_ptr<Shape> sphere);
    virtual ~Layer();
    void reset();
    void update();
    void draw(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog);
    bool isAtRest();
    

private:
    int rows;
    int cols;
    float startHeight;
    float restHeight;
    float expansion;
    float curHeight;
    float speed;
    bool atRest;
    Eigen::Vector3f color;
    Eigen::Vector3f offset;
    std::shared_ptr<Shape> sphere;
};

#endif

