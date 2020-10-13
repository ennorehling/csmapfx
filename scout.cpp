#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include <ctime>
#include <vector>
#include <ostream>
#include <sstream>
#include <set>

#include "datafile.h"
#include "scout.h"

// Phenotype class
class Phenotype
{
public:
	enum {
		NO, O, SO, SW, W, NW,
		MAX_DIRECTION
	};

	enum { GENOM_SIZE = 12*6 };
	enum { SHIP_RANGE = 12 };

	Phenotype();
	Phenotype(const Phenotype& parent1, const Phenotype& parent2);

	const std::vector<char>& getGenom() const { return genom; }
	double getFitness() const { return fitness; }

	bool operator< (const Phenotype& rhs) const
	{
		return fitness > rhs.fitness;
	}

	void mutate();
	void rate(bool(*isOcean)(int x, int y));
	void random_rate();

private:	
	static const int mutation_probability = 5;

private:
	std::vector<char> genom;
	double fitness;
};

Phenotype::Phenotype() : genom(GENOM_SIZE)
{
    srand(time(0));
	for (size_t i = 0; i < genom.size(); i++)
	{
		genom[i] = rand() % Phenotype::MAX_DIRECTION;
	}
}

Phenotype::Phenotype(const Phenotype& parent1, const Phenotype& parent2) : genom(GENOM_SIZE)
{
    srand(time(0));
    // generate new genom by recombination of parent1 and parent2
	size_t crossover = rand() % Phenotype::GENOM_SIZE;
	//size_t crossover2 = rand() % Phenotype::GENOM_SIZE;

	//if (crossover > crossover2)
		//std::swap(crossover, crossover2);

	// copy first part of genom from parent1...
	for (size_t i = 0; i < crossover; i++)
		genom[i] = parent1.genom[i];

	// ...and second part of genom from parent2...
	for (size_t i = crossover; i < GENOM_SIZE; i++)
		genom[i] = parent2.genom[i];

	// ...and third part of genom from parent1 again
	//for (size_t i = crossover2; i < GENOM_SIZE; i++)
	//	genom[i] = parent1.genom[i];
}

void Phenotype::mutate()
{
    srand(time(0));
    
	const double mutation = mutation_probability/100.0;

	// mutate every gen by mutation_probability %
	for (size_t i = 0; i < GENOM_SIZE; i++)
		if (rand()/(double)RAND_MAX < mutation)
			genom[i] = rand() % Phenotype::MAX_DIRECTION;
}

void Phenotype::random_rate()
{
    srand(time(0));

	fitness = std::floor(fitness) + rand() / (double)RAND_MAX;
}

void Phenotype::rate(bool(*isOcean)(int x, int y))
{
    srand(time(0));

	// NO, O, SO, SW, W, NW
	const int offset_x[6] = {  0, +1, +1,  0, -1, -1 };
	const int offset_y[6] = { +1,  0, -1, -1,  0, +1 };

	std::set< std::pair<int,int> > visited;

	double fit = 0;

	for (int ship = 0; ship < GENOM_SIZE / SHIP_RANGE; ship++)
	{
		const size_t begin = ship * SHIP_RANGE;
		const size_t end = (ship+1) * SHIP_RANGE;

		char coast = SO;
		int x = 3;
		int y = -1;

		for (size_t i = begin; i < end; i++)
		{
			char dir = genom[i];		// current sail direction

			// check if ship leaves coast in correct direction
			// break journey if it doesn't
			if (coast != MAX_DIRECTION &&
				dir != coast &&
				dir != (coast+1) % MAX_DIRECTION &&
				dir != (MAX_DIRECTION+coast-1) % MAX_DIRECTION)
			{
				//fit -= 2*(end - i);
				break;
			}

			// check if ship leaves coast to an ocean region
			if (coast != MAX_DIRECTION && !isOcean(x+offset_x[dir], y+offset_y[dir]))
			{
				//fit -= 2*(end - i);
				break;
			}

			// ship leaves coast
			coast = MAX_DIRECTION;

			x += offset_x[dir];
			y += offset_y[dir];

			// update visited regions
			visited.insert(std::make_pair(x, y));

			for (int i = 0; i < 6; i++)
			{
				// put all explored ocean on a list
				int nx = x+offset_x[i];
				int ny = y+offset_y[i];

				if (isOcean(nx,ny))
					visited.insert(std::make_pair(nx, ny));
			}

			// put ship on coast if it hits on a land region
			if (!isOcean(x,y))
			{
				coast = (dir + MAX_DIRECTION/2) % MAX_DIRECTION;	// ship lies on coast where it came from
				//fit -= end - i;
				break;
			}
		}
	}

	// calc fitness bonus from visited regions
	fitness = fit + visited.size();
}

// ScoutData class
class Scout::ScoutData
{
public:
	ScoutData();
	void rate_all(bool(*cond)(int x, int y));
	void select_best(bool(*cond)(int x, int y));
	void mutate_some(bool(*cond)(int x, int y));

	void get_genom(std::ostream& out, size_t which) const;
	void show(std::ostream& out) const;
	
private:
	static const size_t population_count = 50;
	static const size_t elite_count = 10;
	static const size_t survivor_count = 0;	// elite + survivor + clone_from_elite = population

private:
	typedef std::list<Phenotype> generation_t;

	generation_t generation;
	bool rated;
};

Scout::ScoutData::ScoutData() : rated(false)
{
	for (size_t i = 0; i < population_count; ++i)
		generation.push_back(Phenotype());
}

void Scout::ScoutData::rate_all(bool(*cond)(int x, int y))
{
	if (!rated)
	{
		rated = true;
		generation_t::iterator itor = generation.begin();
		generation_t::iterator end = generation.end();
		for (; itor != end; ++itor)
			itor->rate(cond);
	}

	/*
	generation_t::iterator itor = generation.begin();
	generation_t::iterator end = generation.end();
	for (; itor != end; ++itor)
		itor->random_rate();
	*/

	generation.sort();
}

void Scout::ScoutData::select_best(bool(*cond)(int x, int y))
{
    srand(time(0));

	while (generation.size() > elite_count + survivor_count)
		generation.pop_back();

	std::vector<Phenotype*> elite;	// direct random access to elite Phenotypes

	generation_t::iterator itor = generation.begin();
	for (size_t i = 0; i < elite_count && itor != generation.end(); ++i, ++itor)
		elite.push_back(&*itor);

	// fill population with siblings from the elite until pop is up to population_count again
	while (generation.size() < population_count)
	{
		size_t parent1 = rand() % elite_count;
		size_t parent2 = rand() % elite_count;
		while (parent1 == parent2)
			parent2 = rand() % elite_count;

		generation.push_back( Phenotype(*elite[parent1], *elite[parent2]) );
		generation.back().rate(cond);
	}
}

void Scout::ScoutData::mutate_some(bool(*cond)(int x, int y) )
{
	generation_t::iterator itor = generation.begin();
	generation_t::iterator end = generation.end();
	//for (size_t i = 0; i < elite_count && itor != end; ++i, ++itor)
	//{
	//	// don't mutate the elite
	//}

	++itor; // don't mutate best

	for (; itor != end; ++itor)
	{
		itor->mutate();
		itor->rate(cond);
	}
}

void Scout::ScoutData::get_genom(std::ostream& out, size_t which) const
{
	generation_t::const_iterator pheno = generation.begin();
	for (; pheno != generation.end() && which != 0; ++pheno, --which)
	{
		// go to Phenotype number 'which'
	}

	if (pheno == generation.end())
	{
		out << "index out of bounds";
		return;
	}

	const std::vector<char>& genom = pheno->getGenom();
	
	const char* direction[] = { "NO", "O", "SO", "SW", "W", "NW" };

	for (size_t i = 0; i < genom.size(); i++)
	{
		if (!(i % Phenotype::SHIP_RANGE) && i != 0)
			out << "NACH ";

		out << direction[genom[i]] << ' ';
	}
}

void Scout::ScoutData::show(std::ostream& out) const
{
	// show fitness of elite
	generation_t::const_iterator itor = generation.begin();
	generation_t::const_iterator end = generation.end();
	for (size_t i = 0; i < elite_count && itor != end; ++i, ++itor)
	{
		out << " " << itor->getFitness();
	}
}

// implementation of class Scout
Scout::Scout() : data(new ScoutData)
{
}

Scout::~Scout()
{
	delete data;
}

void Scout::reset()
{
	delete data;
	data = 0;

	data = new ScoutData;
}

void Scout::get(std::ostream& out, size_t which) const
{
	data->get_genom(out, which);
}

void Scout::step(bool(*cond)(int x, int y))
{
	data->rate_all(cond);
	data->select_best(cond);
	data->mutate_some(cond);

	//std::ostringstream out;
	//out << "Step.\n";

	//OutputDebugString(out.str().c_str());
	show();
}

void Scout::show()
{
	std::ostringstream out;
	data->show(out);

	out << "\n";
#ifdef WIN32
	OutputDebugString(out.str().c_str());
#endif
}
