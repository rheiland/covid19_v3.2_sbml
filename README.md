# covid19_v3.2_sbml
Incorporate libRoadrunner and SBML models into the COVID19 model (https://github.com/pc4covid19/COVID19/releases/tag/0.3.2).

## Installation/Build

* In `PhysiCell/beta`, run `setup_libroadrunner.py` and follow the instructions there.
* Run `make` (may need to edit the paths for `LIBRR_DIR` and `LIBRR_CFLAGS`)

### macOS
NOTE (currently): since the libroadrunner binary libs are built for native macOS (i.e., built with clang, not gcc), we need to use clang++ here. And since Apple's clang++ doesn't support OpenMP (at least up to `Apple clang version 11.x`), one would need to install and use a clang-compatible OpenMP lib, e.g. via `brew install libomp`.
Then, for example, we can compile on macOS:
```
~/git/covid19_v3.2_sbml/COVID19-0.3.2-sbml/PhysiCell$ make
clang++ -march=native  -fomit-frame-pointer -Xpreprocessor -fopenmp -m64 -std=c++11 -D LIBROADRUNNER  -I/Users/heiland/libroadrunner/roadrunner-osx-10.9-cp36m/include/rr/C  -c ./BioFVM/BioFVM_vector.cpp 
...
clang++ -march=native  -fomit-frame-pointer -Xpreprocessor -fopenmp -m64 -std=c++11 -D LIBROADRUNNER  -I/Users/heiland/libroadrunner/roadrunner-osx-10.9-cp36m/include/rr/C  -c ./custom_modules/epithelium_submodel.cpp 
Your OS= -D OSX -D IA32
LIBRR_CFLAGS= -I/Users/heiland/libroadrunner/roadrunner-osx-10.9-cp36m/include/rr/C
LIBRR_LIBS= /Users/heiland/libroadrunner/roadrunner-osx-10.9-cp36m/lib

clang++ -march=native  -fomit-frame-pointer -Xpreprocessor -fopenmp -m64 -std=c++11 -D LIBROADRUNNER  -I/Users/heiland/libroadrunner/roadrunner-osx-10.9-cp36m/include/rr/C  -L/usr/local/opt/libomp/lib -lomp -o COVID19_sbml BioFVM_vector.o BioFVM_mesh.o BioFVM_microenvironment.o BioFVM_solvers.o BioFVM_matlab.o BioFVM_utilities.o BioFVM_basic_agent.o BioFVM_MultiCellDS.o BioFVM_agent_container.o   pugixml.o PhysiCell_phenotype.o PhysiCell_cell_container.o PhysiCell_standard_models.o PhysiCell_cell.o PhysiCell_custom.o PhysiCell_utilities.o PhysiCell_constants.o  PhysiCell_SVG.o PhysiCell_pathology.o PhysiCell_MultiCellDS.o PhysiCell_various_outputs.o PhysiCell_pugixml.o PhysiCell_settings.o custom.o submodel_data_structures.o internal_viral_dynamics.o internal_viral_response.o receptor_dynamics.o immune_submodels.o epithelium_submodel.o main.cpp -L/Users/heiland/libroadrunner/roadrunner-osx-10.9-cp36m/lib -lroadrunner_c_api

created COVID19_sbml
```

### Windows/MinGW

(someone care to contribute?)

### Linux

(you're all wizards, you know what to do)

## High-level explanation of the code

Search for "sbml" in the config file, e.g., https://github.com/rheiland/covid19_v3.2_sbml/blob/master/COVID19-0.3.2-sbml/PhysiCell/config/PhysiCell_settings.xml#L428 (note that the links here may become stale, out of sync with the actual file).
This is defined in a `<molecular>` element and is how we associate a SBML model with a cell type. In the current model, we only define a SBML model for the "lung epithelium" cell_definition. We also define a mapping in the same `<molecular>` element that will (help) map the SBML species to a cell's custom data.

We added the SBML filename to the Cell_Definition class: https://github.com/rheiland/covid19_v3.2_sbml/blob/master/COVID19-0.3.2-sbml/PhysiCell/core/PhysiCell_cell.h#L124

We added code to parse the `<molecular>` element at https://github.com/rheiland/covid19_v3.2_sbml/blob/master/COVID19-0.3.2-sbml/PhysiCell/core/PhysiCell_cell.cpp#L2178.

The SBML-related functionality is currently done in custom.cpp: `create_cell_types`, `setup_tissue`, and ??. It will depend on the time scale that the SBML model needs to be solved.

Make sure your executable can find the libroadrunner libraries, e.g. for macOS:
```
$ export DYLD_LIBRARY_PATH=/Users/heiland/libroadrunner/roadrunner-osx-10.9-cp36m/lib
```

## Sample run
```
~/git/covid19_v3.2_sbml/COVID19-0.3.2-sbml/PhysiCell$ COVID19_sbml
...

------------->>>>>  Creating rrHandle, loadSBML file

------------->>>>>  SBML file = ./config/Toy_SBML_Model_1.xml
Placing 279 virions ... 
Using PhysiCell version 1.7.2beta
	Please cite DOI: 10.1371/journal.pcbi.1005991
	Project website: http://PhysiCell.MathCancer.org

See ALL_CITATIONS.txt for this list.


***** This is COVID19 integrated version 0.3.2. *****

current simulated time: 0 min (max: 8640 min)
total agents: 2843
...
``` 
