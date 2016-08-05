#pragma once

using namespace std;

class InvalidEventPipeException :
	public invalid_argument
{
public:
	InvalidEventPipeException(const string& what_arg);
	~InvalidEventPipeException();
};

