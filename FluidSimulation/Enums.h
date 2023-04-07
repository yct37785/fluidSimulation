#pragma once

// defines
#define PI 3.141592653589793
const static int WINDOWS_WIDTH = 800;
const static int WINDOWS_HEIGHT = 800;

// common fluid
const float G = -9.81f;	// gravity = -9.81m/s^2, adjustable depending on visual
const float Kcfl = 1.f;	// timestep scale, 1 - 5
// eulerian
const float H = 1.f;	// width/height of a grid cell
const float Hoffset = H * 0.5f;
// lagrangian
const float REST_DENS = 300.f;  // rest density
const float GAS_CONST = 2000.f; // const for equation of state
const float Hrad = 16.f;           // kernel radius
const float Hrad2 = pow(Hrad, 2);        // radius^2 for optimization
const float MASS = 2.5f;        // assume all particles have the same mass
const float VISC = 200.f;       // viscosity constant
const float POLY6 = 4.f / (PI * pow(Hrad, 8.f));
const float POLY6_GRAD = -10.f / (PI * pow(Hrad, 5.f));
const float VISC_LAP = 40.f / (PI * pow(Hrad, 5.f));
const float EPS = Hrad; // boundary epsilon
const float BOUND_DAMPING = -0.5f;

// enum
enum INPUT_TYPES {
	INPUT_UNFREEZE_FLUID,
	INPUT_UNFREEZE_FLUID_2,
	INPUT_FWD,
	INPUT_BWD,
	INPUT_LEFT,
	INPUT_RIGHT,
	INPUT_UP,
	INPUT_DW,
	INPUT_ENABLE_WIREFRAME,
	INPUT_DISABLE_WIREFRAME,
	INPUT_QUIT,
	INPUT_TOTAL
};

enum CELL_TYPES {
	CELL_LIQUID,
	CELL_AIR,
	CELL_PARTIALLY_SOLID
};