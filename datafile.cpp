#include "datafile.h"

#include "version.h"
#include "main.h"
#include "fxhelper.h"
#include "terrain.h"

#include <ctime>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#define HELP_GUARD 16

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

int datakey::parseType(const FXString& type, enum block_type btype)
{
    if (type.empty()) {
        return TYPE_EMPTY;
    }
    if (btype == block_type::TYPE_COMBATSPELL)
    {
        if (type == "Name") {
            return TYPE_COMBATSPELL_NAME;
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
    if (type == "name")
		return TYPE_COMBATSPELL_NAME;
	if (type == "Beschr")
		return TYPE_DESCRIPTION;
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
	if (type == "Parteiname")
		return TYPE_FACTIONNAME;
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

FXString datakey::key() const
{
    switch (type()) {
    case TYPE_UNKNOWN:
        return m_key;
    case TYPE_EMPTY:
        return "";
    case TYPE_NAME:
        return "Name";
    case TYPE_COMBATSPELL_NAME:
        return "name";
    case TYPE_DESCRIPTION:
        return "Beschr";
    case TYPE_TERRAIN:
        return "Terrain";
    case TYPE_ISLAND:
        return "Insel";
    case TYPE_ID:
        return "id";
    case TYPE_FACTION:
        return "Partei";
    case TYPE_OTHER_FACTION:
        return "Anderepartei";
    case TYPE_FACTIONNAME:
        return "Parteiname";
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
    }
	return FXString();
}

int datakey::getInt() const
{
	return strtol(m_value.text(), NULL, 10);
}

// parses str and returns created datakey object or NULL pointer
bool datakey::parse(char* str, enum block_type btype, bool isUtf8)
{
	if (!str)
		return false;

	// skip indentation
	while(*str && isspace(*str))
		str++;

	// Ist die Zeile ein Tag?
	if (*str != '\"' && *str != '-' && (*str < '0' || *str > '9'))
        return false;		

	char *srch = str, *semikolon = NULL;

	for (; *srch; srch++)		// search for semicolon ';' in this line
	{
		if (*srch == ';')
			semikolon = srch;
		else if (*srch == '\"')
			semikolon = NULL;
	}

	// ok, so assign new values

	if (semikolon)
		key(semikolon+1, btype);			// ... and the FXString::operator=() copies the string itself.
	else
		key("", btype);					// no semikolon found.

	if (*str == '\"')				// cuts the " at start and end of the string
	{
		str++;

		if (semikolon)
		{
			if (semikolon[-1] == '\"')
				semikolon--;
		}
		else
		{
			while (*srch == '\0' && srch > str)
				srch--;

			if (*srch == '\"')
				*srch = '\0';
		}
	}
	else
		m_type |= TYPE_INTEGER;		// no "" found -> integer

	if (semikolon)
		*semikolon = '\0';			// 0 byte...
	
	/*
	\\ ist ein \, \" ist ein ", einzelen " sind Anfang und Ende 
	von Strings, \n ist ein Zeilenumbruch, (\x ist x, falls keine Sonderregel)
	*/

	int length;
	if (semikolon)
		length = semikolon - str;
	else
		length = srch - str;

	if (length < 0)
	{
		value("");
		return true;
	}
	
	FXString val('\0', length+1);	// Speicherplatz reservieren

	char *ptr = &val[0];
	for (; *str; str++, ptr++)
	{
		if (*str == '\\')
		{
			str++;
			if (*str == 'n')
				*ptr = '\n';
			else if (*str == '\0')
				break;
			else
				*ptr = *str;
		}
		else
			*ptr = *str;
	}

	val.length(ptr - &val[0]);
	
    if (!isUtf8) {
        val = iso2utf(val);
    }
	value(val);						// Wert speichern

	return true;
}

// =============================
// === datablock implementation

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

const FXString datablock::terrainString() const
{
	const FXString type = value(TYPE_TERRAIN);
	if (!type.empty())
		return type;

    switch (terrain()) {
    case data::TERRAIN_OCEAN:
        return "Ozean";
    case data::TERRAIN_SWAMP:
        return "Sumpf";
    case data::TERRAIN_PLAINS:
        return "Ebene";
    case data::TERRAIN_DESERT:
        return FXString(L"W\u00fcste");
    case data::TERRAIN_FOREST:
        return "Wald";
    case data::TERRAIN_HIGHLAND:
        return "Hochland";
    case data::TERRAIN_MOUNTAIN:
        return "Berge";
    case data::TERRAIN_GLACIER:
        return "Gletscher";
    case data::TERRAIN_VOLCANO:
        return "Vulkan";
    case data::TERRAIN_VOLCANO_ACTIVE:
        return "Aktiver Vulkan";
    case data::TERRAIN_ICEBERG:
        return "Eisberg";
    case data::TERRAIN_ICEFLOE:
        return "Eisscholle";
    case data::TERRAIN_CORRIDOR:
        return "Gang";
    case data::TERRAIN_WALL:
        return "Wand";
    case data::TERRAIN_HALL:
        return "Halle";
    case data::TERRAIN_FOG:
        return "Nebel";
    case data::TERRAIN_THICKFOG:
        return "Dichter Nebel";
    case data::TERRAIN_PACKICE:
        return "Packeis";
    case data::TERRAIN_FIREWALL:
        return "Feuerwand";
    case data::TERRAIN_MAHLSTROM:
        return "Mahlstrom";
    }
    return "Unbekannt";
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
	if (terrain == "Wueste" || terrain == "W\u00fcste" || terrain == FXString(L"W\u00fcste"))
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
		return "Eressea";
	if (plane == 1)
		return "Astralraum";
	if (plane == 1137)
		return "Arena";
	if (plane == 59034966)
		return "Eternath";
	if (plane == 2000)
		return "Weihnachtsinsel";
	
	// else...
	return FXString().format("Ebene %d", plane);
}

// parses str and returns created datablock object or NULL pointer
bool datablock::parse(char* str)
{
	if (!str)
		return false;

	// skip indentation
	while (*str && isspace(*str))
		str++;

	if (*str < 'A' || *str > 'Z')
		return false;

	char* srch = str, *space = NULL;

	for (; *srch; srch++)
	{
		if (*srch == ' ' && !space)
			space = srch;
		
		if (*srch == '\"' || *srch == ';')	// can't be a '\"' or ';'
			return false;
	}

	// unset flags
	flags(FLAG_NONE);

	if (space)
	{
		*space++ = '\0';
		infostr(space);
	}
	else
		infostr(FXString());

	string(str);

	// unset all other states
	terrain(data::TERRAIN_UNKNOWN);
	// depth(0);
	attachment(NULL);
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

bool datablock::hasKey(const key_type& type) const
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

// ===========================
// === datafile implementation

datafile::datafile() : m_factionId(0), m_recruitment(0), m_turn(-1), m_activefaction(m_blocks.end())
{
	m_cmds.modified = false;
}

// helper function that strips lines and return pointer to next line
static inline char* getNextLine(char* str)
{
	char* next = str;

	// search start of next line
	while(*next && *next != '\n')
		next++;		
		
	// overwrite cr and newline
	if (next > str && next[-1] == '\r')
		next[-1] = '\0';
	if(*next)
		*next++ = '\0';		// mark end of line

	return next;
}

const char* UTF8BOM = "\xEF\xBB\xBF";

int datafile::turn() const
{
    if (m_turn < 0) {
        datablock::citor block;
        for (block = m_blocks.begin(); block != m_blocks.end(); block++)
        {
            // set turn number to that found in version block
            if (block->type() == block_type::TYPE_VERSION) {
                m_turn = block->valueInt(TYPE_TURN, m_turn);
                break;
            }
        }
    }
    return m_turn;
}

// loads file, parses it and returns number of block
int datafile::load(const char* filename)
{
	if (!filename)
		return 0;

	FXString data;

	FXString filename_str = filename;
	// load plain text CR
	FXFileStream file;
	file.open(filename,FXStreamLoad);
	if (file.status() != FXStreamOK)
	{
		throw std::runtime_error(FXString(L"Datei konnte nicht ge\u00f6ffnet werden.").text());
	}

	FXString buffer;
	buffer.length(1024*100+1);

	do
	{
		memset(&buffer[0], 0, buffer.length());
		file.load(&buffer[0], buffer.length()-1);
		data.append(buffer.text());

	} while(file.status() == FXStreamOK);
		
	file.close();

	// ...

	m_blocks.clear();
	datablock *block = NULL, newblock;
	datakey key;
    bool utf8 = true;
	char *ptr, *next = &data[0];

	// check for utf8 BOM: EF BB BF
	if (!strncmp(next, UTF8BOM, strlen(UTF8BOM))) {
        next += strlen(UTF8BOM);
	}

	while(*(ptr = next))
	{
		// strip the line
		next = getNextLine(ptr);

		// erster versuch: enth\u00e4lt die Zeile einen datakey?
		if (key.parse(ptr, block ? block->type() : block_type::TYPE_UNKNOWN, utf8))
		{
			if (block)
			{
                int terrain;
                switch (key.type())
                {
                case TYPE_CHARSET:
                    if (utf8 && key.value() != "UTF-8")
                    {
                        utf8 = false;
                    }
                    break;
                case TYPE_TERRAIN:
                    terrain = datablock::parseTerrain(key.value());

                    if (terrain == data::TERRAIN_UNKNOWN)
                    {
                        block->data().push_back(key);		// need textual representation of terrain type

                        terrain = datablock::parseSpecialTerrain(key.value());
                    }

                    block->terrain(terrain);
                    break;
                default:
                    block->data().push_back(key);			// appends "Value";Key - tags
                }
			}
		}
		// zweiter versuch: enth\u00e4lt die Zeile einen datablock-header?
		else if (newblock.parse(ptr))
		{
			m_blocks.push_back(newblock);		// appends BLOCK Info - tags
			block = &m_blocks.back();
		}
        else if (block) {
            /* fix for a bug introduced by version 1.2.7 */
            const char* semi = strchr(ptr, ';');
            if (semi) {
                key.key(FXString(semi + 1), block->type());
                key.value(FXString(ptr, semi - ptr));
                block->data().push_back(key);
            }
        }
	}
    createHashTables();
	return m_blocks.size();
}

// saves file
int datafile::save(const char* filename, map_type map_filter)
{
	if (!filename)
		return 0;

	// open file for writing
	std::ostringstream file;

	FXFileStream plainfile;
	FXStream *filestr_ptr;

	FXString filename_str = filename;
	plainfile.open(filename, FXStreamSave);
	filestr_ptr = &plainfile;

	FXStream& filestr = *filestr_ptr;
	if (filestr.status() != FXStreamOK)
	{
		return -1;
	}

    datablock::itor block;
    datablock::itor last_block = m_blocks.end();
    int maxDepth = 0;
    for (block = m_blocks.begin(); block != last_block; ++block) {
		bool hideKeys = false;
        block_type type = block->type();

        if (maxDepth) {
            if (block->depth() <= maxDepth) {
                // reset the skip-child behavior
                maxDepth = 0;
            }
            else {
                // do not print child-blocks now
                continue;
            }
        }

        if (type == block_type::TYPE_ISLAND)
        {
            // No support for Magellan-style islands
            continue;
        }

        if (map_filter == map_type::MAP_MINIMAL) {
            /* skip over anything except version or region  */
            if (type != block_type::TYPE_REGION && type != block_type::TYPE_VERSION) {
                continue;
            }
        }
        else if (map_filter != map_type::MAP_FULL) {
            /* do not include these blocks at all */
            if (type == block_type::TYPE_EFFECTS
                || type == block_type::TYPE_MESSAGE
                || type == block_type::TYPE_MESSAGETYPE
                || type == block_type::TYPE_DURCHREISE
                || type == block_type::TYPE_DURCHSCHIFFUNG) {
                continue;
            }

            /* skip these blocks with all their children */
            if (type == block_type::TYPE_UNIT
                || type == block_type::TYPE_BATTLE
                || type == block_type::TYPE_SHIP) {
                maxDepth = block->depth();
                continue;
            }
        }

        // Blocknamen + ID-Nummern ausgeben
		file << block->string();

		// VERSION auf mindestens 64 hochsetzen
		if (type == block_type::TYPE_VERSION)
			if (block->info() < 64)
				block->infostr("64");

		if (type == block_type::TYPE_REGION || type == block_type::TYPE_BATTLE ||
				block->x() != 0 || block->y() != 0)
			file << ' ' << block->x() << ' ' << block->y();
		
		if (type == block_type::TYPE_COMBATSPELL || block->info())
			file << ' ' << block->info();
		file << std::endl;

        if (type == block_type::TYPE_VERSION)
        {
            file << "\"UTF-8\";charset" << std::endl;
        }

		if (type == block_type::TYPE_REGION)
		{
            file << '\"' << block->terrainString().text() << "\";Terrain" << std::endl;
        }
		// Konfiguration-Block anpassen und Charset auf ISO-8859-1 setzen
		else if (type == block_type::TYPE_VERSION)
		{
			for (datakey::itor tags = block->data().begin(); tags != block->data().end(); tags++)
			{
				if (tags->type() == TYPE_KONFIGURATION)
				{
                    tags->value(getConfigurationName(map_filter));
				}
			}
		}
		else if (type == block_type::TYPE_UNIT)
		{
			// search for command block of unit
			datablock::itor cmd = block;
			for (cmd++; cmd != last_block && cmd->depth() > block->depth(); cmd++)
				if (cmd->type() == block_type::TYPE_COMMANDS)
					break;				// found

			bool confirmed = block->valueInt(TYPE_ORDERS_CONFIRMED) != 0;

			if (cmd != last_block && cmd->type() == block_type::TYPE_COMMANDS)
			{
				// att_commands' confirmed attribute overwrites the tag
				if (att_commands* cmds = dynamic_cast<att_commands*>(cmd->attachment()))
					confirmed = cmds->confirmed;
			}

			if (confirmed)	// TYPE_ORDERS_CONFIRMED
				file << "1;ejcOrdersConfirmed" << std::endl;
		}
		else if (type == block_type::TYPE_COMMANDS)
		{
			if (att_commands* cmds = dynamic_cast<att_commands*>(block->attachment()))
			{
				hideKeys = true;		// hide original commands

				for (att_commands::cmdlist_t::iterator it = cmds->commands.begin(); it != cmds->commands.end(); it++)
				{
					file << '\"';

					FXString value = *it;
					/*
					\\ ist ein \, \" ist ein ", einzelen " sind Anfang und Ende 
					von Strings, \n ist ein Zeilenumbruch, (\x ist x, falls keine Sonderregel)
					*/
					for (int i = 0; i < value.length(); i++)
					{
						char c = value[i];

						if (c == '\\' || c == '\"')
							file << '\\';

						if (c == '\n')
							file << "\\n";
						else
							file << c;
					}

					file << '\"' << std::endl;
				}
			}
		}

		// Datakeys ausgeben
		datakey::itor tags = block->data().begin();
		datakey::itor iend = block->data().end();

		if (hideKeys)
			tags = iend;		// don't save keys of this block

		for (; tags != iend; tags++)
		{
            if (type == block_type::TYPE_FACTION) {
                if (map_filter != map_type::MAP_FULL) {
                    if (tags->type() != TYPE_BANNER
                        && tags->type() != TYPE_LOCALE
                        && tags->type() != TYPE_FACTIONNAME
                        && tags->type() != TYPE_EMAIL)
                    {
                        continue;
                    }
                }
            }
			else if (type == block_type::TYPE_REGION)
			{
                if (map_filter != map_type::MAP_FULL)
                {
                    if (tags->type() == TYPE_VISIBILITY) {
                        continue;
                    }
                    if (map_filter == map_type::MAP_MINIMAL)
                    {
                        int ttype = tags->type();
                        if (ttype != TYPE_NAME && ttype != TYPE_ISLAND && ttype != TYPE_ID)
                            continue;
                    }
                }
			}
			else if (type == block_type::TYPE_UNIT)
			{
				if (tags->type() == TYPE_ORDERS_CONFIRMED)
					continue;		// will be set above
			}

			FXString value = tags->value();
			// String wird mit "" umgeben...
			if (tags->isInt())
				file << value.text();				
			else
			{
				file << '\"';

				/*
				 \\ ist ein \, \" ist ein ", einzelen " sind Anfang und Ende 
				von Strings, \n ist ein Zeilenumbruch, (\x ist x, falls keine Sonderregel)
				*/
				for (int i = 0; i < value.length(); i++)
				{
					char c = value[i];

					if (c == '\\' || c == '\"')
						file << '\\';

					if (c == '\n')
						file << "\\n";
					else
						file << c;
				}

				file << '\"';
			}
				
			// key
            FXString key = tags->key();
            if (!key.empty())
			{
				file << ';' << key.text();
			}

			file << std::endl;
		}

        // save block to file
		std::string output = file.str();		
		filestr.save(output.c_str(), output.size());
		file.str("");
	}

	return m_blocks.size();
}

void datafile::mergeBlock(datablock::itor& block, const datablock::itor& begin, const datablock::itor& end, block_type parent_type)
{
    block_type type = block->type();
    int info = block->info();
    datablock::itor child = begin;
    datablock::itor insert = begin;
    for (child++; child != end && child->type() != parent_type; ++child) {
        if (child->type() == type && child->info() == info) {
            // we already have a block of this type
            break;
        }
    }
    if (child == end || child->type() != type) {
        // we do not have this kind of block
        m_blocks.insert(++insert, *block);
    }
}

void datafile::findOffset(datafile* new_cr, int* x_offset, int* y_offset) const
{
    // first, create an index of all regions with an Id in new_cr:
    std::map<int, datablock::itor> regionIndex;
    for (regions_map::const_iterator it = new_cr->m_regions.begin(); it != new_cr->m_regions.end(); ++it)
    {
        const datablock::itor& block = (*it).second;
        int plane = block->info();
        // only comparing plane 0
        if (plane == 0) {
            int id = block->valueInt(TYPE_ID, -1);
            if (id > 0) {
                regionIndex[id] = block;
            }
        }
    }

    // second, try finding a region from this report in the index:
    for (regions_map::const_iterator it = m_regions.begin(); it != m_regions.end(); ++it)
    {
        const datablock::itor& block = (*it).second;
        int id = block->valueInt(TYPE_ID, -1);
        int plane = block->info();
        if (plane == 0) {
            std::map<int, datablock::itor>::const_iterator match = regionIndex.find(id);
            if (match != regionIndex.end()) {
                const datablock::itor& other = (*match).second;
                int xo = block->x() - other->x();
                int yo = block->y() - other->y();
                if (xo != 0 || yo != 0) {
                    if (x_offset) *x_offset = xo;
                    if (y_offset) *y_offset = yo;
                }
                return;
            }
        }
    }
}

void datafile::removeTemporary()
{
    for (datablock::itor block = m_blocks.begin(); block != m_blocks.end(); ++block) {
        /* remove visibility status from older of the two reports */
        if (block->type() == block_type::TYPE_REGION)
        {
            block->removeKey(TYPE_VISIBILITY);
        }
    }
}

void datafile::merge(datafile * old_cr, int x_offset, int y_offset)
{
    // dann: Datei an den aktuellen CR anfuegen (nur Karteninformationen)
    datablock::itor old_end = old_cr->m_blocks.end();
    bool copy_children = false;
    for (datablock::itor old_r = old_cr->m_blocks.begin(); old_r != old_end;++old_r)
    {
        // handle only regions
        if (old_r->type() == block_type::TYPE_REGION)
        {
            bool was_seen = 0 != (old_r->flags() & datablock::FLAG_REGION_SEEN);
            int x = old_r->x();
            int y = old_r->y();
            int plane = old_r->info();
            if (plane == 0) {
                x += x_offset;
                y += y_offset;
            }
            datablock::itor new_r = region(x, y, plane);

            if (new_r != m_blocks.end())            // add some info to old cr (island names)
            {
                bool is_seen = (0 != (new_r->flags() & datablock::FLAG_REGION_SEEN));
                copy_children = false;
                if (const datakey* islandkey = old_r->valueKey(TYPE_ISLAND))
                {
                    if (!new_r->valueKey(TYPE_ISLAND))
                    {
                        if (islandkey && !islandkey->isInt())        // add only Vorlage-style islands (easier)
                            new_r->data().push_back(*islandkey);
                    }
                }
                if (was_seen) {
                    // old return contained good data that we may want to keep
                    if (!is_seen) {
                        for (const datakey& key : old_r->data())
                        {
                            if (key.type() == TYPE_NAME || key.type() == TYPE_TERRAIN) {
                                continue;
                            }
                            if (!new_r->hasKey(key.type())) {
                                new_r->data().push_back(key);
                            }
                        }
                    }
                    // copy child blocks if we don't have them
                    for (old_r++; old_r != old_end && old_r->type() != block_type::TYPE_REGION; ++old_r)
                    {
                        datablock::itor new_end = m_blocks.end();
                        block_type type = old_r->type();
                        if (type == block_type::TYPE_PRICES) {
                            mergeBlock(old_r, new_r, new_end, block_type::TYPE_REGION);
                        }
                        else if (!is_seen)
                        {
                            if (type == block_type::TYPE_BORDER || type == block_type::TYPE_RESOURCE) {
                                mergeBlock(old_r, new_r, new_end, block_type::TYPE_REGION);
                            }
                        }
                    }
                }
                if (old_r == old_end) {
                    break;
                }
                continue;
            }
            else // append region to this cr
            {
                if (x_offset || y_offset) {
                    if (old_r->info() == 0) {
                        old_r->move(x_offset, y_offset);
                    }
                }
                copy_children = true;
                old_r->attachment(nullptr);
                m_blocks.push_back(*old_r);
            }
        }
        else if (copy_children) {
            /* part of a new region that is appended to the report, copy detail blocks */
            switch (old_r->type()) {
            case block_type::TYPE_BUILDING:
            case block_type::TYPE_PRICES:
            case block_type::TYPE_BORDER:
            case block_type::TYPE_RESOURCE:
                m_blocks.push_back(*old_r);
                break;
            }
        }
    }
    createHashTables();
}

const char* datafile::getConfigurationName(map_type type)
{
    if (type == map_type::MAP_NORMAL) {
        return "CSMapFX:Export";
    }
    if (type == map_type::MAP_MINIMAL) {
        return "CSMapFX:Map";
    }
    return "CSMapFX";
}

// ====================================
// === datafile command loading routine

// loads command file and attaches the commands to the units
int datafile::loadCmds(const FXString &filename)
{
	cmdfilename("");
	modifiedCmds(false);

	if (filename.empty())
		return 0;

	if (activefaction() == m_blocks.end())
		throw std::runtime_error("Kein Report geladen, kann Befehlsdatei nicht einlesen.");


	// load plain text file
	FXFileStream file;
	file.open(filename,FXStreamLoad);
	if (file.status() != FXStreamOK)
        throw std::runtime_error(FXString(L"Datei konnte nicht ge\u00f6ffnet werden.").text());

	// load file into 'data'
	FXString data;

	FXString buffer;
	buffer.length(1024*100+1);

	do
	{
		memset(&buffer[0], 0, buffer.length());
		file.load(&buffer[0], buffer.length()-1);
		data.append(buffer.text());

	} while(file.status() == FXStreamOK);
	
	buffer.clear();
	file.close();

	// read in the data
	char *ptr, *next = &data[0];

	// check for utf8 BOM: EF BB BF
	if (!strncmp(next, UTF8BOM, strlen(UTF8BOM))) {
        next += strlen(UTF8BOM);
	}

	// search first non-empty line
	while(*(ptr = next))
	{
		// strip the line
		next = getNextLine(ptr);

		// skip indentation
		while (*ptr && isspace(*ptr))
			ptr++;

		// when not empty, break
		if (*ptr && *ptr != ';')
			break;
	}

	// found first non-empty line
	FXString line = ptr;
	if (!*ptr || (line.section(' ', 0) != "ERESSEA" && line.section(' ', 0) != "PARTEI"))
		throw std::runtime_error(FXString(L"Keine g\u00fcltige Befehlsdatei.").text());

	// parse header line:
	// ERESSEA ioen "PASSWORT"
	FXString id36 = line.section(' ', 1);
    FXString password = line.section(' ', 2);

    FXint pos = password.find('"', 0);
    while (pos >= 0) {
        password.erase(pos);
        pos = password.find('"', pos);
    }
    if (!password.empty()) {
        m_password = password;
    }

	char* endptr;
	int factionId = strtol(id36.text(), &endptr, 36);
    m_factionId = factionId;

    if (endptr && *endptr)		// id36 string has to be consumed by strtol
		throw std::runtime_error((L"Keine g\u00fcltige Parteinummer: " + id36).text());
	if (factionId != activefaction()->info())
		throw std::runtime_error((L"Die Befehle sind f\u00fcr eine andere Partei: " + id36).text());

    // consider command file as correct, read in commands
	m_cmds.prefix_lines.clear();
	m_cmds.region_lines.clear();
	m_cmds.region_order.clear();

	FXString cmd;

    int headerindent = 0, indent = 0;
	// process lines
	while(*(ptr = next))
	{
		// strip the line
		next = getNextLine(ptr);

		// check if line is REGION or EINHEIT command
		line = ptr;
        indent = line.find_first_not_of("\t ");
		cmd = line.trim().before(' ');
		cmd.upper();

        if (cmd == "REGION" || cmd == "EINHEIT") {
            break;
        }

        // add line to prefix
        if (!line.empty()) {
            m_cmds.prefix_lines.push_back(line);
        }
	}

	// check for end of file:
	if (!*ptr)
		throw std::runtime_error("Keine Befehle gefunden.");

	att_commands* cmds_list = NULL;
	std::vector<int> *unit_order = NULL;

	do
	{
		if (cmd == "REGION" || cmd == "EINHEIT")
		{
			FXString param = line.section(' ', 1);
            headerindent = indent;

			if (cmd == "REGION")
			{
				
				FXString xstr = param.section(',', 0);
				FXString ystr = param.section(',', 1);
				FXString zstr = param.section(',', 2);

				int x = strtol(xstr.text(), &endptr, 10);
				int y = strtol(ystr.text(), &endptr, 10);
				int z = strtol(zstr.text(), &endptr, 10);

				// add to order list if not already in it
				if (m_cmds.region_lines.find(koordinates(x, y, z)) == m_cmds.region_lines.end())
				{
					m_cmds.region_order.push_back(std::make_pair(koordinates(x, y, z), std::vector<int>()));
					unit_order = &m_cmds.region_order.back().second;
				}
				else
					unit_order = NULL;

				cmds_list = &m_cmds.region_lines[koordinates(x, y, z)];
			}
			else if (cmd == "EINHEIT")
			{
				int unitId = strtol(param.text(), &endptr, 36);

				// add to order list for units of this region
				if (unit_order)
					unit_order->push_back(unitId);
				
				// search for command block of unit
				int unitDepth = -1;
				datablock::itor block = unit(unitId);
				if (block != m_blocks.end())
				{
					unitDepth = block->depth();
					block++;
				}
                else {
                    throw std::runtime_error(("Einheit nicht gefunden: " + line).text());
                }
                for (cmds_list = NULL; block != m_blocks.end() && block->depth() > unitDepth; block++) {
                    if (block->type() == block_type::TYPE_COMMANDS) {
                        // TODO: why can't this be a static_cast?
                        if (att_commands *cmds = dynamic_cast<att_commands *>(block->attachment())) {
                            cmds_list = cmds;
                            break;
                        }
                    }
                }

                if (!cmds_list) {
                    throw std::runtime_error(("Einheit hat keinen Befehlsblock: " + line).text());
                }
			}

			if (cmds_list) {
				cmds_list->confirmed = false;
				cmds_list->prefix_lines.clear();
				cmds_list->commands.clear();
				cmds_list->postfix_lines.clear();
				cmds_list->header = line;
			}
		}
		else if (cmds_list)
		{
            if (!line.empty()) {
                if (line.left(1) == ";") {
                    cmd = line.after(';');
                    cmd.trim().lower();
                    if (flatten(cmd) == "bestaetigt") {
                        // don't add "; bestaetigt" comment, just set confirmed flag
                        cmds_list->confirmed = true;
                    }
                    else if (indent > headerindent) {
                        cmds_list->addCommand(line);
                    }
                    else if (cmds_list->commands.empty()) {
                        cmds_list->prefix_lines.push_back(line);
                    }
                    else {
                        // add to postfix if it follows some commands
                        cmds_list->postfix_lines.push_back(line);
                    }
                }
                else {
                    cmds_list->addCommand(line);
                }
            }
		}

		if (!*(ptr = next))
			break;

		// strip the line
		next = getNextLine(ptr);

		// check if line is REGION oder EINHEIT command
		line = ptr;
        indent = line.find_first_not_of("\t ");
        cmd = line.trim().before(' ');
		cmd.upper();

	} while (flatten(cmd) != "naechster");

	cmdfilename(filename);
	return true;
}

// saves command file
int datafile::saveCmds(const FXString& filename, const FXString& password, bool stripped)
{
	if (filename.empty())
		return -1;

    if (activefaction() == m_blocks.end())
		return -1;

	// Datei zum Schreiben \u00f6ffnen
	std::ofstream out(filename.text());
	if (!out.is_open())
		return -1;

	// Alles ok soweit...
	if (!stripped)
		modifiedCmds(false);	// command export doesn't change modified flag

	// get recruitment costs
	if (!recruitment())
		m_recruitment = 100;

	// save header and echeck information	// "Eressea";Spiel
	if (m_blocks.front().value("Spiel") == "Eressea"
		|| m_blocks.front().value("Spiel") == "E3")
		out << "ERESSEA";
	else
		out << "PARTEI";
	
	out << " " << activefaction()->id() << " ";
	
	if (password.empty())	
		out << "\"HIER-PASSWORT\"" << "\n";
	else
		out << "\"" << password << "\"\n";
    
	// output prefix lines
	if (!stripped && !m_cmds.prefix_lines.empty())
	{
		for (std::vector<FXString>::iterator itor = m_cmds.prefix_lines.begin(); itor != m_cmds.prefix_lines.end(); itor++)
			out << (*itor).text() << "\n";
	}
	else
	{
		out << "\n";
		out << "; ECHECK -v4.7 -l -w3 -r" << recruitment() << "\n";
		out << "\n";
		out << "; " << CSMAP_APP_TITLE_VERSION << "\n";
		out << "\n";
	}

	// loop through regions and units and complete the ordering lists
	int factionId = activefaction()->info();

	std::vector<int> *unit_order = NULL;
	for (datablock::itor region = m_blocks.end(), block = m_blocks.begin(); block != m_blocks.end(); block++)
	{
		if (block->type() == block_type::TYPE_REGION)
			region = block;
		else if (block->type() == block_type::TYPE_UNIT)
		{
			if (block->valueInt(TYPE_FACTION) != factionId)
				continue;

			// add region to list when first unit of active faction occurs
			if (region != m_blocks.end())
			{
				koordinates koord(region->x(), region->y(), region->info());

				if (m_cmds.region_lines.find(koord) == m_cmds.region_lines.end())
				{
					// not in list yet, create header
					att_commands* cmds = &m_cmds.region_lines[koord];

					cmds->header = "REGION " + FXStringVal(region->x()) + "," + FXStringVal(region->y());
					if (region->info())
						cmds->header += "," + FXStringVal(region->info());

                    cmds->header += " ; " + region->value(TYPE_NAME);
					cmds->header += " (" + region->terrainString() + ")";

					//  ; ECheck Lohn 13
					if (int salary = region->valueInt("Lohn"))
						cmds->prefix_lines.push_back("; ECheck Lohn " + FXStringVal(salary));

					// put into the list
					m_cmds.region_order.push_back(std::make_pair(koord, std::vector<int>()));

					unit_order = &m_cmds.region_order.back().second;
				}
				else
				{
					cmds_t::region_list_t::iterator end = m_cmds.region_order.end();
					for (cmds_t::region_list_t::iterator itor = m_cmds.region_order.begin(); itor != end; itor++)
						if (itor->first == koord)
						{
							unit_order = &itor->second;
							break;
						}
				}

				region = m_blocks.end();		// add region only before the first unit
			}

			// add unit
			int id = block->info();

			size_t i;
			for (i = 0; i < unit_order->size(); i++)
				if ((*unit_order)[i] == id)
					break;

			// when not found, add it
			if (i == unit_order->size())
				unit_order->push_back(id);
		}
	}

	// loop through the ordering lists and print out the commands
	for (cmds_t::region_list_t::iterator regord = m_cmds.region_order.begin(); regord != m_cmds.region_order.end(); regord++)
	{
		// REGION -144,142 ; Olymp-Vorgebirge (Hochland, 3 Personen, 146245$ Silber) [Draconis]

		// output region header + text lines
		att_commands* cmds = &m_cmds.region_lines[regord->first];

		out << cmds->header.text() << "\n";

		// output prefix lines
		for (att_commands::cmdlist_t::iterator itor = cmds->prefix_lines.begin(); itor != cmds->prefix_lines.end(); itor++)
			out << (*itor).text() << "\n";

		// output changed commands
		for (att_commands::cmdlist_t::iterator itor = cmds->commands.begin(); itor != cmds->commands.end(); itor++)
			out << "    " << (*itor).text() << "\n";

		// output postfix lines
        out << "\n";
        for (att_commands::cmdlist_t::iterator itor = cmds->postfix_lines.begin(); itor != cmds->postfix_lines.end(); itor++)
			out << (*itor).text() << "\n";

		// output units in order
		std::vector<int>& order = regord->second;

		for (std::vector<int>::iterator uid = order.begin(); uid != order.end(); uid++)
		{
			datablock::itor unit = this->unit(*uid);
			if (unit == m_blocks.end())
			{
				// no unit there is. problem we have.
				out << "  ; Einheit " << *uid << " (base10) nicht gefunden!\n";
				continue;
			}

			// search for command block of unit
			datablock::itor cmdb = unit;
			for (cmdb++; cmdb != m_blocks.end() && cmdb->depth() > unit->depth(); cmdb++)
				if (cmdb->type() == block_type::TYPE_COMMANDS)
					break;				// found

			if (cmdb == m_blocks.end() || cmdb->type() != block_type::TYPE_COMMANDS)
			{
				out << "  ; Einheit " << unit->id() << " hat keinen Befehlsblock!\n";
				continue;
			}

			// unit has command block
			//  EINHEIT wz5t;  Botschafter des Konzils [1,146245$,Beqwx(1/3)] k\u00e4mpft nicht

			att_commands* attcmds = dynamic_cast<att_commands*>(cmdb->attachment());
			if (attcmds && !attcmds->header.empty())
				out << attcmds->header.text() << "\n";
			else
			{
				// get amount of silber from GEGENSTAENDE block
				int silver = 0;
		
				datablock::itor items = unit;
				for (items++; items != m_blocks.end() && items->depth() > unit->depth(); items++)
					if (items->type() == block_type::TYPE_ITEMS)
					{
						silver = items->valueInt(TYPE_SILVER);
						break;
					}

				// output unit header
				out << "EINHEIT " << unit->id();

				out << ";  " << (unit->value(TYPE_NAME)).text();
			
				out << " " << "[" << unit->valueInt(TYPE_NUMBER) << "," << silver << "$]";

				out << "\n";
			}

			// output attachment (changed) or default commands
			if (attcmds)
			{
				if (attcmds->confirmed)
					out << "; bestaetigt\n";

				// output prefix lines
				for (att_commands::cmdlist_t::iterator itor = attcmds->prefix_lines.begin(); itor != attcmds->prefix_lines.end(); itor++)
					out << (*itor).text() << "\n";

				// output changed commands
				{
					for (att_commands::cmdlist_t::iterator itor = attcmds->commands.begin(); itor != attcmds->commands.end(); itor++)
						out << "    " << (*itor).text() << "\n";
				}

                out << "\n";
                // output postfix lines
				for (att_commands::cmdlist_t::iterator itor = attcmds->postfix_lines.begin(); itor != attcmds->postfix_lines.end(); itor++)
					out << (*itor).text() << "\n";
			}		
			else
			{
				// output default commands
				datakey::list_type &list = cmdb->data();

				for (datakey::itor itor = list.begin(); itor != list.end(); itor++)
					out << "    " << itor->value() << "\n";
			}
		}
	}

	out << "\nNAECHSTER\n";
	return 0;
}

// block hash tables
// -----------------
datablock::itor datafile::region(int x, int y, int plane)
{ 
	std::map<koordinates, datablock::itor>::iterator region;
	region = m_regions.find(koordinates(x, y, plane));

	if (region == m_regions.end())
		return m_blocks.end();

	return region->second;
}

datablock::itor datafile::battle(int x, int y, int plane)
{ 
	std::map<koordinates, datablock::itor>::iterator block;
    block = m_battles.find(koordinates(x, y, plane));

	if (block == m_battles.end())
		return m_blocks.end();

	return block->second;
}

bool datafile::getRegion(datablock::itor &out, int x, int y, int plane)
{
    datablock::itor block = region(x, y, plane);
    if (block != m_blocks.end()) {
        out = block;
        return true;
    }
    return false;
}

bool datafile::getBattle(datablock::itor &out, int x, int y, int plane)
{
    out = battle(x, y, plane);
    return (out != m_blocks.end());
}

bool datafile::hasBattle(int x, int y, int plane) const
{
    std::map<koordinates, datablock::itor>::const_iterator block;
    block  = m_battles.find(koordinates(x, y, plane));
    return (block != m_battles.end());
}

bool datafile::hasRegion(int x, int y, int plane) const
{
    std::map<koordinates, datablock::itor>::const_iterator region;
    region = m_regions.find(koordinates(x, y, plane));
    return (region != m_regions.end());
}

bool datafile::deleteRegion(datablock* block)
{
    datablock::itor first = region(block->x(), block->y(), block->info());
    // found the region. delete all blocks until next region.
    if (first == m_blocks.end()) {
        return false;
    }
    datablock::itor last = first;
    for (++last; last != m_blocks.end(); ++last) {
        // block is in region?
        if (last->depth() <= first->depth()) {
            m_blocks.erase(first, last);
            return true;
        }
    }
    m_blocks.erase(first, m_blocks.end());
    return true;
}

datablock::itor datafile::group(int id)
{ 
	std::map<int, datablock::itor>::iterator unit = m_groups.find(id);

	if (unit == m_groups.end()) {
        return m_blocks.end();
    }

	return unit->second;
}

bool datafile::getGroup(datablock::itor& out, int id)
{
    datablock::itor block = group(id);
    if (block != m_blocks.end()) {
        out = block;
        return true;
    }
    return false;
}

datablock::itor datafile::unit(int id)
{ 
	std::map<int, datablock::itor>::iterator unit = m_units.find(id);

	if (unit == m_units.end()) {
        return m_blocks.end();
    }

	return unit->second;
}

bool datafile::getUnit(datablock::itor& out, int id)
{
    datablock::itor block = unit(id);
    if (block != m_blocks.end()) {
        out = block;
        return true;
    }
    return false;
}

datablock::itor datafile::faction(int id)
{
	std::map<int, datablock::itor>::iterator faction = m_factions.find(id);

	if (faction == m_factions.end())
		return m_blocks.end();

	return faction->second;
}

bool datafile::getFaction(datablock::itor& out, int id)
{
    datablock::itor block = faction(id);
    if (block != m_blocks.end()) {
        out = block;
        return true;
    }
    return false;
}

datablock::itor datafile::building(int id)
{
    std::map<int, datablock::itor>::iterator building = m_buildings.find(id);

    if (building == m_buildings.end()) {
        return m_blocks.end();
    }

    return building->second;
}

bool datafile::getBuilding(datablock::itor& out, int id)
{ 
    datablock::itor block = building(id);
    if (block != m_blocks.end()) {
        out = block;
        return true;
    }
    return false;
}

datablock::itor datafile::ship(int id)
{ 
	std::map<int, datablock::itor>::iterator ship = m_ships.find(id);

    if (ship == m_ships.end()) {
        return m_blocks.end();
    }

	return ship->second;
}

bool datafile::getShip(datablock::itor& out, int id)
{
    datablock::itor block = ship(id);
    if (block != m_blocks.end()) {
        out = block;
        return true;
    }
    return false;
}

datablock::itor datafile::island(int id)
{ 
	std::map<int, datablock::itor>::iterator island = m_islands.find(id);

	if (island == m_islands.end())
		return m_blocks.end();

	return island->second;
}

bool datafile::getIsland(datablock::itor& out, int id)
{
    datablock::itor block = island(id);
    if (block != m_blocks.end()) {
        out = block;
        return true;
    }
    return false;
}

datablock::itor datafile::dummyToItor(const datablock* block)
{
	for (datablock::itor itor = m_dummyBlocks.begin(); itor != m_dummyBlocks.end(); itor++)
		if (block == &*itor)
			return itor;

	return m_blocks.end();
}

void datafile::findSelection(const datablock* select, datablock::itor& out, datablock::itor& region)
{
    datablock::itor end = m_blocks.end();

    region = end;
    out = end;
    for (datablock::itor itor = m_blocks.begin(); itor != m_blocks.end(); ++itor) {
        datablock& block = *itor;
        if (block.type() == block_type::TYPE_REGION) {
            region = itor;
        }
        if (&block == select) {
            out = itor;
            break;
        }
    }
}

FXString datafile::regionName(const datablock& block)
{
    FXString rname;
    if (block.type() == block_type::TYPE_REGION) {
        rname = block.value(TYPE_NAME);
        if (rname.empty()) {
            rname = block.terrainString();
        }
    } else {
        datablock::itor select;
        if (getRegion(select, block.x(), block.y(), block.info())) {
            rname = select->value(TYPE_NAME);
            if (rname.empty()) {
                rname = select->terrainString();
            }
        }
        else {
            return "Unbekannt";
        }
    }
    return rname;
}

void datafile::createHierarchy()
{
	typedef std::vector<block_type> stack;
	typedef std::set<block_type> tset;
	typedef std::map<block_type, tset> types_to_tset;

	types_to_tset enclosed;				// map of PARENT-BLOCK -> set of CHILD_BLOCKs
	
	// fill map
	for (int type = (int) block_type::TYPE_UNKNOWN+1; type != (int)block_type::TYPE_LAST; type++)
		enclosed[(block_type)type].insert(block_type::TYPE_UNKNOWN);

	enclosed[block_type::TYPE_VERSION].insert(block_type::TYPE_BATTLE);
		enclosed[block_type::TYPE_BATTLE].insert(block_type::TYPE_MESSAGE);
	enclosed[block_type::TYPE_VERSION].insert(block_type::TYPE_MESSAGETYPE);
	enclosed[block_type::TYPE_VERSION].insert(block_type::TYPE_ISLAND);
	enclosed[block_type::TYPE_VERSION].insert(block_type::TYPE_TRANSLATION);
	enclosed[block_type::TYPE_VERSION].insert(block_type::TYPE_REGION);
		enclosed[block_type::TYPE_REGION].insert(block_type::TYPE_SCHEMEN);
		enclosed[block_type::TYPE_REGION].insert(block_type::TYPE_BORDER);
		enclosed[block_type::TYPE_REGION].insert(block_type::TYPE_RESOURCE);
		enclosed[block_type::TYPE_REGION].insert(block_type::TYPE_ITEMS);
		enclosed[block_type::TYPE_REGION].insert(block_type::TYPE_UNIT);
			enclosed[block_type::TYPE_UNIT].insert(block_type::TYPE_EFFECTS);
			enclosed[block_type::TYPE_UNIT].insert(block_type::TYPE_COMMANDS);
			enclosed[block_type::TYPE_UNIT].insert(block_type::TYPE_ITEMS);
			enclosed[block_type::TYPE_UNIT].insert(block_type::TYPE_SPELLS);
			enclosed[block_type::TYPE_UNIT].insert(block_type::TYPE_COMBATSPELL);
			enclosed[block_type::TYPE_UNIT].insert(block_type::TYPE_UNITMESSAGES);
			enclosed[block_type::TYPE_UNIT].insert(block_type::TYPE_TALENTS);
		enclosed[block_type::TYPE_REGION].insert(block_type::TYPE_BUILDING);
			enclosed[block_type::TYPE_BUILDING].insert(block_type::TYPE_EFFECTS);
		enclosed[block_type::TYPE_REGION].insert(block_type::TYPE_DURCHREISE);
		enclosed[block_type::TYPE_REGION].insert(block_type::TYPE_DURCHSCHIFFUNG);
		enclosed[block_type::TYPE_REGION].insert(block_type::TYPE_SHIP);
			enclosed[block_type::TYPE_SHIP].insert(block_type::TYPE_EFFECTS);
		enclosed[block_type::TYPE_REGION].insert(block_type::TYPE_PRICES);
		enclosed[block_type::TYPE_REGION].insert(block_type::TYPE_EFFECTS);
		enclosed[block_type::TYPE_REGION].insert(block_type::TYPE_MESSAGE);
	enclosed[block_type::TYPE_FACTION].insert(block_type::TYPE_ZAUBER);
		enclosed[block_type::TYPE_ZAUBER].insert(block_type::TYPE_KOMPONENTEN);
	enclosed[block_type::TYPE_VERSION].insert(block_type::TYPE_TRANK);
		enclosed[block_type::TYPE_TRANK].insert(block_type::TYPE_ZUTATEN);
	enclosed[block_type::TYPE_VERSION].insert(block_type::TYPE_FACTION);
		enclosed[block_type::TYPE_FACTION].insert(block_type::TYPE_GROUP);
			enclosed[block_type::TYPE_GROUP].insert(block_type::TYPE_ALLIANCE);
		enclosed[block_type::TYPE_FACTION].insert(block_type::TYPE_ALLIANCE);
		enclosed[block_type::TYPE_FACTION].insert(block_type::TYPE_BATTLE);
		enclosed[block_type::TYPE_FACTION].insert(block_type::TYPE_MESSAGE);
		enclosed[block_type::TYPE_FACTION].insert(block_type::TYPE_OPTIONS);
		enclosed[block_type::TYPE_FACTION].insert(block_type::TYPE_ITEMS);

	// build hierarchy
	stack parents;						// parent block types. size() == depth in hierarchy.

	for (datablock::itor block = m_blocks.begin(); block != m_blocks.end(); block++)
	{
        block_type type = block->type();

		while (!parents.empty())
		{
			tset& types = enclosed[parents.back()];

			// found in hierarchy config?
			if (types.find(type) != types.end())
				break;

			// parent-block cannot have this block as child
            parents.pop_back();
		}

		parents.push_back(type);

		block->depth(parents.size());
	}
}

void datafile::createHashTables()
{
	m_battles.clear();
	m_regions.clear();
	m_units.clear();
    m_groups.clear();
	m_factions.clear();
	m_buildings.clear();
	m_ships.clear();
	m_islands.clear();
	m_dummyBlocks.clear();

	m_activefaction = m_blocks.end();
	m_recruitment = 0;
	m_turn = 0;

	datablock* region = NULL;
	int region_own = 0;
	int region_ally = 0;
	int region_enemy = 0;

	std::map<int, int> allied_status;	// what factions do we have HELP stati set to?
	std::set<int> unit_got_taxes;		// units that got taxes (MSG id 1264208711); the regions will get a coins icon

    createHierarchy();
	datablock::itor block;
	for (block = m_blocks.begin(); block != m_blocks.end(); block++)
	{
		// set turn number to that found in version block
		if (block->type() == block_type::TYPE_VERSION)
			m_turn = block->valueInt(TYPE_TURN, m_turn);

		// set faction as active faction (for ally-state)
		if (block->type() == block_type::TYPE_FACTION)
		{
			if (m_activefaction == m_blocks.end())
				if (/*block->value(TYPE_TYPE) != "" ||*/ block->value(TYPE_OPTIONS) != "")
				{
					m_activefaction = block;		// set active faction here

					// get turn from faction block if VERSION block has none
					if (!m_turn)
						m_turn = block->valueInt(TYPE_TURN, m_turn);
					if (!m_recruitment)
						m_recruitment = block->valueInt(TYPE_RECRUITMENTCOST, m_recruitment);
					break;
				}
		}
	}

	// continue to evaluate ALLIANCE blocks for active faction
	if (activefaction() != m_blocks.end())
	{
		int factionDepth = activefaction()->depth();
		block = activefaction();
		for (block++; block != m_blocks.end() && block->depth() > factionDepth; block++)
		{
            if (block->type() == block_type::TYPE_ALLIANCE) {
                int status = block->valueInt("Status", 0);
                allied_status[block->info()] = status;
            }
            else if (block->type() == block_type::TYPE_GROUP) {
                break;
            }
		}
	}

	for (block = m_blocks.begin(); block != m_blocks.end(); block++)
	{
        // add battle to list
        if (block->type() == block_type::TYPE_BATTLE)
        {
            m_battles[koordinates(block->x(), block->y(), block->info())] = block;
        }
        // add region to region list
        else if (block->type() == block_type::TYPE_REGION)
		{
			if (region)
			{
				region->setFlags(region_own ? datablock::FLAG_REGION_SEEN : 0);

				int own_log = 0, ally_log = 0, enemy_log = 0;

				while (region_own)
					own_log++, region_own /= 2;

				while (region_ally)
					ally_log++, region_ally /= 2;

				while (region_enemy)
					enemy_log++, region_enemy /= 2;

				if (own_log) own_log++;		// f\u00fcr st\u00e4rkere Auspr\u00e4gung
				if (ally_log) ally_log++;
				if (enemy_log) enemy_log++;

				// generate new style flag information. log_2(people) = 1 to 13
				if (own_log || ally_log || enemy_log)
				{
					att_region* stats = new att_region;
					region->attachment(stats);

                    stats->people.reserve(enemy_log ? 3 : 2);
					stats->people.push_back(own_log / 13.0f);
					stats->people.push_back(ally_log / 13.0f);
					if (enemy_log)
						stats->people.push_back(enemy_log / 13.0f);
				}
			}

			region = &*block;
			region->flags(region->flags() & (datablock::FLAG_BLOCKID_BIT0|datablock::FLAG_BLOCKID_BIT1));	// unset all flags except BLOCKID flags
			region_own = region_ally = region_enemy = 0;

			if (block->value(TYPE_VISIBILITY) == "lighthouse")
				region->setFlags(datablock::FLAG_LIGHTHOUSE);		// region is seen by lighthouse
			else if (block->value(TYPE_VISIBILITY) == "travel")
				region->setFlags(datablock::FLAG_TRAVEL);			// region is seen by traveling throu

			// put newest region into map
			if (m_regions.find(koordinates(block->x(), block->y(), block->info())) !=
					m_regions.end())
			{
				int region_turn = block->valueInt(TYPE_TURN, m_turn);

				datablock::itor oldregion = m_regions[koordinates(block->x(),block->y(), block->info())];

				int old_region_turn = oldregion->valueInt(TYPE_TURN, m_turn);

				if (region_turn >= old_region_turn)
					m_regions[koordinates(block->x(), block->y(), block->info())] = block;
			}
			else
				m_regions[koordinates(block->x(), block->y(), block->info())] = block;

			// get region owner (E3 only)
			if (activefaction() != m_blocks.end())
			{
				int ownerId = block->valueInt("owner", -1);
				if (ownerId == activefaction()->info())
					region->setFlags(datablock::FLAG_REGION_OWN);
                else if (allied_status[ownerId] & HELP_GUARD) {
                    region->setFlags(datablock::FLAG_REGION_ALLY);
                }
                else if (ownerId != -1)
					region->setFlags(datablock::FLAG_REGION_ENEMY);
			}
		}

        // add ships and buildings to their lists
        else if (block->type() == block_type::TYPE_SHIP)
			m_ships[block->info()] = block;
        else if (block->type() == block_type::TYPE_BUILDING)
			m_buildings[block->info()] = block;

		// generate list of units that got a "got taxes" message (E3 only)
        else if (block->type() == block_type::TYPE_MESSAGE) {
            if (block->value("type") == "1264208711")
            {
                //std::set<int> unit_got_taxes;
                int unitId = block->valueInt("unit", -1);
                unit_got_taxes.insert(unitId);
            }
        }
        // add units to unit list
        else if (block->type() == block_type::TYPE_GROUP)
        {
            m_groups[block->info()] = block;
        }
        else if (block->type() == block_type::TYPE_UNIT)
		{
			m_units[block->info()] = block;

			int factionId = block->valueInt(TYPE_FACTION, -1);

			if (faction(factionId) == m_blocks.end())
			{
				datablock faction;
				faction.string("PARTEI");
				faction.infostr(FXStringVal(factionId));
				
				if (factionId == 0 || factionId == 666)	// Monster (0) and the new Monster (ii)
				{
					datakey key;
					key.key("Parteiname", block->type());
					key.value("Monster");
					faction.data().push_back(key);
				}

                m_dummyBlocks.push_back(faction);
				m_factions[factionId] = --m_dummyBlocks.end();
			}

			// set attachment for unit of active faction
			if (activefaction() != m_blocks.end() && factionId == activefaction()->info())
			{
				// search for command block of unit
				datablock::itor cmd = block;
				for (cmd++; cmd != m_blocks.end() && cmd->depth() > block->depth(); cmd++)
					if (cmd->type() == block_type::TYPE_COMMANDS)
						break;				// found

				// add att_commands to command block
				if (cmd != m_blocks.end() && cmd->type() == block_type::TYPE_COMMANDS && !cmd->attachment())
				{
					att_commands* cmds = new att_commands;
					cmd->attachment(cmds);
					
					cmds->confirmed = block->valueInt(TYPE_ORDERS_CONFIRMED) != 0;

					datakey::list_type &list = cmd->data();

					for (datakey::itor itor = list.begin(); itor != list.end(); itor++)
						cmds->commands.push_back(itor->value());
				}
			}
		}

		// .. and factions to faction list
        else if (block->type() == block_type::TYPE_FACTION) {
            m_factions[block->info()] = block;
        }
        else if (block->type() == block_type::TYPE_ALLIANCE) {
            // alliance as placeholder-faction
            if (faction(block->info()) == m_blocks.end()) {
                m_factions[block->info()] = block;
            }
        }
		// .. and islands to island list
        else if (block->type() == block_type::TYPE_ISLAND) {
            m_islands[block->info()] = block;
        }

		if (region)
		{
			if (block->type() == block_type::TYPE_UNIT)
			{
				region->setFlags(datablock::FLAG_TROOPS);			// region has units

				// count persons
				int number = block->valueInt(TYPE_NUMBER, 0);

				enum
				{
					UNIT_ENEMY,
					UNIT_OWN,
					UNIT_ALLY,
				} owner = UNIT_ENEMY;

				if (activefaction() != m_blocks.end())
				{
					int factionId = block->valueInt(TYPE_FACTION, -1);
					if (factionId == activefaction()->info())
					{
						region_own += number;
						owner = UNIT_OWN;
					}
					else if (allied_status[factionId] != 0)
					{
						region_ally += number;
						owner = UNIT_ALLY;
					}
					else
					{
						region_enemy += number;
						owner = UNIT_ENEMY;
					}
				}

				if (block->valueInt("bewacht") == 1)
				{
					if (owner == UNIT_OWN)
						region->setFlags(datablock::FLAG_GUARD_OWN);
					else if (owner == UNIT_ALLY)
						region->setFlags(datablock::FLAG_GUARD_ALLY);
					else
						region->setFlags(datablock::FLAG_GUARD_ENEMY);
				}

				if (unit_got_taxes.find( block->info() ) != unit_got_taxes.end())
					region->setFlags(datablock::FLAG_REGION_TAXES);

				if (block->value(TYPE_FACTION) == "0")
					region->setFlags(datablock::FLAG_MONSTER);		// Monster (0) in region
				if (block->value(TYPE_FACTION) == "666")
					region->setFlags(datablock::FLAG_MONSTER);		// Monster (ii) in region
				if (block->valueInt(TYPE_FACTION) == 0 || block->valueInt(TYPE_FACTION) == 666
					|| block->value(TYPE_FACTION).empty())
				{
					if (block->value(TYPE_TYPE) == "Skelette" ||
							block->value(TYPE_TYPE) == "Skelettherren" ||
							block->value(TYPE_TYPE) == "Zombies" ||
							block->value(TYPE_TYPE) == "Juju-Zombies" ||
							block->value(TYPE_TYPE) == "Ghoule" ||
							block->value(TYPE_TYPE) == "Ghaste" ||
							block->value(TYPE_TYPE) == "Ents" ||
							block->value(TYPE_TYPE) == "Bauern" ||
							block->value(TYPE_TYPE) == FXString(L"Hirnt\u00f6ter"))
						region->setFlags(datablock::FLAG_MONSTER);		// monsters in the region
					else if (block->value(TYPE_TYPE) == "Seeschlangen")
						region->setFlags(datablock::FLAG_SEASNAKE);		// sea snake in region
					else if (block->value(TYPE_TYPE) == "Jungdrachen" ||
							block->value(TYPE_TYPE) == "Drachen" ||
							block->value(TYPE_TYPE) == "Wyrme")
						region->setFlags(datablock::FLAG_DRAGON);		// a dragon is in the region!
				}
			}
			else if (block->type() == block_type::TYPE_DURCHSCHIFFUNG)
				region->setFlags(datablock::FLAG_SHIPTRAVEL);		// region has travelled by ship
			else if (block->type() == block_type::TYPE_BUILDING)
			{
				region->setFlags(datablock::FLAG_CASTLE);			// region has a building
				
				if (block->value(TYPE_TYPE) == "Wurmloch")
					region->setFlags(datablock::FLAG_WORMHOLE);		// a wormhole is in the region!
			}
			else if (block->type() == block_type::TYPE_SHIP)
				region->setFlags(datablock::FLAG_SHIP);				// region has ships inside
			else if (block->type() == block_type::TYPE_BORDER)
			{
				const FXString& type = block->value("typ");

				if (type == "Strasse" || type == "Stra\u00dfe" || type == FXString(L"Stra\u00dfe"))	// TO FIX!
				{												// region has street in some direction
					int direction = block->valueInt("richtung");
					if (direction >= 0 && direction <= 5)
					{
						if (block->valueInt("prozent") == 100)			// region has a complete street
							region->setFlags(datablock::FLAG_STREET << direction);
						else											// region has incomplete street
							region->setFlags(datablock::FLAG_STREET_UNDONE << direction);
					}
				}
			}
		}
	}

	// islands
	std::list<datablock::itor> floodislands;		// regions whose island names flood the island

	for (block = m_blocks.begin(); block != m_blocks.end(); block++)
	{
        if (block->type() != block_type::TYPE_REGION)
			continue;
        
		// get regions name
		if (const datakey* islandkey = block->valueKey(TYPE_ISLAND))
		{
			FXString name;

			if (islandkey->isInt())		// Magellan-style: integer-Island-tags and ISLAND blocks with names
			{
				datablock::itor island = this->island(islandkey->getInt());
				if (island != m_blocks.end())
					name = island->value(TYPE_NAME);
				else
					name = "Insel " + FXStringVal(islandkey->getInt());
			}
			else						// Vorlage-style: string-Island-tags that flood the island
			{
				name = islandkey->value();

				floodislands.push_back(block);		// Vorlage-style floods the islands
			}

			if (name.empty())
				name = "Unbekannte Insel";

			att_region* stats = dynamic_cast<att_region*>(block->attachment());
			if (!stats) {
				stats = new att_region;
				block->attachment(stats);
			}

			stats->island = name;
		}
	}

	// flood island names. add regions that get a name to list so that they also flood.
	int offsets[][2] = { {0,1}, {0,-1}, {1,0}, {-1,0}, {1,-1}, {-1,1} };

	for (std::list<datablock::itor>::iterator itor = floodislands.begin(); itor != floodislands.end(); itor++)
	{
		FXString name;
		if (att_region* stats = dynamic_cast<att_region*>((*itor)->attachment()))
			name = stats->island;

		int x = (*itor)->x(), y = (*itor)->y(), z = (*itor)->info();

		// get neighbours
		for (int i = 0; i < 6; i++)
		{
			int nx = x + offsets[i][0], ny = y + offsets[i][1];

			datablock::itor neighbour = this->region(nx, ny, z);
			if (neighbour != m_blocks.end())
			{
				// only flood to "Festland"
				if (neighbour->terrain() == data::TERRAIN_OCEAN ||
					neighbour->terrain() == data::TERRAIN_MAHLSTROM ||
					neighbour->terrain() == data::TERRAIN_FIREWALL)
					continue;

				att_region* stats = dynamic_cast<att_region*>(neighbour->attachment());
				if (!stats) {
					stats = new att_region;
					neighbour->attachment(stats);
				}

				if (stats->island.empty()) {
					stats->island = name;
				
					floodislands.push_back(neighbour);
				}
			}
		}
	}
}
