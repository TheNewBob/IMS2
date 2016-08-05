#pragma once

/**
 * \brief A sequential animation has a clear begining and end and can move in both directions between them. 
 */
class IMS_Animation_Sequential :
	public IMS_Animation_Base
{
public:
	/**
	 * \param _data Pointer to the data defining this animation
	 */
	IMS_Animation_Sequential(ANIMATIONDATA *_data);
	~IMS_Animation_Sequential();

	virtual AnimationEvent_Base *PropagateAnimation(double simdt);


};

