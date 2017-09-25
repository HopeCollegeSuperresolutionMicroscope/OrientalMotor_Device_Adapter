#ifndef _ORIENTALFOCUSKNOBS_
#define _ORIENTALFOCUSKNOBS_

#include <map>
#include <string>
#include <vector>

class KnobOptionsResolver;

//knob singleton handler class
/** 
Note, assess interaction between units and revise
**/
typedef struct AdjusterKnob
{

	//Single Rotation travel (um)
	double single_rot_travel_um_;
	//Absolute Total Travel of knob (um)
	double abs_total_travel_um_;
	//Righty-Tighty Rule Followed
	bool righty_tighty_;
	//Right and Left (rotational) Stops (depending on Method)
	//Not Really Sure This Makes a Difference
	bool right_stop_;
	bool left_stop_;

	//Name String for Identification in Program
	std::string name_;

	AdjusterKnob( double single_rot_travel_um, double abs_total_travel_um, bool righty_tighty, bool right_stop, bool left_stop, std::string name ); 

} AdjusterKnob;

//Possibly Extern
//Try .cpp instead defition
class KnobOptionsResolver
{
	//Resolver Class registers all hardware knobs through new header files
	private:
		KnobOptionsResolver();
		~KnobOptionsResolver();


	public:
		//Register AdjusterKnob From other custom made files
		static bool RegisterKnobOption( AdjusterKnob const * knob );
		
		//read all keys
		static std::vector<std::string> ReadAllOptionNames( void );

		//Return single KnobOption
		static const AdjusterKnob* GetKnobOption( std::string name );

		//Return single KnobOption By Index
		static const AdjusterKnob* GetKnobOptionByIndex( unsigned int index );
		
	private:
		//Stores Name and adjusterKnob
		static std::map< std::string, const AdjusterKnob* > knobOptions_;
		static std::map< std::string, const AdjusterKnob* >::iterator iterator_;

};

#endif  //_ORIENTALFOCUSKNOBS_