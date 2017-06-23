#pragma once

class GUI_BaseElement;

class GUI_BaseElementState
{
	friend class GUI_BaseElement;
public:
	GUI_BaseElementState(GUI_BaseElement *owner) : owner(owner) {};

	virtual ~GUI_BaseElementState() 
	{
		for (auto i = sharers.begin(); i != sharers.end(); ++i)
		{
			CancelSharingWith(*(i));
		}
	};


	/**
	 * \return True if the state is owned (i.e. was created by) the passed element.
	 * \note Any object for which this does not return true MUST NOT delete this instance!/
	 */
	bool IsOwnedBy(GUI_BaseElement *which)
	{
		return which == owner;
	};

	
	/**
	 * \brief Registers a GUI element as a sharer of this state and passes itself to it.
	 */
	void ShareWith(GUI_BaseElement *sharer)
	{
		assert(find(sharers.begin(), sharers.end(), sharer) == sharers.end() && "GUI element attempting to register same sharer twice!");
		sharers.push_back(sharer);
		sharer->setState(this);
	}

	
	/**
	 * \brief Lets the state know that it is no longer shared with the passed element.
	 * \note Setting or revoking state of the sharer is the responsibility of the caller!
	 */
	void CancelSharingWith(GUI_BaseElement *sharer)
	{
		auto sharer_it = find(sharers.begin(), sharers.end(), sharer);
		assert(sharer_it != sharers.end() && "GUI element attempting to cancel sharing state without actually sharing it!");
		sharers.erase(sharer_it);
	}


protected:
	GUI_BaseElement *owner;						//!< The creator and owner of this state. The lifetime of this object is equal to that of the owner.
	vector<GUI_BaseElement*> sharers;			//!< GUI elements that share this state. They will be informed if the object is destroyed or changes context.

	/**
	 * \brief Propagates a state change to all sharers and the owner.
	 */
	 void propagateStateChange()
	{
		owner->updatenextframe = true;
		for (auto i = sharers.begin(); i != sharers.end(); ++i)
		{
			(*i)->updatenextframe = true;
		}
	}
};
