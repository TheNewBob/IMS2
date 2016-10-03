/**
* \brief Notifies that the touchdown points have changed and need to be reset in orbiter
* \see EventHandler.h
*/
class TdPointsChangedEvent :
	public Event_Base
{
public:
	TdPointsChangedEvent() : Event_Base(TDPOINTSCHANGEDEVENT){};
	~TdPointsChangedEvent(){};
};
