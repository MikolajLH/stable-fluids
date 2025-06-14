#include <vector>


class Simulation {
public:
	Simulation(size_t N): N{N}, data(N, 0.f) {}

	void update(float dt);

	void add_source();

private:
	size_t N;
	std::vector<float>data;

	void diffuse();
	void advect();
	void project();
	void set_bnd();
	void vel_step();
	void den_step();
};