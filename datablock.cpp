#include "datafile.h"

#include "fxhelper.h"
#include "terrain.h"

#include <cassert>

att_commands::att_commands(const datablock& source)
{
    const datakey::list_type& list = source.data();

    for (datakey::list_type::const_iterator itor = list.begin(); itor != list.end(); itor++) {
        commands.push_back((*itor).value());
    }
}

void att_commands::addCommand(const FXString &line) {
    std::copy(postfix_lines.begin(), postfix_lines.end(),
        std::back_inserter(commands));
    postfix_lines.clear();
    commands.push_back(line);
}

// ===========================
// === datakey implementation

void datakey::key(const FXString& type, enum block_type btype)
{
	m_type = parseType(type, btype);

	if (m_type == TYPE_UNKNOWN)
		m_key = type;
	else
		m_key.clear();
}

void datakey::value(const FXString& s)
{
	m_value = s;
}

void datakey::value(const FXchar* str, FXint length)
{
    m_value.assign(str, length);
}

int datakey::parseType(const FXString& type, enum block_type btype)
{
    if (type.empty()) {
        return TYPE_EMPTY;
    }
    if (type == "Name") {
        if (btype == block_type::TYPE_COMBATSPELL || btype == block_type::TYPE_GROUP)
        {
            return TYPE_LOWERCASE_NAME;
        }
        return TYPE_NAME;
    }
    if (btype == block_type::TYPE_RESOURCE)
    {
        if (type == "number") {
            return TYPE_RESOURCE_COUNT;
        }
        if (type == "skill") {
            return TYPE_RESOURCE_SKILL;
        }
        if (type == "type") {
            return TYPE_RESOURCE_TYPE;
        }
        return TYPE_NAME;
    }
    if (btype == block_type::TYPE_MESSAGE) {
        // MESSAGE tags
        if (type == "type")
            return TYPE_MSG_TYPE | TYPE_INTEGER;
        if (type == "mode")
            return TYPE_MSG_MODE | TYPE_INTEGER;
        if (type == "cost")
            return TYPE_MSG_COST | TYPE_INTEGER;
        if (type == "amount")
            return TYPE_MSG_AMOUNT | TYPE_INTEGER;
        if (type == "region")
            return TYPE_MSG_REGION;
    }
    if (type == "Bauern")
        return TYPE_PEASANTS;
    if (type == "name")
		return TYPE_LOWERCASE_NAME;
	if (type == "privat")
		return TYPE_NOTES;
	if (type == "Beschr")
		return TYPE_DESCRIPTION;
    if (type == "Baeume" || type == L"B\u00e4ume")
        return TYPE_TREES;
    if (type == "Schoesslinge" || type == L"Sch\u00f6\u00dflinge")
        return TYPE_SAPLINGS;
    if (type == "Lohn")
        return TYPE_SALARY;
    if (type == "Terrain")
		return TYPE_TERRAIN;
	if (type == "Insel")
		return TYPE_ISLAND;
	if (type == "id")
		return TYPE_ID|TYPE_INTEGER;
	if (type == "Partei")
		return TYPE_FACTION;
	if (type == "Anderepartei")
		return TYPE_OTHER_FACTION;
	if (type == "Verraeter")
		return TYPE_TRAITOR;
	if (type == "Parteiname")
		return TYPE_FACTIONNAME;
    if (type == "Parteitarnung")
        return TYPE_FACTIONSTEALTH;
    if (type == "Anzahl")
		return TYPE_NUMBER;
	if (type == "Burg")
		return TYPE_BUILDING;
	if (type == "Schiff")
		return TYPE_SHIP;
	if (type == "Typ")
		return TYPE_TYPE;
	if (type == "Groesse")
		return TYPE_SIZE;
	if (type == "Konfiguration")
		return TYPE_KONFIGURATION;
    if (type == "weight")
        return TYPE_WEIGHT;
    if (type == "gruppe")
        return TYPE_GROUP;
    if (type == "typprefix")
        return TYPE_PREFIX;
    if (type == "Kampfstatus")
        return TYPE_STATUS;
    if (type == "hp")
        return TYPE_HITPOINTS;
    if (type == "familiarmage")
        return TYPE_FAMILIARMAGE;
    if (type == "bewacht")
        return TYPE_GUARDING;
    if (type == "hunger")
        return TYPE_HUNGER;
    if (type == "hero")
        return TYPE_HERO;
    if (type == "charset")
		return TYPE_CHARSET;
	if (type == "visibility")
		return TYPE_VISIBILITY;
	if (type == "Runde")
		return TYPE_TURN | TYPE_INTEGER;
	if (type == "Silber")
		return TYPE_SILVER;
	if (type == "Rekrutierungskosten")
		return TYPE_RECRUITMENTCOST;
	if (type == "Aura")
		return TYPE_AURA;
	if (type == "Auramax")
		return TYPE_AURAMAX;
	if (type == "Optionen")
		return TYPE_OPTIONS;
	if (type == "email")
		return TYPE_EMAIL;
	if (type == "banner")
		return TYPE_BANNER;
	if (type == "locale")
		return TYPE_LOCALE;
	if (type == "Besitzer")
		return TYPE_OWNER;
	if (type == "ejcOrdersConfirmed")
		return TYPE_ORDERS_CONFIRMED;
    if (type == "Schaden")
        return TYPE_DAMAGE;
    if (type == "Kapitaen")
        return TYPE_CAPTAIN;
    if (type == "speed")
        return TYPE_SPEED;
    if (type == "Kueste")
        return TYPE_COAST;
    if (type == "capacity")
        return TYPE_CAPACITY;
    if (type == "cargo")
        return TYPE_CARGO;
    if (type == "Ladung")
        return TYPE_LOAD;
    if (type == "MaxLadung")
        return TYPE_MAXLOAD;
	return TYPE_UNKNOWN;
}

const FXint datakey::valueInt(FXint section) const
{
    return FXIntVal(m_value.section(' ', section));
}

FXString datakey::key() const
{
    switch (type()) {
    case TYPE_UNKNOWN:
        return m_key;
    case TYPE_EMPTY:
        return "";
    case TYPE_NAME:
        return "Name";
    case TYPE_LOWERCASE_NAME:
        return "name";
    case TYPE_NOTES:
        return "privat";
    case TYPE_DESCRIPTION:
        return "Beschr";
    case TYPE_SAPLINGS:
        return "Schoesslinge";
    case TYPE_SALARY:
        return "Lohn";
    case TYPE_TREES:
        return "Baeume";
    case TYPE_PEASANTS:
        return "Bauern";
    case TYPE_TERRAIN:
        return "Terrain";
    case TYPE_ISLAND:
        return "Insel";
    case TYPE_ID:
        return "id";
    case TYPE_FACTION:
        return "Partei";
    case TYPE_TRAITOR:
        return "Verraeter";
    case TYPE_OTHER_FACTION:
        return "Anderepartei";
    case TYPE_FACTIONNAME:
        return "Parteiname";
    case TYPE_FACTIONSTEALTH:
        return "Parteitarnung";
    case TYPE_NUMBER:
        return "Anzahl";
    case TYPE_BUILDING:
        return "Burg";
    case TYPE_SHIP:
        return "Schiff";
    case TYPE_TYPE:
        return "Typ";
    case TYPE_SIZE:
        return "Groesse";
    case TYPE_RESOURCE_TYPE:
        return "type";
    case TYPE_RESOURCE_SKILL:
        return "skill";
    case TYPE_RESOURCE_COUNT:
        return "number";
    case TYPE_KONFIGURATION:
        return "Konfiguration";
    case TYPE_WEIGHT:
        return "weight";
    case TYPE_GROUP:
        return "gruppe";
    case TYPE_PREFIX:
        return "typprefix";
    case TYPE_HITPOINTS:
        return "hp";
    case TYPE_STATUS:
        return "Kampfstatus";
    case TYPE_FAMILIARMAGE:
        return "familiarmage";
    case TYPE_GUARDING:
        return "bewacht";
    case TYPE_HUNGER:
        return "hunger";
    case TYPE_HERO:
        return "hero";
    case TYPE_CHARSET:
        return "charset";
    case TYPE_VISIBILITY:
        return "visibility";
    case TYPE_TURN:
        return "Runde";
    case TYPE_SILVER:
        return "Silber";
    case TYPE_RECRUITMENTCOST:
        return "Rekrutierungskosten";
    case TYPE_AURA:
        return "Aura";
    case TYPE_AURAMAX:
        return "Auramax";
    case TYPE_OPTIONS:
        return "Optionen";
    case TYPE_EMAIL:
        return "email";
    case TYPE_BANNER:
        return "banner";
    case TYPE_LOCALE:
        return "locale";
    case TYPE_OWNER:
        return "Besitzer";
    case TYPE_ORDERS_CONFIRMED:
        return "ejcOrdersConfirmed";
    case TYPE_DAMAGE:
        return "Schaden";
    case TYPE_CAPTAIN:
        return "Kapitaen";
    case TYPE_SPEED:
        return "speed";
    case TYPE_COAST:
        return "Kueste";
    case TYPE_CAPACITY:
        return "capacity";
    case TYPE_CARGO:
        return "cargo";
    case TYPE_LOAD:
        return "Ladung";
    case TYPE_MAXLOAD:
        return "MaxLadung";
    case TYPE_MSG_TYPE:
        return "type";
    case TYPE_MSG_REGION:
        return "region";
    case TYPE_MSG_AMOUNT:
        return "amount";
    case TYPE_MSG_COST:
        return "cost";
    case TYPE_MSG_MODE:
        return "mode";
	case TYPE_LAST:
		break;
    }
	return FXString_Empty;
}

int datakey::getInt() const
{
	return FXIntVal(m_value, 10);
}

// parses str and returns created datakey object or NULL pointer
bool datakey::parse(const char* str, enum block_type btype, bool isUtf8)
{
	if (!str)
		return false;

	// skip indentation
	while(*str && isspace(*str))
		str++;

    const char* begin = str;
    int mask = 0;
	// Ist die Zeile ein Tag?
    if (*str == '\"') {
        ++begin;
    }
    else if (*str == '-' || (*str >= '0' && *str <= '9')) {
        mask = TYPE_INTEGER;
    }
    else {
        return false;
    }


	const char *srch = str + 1, *semikolon = NULL;

	for (; *srch; srch++)		// search for semicolon ';' in this line
	{
		if (*srch == ';')
			semikolon = srch;
		else if (*srch == '\"')
			semikolon = NULL;
	}

	// ok, so assign new values

    if (semikolon) {
        key(semikolon + 1, btype);			// ... and the FXString::operator=() copies the string itself.
    }
    else {
        key("", btype);					// no semikolon found.
    }
    m_type |= mask;
    const char* end = semikolon;
    if (end) {
        if (end > str && end[-1] == '\"')
        {
            --end;
        }
    }
	else
	{
        for (end = srch - 1;  end >= begin; --end) {
            if (*end == '\"') {
                break;
            }
		}
	}
    if (end >= begin) {
        FXint nsrc = (FXint)(end - begin);
        FXString val;
        if (isUtf8) {
            val.assign(begin, nsrc);
        }
        else {
            val = iso2utf(begin, nsrc);
        }
        val.substitute("\\\\", "\\");
        val.substitute("\\\"", "\"");
        value(val);
        return true;
    }
    return false;
}

FXString datakey::translatedKey(const char* loc) const
{
    switch (type()) {
    case TYPE_BUILDING:
        return FXString(L"Geb\u00e4ude");
    case TYPE_OWNER:
        return FXString("Besitzer");
    case TYPE_CAPTAIN:
        return FXString(L"Kapit\u00e4n");
    case TYPE_SPEED:
        return FXString("Geschwindigkeit");
    case TYPE_UNKNOWN:
    default:
        return key();
    }
}

// =============================
// === datablock implementation

FXString datablock::TERRAIN_NAMES[] =
{
    FXString("Unbekannt"),
    FXString("Ozean"),
    FXString("Sumpf"),
    FXString("Ebene"),
    FXString(L"W\u00fcste"),
    FXString("Wald"),
    FXString("Hochland"),
    FXString("Berge"),
    FXString("Gletscher"),
    FXString("Vulkan"),
    FXString("Aktiver Vulkan"),
    FXString("Packeis"),
    FXString("Eisberg"),
    FXString("Eisscholle"),
    FXString("Gang"),
    FXString("Wand"),
    FXString("Halle"),
    FXString("Nebel"),
    FXString("Dichter Nebel"),
    FXString("Feuerwand"),
    FXString("Mahlstrom")
};

/*static*/ datablock::blocknames datablock::BLOCKNAMES[] =
{
	{ block_type::TYPE_VERSION, "VERSION" },
	{ block_type::TYPE_OPTIONS, "OPTIONEN" },
	{ block_type::TYPE_FACTION, "PARTEI" },
	{ block_type::TYPE_GROUP, "GRUPPE" },
	{ block_type::TYPE_ALLIANCE, "ALLIANZ" },
	{ block_type::TYPE_REGION, "REGION" },
	{ block_type::TYPE_ISLAND, "ISLAND" },
	{ block_type::TYPE_SCHEMEN, "SCHEMEN" },
	{ block_type::TYPE_RESOURCE, "RESOURCE" },
	{ block_type::TYPE_PRICES, "PREISE" },
	{ block_type::TYPE_DURCHREISE, "DURCHREISE" },
	{ block_type::TYPE_DURCHSCHIFFUNG, "DURCHSCHIFFUNG" },
	{ block_type::TYPE_BORDER, "GRENZE" },
	{ block_type::TYPE_BORDER, "BORDER" },
	{ block_type::TYPE_SHIP, "SCHIFF" },
	{ block_type::TYPE_BUILDING, "BURG" },
	{ block_type::TYPE_UNIT, "EINHEIT" },
	{ block_type::TYPE_UNITMESSAGES, "EINHEITSBOTSCHAFTEN" },
	{ block_type::TYPE_TALENTS, "TALENTE" },
	{ block_type::TYPE_SPELLS, "SPRUECHE" },
	{ block_type::TYPE_COMBATSPELL, "KAMPFZAUBER" },
	{ block_type::TYPE_ZAUBER, "ZAUBER" },
	{ block_type::TYPE_KOMPONENTEN, "KOMPONENTEN" },
	{ block_type::TYPE_TRANK, "TRANK" },
	{ block_type::TYPE_ZUTATEN, "ZUTATEN" },
	{ block_type::TYPE_ITEMS, "GEGENSTAENDE" },
	{ block_type::TYPE_COMMANDS, "COMMANDS" },
	{ block_type::TYPE_EFFECTS, "EFFECTS" },
	{ block_type::TYPE_MESSAGE, "MESSAGE" },
	{ block_type::TYPE_BATTLE, "BATTLE" },
	{ block_type::TYPE_MESSAGETYPE, "MESSAGETYPE" },
	{ block_type::TYPE_TRANSLATION, "TRANSLATION" },
	{ block_type::TYPE_UNKNOWN, NULL }
};

/*static*/ block_type datablock::parseType(const FXString& type)
{
	// region moved to top (performance)
	if (type == "REGION")
		return block_type::TYPE_REGION;

    for (int i = 0; BLOCKNAMES[i].name ; i++)
		if (type == BLOCKNAMES[i].name)
			return BLOCKNAMES[i].id;
	
	return block_type::TYPE_UNKNOWN;
}

const FXString& datablock::terrainString() const
{
	const FXString &type = value(TYPE_TERRAIN);
	if (!type.empty())
		return type;
 
    int t = terrain();
    if (t >= data::TERRAIN_UNKNOWN && t < data::TERRAIN_LAST) {
        return TERRAIN_NAMES[t];
    }
    
    return TERRAIN_NAMES[data::TERRAIN_UNKNOWN];
}

FXString datablock::getName() const
{
    FXString name;
    if (type() == block_type::TYPE_FACTION || type() == block_type::TYPE_ALLIANCE) {
        name = value(TYPE_FACTIONNAME);
        if (name.empty()) {
            name = "Partei " + id();
        }
    }
    else {
        name = value(TYPE_NAME);
        if (name.empty()) {
            switch(type()) {
            case block_type::TYPE_UNIT:
                return "Einheit " + id();
            case block_type::TYPE_REGION:
                name = terrainString();
                if (name.empty()) {
                    name = "Unbekannt";
                }
                break;
            default:
                return value(TYPE_TYPE) + " " + id();
            }
        }
    }
    return name;
}

FXString datablock::getLabel() const
{
    if (type() == block_type::TYPE_REGION || type() == block_type::TYPE_BATTLE) {
        FXString label, name = getName();
        if (info()) {
            label.format("%s (%d,%d,%s)", name.text(), x(), y(), planeName(info()).text());
        }
        else {
            label.format("%s (%d,%d)", name.text(), x(), y());
        }
        return label;
    }
    else {
        return getName() + " (" + id() + ")";
    }
}

/*static*/ int datablock::parseTerrain(const FXString& terrain)
{
	// this terrain does not need textual representation

	if (terrain == "Ozean")
		return data::TERRAIN_OCEAN;
	if (terrain == "Sumpf")
		return data::TERRAIN_SWAMP;
	if (terrain == "Ebene")
		return data::TERRAIN_PLAINS;
	if (terrain == "Wueste" || terrain == FXString(L"W\u00fcste"))
		return data::TERRAIN_DESERT;
	if (terrain == "Wald")
		return data::TERRAIN_FOREST;
	if (terrain == "Hochland")
		return data::TERRAIN_HIGHLAND;
	if (terrain == "Berge")
		return data::TERRAIN_MOUNTAIN;
	if (terrain == "Gletscher")
		return data::TERRAIN_GLACIER;
	if (terrain == "Vulkan")
		return data::TERRAIN_VOLCANO;
	if (terrain == "Eisberg")
		return data::TERRAIN_ICEBERG;
	if (terrain == "Feuerwand")
		return data::TERRAIN_FIREWALL;
	if (terrain == "Mahlstrom")
		return data::TERRAIN_MAHLSTROM;
    if (terrain == "Aktiver Vulkan")
        return data::TERRAIN_VOLCANO_ACTIVE;
    if (terrain == "Packeis")
        return data::TERRAIN_PACKICE;
    if (terrain == "Eisscholle")
        return data::TERRAIN_ICEFLOE;
    if (terrain == "Wand")
        return data::TERRAIN_WALL;
    if (terrain == "Halle")
        return data::TERRAIN_HALL;
    if (terrain == "Gang")
        return data::TERRAIN_CORRIDOR;
    if (terrain == "Rauchender Vulkan")
        return data::TERRAIN_VOLCANO_ACTIVE;
    if (terrain == "Nebel")
        return data::TERRAIN_FOG;
    else if (terrain == "Dichter Nebel")
        return data::TERRAIN_THICKFOG;

	return data::TERRAIN_UNKNOWN;
}

/*static*/ int datablock::parseSpecialTerrain(const FXString& terrain)
{
	// (terrain that uses image of another terrain)

	// this terrain types should be kept as text,
	// so if you resave the file, it's exact terrain type (_Aktiver_ Vulkan) 
	// could be saved.

	return data::TERRAIN_UNKNOWN;
}

/*static*/ FXString datablock::planeName(int plane)
{
	if (plane == 0)
		return "Standardebene";
	if (plane == 1)
		return "Astralraum";
	if (plane == 1137)
		return "Arena";
	if (plane == 59034966)
		return "Eternath";
	if (plane == 2000)
		return "Weihnachtsinsel";
	
	// else...
	return FXStringFormat("Ebene %d", plane);
}

// try to parse str as a datablock header
bool datablock::parse(const char* str)
{
	if (!str)
		return false;

	// skip potential indentation
	while (*str && isspace(*str))
		str++;

	if (*str < 'A' || *str > 'Z')
		return false;

	const char* srch = str + 1, *space = NULL;

    // TODO: can we rewrite this using FXString members?
	for (; *srch; srch++)
	{
		if (*srch == ' ' && !space)
			space = srch;
		
		if (*srch == '\"' || *srch == ';')	// block headers cannot contain '\"' or ';'
			return false;
	}

	// unset flags
	flags(FLAG_NONE);

    if (space)
	{
        FXString name(str, space - str);
        string(name);
		infostr(FXString(space + 1));
	}
    else {
        string(FXString(str));
        infostr(FXString_Empty);
    }

	// unset all other states
	terrain(data::TERRAIN_UNKNOWN);
	attachment(nullptr);
	return true;
}

datablock::datablock() : m_type(block_type::TYPE_UNKNOWN), m_info(0), m_x(0),m_y(0), m_terrain(0), m_flags(0), m_depth(-1), m_attachment(0)
{
}

datablock::~datablock()
{
	delete m_attachment;
}

void datablock::string(const FXString& s)
{
	m_type = parseType(s);

	if (m_type == block_type::TYPE_UNKNOWN)
		m_string = s;
	else
		m_string.clear();
}

const FXString datablock::string() const
{
	if (m_type == block_type::TYPE_UNKNOWN)
		return m_string;

    for (int i = 0; BLOCKNAMES[i].name; i++)
		if (m_type == BLOCKNAMES[i].id)
			return BLOCKNAMES[i].name;

	return "";
}

// return info() as base36
FXString datablock::id() const
{
	int nn = info();
	return (nn > 0) ? FXStringValEx(nn, 36) : "0";
}

void datablock::move(int x_offset, int y_offset)
{
    m_x += x_offset;
    m_y += y_offset;
}

void datablock::infostr(const FXString& s)
{  
	flags(flags() & ~(FLAG_BLOCKID_BIT0|FLAG_BLOCKID_BIT1));
	m_x = m_y = m_info = 0;

	if (!s.empty())
	{
		if (s.find(' ') == -1)
		{
			setFlags(FLAG_BLOCKID_BIT0);

			m_info = strtol(s.text(), NULL, 10);
		}
		else
		{
			setFlags(FLAG_BLOCKID_BIT1);

			m_x = strtol(s.text(), NULL, 10);
			m_y = strtol(s.section(' ',1).text(), NULL, 10);

			m_info = strtol(s.section(' ',2).text(), NULL, 10);
			if (m_info)
				setFlags(FLAG_BLOCKID_BIT0);
		}
	}
}

void datablock::terrain(int terrain)
{
	m_terrain = terrain;
}

void datablock::flags(int flags)
{
	m_flags = flags;
}

void datablock::setFlags(int flags)
{
	m_flags |= flags;
}

void datablock::depth(int depth)
{
	m_depth = depth;
}

void datablock::attachment(::attachment* attach)
{
	delete m_attachment;		// free old attachment and set new
	m_attachment = attach;
}

const char* datablock::UNITKEYS[] = {
    "target", "unit", "mage", "spy", "teacher", "student", nullptr
};

bool datablock::hasReference(const datablock& target) const
{
    if (target.type() == block_type::TYPE_REGION) {
        FXString match;
        match.format("%d %d %d", target.x(), target.y(), target.info());
        for (const datakey& key : m_data)
        {
            if (key.value() == match) return true;
        }
    }
    else if (target.type() == block_type::TYPE_SHIP) {
        int id = target.info();
        int uid = valueInt("ship");
        if (uid == id) return true;
    }
    else if (target.type() == block_type::TYPE_BUILDING) {
        int id = target.info();
        int uid = valueInt("building");
        if (uid == id) return true;
    }
    else if (target.type() == block_type::TYPE_UNIT) {
        int id = target.info();
        for (int i = 0; UNITKEYS[i]; ++i)
        {
            int uid = valueInt(UNITKEYS[i]);
            if (uid == id) return true;
        }
    }
    return false;
}

int datablock::getReference(block_type type) const
{
    if (type == block_type::TYPE_SHIP) {
        int uid = valueInt("ship");
        if (uid > 0) {
            return uid;
        }
    }
    else if (type == block_type::TYPE_BUILDING) {
        int uid = valueInt("building");
        if (uid > 0) {
            return uid;
        }
    }
    else if (type == block_type::TYPE_UNIT) {
        for (int i = 0; UNITKEYS[i]; ++i) {
            int uid = valueInt(UNITKEYS[i]);
            if (uid > 0) {
                return uid;
            }
        }
    }
    return 0;
}

bool datablock::hasKey(const key_type type) const
{
    for (datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
    {
        if (srch->type() == type) {
            return true;
        }
    }
    return false;
}

const FXString& datablock::value(const FXString& key) const
{
    for (datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
    {
        if (key == srch->key()) {
            return srch->value();
        }
    }
	return FXString_Empty;
}

int datablock::valueSkill(const FXString &skill) const
{
    for (datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
    {
        if (skill == srch->key()) {
            return srch->valueInt(1);
        }
    }
    return 0;
}

const FXString& datablock::value(key_type key) const
{
	for(datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
		if (srch->type() == key)
			return srch->value();

	return FXString_Empty;
}

int datablock::valueInt(const FXString& key, int def /* = 0 */) const
{
    for (datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++) {
        if (key == srch->key()) {
            assert(srch->isInt());
            return srch->getInt();
        }
    }

	return def;
}

int datablock::valueInt(key_type key, int def /* = 0 */) const
{
    for (datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
    {
        if (srch->type() == key)
        {
            assert(srch->isInt());
            return srch->getInt();
        }
    }
	return def;
}

const datakey* datablock::valueKey(int key) const
{
	for(datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
		if (srch->type() == key)
			return &*srch;

	return NULL;
}

bool datablock::removeKey(int key)
{
    for (datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
    {
        if (srch->type() == key) {
            m_data.erase(srch);
            return true;
        }
    }
    return false;
}

