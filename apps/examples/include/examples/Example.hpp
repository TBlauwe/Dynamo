#ifndef DYNAMO_EXAMPLE_HPP
#define DYNAMO_EXAMPLE_HPP

#include <dynamo/simulation.hpp>

class Example
{
public:
	Example(const char * name) : name{name}
	{
		initialize_simulation();
		initialize_domain();
		initialize_cognitive_models();
		initialize_strategies();
		initialize_processes();
		populate_simulation();
	}

	inline Simulation& simulation() { return sim; }

private:
	virtual void initialize_simulation() = 0;
	virtual void initialize_domain() = 0;
	virtual void initialize_cognitive_models() = 0;
	virtual void initialize_strategies() = 0;
	virtual void initialize_processes() = 0;
	virtual void populate_simulation() = 0;

private:
	Simulation sim {};
	const char* name;
};

#endif //DYNAMO_EXAMPLE_HPP
