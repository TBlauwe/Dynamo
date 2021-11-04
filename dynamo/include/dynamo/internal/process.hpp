#ifndef DYNAMO_PROCESS_HPP
#define DYNAMO_PROCESS_HPP

#include <chrono>
#include <thread>

#include <functional>
#include <assert.h>
#include <range/v3/all.hpp>
#include <taskflow/taskflow.hpp>
#include <dynamo/internal/types.hpp>

namespace dynamo {
	
	//Forward Declaration
	class Agent;
	
	template<typename TOut>
	class Behaviour
	{
		const char* _name;
		std::function<TOut(Agent)> _callable;
		std::function<bool(Agent)> _activation_callable;

	public:
		Behaviour(const char* name, std::function<bool(Agent)>&& activation_callable, std::function<TOut(Agent)>&& callable) :
			_name{ name },
			_activation_callable{ activation_callable },
			_callable{ callable }
		{}

		template<typename T>
		friend std::ostream& operator<<(std::ostream& os, const Behaviour<T>& behaviour);

		const char* name() const {
			return _name;
		};

		virtual bool is_active(Agent agent) const {
			return _activation_callable(agent);
		};

		virtual TOut operator()(Agent agent) const {
			return _callable(agent);
		};
	};

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const Behaviour<T>& behaviour)
	{
		os << behaviour.name();
		return os;
	}

	template<typename TOut>
	class Strategy
	{
	protected:
		std::vector<Behaviour<TOut>> behaviours{};

	public:
		Strategy() = default;

		template<typename T>
		void add(T&& behaviour)
		{
			static_assert(std::is_same_v<T, std::decay_t<Behaviour<TOut>>>, "Wrong type passed, must be a Behaviour<TOut>.");
			behaviours.emplace_back(std::forward<T>(behaviour));
		}

		TOut operator()(Agent agent)
		{
			assert(behaviours.size() > 0 && "Strategy with no behaviour. Use add().");

			auto active_behaviours = behaviours | ranges::views::filter([&agent](const Behaviour<TOut>& beh) {
				return beh.is_active(agent);
				}) | ranges::to<std::vector>();

				TOut result = compute(agent, active_behaviours);
				std::cout << agent.name() << " has finished reasonning. The result is : " << result << "\n";
				return result;
		};

		virtual TOut compute(Agent, std::vector<Behaviour<TOut>>) = 0;
	};

	template<typename TOut>
	class RandomStrategy : public Strategy<TOut>
	{
	public:

		TOut compute(Agent agent, std::vector<Behaviour<TOut>> active_behaviours) override
		{
			std::vector<Behaviour<TOut>> out{};
			std::sample(active_behaviours.begin(), active_behaviours.end(), std::back_inserter(out), 1, std::mt19937{ std::random_device{}() });
			return out[0](agent);
		}
	};

	class Reasonner
	{
	public:
		Reasonner(Agent agent) : agent{ agent } {}

		inline operator tf::Taskflow && () { 
			build();
			return std::move(taskflow); 
		}

	protected:
		Agent agent;
		tf::Taskflow taskflow{};

	protected:

		template<typename T>
		tf::Task emplace(T&& t)
		{
			return taskflow.emplace([a = this->agent, &t]() {
				std::forward<T>(t)(a);
				});
		};

	private:
		virtual void build() = 0;
	};
}
#endif //DYNAMO_PROCESS_HPP
