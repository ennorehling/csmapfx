#ifndef CSMAP_SCOUT_H_
#define CSMAP_SCOUT_H_

class Scout
{
public:
	Scout();
	~Scout();

	void step(bool(*cond)(int x, int y));
	void show();
	void get(std::ostream& out, size_t which) const;

	void reset();

private:
	class ScoutData;

	ScoutData* data;
};

#endif // CSMAP_SCOUT_H_
