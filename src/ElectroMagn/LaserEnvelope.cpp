
#include "LaserEnvelope.h"

#include "Params.h"
#include "Patch.h"
#include "cField3D.h"
#include "Field3D.h"
#include "ElectroMagn.h"
#include "Profile.h"
#include "ElectroMagnFactory.h"
#include "EnvelopeBC.h"
#include "EnvelopeBC_Factory.h"
#include <complex>
#include "SimWindow.h"


using namespace std;

LaserEnvelope::LaserEnvelope( Params &params, Patch *patch, ElectroMagn *EMfields ) :
    profile_( NULL ),
    cell_length( params.cell_length ),
    timestep( params.timestep )
    
{

    PyObject *profile = NULL;
    if( !PyTools::extract_pyProfile( "envelope_profile", profile, "LaserEnvelope" ) ) {
        MESSAGE( "No envelope profile set !" );
    }
    profile_ = new Profile( profile, params.nDim_field+1, "envelope" );
    
    // params.Laser_Envelope_model = true;
    
    ostringstream name( "" );
    name << "Laser Envelope " << endl;
    ostringstream info( "" );
    
    // Read laser envelope parameters
    std:: string envelope_solver  = "explicit"; // default value
    
    PyTools::extract( "envelope_solver", envelope_solver, "LaserEnvelope" );
    
    
    
    // double omega_value(0);
    // PyTools::extract("omega",omega_value,"LaserEnvelope");
    
    //double a0_laser;
    //PyTools::extract("a0",a0_laser,"LaserEnvelope");
    
    //double waist_laser;
    //PyTools::extract("waist",waist_laser,"LaserEnvelope");
    
    
    info << "\t Laser Envelope parameters: "<< endl;
    // envelope solver
    info << "\t\tEnvelope solver    : " << envelope_solver << endl;
    
    
    // a0
    //info << "\t\t\ta0                 : " << a0_laser << endl;
    // waist
    //info << "\t\t\twaist              : " << waist_laser << endl;
    
    // Display info
    if( patch->isMaster() ) {
        MESSAGE( info.str() );
    }
    
    EnvBoundCond = EnvelopeBC_Factory::create( params, patch );
    
    if( patch->isMaster() ) {
        for( unsigned int i=0 ; i<params.grid_length.size() ; i++ ) {
            MESSAGE( 1, "\tdimension " << i );
            MESSAGE( 1, "\t- Envelope boundary conditions: " << "(" << params.Env_BCs[i][0] << ", " << params.Env_BCs[i][1] << ")" );
        }
    }
}

// Cloning constructor
LaserEnvelope::LaserEnvelope( LaserEnvelope *envelope, Patch *patch, ElectroMagn *EMfields, Params &params, unsigned int n_moved ) :
    cell_length( envelope->cell_length ), timestep( envelope->timestep )
{
    if( n_moved ==0 ) {
        profile_ = new Profile( envelope->profile_ );
    }
    EnvBoundCond = EnvelopeBC_Factory::create( params, patch );
}


LaserEnvelope::~LaserEnvelope()
{
    // Pb wih clones, know problem
    //if (profile_ != NULL) {
    //    delete profile_;
    //    profile_ = NULL;
    //}
    
    if( A0_ ) {
        delete A0_;
    }
    if( A_ ) {
        delete A_;
    }
    
    if( Phi_ ) {
        delete Phi_;
    }
    if( Phi_m ) {
        delete Phi_m;
    }
    
    if( GradPhix_ ) {
        delete GradPhix_;
    }
    if( GradPhix_m ) {
        delete GradPhix_m;
    }
    
    if( GradPhiy_ ) {
        delete GradPhiy_;
    }
    if( GradPhiy_m ) {
        delete GradPhiy_m;
    }
    
    if( GradPhiz_ ) {
        delete GradPhiz_;
    }
    if( GradPhiz_m ) {
        delete GradPhiz_m;
    }
    
    int nBC = EnvBoundCond.size();
    for( int i=0 ; i<nBC ; i++ )
        if( EnvBoundCond[i]!=NULL ) {
            delete EnvBoundCond[i];
        }
}



void LaserEnvelope::boundaryConditions( int itime, double time_dual, Patch *patch, Params &params, SimWindow *simWindow )
{
    // Compute Envelope Bcs
    if( !( simWindow && simWindow->isMoving( time_dual ) ) ) {
        if( EnvBoundCond[0]!=NULL ) { // <=> if !periodic
            EnvBoundCond[0]->apply( this, time_dual, patch );
            EnvBoundCond[1]->apply( this, time_dual, patch );
        }
    }
    if( EnvBoundCond.size()>2 ) {
        if( EnvBoundCond[2]!=NULL ) { // <=> if !periodic
            EnvBoundCond[2]->apply( this, time_dual, patch );
            EnvBoundCond[3]->apply( this, time_dual, patch );
        }
    }
    if( EnvBoundCond.size()>4 ) {
        if( EnvBoundCond[4]!=NULL ) { // <=> if !periodic
            EnvBoundCond[4]->apply( this, time_dual, patch );
            EnvBoundCond[5]->apply( this, time_dual, patch );
        }
    }
    
} // end LaserEnvelope::boundaryConditions





