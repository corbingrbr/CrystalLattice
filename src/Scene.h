#pragma once
#ifndef __Scene__
#define __Scene__

#include <vector>
#include <memory>
#include <string>

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

class MatrixStack;
class Program;
class Shape;
class SimpleCubic;
class Crystal;

class Scene
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	
	Scene();
	virtual ~Scene();
	
	void load(const std::string &RESOURCE_DIR);
	void update();
    void expand();
    void contract();
    void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog);
    

private:

    std::shared_ptr<Shape> eighth;
    std::shared_ptr<Crystal> simpleCrystal;
    

};

#endif
