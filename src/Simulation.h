#include <vector>


class Simulation {
public:
	Simulation(size_t N);

	void update(float dt);

	void add_source();

	float* vx();
	float* vy();
	float* densities();

	size_t N;

	std::vector<float>vx_1;
	std::vector<float>vx_2;

	std::vector<float>vy_1;
	std::vector<float>vx_2;

	std::vector<float>densities_1;
	std::vector<float>densities_2;

	void diffuse();
	void advect();
	void project();
	void set_bnd();
	void vel_step();
	void den_step();
};