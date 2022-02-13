#ifndef _CSMAP_DATAFILE
#define _CSMAP_DATAFILE

#include <list>
#include <vector>
#include <map>
#include <set>
#include <fx.h>

// ==================
// === datakey class

typedef enum key_type
{
    TYPE_UNKNOWN,
    TYPE_EMPTY,
    TYPE_NAME,
    TYPE_DESCRIPTION,
    TYPE_TERRAIN,
    TYPE_ISLAND,
    TYPE_ID,
    TYPE_FACTION,
    TYPE_FACTIONNAME,
    TYPE_NUMBER,
    TYPE_TYPE,
    TYPE_SKILL,
    TYPE_KONFIGURATION,
    TYPE_CHARSET,
    TYPE_VISIBILITY,
    TYPE_TURN,
    TYPE_SILVER,
    TYPE_RECRUITMENTCOST,
    TYPE_AURA,
    TYPE_AURAMAX,
    TYPE_OPTIONS,
    TYPE_EMAIL,
    TYPE_BANNER,
    TYPE_LOCALE,
    TYPE_OWNER,
    TYPE_ORDERS_CONFIRMED,		// ejcOrdersConfirmed, special tag
    TYPE_LAST,

    TYPE_MASK = (1 << 7) - 1,
    TYPE_INTEGER = 1 << 7
} key_type;

class datakey
{
public:
	datakey() : m_type(0) {}
	datakey(int type, const FXString& value) : m_type(type), m_value(value) {}
	~datakey() {}

	const FXString& value() const { return m_value; }
	key_type type() const { return (key_type)(m_type & TYPE_MASK); }
	const FXString key() const;

	void key(const FXString& s);
	void value(const FXString& s);

	bool isInt() const { return (m_type & TYPE_INTEGER) != 0; }
	int getInt() const;

	// parses str and create datakey object
	static int parseType(const FXString& type);
	bool parse(char* str, bool isUtf8 = true);

	typedef std::vector<datakey/*, boost::pool_allocator<datakey>*/ > list_type;
	typedef list_type::iterator itor;

protected:
	int m_type;
	FXString m_key, m_value;
};
	
// =========================
// === attachment base class

// base class for block attachment classes
// these classes could contain additional information
// like new commands of a unit, region symbol flags (map) or
// pointer to previous' turns unit.
class attachment
{
public:
	attachment() {}
	virtual ~attachment() {}
};

class att_region : public attachment
{
public:
	typedef std::vector<float> peoplelist_t;

	peoplelist_t	people;

	FXString		island;			// name of island

	enum	// not yet used!!!
	{
		FLAG_STREET = (1 << 0),	// street in region (first)
		FLAG_STREET_NW= (1 << 0),	// street to north west
		FLAG_STREET_NO= (1 << 1),	// street to north east
		FLAG_STREET_O = (1 << 2),	// street to east
		FLAG_STREET_SO= (1 << 3),	// street to south east
		FLAG_STREET_SW= (1 << 4),	// street to south west
		FLAG_STREET_W = (1 << 5),	// street to west

		FLAG_STREET_UNDONE = (1 << 6),	// incomplete street in region (first)
		FLAG_STREET_UNDONE_NW= (1 << 6),	// incomplete street to north west
		FLAG_STREET_UNDONE_NO= (1 << 7),	// incomplete street to north east
		FLAG_STREET_UNDONE_O = (1 << 8),	// incomplete street to east
		FLAG_STREET_UNDONE_SO= (1 << 9),	// incomplete street to south east
		FLAG_STREET_UNDONE_SW= (1 << 10),	// incomplete street to south west
		FLAG_STREET_UNDONE_W = (1 << 11),	// incomplete street to west
	};
	int				streets;		// street symbols

	enum	// not yet used!!!
	{
		FLAG_CASTLE	   = (1 << 0),	// there is a building/tower/castle in this region
		FLAG_SHIP	   = (1 << 2),	// there are ships in this region
		FLAG_SHIPTRAVEL= (1 << 3),	// ships travelled the region
		
		FLAG_LIGHTHOUSE= (1 << 4),	// region is seen by lighthouse
		FLAG_TRAVEL    = (1 << 5),	// region is seen by traveling throu

		FLAG_MONSTER   = (1 << 6),	// monster in region
		FLAG_SEASNAKE  = (1 << 7),	// sea snake in region
		FLAG_DRAGON    = (1 << 8),	// dragon/wyrm in region

		FLAG_WORMHOLE  = (1 << 9),	// wurmloch in region

		FLAG_TROOPS	    = (1 << 10),	// units are in this region
		FLAG_REGION_SEEN= (1 << 11),	// region is seen by own units

		FLAG_GUARDED      = (1 << 12),		// unit guard the region
		FLAG_GUARDED_OWN  = (1 << 12),		// unit guard the region
		FLAG_GUARDED_ALLY = (1 << 13),		// unit guard the region
		FLAG_GUARDED_ENEMY= (1 << 14),		// unit guard the region
	};
	int				symbols;		// other symbols
};

class att_commands : public attachment
{
public:
	att_commands() : confirmed(false) { }

    void addCommand(const FXString &line);
	typedef std::vector<FXString> cmdlist_t;

	FXString	header;
	cmdlist_t	prefix_lines;
	cmdlist_t	commands;
	cmdlist_t	postfix_lines;
	bool		confirmed;
};

// ====================
// === datablock class

// types of datablocks
typedef enum class block_type
{
    TYPE_UNKNOWN,
    TYPE_VERSION,
    TYPE_OPTIONS,
    TYPE_FACTION,
    TYPE_GROUP,
    TYPE_ALLIANCE,
    TYPE_REGION,
    TYPE_ISLAND,
    TYPE_SCHEMEN,
    TYPE_RESOURCE,
    TYPE_PRICES,
    TYPE_DURCHREISE,
    TYPE_DURCHSCHIFFUNG,
    TYPE_BORDER,
    TYPE_SHIP,
    TYPE_BUILDING,
    TYPE_UNIT,
    TYPE_UNITMESSAGES,
    TYPE_TALENTS,
    TYPE_SPELLS,
    TYPE_COMBATSPELL,
    TYPE_ZAUBER,
    TYPE_KOMPONENTEN,
    TYPE_TRANK,
    TYPE_ZUTATEN,
    TYPE_ITEMS,
    TYPE_COMMANDS,
    TYPE_EFFECTS,
    TYPE_MESSAGE,
    TYPE_BATTLE,
    TYPE_MESSAGETYPE,
    TYPE_TRANSLATION,
    TYPE_LAST
} block_type;


// contains information for "NAME Info"
// like: MESSAGES 36535
// string() is empty for all type()s, except for TYPE_UNKNOWN!
class datablock
{
public:
    typedef std::list<datablock/*, boost::fast_pool_allocator<datablock>*/ > list_type;
    typedef list_type::iterator itor;
    typedef list_type::const_iterator citor;
    
    datablock();
	~datablock();

    block_type type() const { return m_type; }
	int info() const { return m_info; }	// identifier for UNIT, SCHIFF... plane for REGION
	FXString id() const;					// identifier as base36
	int x() const { return m_x; }
	int y() const { return m_y; }
	int terrain() const { return m_terrain; }
	int flags() const { return m_flags; }
	int depth() const { return m_depth; }
	const FXString string() const;
	datakey::list_type& data(){ return m_data; }

	void string(const FXString& s);
	void infostr(const FXString& s);
	void terrain(int terrain);
	void flags(int flags);				// set <flags> and unset all other flags
	void setFlags(int flags);				// set <flags> and don't modify the other flags
	void depth(int depth);				// set depth of the block

	::attachment* attachment() const { return m_attachment; }
	void attachment(::attachment* attach);
	
	const FXString terrainString() const;

	bool hasKey(const datakey& key) const;
	const FXString value(const char* key) const;
	const FXString value(key_type key) const;
	int valueInt(const char* key, int def = 0) const;
	int valueInt(int key, int def = 0) const;
	const datakey* valueKey(int key) const;

    // Flags for map icons
	enum
	{
		FLAG_CASTLE	= (1 << 0),		// there is a building/tower/castle in this region
        FLAG_REGION_TAXES = (1 << 1),	// we got taxes in this region (E3 only)
        FLAG_SHIP	= (1 << 2),		// there are ships in this region
		FLAG_SHIPTRAVEL= (1 << 3),	// ships travelled the region
		
		FLAG_LIGHTHOUSE= (1 << 4),	// region is seen by lighthouse
		FLAG_TRAVEL = (1 << 5),		// region is seen by traveling throu

		FLAG_MONSTER= (1 << 6),		// monster in region
		FLAG_SEASNAKE= (1 << 7),	// sea snake in region
		FLAG_DRAGON = (1 << 8),	// dragon/wyrm in region

		FLAG_WORMHOLE = (1 << 9),	// wurmloch in region

		FLAG_TROOPS	= (1 << 10),		// units are in this region
		
		FLAG_GUARD_OWN	= (1 << 11),	// region is guarded by *
		FLAG_GUARD_ALLY	= (1 << 12),
		FLAG_GUARD_ENEMY= (1 << 13),

		FLAG_REGION_OWN   = (1 << 14),	// region is owned by * (E3 only)
		FLAG_REGION_ALLY  = (1 << 15),
		FLAG_REGION_ENEMY = (1 << 16),

		FLAG_STREET = (1 << 17),	// street in region (first)
		FLAG_STREET_NW= (1 << 17),	// street to north west
		FLAG_STREET_NO= (1 << 18),	// street to north east
		FLAG_STREET_O = (1 << 19),	// street to east
		FLAG_STREET_SO= (1 << 20),	// street to south east
		FLAG_STREET_SW= (1 << 21),	// street to south west
		FLAG_STREET_W = (1 << 22),	// street to west

		FLAG_STREET_UNDONE = (1 << 23),	// incomplete street in region (first)
		FLAG_STREET_UNDONE_NW= (1 << 23),	// incomplete street to north west
		FLAG_STREET_UNDONE_NO= (1 << 24),	// incomplete street to north east
		FLAG_STREET_UNDONE_O = (1 << 25),	// incomplete street to east
		FLAG_STREET_UNDONE_SO= (1 << 26),	// incomplete street to south east
		FLAG_STREET_UNDONE_SW= (1 << 27),	// incomplete street to south west
		FLAG_STREET_UNDONE_W = (1 << 28),	// incomplete street to west

		FLAG_BLOCKID_BIT0 = (1 << 29),	// number of ids: +1
		FLAG_BLOCKID_BIT1 = (1 << 30),	// +2

        FLAG_REGION_SEEN = (1 << 31),	// region is seen by own units

		FLAG_NONE	= 0				// no flags are set
	};

	// parses str and create datablock object
	static block_type parseType(const FXString& type);
	static int parseTerrain(const FXString& str);			// Plains, Mountains, ... Volcano
	static int parseSpecialTerrain(const FXString& str);	// Active volcano, ... (terrain that uses image of another terrain)
	static FXString planeName(int plane);					// Eressea,Astralraum,Weihnachtsinsel...
	bool parse(char* str);


protected:
    block_type m_type;
    int m_info;
	int m_x, m_y, m_terrain;
	int m_flags, m_depth;
	FXString m_string;
	datakey::list_type m_data;

	::attachment* m_attachment;

	struct blocknames
	{
		block_type id;
		const char *name;
	};
	static blocknames BLOCKNAMES[];
};

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

	int turn() const { return m_turn; }
	int recruitment() const { return m_recruitment; }
	datablock::itor activefaction() { return m_activefaction; }

	int load(const char* filename);
	int save(const char* filename, map_type map_filter);
    void merge(datafile * new_cr);

    void createHierarchy();
	void createHashTables();

	int loadCmds(const FXString &filename);
	int saveCmds(const FXString &filename, const FXString &passwd, bool stripped);

    FXString getPassword() const { return m_password; }
    int getFactionId() const { return m_factionId; }

	datablock::list_type& blocks() { return m_blocks; }

    bool getUnit(datablock::itor& out, int id);
	bool getBuilding(datablock::itor& out, int id);
	bool getShip(datablock::itor& out, int id);
	bool getFaction(datablock::itor& out, int id);
    bool getIsland(datablock::itor& out, int id);
    datablock::itor getBattle(int x, int y, int plane);
    bool hasBattle(int x, int y, int plane) const;
	bool getRegion(datablock::itor &out, int x, int y, int plane);
    bool hasRegion(int x, int y, int plane) const;
    bool deleteRegion(datablock* region);
    datablock::itor dummyToItor(const datablock* block);

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

protected:
    void mergeBlock(datablock::itor& block, const datablock::itor& begin, const datablock::itor& end, block_type parent_type);
    const char* getConfigurationName(map_type type);

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
	int m_turn;
	datablock::itor m_activefaction;

	// hash tables
	std::map<int, datablock::itor> m_units, m_factions, m_buildings, m_ships, m_islands;
	std::map<koordinates, datablock::itor> m_regions;
	std::map<koordinates, datablock::itor> m_battles;

	// temporaries (like FACTION block that don't exist in CR)
	datablock::list_type m_dummyBlocks;
};

#endif //_CSMAP_DATAFILE
