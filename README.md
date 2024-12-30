# Interactive Cloth Physics
A physics-based cloth simulation implemented in C++ using SFML. Features include:

Realistic cloth movement with gravity and wind effects
Interactive tearing mechanism
Color visualization of cloth tension
Verlet integration for stable physics simulation

Inspired by 'Building a Particle Simulation Physics Engine in C++' tutorial.
Prerequisites
SFML Installation
This project requires SFML 2.6.2. You can install it using Homebrew on macOS:
brew install sfml@2
Environment Setup
After installing SFML, you need to set up your environment variables. Add these lines to your ~/.zshrc or ~/.bash_profile:
export SFML_DIR="/usr/local/opt/sfml@2"
export LIBRARY_PATH="$SFML_DIR/lib"
export CPLUS_INCLUDE_PATH="$SFML_DIR/include"
After adding these lines, reload your shell configuration:
source ~/.zshrc  # or source ~/.bash_profile
Building the Project
Compilation Command
Use the following command to compile the project:
clang++ -std=c++17 -I/usr/local/opt/sfml@2/include -L/usr/local/opt/sfml@2/lib -lsfml-graphics -lsfml-window -lsfml-system main.cpp -o main
Running the Simulation
After successful compilation, run the simulation with:
./main
Controls

Left Mouse Button: Click and drag to tear the cloth
Close Window Button: Exit the simulation

Project Structure

main.cpp: Main simulation loop and setup
particle.h: Particle class implementation for cloth points
constraint.h: Constraint class for cloth connections
input_handler.h: Mouse interaction handling

Physics Parameters
The simulation can be customized by modifying these constants in main.cpp:

GRAVITY: Controls the downward force
WIND_STRENGTH: Controls the strength of wind effect
WIND_FREQUENCY: Controls how fast the wind changes
ROW and COL: Define the cloth's dimensions
REST_DISTANCE: Controls spacing between particles

Troubleshooting
If you encounter linking errors, ensure:

SFML is correctly installed: brew list sfml@2
Environment variables are properly set: echo $LIBRARY_PATH
SFML version matches (2.6.2): brew info sfml@2

License
MIT
Acknowledgments

Original tutorial: "Building a Particle Simulation Physics Engine in C++"
SFML Documentation and Community