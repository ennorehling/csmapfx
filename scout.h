#ifndef CSMAP_SCOUT_H_
#define CSMAP_SCOUT_H_

#include <boost/function.hpp>

class Scout
{
public:
	Scout();
	~Scout();

	void step(boost::function<bool(int x, int y)> cond);
	void show();
	void get(std::ostream& out, size_t which) const;

	void reset();

private:
	class ScoutData;

	ScoutData* data;
};

#endif // CSMAP_SCOUT_H_
