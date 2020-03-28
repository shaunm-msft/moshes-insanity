/********************************************************************
 *                                                                  *
 *   Source File: StepLang.h                                        *
 *   Language (sort of) for animation                               *
 *                                                                  *
 *   by Shaun Miller, 1994                                          *
 *                                                                  *
 ********************************************************************/



#ifndef __STEPLANGUAGE__
#define __STEPLANGUAGE__

class StepLanguage {
	public:
		enum operation {cubeselect,cuberotate,cubemovetopoint};

		operation op;
		union tagarg {
			int cube;
			int point;
			double radians[3];
		} arg;

	public:
		~StepLanguage () {};
		StepLanguage () {};
		StepLanguage (const StepLanguage &a) {*this = a;}
		StepLanguage& operator = (const StepLanguage &a) {
			op = a.op;
			arg.radians[0] = a.arg.radians[0];
			arg.radians[1] = a.arg.radians[1];
			arg.radians[2] = a.arg.radians[2];
         return *this;
		}
};

#endif