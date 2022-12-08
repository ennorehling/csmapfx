#ifndef _CSMAP_DATAFILE
#define _CSMAP_DATAFILE

#include <list>
#include <vector>
#include <map>
#include <set>
#include <fx.h>

#include "datablock.h"

typedef enum class map_type {
    MAP_FULL,
    MAP_NORMAL,
    MAP_MINIMAL,
} map_type;

class datafile
{
public:
	datafile();

	const FXString& filename() const { return m_filename; }
	void filename(const FXString& s) { m_filename = s; }
	
	const FXString& cmdfilename() const { return m_cmdfilename; }
	void cmdfilename(const FXString& s) { m_cmdfilename = s; }

	bool modifiedCmds() const { return m_cmds.modified; }
	void modifiedCmds(bool mod) { m_cmds.modified = mod; }

    int turn() const;
	int recruitment() const { return m_recruitment; }
    int getFactionId() const { return m_factionId; }
	datablock::itor activefaction() { return m_activefaction; }

	int load(const char* filename);
	int save(const char* filename, map_type map_filter);

    void removeTemporary();
    void findOffset(datafile* new_cr, int* x_offset, int* y_offset) const;
    void merge(datafile* old_cr, int x_offset = 0, int y_offset = 0);

    int loadCmds(const FXString &filename);
	int saveCmds(const FXString &filename, const FXString &passwd, bool stripped);

    FXString getPassword() const { return m_password; }

	datablock::list_type& blocks() { return m_blocks; }

    bool hasUnits() const { return !m_units.empty(); }
    bool getUnit(datablock::itor& out, int id);
    bool getGroup(datablock::itor& out, int id);
	bool getBuilding(datablock::itor& out, int id);
	bool getShip(datablock::itor& out, int id);
	bool getFaction(datablock::itor& out, int id);
	bool hasFaction(int id);
    bool getIsland(datablock::itor& out, int id);
    bool getBattle(datablock::itor& out, int x, int y, int plane);
    bool hasBattle(int x, int y, int plane) const;
	bool getRegion(datablock::itor &out, int x, int y, int plane);
    bool hasRegion(int x, int y, int plane) const;
    bool deleteRegion(datablock* region);
    FXString regionName(const datablock& block);

    void findSelection(const datablock* select, datablock::itor& out, datablock::itor& region);
	typedef std::list<datafile>::iterator itor;

	// data of selection state (what region, what unit is actually selected?)
	struct SelectionState
	{
        int selected;			// flags; what iterator contains valid information?
		int map;				// flags that indicate map states

		datablock::itor region, faction, building, ship, unit;

		int sel_x, sel_y, sel_plane;

		std::set<datablock*> regionsSelected;
        //Selection regionsSelected;

		datablock::itor activefaction;

        int selChange, fileChange;	// incremented on changes to any datafile (loaded, closed or changed)

		// selected flags
		enum
		{
            REGION = (1<<0),			// region marked, iterator region is valid
			UNKNOWN_REGION = (1<<1),	// unknown region selected, sel_(x|y|plane) is valid
            MULTIPLE_REGIONS = (1<<2),	// some regions selected
			FACTION = (1<<3),
			BUILDING = (1<<4),
			SHIP = (1<<5),
			UNIT = (1<<6),
			BATTLE = (1<<7),
			CONFIRMED = (1<<8),
		};

		// map flags
		enum
		{
			ACTIVEFACTION = (1<<0),
			MAPCHANGED = (1<<1),
			NEWFILE = (1<<2)
		};

		// mini-c'tor
		SelectionState() : selected(0), map(0), sel_x(0), sel_y(0), sel_plane(0), selChange(0), fileChange(0) {}
        SelectionState(const SelectionState& state) {
            sel_x = state.sel_x;
            sel_y = state.sel_y;
            sel_plane = state.sel_plane;
            selChange = state.selChange;
            fileChange = state.fileChange;
            selChange = state.selChange;
            selected = state.selected;
            region = state.region;
            faction = state.faction;
            building = state.building;
            ship = state.ship;
            unit = state.unit;
            map = state.map;
            regionsSelected = state.regionsSelected;
            selected = state.selected;
        }
	};

    void rebuildIslands() { createHashTables(); }
    void rebuildRegions() { createHashTables(); }

protected:
    void createHashTables();
    void createHierarchy();

    void mergeBlock(datablock::itor& block, const datablock::itor& begin, const datablock::itor& end, block_type parent_type);
    const char* getConfigurationName(map_type type);

    datablock::itor group(int id);
    datablock::itor unit(int id);
    datablock::itor building(int id);
    datablock::itor ship(int id);
    datablock::itor faction(int id);
    datablock::itor island(int id);
    datablock::itor region(int x, int y, int plane);
    datablock::itor battle(int x, int y, int plane);

	struct koordinates
	{
		koordinates(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}

		bool operator==(const koordinates& rhs) const
		{
            return x == rhs.x && y == rhs.y && z == rhs.z;
		}

		bool operator<(const koordinates& rhs) const
		{
			if (x < rhs.x) return true;
			if (x == rhs.x)
			{
				if (y < rhs.y) return true;
				if (y == rhs.y)
					if (z < rhs.z)
						return true;
			}
			return false;
		}

		int x, y, z;
	};

	// command file stuff
	struct cmds_t
	{
		bool modified;

        // command lines before any units
		std::vector<FXString> prefix_lines;

		// command lines in REGIONs
		typedef std::map<koordinates, att_commands> region_map_t;
		typedef std::list< std::pair<koordinates, std::vector<int> > > region_list_t;

		region_map_t region_lines;
		region_list_t region_order;		// keeps the correct order of regions and units in it

	} m_cmds;

	// the data blocks
	datablock::list_type m_blocks;

    // file names of report and command files, password (from commands)
    FXString m_filename, m_cmdfilename;
    // some collected information
    FXString m_password;
    int m_factionId;
    int m_recruitment;
	mutable int m_turn;
	datablock::itor m_activefaction;

	// hash tables
	std::map<int, datablock::itor> m_units, m_factions, m_buildings, m_ships, m_islands, m_groups;
    typedef std::map<koordinates, datablock::itor> regions_map;
    regions_map m_regions;
	std::map<koordinates, datablock::itor> m_battles;
};

#endif //_CSMAP_DATAFILE
