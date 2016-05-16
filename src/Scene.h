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
    void nextCrystal();
    std::shared_ptr<Crystal> getCrystal();
    void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog);
    void expand();
    void contract();
    void toggleTranslucency();
    void toggleLayers();
    void toggleInspection();

private:
    
    int whichCrystal;
    std::shared_ptr<Shape> eighth;
    std::shared_ptr<Shape> half;
    std::shared_ptr<Shape> sphere;
    std::vector<std::shared_ptr<Crystal> > crystals;
    

};

#endif
