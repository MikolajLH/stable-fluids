#include <vector>


class Simulation {
public:
	Simulation(size_t N): N{N}, data(N, 0.f) {}

	void update(float dt);

private:
	size_t N;
	std::vector<float>data;
};