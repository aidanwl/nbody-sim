# NBody Simulator
Aidan Lam and Lucas Teichmann

### Description

This project simulates the motion of N-body gravitational systems using Newtonian physics. Built in C with raylib, the simulator models interactions between multiple bodies using Verlet integration, allowing users to explore stable orbits and chaotic systems in real time. Features include adjustable simulation speed, trajectory visualization, camera controls, and body creation/editing. 

The project is structured with modular physics, rendering, math, and UI components, making it easier to extend the project to higher-order integrators, collision systems, and 3D simulation.

### Installation

This proejct uses raylib, which depends on several X11 and OpenGL development packages on Linux systems. 

Install the required dependencies with:

```bash
sudo apt install libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev
sudo apt install libgl1-mesa-dev libglu1-mesa-dev
```

### Building

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Running

To run the program, run this file in the build directory:

```bash
./nbody
```
