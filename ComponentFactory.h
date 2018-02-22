#pragma once

/**
 * \brief Provides static factories for components
 */
class ComponentFactory
{
public:
	ComponentFactory();
	~ComponentFactory();

	/**
	 * \return An instance of the component with the appropriate name and initial values
	 */
	static IMS_Component_Base *CreateNew(string componentname, IMS_Module *containingmodule);

	/**
	 * \return A component instance initialised with the values from the serialized string.
	 */
	static IMS_Component_Base *CreateFromSerializedString(string serializedstring, IMS_Module *containingmodule);
};

