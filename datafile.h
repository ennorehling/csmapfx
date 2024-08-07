#ifndef _CSMAP_DATAFILE
#define _CSMAP_DATAFILE

#include <list>
#include <vector>
#include <unordered_map>
#include <set>
#include <fstream>

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

    enum class special_faction : int {
        ANONYMOUS = -1,
        TRAITOR = -2
    };

	const FXString& filename() const { return m_filename; }
	void filename(const FXString& s) { m_filename = s; }
	
	const FXString& cmdfilename() const { return m_cmdfilename; }
	void cmdfilename(const FXString& s) { m_cmdfilename = s; }

	bool modifiedCmds() const { return m_cmds.modified; }
	void modifiedCmds(bool mod) { m_cmds.modified = mod; }

    FXString getVersion();
    static int compareVersions(const FXString &lhs, const FXString &rhs);
    int turn();
	int recruitment() const { return m_recruitment; }
    int getFactionId() const { return m_factionId; }

    int getFactionIdForUnit(const datablock* unit) const;
    FXString getFactionName(int factionId);

    datablock* getMessageTarget(const datablock& msg);

    datablock::itor activefaction() { return m_activefaction; }
    bool hasActiveFaction() const { return m_factionId > 0; }
    int getActiveFactionId() const { return m_factionId; }

	bool load(const FXString& filename, FXString & outError);
	int save(const char* filename, map_type map_filter, std::set<datablock*> const* const selection = nullptr);

    void removeTemporary();
    void findOffset(datafile* new_cr, int* x_offset, int* y_offset) const;
    void merge(datafile* old_cr, int x_offset = 0, int y_offset = 0);

    int loadCmds(const FXString &filename);
	void writeCmds(std::ostream &out, const att_commands * cmds);
	int saveCmds(const FXString &filename, const FXString &passwd, bool stripped);

    FXString getPassword() const { return m_password; }

	datablock::list_type& blocks() { return m_blocks; }
    bool getFirst(datablock::itor& out, block_type type);
    bool getNext(datablock::itor& iter, block_type type);

    bool hasUnits() const { return !m_units.empty(); }
    bool getParent(datablock::itor& out, const datablock::itor& child);
    bool hasChild(const datablock::itor& parent, const datablock::itor &child);
    bool getChild(datablock::itor& out, const datablock::itor& parent, block_type type);
    bool getCommands(datablock::itor& out, const datablock::itor& unit);
    bool getUnit(datablock::itor& out, int id);
    bool getGroup(datablock::itor& out, int id);
	bool getBuilding(datablock::itor& out, int id);
	bool getShip(datablock::itor& out, int id);
	bool getFaction(datablock::itor& out, int id);
	bool hasFaction(int id);
    bool getBattle(datablock::itor& out, int x, int y, int plane);
    bool hasBattle(int x, int y, int plane) const;
	bool getRegion(datablock::itor &out, int x, int y, int plane);
    bool getRegion(datablock::itor& out, const datablock& block) {
        return getRegion(out, block.x(), block.y(), block.info());
    }
    bool hasRegion(int x, int y, int plane) const;
    bool deleteRegion(const datablock& region);
    void deleteRegions(std::set<datablock*>& regions);
    void addRegion(const datablock& region);
    FXString unitName(const datablock& block, bool verbose = false);
    static FXString regionCoordinates(const datablock& block);
    void setConfirmed(datablock::itor& unit, bool value = true);
    bool isConfirmed(const datablock& block) const;

    void findSelection(const datablock* select, datablock::itor& out, datablock::itor& region);
	typedef std::list<datafile>::iterator itor;

	// data of selection state (what region, what unit is actually selected?)
	struct SelectionState
	{
        int selected = 0;			// flags; what iterator contains valid information?

		datablock::itor region, faction, building, ship, unit;

		int sel_x = 0, sel_y = 0, sel_plane = 0;

		std::set<datablock*> regionsSelected;

        int selChange = 0, fileChange = 0;	// incremented on changes to any datafile (loaded, closed or changed)

		// selected flags
		enum
		{
            REGION = (1<<0),			// region marked, iterator region is valid
			UNKNOWN_REGION = (1<<1),	// unknown region selected, sel_(x|y|plane) is valid
			FACTION = (1<<2),
			BUILDING = (1<<3),
			SHIP = (1<<4),
			UNIT = (1<<5),
            CONFIRMATION = (1 << 6)    // confirmation status of the current unit has changed, update widgets that care
		};

        void transfer(datafile* old_cr, datafile* new_cr, int x_offset = 0, int y_offset = 0);
        void clear();

		// mini-c'tor
		SelectionState() {}
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
            regionsSelected = state.regionsSelected;
            selected = state.selected;
        }
	};

    void createIslands();
    void createHashTables();
    void parseMessages();

    const FXRectangle &getContentSize(int visiblePlane);

protected:
    std::unordered_map<int, FXRectangle> contentSizes;

    static bool isEphemeral(block_type type);
    datablock::itor eraseRegion(const datablock::itor& region);
    datablock::itor eraseBlocks(const datablock::itor& begin, const datablock::itor& end);
    void updateHashTables(const datablock::itor& start);
    void floodIslandNames();
    void createHierarchy();
    
    static void openFile(const char* filename, std::ifstream& stream, std::ios::openmode mode = std::ios::in);

    void mergeBlock(datablock::itor& block, const datablock::itor& begin, datablock::itor& end);
    const char* getConfigurationName(map_type type);

    datablock::itor group(int id);
    datablock::itor unit(int id);
    datablock::itor building(int id);
    datablock::itor ship(int id);
    datablock::itor faction(int id);
    datablock::itor island(int id);
    datablock::itor region(int x, int y, int plane);
    datablock::itor battle(int x, int y, int plane);

	struct Coordinates
	{
		Coordinates(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}

		bool operator==(const Coordinates& rhs) const
		{
            return x == rhs.x && y == rhs.y && z == rhs.z;
		}

		bool operator<(const Coordinates& rhs) const
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

    struct CoordinatesHash
    {
        std::size_t operator()(const Coordinates &k) const
        {
            // Compute individual hash values for first,
            // second and third and combine them using XOR
            // and bit shifting:

            return k.x ^ k.y ^ k.z;
        }
    };

    // command file stuff
	struct cmds_t
	{
		bool modified;

        // command lines before any orders
		std::vector<FXString> prefix_lines;

		// command lines in REGIONs
		typedef std::unordered_map<Coordinates, att_commands, CoordinatesHash> region_map_t;

		region_map_t region_lines;
	} m_cmds;

	// the data blocks
	datablock::list_type m_blocks;

    // file names of report and command files, password (from commands)
    FXString m_filename, m_cmdfilename;
    // some collected information
    FXString m_password;
    int m_factionId;
    int m_recruitment;
	int m_turn;
    FXString m_version;
	datablock::itor m_activefaction;

    // hash tables
    typedef std::unordered_map<int, datablock::itor> blockhash_t;
    blockhash_t m_units, m_factions, m_buildings, m_ships, m_islands, m_groups;
    typedef std::unordered_map<Coordinates, datablock::itor, CoordinatesHash> regions_map;
    regions_map m_regions;
    regions_map m_battles;
};

#endif //_CSMAP_DATAFILE
