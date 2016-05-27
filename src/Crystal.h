#ifndef _CRYSTAL_H
#define _CRYSTAL_H

#include <memory>
#include <map>
#include <vector>
#include <string>
#include <utility>

#include <Eigen/Dense>

class MatrixStack;
class Program;
class UnitCell;
class Shape;
class Layer;

class Crystal
{

public:
    
    enum Type { SIMPLE, BODY, FACE };        

    
    Crystal(int type, std::shared_ptr<Shape> eighth, std::shared_ptr<Shape> half, std::shared_ptr<Shape> sphere);
    virtual ~Crystal();
    void init();
    void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog);
    void expand();
    void contract();
    void toggleTranslucency();
    void setDrawLayers();
    void toggleLayers();
    void toggleInspection();

private:
    
    struct Cell 
    {
        float distance;
        Eigen::Vector3d bounds;
        Eigen::Vector4f pos;
        Eigen::Vector3d ndx;

    Cell(Eigen::Vector3d bounds, Eigen::Vector4f pos, Eigen::Vector3d ndx) : distance(0.0), bounds(bounds), pos(pos), ndx(ndx) {}
    };
    

    void drawCells(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog);
    void drawLayers(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog);
    void drawInspect(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog);

    void initCellPositions();
    float calcCellDistance(Eigen::Matrix4f m, Eigen::Vector4f v);
    void sortCells(Eigen::Matrix4f viewMatrix);

    static bool sortAlg(Cell c1, Cell c2)
    {
        return c1.distance > c2.distance;
    }

    void createSimpleLayers();
    void createBodyLayers();
    void createFaceLayers();

    void drawSimpleInspect(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog);
    void drawBodyInspect(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog);
    void drawFaceInspect(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog);
    void drawEighth(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog, float rot, Eigen::Vector3f translate);
    void drawHalf(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog, float rot, Eigen::Vector3f translate);

    int type;
    int rows;
    int cols;
    int height;
    float scale;
    float expansion;
    float inspctExp;
    bool translucent;
    bool layersDraw;
    bool inspecting;


    std::shared_ptr<UnitCell> unit;
    std::shared_ptr<Shape> eighth;
    std::shared_ptr<Shape> half;
    std::shared_ptr<Shape> sphere;
    std::map<std::string, Eigen::Vector3f> colors;
    std::vector<Cell> cells;
    std::vector<std::shared_ptr<Layer> > layers;
};

#endif
