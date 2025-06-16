#include "Simulation.h"


Simulation::Simulation(size_t N) :
	N{ N }, size{ (N + 2) * (N + 2)},
	d_vx(size, 0.f), d_vx_prev(size, 0.f), d_vy(size, 0.f), d_vy_prev(size, 0.f), d_dens(size, 0.f), d_dens_prev(size, 0.f),
	vx{ nullptr }, vx_prev{ nullptr }, vy{ nullptr }, vy_prev{ nullptr }, dens{ nullptr }, dens_prev{ nullptr } {

	vx = d_vx.data();
	vx_prev = d_vx_prev.data();

	vy = d_vy.data();
	vy_prev = d_vy_prev.data();

	dens = d_dens.data();
	dens_prev = d_dens_prev.data();
}

size_t Simulation::IX(size_t i, size_t j) {
	return i + (N + 2) * j;
}

void Simulation::SWAP(float* x0, float* x) {
	float* tmp = x0;
	x0 = x;
	x = tmp;
}


void Simulation::add_source(size_t N, float* x, float* s, float dt) {
	size_t size = (N + 2) * (N + 2);
	for (size_t i = 0; i < size; i++) x[i] += dt * s[i];
}

void Simulation::diffuse(size_t N, int b, float* x, float* x0, float diff, float dt) {
	const float a = dt * diff * N * N;

	for (size_t k = 0; k < 20; k++) {
		for (size_t i = 1; i <= N; i++) {
			for (size_t j = 1; j <= N; j++) {

				x[IX(i, j)] = (x0[IX(i, j)] + a * (x[IX(i - 1, j)] + x[IX(i + 1, j)] + x[IX(i, j - 1)] + x[IX(i, j + 1)])) / (1.f + 4.f * a);
			}
		}
		set_bnd(N, b, x);
	}
}

void Simulation::advect(size_t N, int b, float* d, float* d0, float* u, float* v, float dt) {

	const float dt0 = dt * N;
	for (size_t i = 1; i <= N; i++) {
		for (size_t j = 1; j <= N; j++) {
			float x = i - dt0 * u[IX(i, j)];
			float y = j - dt0 * v[IX(i, j)];

			if (x < 0.5f) x = 0.5f;
			if (x > (float)N + 0.5f) x = (float)N + 0.5f;

			size_t i0 = (size_t)x;
			size_t i1 = i0 + 1;

			if (y < 0.5f) y = 0.5f;
			if (y > (float)N + 0.5f) y = (float)N + 0.5f;
			size_t j0 = (size_t)y;
			size_t j1 = j0 + 1;

			float s1 = x - i0;
			float s0 = 1 - s1;
			float t1 = y - j0;
			float t0 = 1 - t1;

			d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) + s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
		}
	}

	set_bnd(N, b, d);
}

void Simulation::project(size_t N, float* u, float* v, float* p, float* div) {
	const float h = 1.0f / ((float)N);
	for (size_t i = 1; i <= N; i++) {
		for (size_t j = 1; j <= N; j++) {
			div[IX(i, j)] = -0.5f * h * (u[IX(i + 1, j)] - u[IX(i - 1, j)] +
				v[IX(i, j + 1)] - v[IX(i, j - 1)]);
			p[IX(i, j)] = 0.f;
		}
	}

	set_bnd(N, 0, div);
	set_bnd(N, 0, p);

	for (size_t k = 0; k < 20; k++) {
		for (size_t i = 1; i <= N; i++) {
			for (size_t j = 1; j <= N; j++) {
				p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] + p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4.f;
			}
		}
		set_bnd(N, 0, p);
	}

	for (size_t i = 1; i <= N; i++) {
		for (size_t j = 1; j <= N; j++) {
			u[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) / h;
			v[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) / h;
		}
	}

	set_bnd(N, 1, u);
	set_bnd(N, 2, v);
}

void Simulation::set_bnd(size_t N, int b, float* x) {
	for (size_t i = 1; i <= N; i++) {
		x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
		x[IX(N + 1, i)] = b == 1 ? -x[IX(N, i)] : x[IX(N, i)];
		x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
		x[IX(i, N + 1)] = b == 2 ? -x[IX(i, N)] : x[IX(i, N)];
	}

	x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
	x[IX(0, N + 1)] = 0.5f * (x[IX(1, N + 1)] + x[IX(0, N)]);
	x[IX(N + 1, 0)] = 0.5f * (x[IX(N, 0)] + x[IX(N + 1, 1)]);
	x[IX(N + 1, N + 1)] = 0.5f * (x[IX(N, N + 1)] + x[IX(N + 1, N)]);
}

void Simulation::vel_step(size_t N, float* u, float* v, float* u0, float* v0, float visc, float dt) {
	add_source(N, u, u0, dt); add_source(N, v, v0, dt);
	SWAP(u0, u); diffuse(N, 1, u, u0, visc, dt);
	SWAP(v0, v); diffuse(N, 2, v, v0, visc, dt);
	project(N, u, v, u0, v0);
	SWAP(u0, u); SWAP(v0, v);
	advect(N, 1, u, u0, u0, v0, dt); advect(N, 2, v, v0, u0, v0, dt);
	project(N, u, v, u0, v0);
}

void Simulation::dens_step(size_t N, float* x, float* x0, float* u, float* v, float diff, float dt) {
	add_source(N, x, x0, dt);
	SWAP(x0, x); diffuse(N, 0, x, x0, diff, dt);
	SWAP(x0, x); advect(N, 0, x, x0, u, v, dt);
}
