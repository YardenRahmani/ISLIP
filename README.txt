## ISLIP

ISLIP and Traffic Generator for Switch Simulation is a project that simulates the transport of packets in a switch.
The packets arrival follows a Binomial distribution, with packets arriving at each time step and each input port having a chance 'p.'


## Skills Demonstrated in the Project

- C programming
- Multi-dimensional dynamic memory usage
- Data simulation using Python script
- Statistical analysis of simulation results
- Data visualization using Matplotlib


## Project Structure

The project consists of two source codes: "tr_gen.c" and "islip.c". Each source code can be compiled into an executable file.

- tr_gen:
This program generates a packet arrival schedule for the switch simulation.
Each time step and input port has an independent chance 'p' of a packet arriving.
The output port of each packet is determined by either a uniform or diagonal distribution.

- islip:
Simulating switch packet handling, this program processes packets arriving based on the generated schedule.
It implements a round-robin scheduling mechanism involving request (from input), grant (from output), and accept (from input) phases.

- Simulation Script:
A Python script, `simulation_script.py`, is provided for running simulations with different 'p' values and analyzing the results.
The script generates time and queue size statistics for multiple runs.


## Build and Run Instructions

1. Compile Source Codes:
   - Compile "tr_gen.c" and "islip.c" using a C compiler. For example, using GCC:
     "gcc tr_gen.c -o tr_gen.exe
     gcc islip.c -o islip.exe"

2. Run Simulation Script:
   - Ensure you have Python3 installed.
   - Open a terminal in the project directory and execute:
     "python3 simulation_script.py"
     This script runs simulations with different 'p' values, generating time and queue size statistics.

3. Execution Notes:
   - Make sure both "tr_gen.exe" and "islip.exe" are in the same directory as the script for proper execution.

4. Adjustment:
   - Adjustments my be needed, based on your development environment.
