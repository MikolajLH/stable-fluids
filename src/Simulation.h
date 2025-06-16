#include <vector>


class Simulation {
public:
	Simulation(size_t N);

	size_t IX(size_t i, size_t j);
	void SWAP(float* x0, float* x);

	size_t N;
	size_t size;

	std::vector<float>d_vx;
	std::vector<float>d_vx_prev;

	std::vector<float>d_vy;
	std::vector<float>d_vy_prev;

	std::vector<float>d_dens;
	std::vector<float>d_dens_prev;

	float* vx;
	float* vx_prev;

	float* vy;
	float* vy_prev;

	float* dens;
	float* dens_prev;

	void add_source(size_t N, float* x, float* s, float dt);
	void diffuse(size_t N, int b, float* x, float* x0, float diff, float dt);
	void advect(size_t N, int b, float* d, float* d0, float* u, float* v, float dt);
	void project(size_t N, float* u, float* v, float* p, float* div);
	void set_bnd(size_t N, int b, float* x);
	void vel_step(size_t N, float* u, float* v, float* u0, float* v0, float visc, float dt);
	void dens_step(size_t N, float* x, float* x0, float* u, float* v, float diff, float dt);
};