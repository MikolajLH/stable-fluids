#ifndef __GRIDSTABLESOLVER_H__
#define __GRIDSTABLESOLVER_H__

class StableSolver
{
public:
    StableSolver();
    ~StableSolver();
    void init();
    void reset();
    void cleanBuffer();
    void start() { running = 1; }
    void stop() { running = 0; }
    int isRunning() { return running; }

    //animation
    void setBoundary(float* value, int flag);
    void projection();
    void advection(float* value, float* value0, float* u, float* v, int flag);
    void diffusion(float* value, float* value0, float rate, int flag);
    void vortConfinement();
    void addSource();
    void animVel();
    void animDen();

    //getter
    int getRowSize() { return rowSize; }
    int getColSize() { return colSize; }
    int getTotSize() { return totSize; }
    float getH() { return h; }
    float getSimSizeX() { return simSizeX; }
    float getSimSizeY() { return simSizeY; }
    float* getVX() { return vx; }
    float* getVY() { return vy; }
    float* getD() { return d; }
    float* getPX() { return px; }
    float* getPY() { return py; }
    float getDens(int i, int j) { return (d[cIdx(i - 1, j - 1)] + d[cIdx(i, j - 1)] + d[cIdx(i - 1, j)] + d[cIdx(i, j)]) / 4.0f; }

    //setter
    void setVX0(int i, int j, float value) { vx0[cIdx(i, j)] = value; }
    void setVY0(int i, int j, float value) { vy0[cIdx(i, j)] = value; }
    void setD0(int i, int j, float value) { d0[cIdx(i, j)] = value; }

private:
    int cIdx(int i, int j) { return j * rowSize + i; }

private:
    int rowSize;
    int colSize;
    int totSize;
    float h;
    float simSizeX;
    float simSizeY;
    float minX;
    float maxX;
    float minY;
    float maxY;

    //params
    int running;
    float visc;
    float diff;
    float vorticity;
    float timeStep;

    float* vx;
    float* vy;
    float* vx0;
    float* vy0;
    float* d;
    float* d0;
    float* px;
    float* py;
    float* div;
    float* p;
    //vorticity confinement
    float* vort;
    float* absVort;
    float* gradVortX;
    float* gradVortY;
    float* lenGrad;
    float* vcfx;
    float* vcfy;
};

#endif