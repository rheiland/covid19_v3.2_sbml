#include "./epithelium_submodel.h"
#include <iomanip>

using namespace PhysiCell;

std::string epithelium_submodel_version = "0.1.0";

Submodel_Information epithelium_submodel_info;

extern int energy_cell_idx;
extern int glucose_microenv_idx;
extern int oxygen_microenv_idx;
extern int glucose_sbml_species_idx;
extern int oxygen_sbml_species_idx;

void epithelium_contact_function( Cell* pC1, Phenotype& p1, Cell* pC2, Phenotype& p2, double dt )
{
	// elastic adhesions
	standard_elastic_contact_function( pC1,p1, pC2, p2, dt );
	return;
}

void energy_based_cell_phenotype(Cell* pCell, Phenotype& phenotype , double dt)
{
	if (pCell->type != 1)
		return;

	std::cout << "------ energy_based_cell_phenotype() ------" << std::endl;

#ifdef LIBROADRUNNER
	rrc::RRVectorPtr vptr;
	rrc::RRCDataPtr result;  // start time, end time, and number of points
	// pC->phenotype.molecular.model_rr = rrHandle;  // assign the intracellular model to each cell
	vptr = rrc::getFloatingSpeciesConcentrations(pCell->phenotype.molecular.model_rr);
	std::cout << "--- before updating:  vptr =" << vptr << std::endl;
	std::cout << "---   			vptr->Count =" << vptr->Count << std::endl;
	for (int idx=0; idx<vptr->Count; idx++)
		std::cout << idx << ", " << vptr->Data[idx] << std::endl;

	// vptr->Data[oxygen_index] += 0.1;
	// rrc::setFloatingSpeciesConcentrations(pCell->phenotype.molecular.model_rr, vptr);

	vptr = rrc::getFloatingSpeciesConcentrations(pCell->phenotype.molecular.model_rr);
	// std::cout << vptr->Count << std::endl;
	std::cout << "--- after updating oxygen:" << std::endl;
	for (int idx=0; idx<vptr->Count; idx++)
		std::cout << idx << ", " << vptr->Data[idx] << std::endl;

	int vi = microenvironment.nearest_voxel_index(pCell->position);
	double oxy_val = microenvironment(vi)[oxygen_microenv_idx];
	double glucose_val = microenvironment(vi)[glucose_microenv_idx];
	std::cout << "oxy_val at voxel of cell = " << oxy_val << std::endl;
	std::cout << "glucose_val at voxel of cell = " << glucose_val << std::endl;

	vptr->Data[oxygen_microenv_idx] = oxy_val;
	vptr->Data[glucose_microenv_idx] = glucose_val;
	rrc::setFloatingSpeciesConcentrations(pCell->phenotype.molecular.model_rr, vptr);

	result = rrc::simulateEx (pCell->phenotype.molecular.model_rr, 0, 10, 10);  // start time, end time, and number of points
	int index = 0;
	// Print out column headers... typically time and species.
	for (int col = 0; col < result->CSize; col++)
	{
		// std::cout << result->ColumnHeaders[index++];
		std::cout << std::left << std::setw(15) << result->ColumnHeaders[index++];
		// if (col < result->CSize - 1)
		// {
		// 	// std::cout << "\t";
		// 	std::cout << "  ";
		// }
	}
	std::cout << "\n";

	index = 0;
	// Print out the data
	for (int row = 0; row < result->RSize; row++)
	{
		for (int col = 0; col < result->CSize; col++)
		{
			// std::cout << result->Data[index++];
			std::cout << std::left << std::setw(15) << result->Data[index++];
			// if (col < result->CSize -1)
			// {
			// 	// std::cout << "\t";
			// 	std::cout << "  ";
			// }
		}
		std::cout << "\n";
	}
	int idx = (result->RSize - 1) * result->CSize + 1;
	std::cout << "Saving last energy value (cell custom var) = " << result->Data[idx] << std::endl;
	pCell->custom_data[energy_cell_idx]  = result->Data[idx];
#endif

}

void epithelium_phenotype( Cell* pCell, Phenotype& phenotype, double dt )
{
	static int debris_index = microenvironment.find_density_index( "debris");

	// SBML model function for "energy" (custom data for each epi cell)
	energy_based_cell_phenotype(pCell, phenotype , dt);

	
	// receptor dynamics 
	// requires faster time scale - done in main function 
	
	// viral dynamics model 
	internal_viral_dynamics_info.phenotype_function(pCell,phenotype,dt); 
	// internal_virus_model(pCell,phenotype,dt);
	
	// viral response model 
	internal_virus_response_model_info.phenotype_function(pCell,phenotype,dt); 
	// internal_virus_response_model(pCell,phenotype,dt);	
	
	// T-cell based death
	TCell_induced_apoptosis(pCell, phenotype, dt ); 
	
	// if I am dead, remove all adhesions 
	static int apoptosis_index = phenotype.death.find_death_model_index( "apoptosis" ); 
	if( phenotype.death.dead == true )
	{
		// detach all attached cells 
		// remove_all_adhesions( pCell ); 
		
		phenotype.secretion.secretion_rates[debris_index] = pCell->custom_data["debris_secretion_rate"]; 
	}
	
/*
	// cell secretion belongs in viral response 
	
	// if I am dead, make sure to still secrete the chemokine 
	static int chemokine_index = microenvironment.find_density_index( "chemokine" ); 
	static int nP = pCell->custom_data.find_variable_index( "viral_protein"); 
	double P = pCell->custom_data[nP];
	
	static int nAV = pCell->custom_data.find_variable_index( "assembled_virion" ); 
	double AV = pCell->custom_data[nAV]; 

	static int nR = pCell->custom_data.find_variable_index( "viral_RNA");
	double R = pCell->custom_data[nR];
	
	if( R >= 1.00 - 1e-16 ) 
	{
		pCell->custom_data["infected_cell_chemokine_secretion_activated"] = 1.0; 
	}

	if( pCell->custom_data["infected_cell_chemokine_secretion_activated"] > 0.1 && phenotype.death.dead == false )
	{
		double rate = AV; // P; 
		rate /= pCell->custom_data["max_apoptosis_half_max"];
		if( rate > 1.0 )
		{ rate = 1.0; }
		rate *= pCell->custom_data[ "infected_cell_chemokine_secretion_rate" ];

		phenotype.secretion.secretion_rates[chemokine_index] = rate; 
		phenotype.secretion.saturation_densities[chemokine_index] = 1.0; 
	}
*/	
	
	// if I am dead, don't bother executing this function again 
	if( phenotype.death.dead == true )
	{
		pCell->functions.update_phenotype = NULL; 
	}
	
	return; 
}

void epithelium_mechanics( Cell* pCell, Phenotype& phenotype, double dt )
{
	static int debris_index = microenvironment.find_density_index( "debris");
	
	pCell->is_movable = false; 
	
	// if I'm dead, don't bother 
	if( phenotype.death.dead == true )
	{
		// the cell death functions don't automatically turn off custom functions, 
		// since those are part of mechanics. 
		// remove_all_adhesions( pCell ); 
		
		// Let's just fully disable now. 
		pCell->functions.custom_cell_rule = NULL; 
		pCell->functions.contact_function = NULL; 

		phenotype.secretion.secretion_rates[debris_index] = pCell->custom_data["debris_secretion_rate"]; 
		return; 
	}	
	
	// this is now part of contact_function 
	/*
	// if I'm adhered to something ... 
	if( pCell->state.neighbors.size() > 0 )
	{
		// add the elastic forces 
		extra_elastic_attachment_mechanics( pCell, phenotype, dt );
	}
	*/
	return; 
}

void epithelium_submodel_setup( void )
{
	Cell_Definition* pCD;
	
	// set up any submodels you need 
	// viral replication 
	
	// receptor trafficking 
	receptor_dynamics_model_setup(); // done 
	// viral replication 
	internal_virus_model_setup();	
	// single-cell response 
	internal_virus_response_model_setup(); 
 	
	// set up epithelial cells
		// set version info 
	epithelium_submodel_info.name = "epithelium model"; 
	epithelium_submodel_info.version = epithelium_submodel_version; 
		// set functions 
	epithelium_submodel_info.main_function = NULL; 
	epithelium_submodel_info.phenotype_function = epithelium_phenotype; 
	epithelium_submodel_info.mechanics_function = epithelium_mechanics; 
	
		// what microenvironment variables do you expect? 
	epithelium_submodel_info.microenvironment_variables.push_back( "virion" ); 
	epithelium_submodel_info.microenvironment_variables.push_back( "interferon 1" ); 
	epithelium_submodel_info.microenvironment_variables.push_back( "pro-inflammatory cytokine" ); 
	epithelium_submodel_info.microenvironment_variables.push_back( "chemokine" ); 
		// what custom data do I need? 
	//epithelium_submodel_info.cell_variables.push_back( "something" ); 
		// register the submodel  
	epithelium_submodel_info.register_model();	
		// set functions for the corresponding cell definition 
	pCD = find_cell_definition( "lung epithelium" ); 
	pCD->functions.update_phenotype = epithelium_submodel_info.phenotype_function;
	pCD->functions.custom_cell_rule = epithelium_submodel_info.mechanics_function;
	pCD->functions.contact_function = epithelium_contact_function; 
	
	return;
}

void TCell_induced_apoptosis( Cell* pCell, Phenotype& phenotype, double dt )
{
	static int apoptosis_index = phenotype.death.find_death_model_index( "Apoptosis" ); 
	static int debris_index = microenvironment.find_density_index( "debris" ); 
	static int proinflammatory_cytokine_index = microenvironment.find_density_index("pro-inflammatory cytokine");
	
	if( pCell->custom_data["TCell_contact_time"] > pCell->custom_data["TCell_contact_death_threshold"] )
	{
		// make sure to get rid of all adhesions! 
		// detach all attached cells 
		// remove_all_adhesions( pCell ); 
		
		#pragma omp critical
		{
		std::cout << "\t\t\t\t" << pCell << " (of type " << pCell->type_name <<  ") died from T cell contact" << std::endl; 
		}
		
		// induce death 
		pCell->start_death( apoptosis_index ); 
		
		pCell->phenotype.secretion.secretion_rates[proinflammatory_cytokine_index] = 0; 
		pCell->phenotype.secretion.secretion_rates[debris_index] = pCell->custom_data["debris_secretion_rate"]; 
		
		pCell->functions.update_phenotype = NULL; 
	}
	
	return; 
}

