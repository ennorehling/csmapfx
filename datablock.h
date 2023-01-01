#pragma once 

#include <list>
#include <vector>
#include <map>
#include <set>
#include <fx.h>

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

// types of datakeys
typedef enum key_type
{
    TYPE_UNKNOWN,
    TYPE_EMPTY,
    TYPE_NAME,
    TYPE_COMBATSPELL_NAME,
    TYPE_DESCRIPTION,
    TYPE_TERRAIN,
    TYPE_ISLAND,
    TYPE_ID,
    TYPE_FACTION,
    TYPE_OTHER_FACTION,
    TYPE_TRAITOR,
    TYPE_FACTIONNAME,
    TYPE_FACTIONSTEALTH,
    TYPE_NUMBER,
    TYPE_BUILDING,
    TYPE_SHIP,
    TYPE_WEIGHT,
    TYPE_GROUP,
    TYPE_PREFIX,
    TYPE_STATUS,
    TYPE_FAMILIARMAGE,
    TYPE_GUARDING,
    TYPE_HUNGER,
    TYPE_HERO,
    TYPE_HITPOINTS,
    TYPE_TYPE,
    TYPE_SIZE,
    TYPE_RESOURCE_TYPE,
    TYPE_RESOURCE_SKILL,
    TYPE_RESOURCE_COUNT,
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
    TYPE_DAMAGE,
    TYPE_CAPTAIN,
    TYPE_COAST,
    TYPE_CAPACITY,
    TYPE_CARGO,
    TYPE_LOAD,
    TYPE_MAXLOAD,
    TYPE_MSG_TYPE,
    TYPE_MSG_COST,
    TYPE_MSG_AMOUNT,
    TYPE_MSG_MODE,
    TYPE_MSG_REGION,
    TYPE_LAST,

} key_type;

#define TYPE_MASK ((1 << 7) - 1)
#define TYPE_INTEGER (1 << 7)

// ==================
// === datakey class

class datakey
{
public:
	datakey() : m_type(0) {}
	datakey(int type, const FXString& value) : m_type(type), m_value(value) {}
	~datakey() {}

	const FXString& value() const { return m_value; }
    enum key_type type() const {
        return (enum key_type)(m_type & TYPE_MASK);
    }
    FXString key() const;

	void key(const FXString& s, enum block_type btype);
	void value(const FXString& s);
	void value(const FXchar * str, FXint length);

	bool isInt() const { return (m_type & TYPE_INTEGER) != 0; }
	int getInt() const;

	// parses str and create datakey object
	static int parseType(const FXString& type, enum block_type btype);
	bool parse(const char* str, enum block_type btype, bool isUtf8 = true);

	typedef std::vector<datakey> list_type;
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

struct region_info {
    FXint Bauern, Silber, Unterh, Rekruten, Pferde;
    FXint Personen, Parteipersonen, Parteisilber;
    std::vector<std::pair<FXString, std::pair<FXint, FXint> > > resources;
};

class att_region : public attachment
{
public:
	typedef std::vector<float> peoplelist_t;

	peoplelist_t people;
    region_info* regioninfos = nullptr;

    ~att_region() {
        delete regioninfos;
    }
    
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


// contains information for "NAME Info"
// like: MESSAGES 36535
// string() is empty for all type()s, except for TYPE_UNKNOWN!
class datablock
{
public:
    typedef std::list<datablock> list_type;
    typedef list_type::iterator itor;
    typedef list_type::const_iterator citor;
    
    datablock();
	~datablock();

    block_type type() const { return m_type; }
	int info() const { return m_info; }	// identifier for UNIT, SCHIFF... plane for REGION
	FXString id() const;					// identifier as base36
	int x() const { return m_x; }
	int y() const { return m_y; }
    void move(int x_offset, int y_offset);

	int terrain() const { return m_terrain; }
	int flags() const { return m_flags; }
	const FXString string() const;
	const datakey::list_type& data() const { return m_data; }

	void string(const FXString& s);
	void infostr(const FXString& s);
	void terrain(int terrain);
	void flags(int flags);				// set <flags> and unset all other flags
	void setFlags(int flags);				// set <flags> and don't modify the other flags

    int depth() const { return m_depth; }
    void depth(int depth);				// set depth of the block

	::attachment* attachment() const { return m_attachment; }
	void attachment(::attachment* attach);
	
	const FXString terrainString() const;
    FXString getName() const;

    /* TYPE_MESSAGE: */
    bool hasReference(datablock* target) const;
    int getReference(block_type type) const;

    bool hasKey(const key_type type) const;
    void addKey(const datakey& data) {
        m_data.push_back(data);
    }
    void setKey(enum key_type type, const FXString& value) {
        for (datakey::itor tags = m_data.begin(); tags != m_data.end(); ++tags)
        {
            if (tags->type() == type) {
                tags->value(value);
                return;
            }
        }
        addKey(datakey(type, value));
    }
    const FXString& value(const FXString& key) const;
	const FXString& value(key_type key) const;
	int valueInt(const FXString& key, int def = 0) const;
	int valueInt(key_type key, int def = 0) const;
	const datakey* valueKey(int key) const;
    bool removeKey(int key);

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
	bool parse(const char* str);


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
    static const char* UNITKEYS[];

};
