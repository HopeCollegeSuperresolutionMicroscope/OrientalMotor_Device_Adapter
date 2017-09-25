#include "OrientalFocusKnobs.h"

std::map< std::string, const AdjusterKnob* > KnobOptionsResolver::knobOptions_;
std::map< std::string, const AdjusterKnob* >::iterator KnobOptionsResolver::iterator_;

AdjusterKnob::AdjusterKnob( double single_rot_travel_um, double abs_total_travel_um, bool righty_tighty, bool right_stop, bool left_stop, std::string name ) :
		single_rot_travel_um_( single_rot_travel_um ),
		abs_total_travel_um_( abs_total_travel_um ),
		righty_tighty_( righty_tighty ),
		right_stop_( right_stop ),
		left_stop_( left_stop ),
		name_( name )
		{
			//Register This Knob to the Resolver
			KnobOptionsResolver::RegisterKnobOption( this );
		};


bool KnobOptionsResolver::RegisterKnobOption( AdjusterKnob const * knob )
	{
		iterator_ = knobOptions_.find( knob->name_ );
		if( iterator_ != knobOptions_.end() )
		{
			//Log Some Type of Error about redundant Adjuster Knobs


			return false;
		}

		knobOptions_[ knob->name_ ] = knob;

		return true;

	};
		
		//read all keys
std::vector<std::string> KnobOptionsResolver::ReadAllOptionNames( void )
	{

		std::vector< std::string > v;

		for( iterator_ = knobOptions_.begin(); iterator_ != knobOptions_.end(); ++iterator_ )
		{
			v.push_back( iterator_->first );
		}

		return v;

	};

//Return single KnobOption
//Soft Returns a null AdjusterKnob Object
//This should be handled in calling class for Error handling
const AdjusterKnob* KnobOptionsResolver::GetKnobOption( std::string name )
	{
		iterator_ = knobOptions_.find( name );
		if( iterator_ == knobOptions_.end() )
		{
			//Log Error
			return nullptr;
		}

		return iterator_->second;

	};

//Return a single KnobOption By an Index
//Returns a nullput if the index is out of range
const AdjusterKnob* KnobOptionsResolver::GetKnobOptionByIndex( unsigned int index )
{
	unsigned int i = 0;
	for( iterator_ = knobOptions_.begin(); iterator_ != knobOptions_.end(); ++iterator_ )
	{
		if( i == index)
		{
			return iterator_->second;
		}
	}

	//Out of Bounds
	return nullptr;

}