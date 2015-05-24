#include "stdafx.h"

#include "SimulationManager.h"

int main(int argc, const char **argv)
{	
	if (argc > 1)
	{
		if (std::string(argv[1]) == "--2D")
		{
			std::cout << "Running 2D simulations" << std::endl;
			auto cs = CSimulationManager<2UL>(argc - 1, &argv[1]);
			cs.RunSimulations();
		}
		if (std::string(argv[1]) == "--3D")
		{
			auto cs = CSimulationManager<3UL>(argc - 1, &argv[1]);
			cs.RunSimulations();
		}
	}
	else
	{
		std::cout << "You have to select 2D or 3D computations";
	}
	return EXIT_SUCCESS;
}