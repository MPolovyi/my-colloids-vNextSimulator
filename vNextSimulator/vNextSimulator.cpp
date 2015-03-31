// first.cpp

/*    Copyright 2009 10gen Inc.
*
*    Licensed under the Apache License, Version 2.0 (the "License");
*    you may not use this file except in compliance with the License.
*    You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
*    Unless required by applicable law or agreed to in writing, software
*    distributed under the License is distributed on an "AS IS" BASIS,
*    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*    See the License for the specific language governing permissions and
*    limitations under the License.
*/

/**
* this is a good first example of how to use mongo from c++
*/

// It is the responsibility of the mongo client consumer to ensure that any necessary windows
// headers have already been included before including the driver facade headers.
#include "stdafx.h"

#include "Simulator.h"
#include "VicsecPPInterractor.h"
#include "UniformNoiseRotation.h"
#include "RectangularTransitionalBorders.h"
#include "DataSnap.h"
#include "SimpleSaver.h"
#include "JSONSaver.h"

#include "SimulationManager.h"

int main(int argc, const char **argv)
{	
	CSimulationManager cs(argc, argv);

	cs.RunSimulations();
	
	return EXIT_SUCCESS;
}