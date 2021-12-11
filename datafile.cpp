#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "version.h"
#include "main.h"
#include "fxhelper.h"
#include "datafile.h"
#include "terrain.h"

#define HELP_GUARD 16

void att_commands::addCommand(const FXString &line) {
    std::copy(postfix_lines.begin(), postfix_lines.end(),
        std::back_inserter(commands));
    postfix_lines.clear();
    commands.push_back(line);
}

// ===========================
// === datakey implementation

void datakey::key(const FXString& type)
{
	m_type = parseType(type);

	if (m_type == TYPE_UNKNOWN)
		m_key = type;
	else
		m_key.clear();
}

void datakey::value(const FXString& s)
{
	m_value = s;
}

/*static*/ int datakey::parseType(const FXString& type)
{
	if (type.empty())
		return TYPE_EMPTY;
	if (type == "Name")
		return TYPE_NAME;
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
	if (type == "Parteiname")
		return TYPE_FACTIONNAME;
	if (type == "Anzahl")
		return TYPE_NUMBER;
	if (type == "Typ")
		return TYPE_TYPE;
	if (type == "skill")
		return TYPE_SKILL;
	if (type == "Konfiguration")
		return TYPE_KONFIGURATION;
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
	if (type == "ejcOrdersConfirmed")
		return TYPE_ORDERS_CONFIRMED;
	
	return TYPE_UNKNOWN;
}

const FXString datakey::key() const
{
	if (type() == TYPE_UNKNOWN)
		return m_key;
	else if (type() == TYPE_EMPTY)
		return "";
	else if (type() == TYPE_NAME)
		return "Name";
	else if (type() == TYPE_DESCRIPTION)
		return "Beschr";
	else if (type() == TYPE_TERRAIN)
		return "Terrain";
	else if (type() == TYPE_ISLAND)
		return "Insel";
	else if (type() == TYPE_ID)
		return "id";
	else if (type() == TYPE_FACTION)
		return "Partei";
	else if (type() == TYPE_FACTIONNAME)
		return "Parteiname";
	else if (type() == TYPE_NUMBER)
		return "Anzahl";
	else if (type() == TYPE_TYPE)
		return "Typ";
	else if (type() == TYPE_SKILL)
		return "skill";
	else if (type() == TYPE_KONFIGURATION)
		return "Konfiguration";
	else if (type() == TYPE_CHARSET)
		return "charset";
	else if (type() == TYPE_VISIBILITY)
		return "visibility";
	else if (type() == TYPE_TURN)
		return "Runde";
	else if (type() == TYPE_SILVER)
		return "Silber";
	else if (type() == TYPE_RECRUITMENTCOST)
		return "Rekrutierungskosten";
	else if (type() == TYPE_AURA)
		return "Aura";
	else if (type() == TYPE_AURAMAX)
		return "Auramax";
	else if (type() == TYPE_OPTIONS)
		return "Optionen";
	else if (type() == TYPE_EMAIL)
		return "email";
	else if (type() == TYPE_BANNER)
		return "banner";
    else if (type() == TYPE_LOCALE)
        return "locale";
    else if (type() == TYPE_ORDERS_CONFIRMED)
		return "ejcOrdersConfirmed";

	return "";
}

int datakey::getInt() const
{
	return strtol(m_value.text(), NULL, 10);
}

// parses str and returns created datakey object or NULL pointer
bool datakey::parse(char* str, bool isUtf8)
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
		key(semikolon+1);			// ... and the FXString::operator=() copies the string itself.
	else
		key("");					// no semikolon found.

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
		
		if (*srch == '\"')	// can't be a '\"'
			return false;
		if (*srch == ';')	// ';' is a comment, stop here
		{
			*srch = '\0';
			break;
		}
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
	depth(0);
	attachment(NULL);
	return true;
}

datablock::datablock() : m_type(block_type::TYPE_UNKNOWN), m_info(0), m_x(0),m_y(0), m_terrain(0), m_flags(0), m_depth(0), m_attachment(0)
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
	char buf[35], b36[] = "0123456789abcdefghijkLmnopqrstuvwxyz";
	char *p=buf+34;
	int nn=(int)info();
	bool negativ = false;
	if (nn < 0)
	{
        nn = -nn;
		negativ = true;
	}
	*p='\0';
	do{
		*--p=b36[nn%36];
		nn/=36;
	}
	while(nn);
	if (negativ)
		*--p='-';
	FXASSERT(buf<=p);
	return FXString(p,buf+34-p);
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

const FXString datablock::value(const char* key) const
{
	for(datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
		if (srch->key() == key)
			return srch->value();

	return "";
}

const FXString datablock::value(key_type key) const
{
	for(datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
		if (srch->type() == key)
			return srch->value();

	return "";
}

int datablock::valueInt(const char* key, int def /* = 0 */) const
{
	for(datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
		if (srch->key() == key)
			return srch->getInt();

	return def;
}

int datablock::valueInt(int key, int def /* = 0 */) const
{
	for(datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
		if (srch->type() == key)
			return srch->getInt();

	return def;
}

const datakey* datablock::valueKey(int key) const
{
	for(datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
		if (srch->type() == key)
			return &*srch;

	return NULL;
}

// ===========================
// === datafile implementation

datafile::datafile() : m_factionId(0), m_recruitment(0), m_turn(0), m_activefaction(end())
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

// loads file, parses it and returns number of block
int datafile::load(const char* filename)
{
	this->filename(filename);

	if (!filename)
		return 0;

	FXString data;

	FXString filename_str = filename;
	// load plain text CR
	FXFileStream file;
	file.open(filename,FXStreamLoad);
	if (file.status() != FXStreamOK)
	{
		this->filename("");
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
		if (key.parse(ptr, utf8))
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
	}

	createHierarchy();		// set depth field of blocks
	createHashTables();		// creates hash tables and set region flags

	return blocks().size();
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
    datablock::itor last_block = blocks().end();
    int maxDepth = 0;
    for (block = blocks().begin(); block != last_block; ++block) {
		bool hideKeys = false;
        block_type type = block->type();

        if (maxDepth) {
            if (block->depth() > maxDepth) {
                // reset the skip-child behavior
                maxDepth = 0;
            }
            else {
                // do not print child-blocks now
                ++block;
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
            if (type == block_type::TYPE_REGION) {
                hideKeys = true;
            }
            else if (type != block_type::TYPE_VERSION) {
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

		// Name-Tag und Terrain-Tag ausgeben
		if (type == block_type::TYPE_REGION)
		{
			FXString name = block->value(TYPE_NAME);
			if (!name.empty())
			{
				file << '\"';

				for (int i = 0; i < name.length(); i++)
				{
					char c = name[i];

					if (c == '\\' || c == '\"')
						file << '\\';

					if (c == '\n')
						file << "\\n";
					else
						file << c;
				}

				file << "\";Name" << std::endl;
			}

			file << '\"' << block->terrainString().text() << "\";Terrain" << std::endl;
            if (const datakey* islandkey = block->valueKey(TYPE_ISLAND))
            {
                file << '\"' << islandkey->value().text() << "\";" << islandkey->key().text() << std::endl;
            }
        }
		// Konfiguration-Block anpassen und Charset auf ISO-8859-1 setzen
		else if (type == block_type::TYPE_VERSION)
		{
			for (datakey::itor tags = block->data().begin(); tags != block->data().end(); tags++)
			{
				if (tags->type() == TYPE_KONFIGURATION)
				{
                    tags->value(CSMAP_APP_TITLE_VERSION);
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
                if (map_filter == map_type::MAP_FULL && tags->type() == TYPE_VISIBILITY)
                    continue;
                // ;Terrain ignorieren. Wird an ;Name angehangen... (s.u.)
                if (tags->type() == TYPE_TERRAIN || tags->type() == TYPE_NAME)
					continue;
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
			if (!tags->key().empty())
			{
				file << ';' << tags->key().text();	// als utf8 ausgeben
			}

			file << std::endl;
		}

        // save block to file
		std::string output = file.str();		
		filestr.save(output.c_str(), output.size());
		file.str("");
	}

	return blocks().size();
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

	if (activefaction() == end())
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
		if (*ptr)
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
				if (block != end())
				{
					unitDepth = block->depth();
					block++;
				}
                else {
                    throw std::runtime_error(("Einheit nicht gefunden: " + line).text());
                }
                for (cmds_list = NULL; block != end() && block->depth() > unitDepth; block++) {
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

    if (activefaction() == end())
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
	if (blocks().front().value("Spiel") == "Eressea"
		|| blocks().front().value("Spiel") == "E3")
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
		out << "; ECHECK -l -w3 -r" << recruitment() << "\n";
		out << "\n";
		out << "; " << CSMAP_APP_TITLE_VERSION << "\n";
		out << "\n";
	}

	// loop through regions and units and complete the ordering lists
	int factionId = activefaction()->info();

	std::vector<int> *unit_order = NULL;
	for (datablock::itor region = end(), block = blocks().begin(); block != end(); block++)
	{
		if (block->type() == block_type::TYPE_REGION)
			region = block;
		else if (block->type() == block_type::TYPE_UNIT)
		{
			if (block->valueInt(TYPE_FACTION) != factionId)
				continue;

			// add region to list when first unit of active faction occurs
			if (region != end())
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

				region = end();		// add region only before the first unit
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
			if (unit == end())
			{
				// no unit there is. problem we have.
				out << "  ; Einheit " << *uid << " (base10) nicht gefunden!\n";
				continue;
			}

			// search for command block of unit
			datablock::itor cmdb = unit;
			for (cmdb++; cmdb != end() && cmdb->depth() > unit->depth(); cmdb++)
				if (cmdb->type() == block_type::TYPE_COMMANDS)
					break;				// found

			if (cmdb == end() || cmdb->type() != block_type::TYPE_COMMANDS)
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
				for (items++; items != end() && items->depth() > unit->depth(); items++)
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
		return end();

	return region->second;
}
datablock::itor datafile::unit(int id)
{ 
	std::map<int, datablock::itor>::iterator unit = m_units.find(id);

	if (unit == m_units.end())
		return end();

	return unit->second;
}
datablock::itor datafile::faction(int id)
{
	std::map<int, datablock::itor>::iterator faction = m_factions.find(id);

	if (faction == m_factions.end())
		return end();

	return faction->second;
}
datablock::itor datafile::building(int id)
{ 
	std::map<int, datablock::itor>::iterator building = m_buildings.find(id);

	if (building == m_buildings.end())
		return end();

	return building->second;
}
datablock::itor datafile::ship(int id)
{ 
	std::map<int, datablock::itor>::iterator ship = m_ships.find(id);

	if (ship == m_ships.end())
		return end();

	return ship->second;
}

datablock::itor datafile::island(int id)
{ 
	std::map<int, datablock::itor>::iterator island = m_islands.find(id);

	if (island == m_islands.end())
		return end();

	return island->second;
}

datablock::itor datafile::end()
{
	return blocks().end();
}

datablock::itor datafile::dummyToItor(const datablock* block)
{
	for (datablock::itor itor = m_dummyBlocks.begin(); itor != m_dummyBlocks.end(); itor++)
		if (block == &*itor)
			return itor;

	return end();
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

	for (datablock::itor block = blocks().begin(); block != end(); block++)
	{
        block_type type = block->type();

		while (parents.size())
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
	m_regions.clear();
	m_units.clear();
	m_factions.clear();
	m_buildings.clear();
	m_ships.clear();
	m_islands.clear();
	m_dummyBlocks.clear();

	m_activefaction = end();
	m_recruitment = 0;
	m_turn = 0;

	datablock* region = NULL;
	int region_own = 0;
	int region_ally = 0;
	int region_enemy = 0;

	std::map<int, int> allied_status;	// what factions do we have HELP stati set to?
	std::set<int> unit_got_taxes;		// units that got taxes (MSG id 1264208711); the regions will get a coins icon
	/*
	MESSAGE <id>
	1264208711;type
	"$unit (zzz) treibt in $region (x,y) Steuern in H\u00f6he von $amount Silber ein.";rendered
	*/

	datablock::itor block;
	for (block = blocks().begin(); block != end(); block++)
	{
		// set turn number to that found in version block
		if (block->type() == block_type::TYPE_VERSION)
			m_turn = block->valueInt(TYPE_TURN, m_turn);

		// set faction as active faction (for ally-state)
		if (block->type() == block_type::TYPE_FACTION)
		{
			if (m_activefaction == end())
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
	if (activefaction() != end())
	{
		int factionDepth = activefaction()->depth();
		block = activefaction();
		for (block++; block != end() && block->depth() > factionDepth; block++)
		{
            if (block->type() == block_type::TYPE_ALLIANCE) {
                int status = block->valueInt("Status", 0);
                allied_status[block->info()] = status;
            }
		}
	}

	for (block = blocks().begin(); block != end(); block++)
	{
        // add region to region list
        if (block->type() == block_type::TYPE_REGION)
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
			if (activefaction() != end())
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
        if (block->type() == block_type::TYPE_SHIP)
			m_ships[block->info()] = block;
        else if (block->type() == block_type::TYPE_BUILDING)
			m_buildings[block->info()] = block;

		// generate list of units that got a "got taxes" message (E3 only)
        if (block->type() == block_type::TYPE_MESSAGE)
			if (block->value("type") == "1264208711")
			{
				//std::set<int> unit_got_taxes;
				int unitId = block->valueInt("unit", -1);
				unit_got_taxes.insert(unitId);
			}

        // add units to unit list
        if (block->type() == block_type::TYPE_UNIT)
		{
			m_units[block->info()] = block;

			int factionId = block->valueInt(TYPE_FACTION, -1);

			if (faction(factionId) == end())
			{
				datablock faction;
				faction.string("PARTEI");
				faction.infostr(FXStringVal(factionId));
				
				if (factionId == 0 || factionId == 666)	// Monster (0) and the new Monster (ii)
				{
					datakey key;
					key.key("Parteiname");
					key.value("Monster");
					faction.data().push_back(key);
				}

                m_dummyBlocks.push_back(faction);
				m_factions[factionId] = --m_dummyBlocks.end();
			}

			// set attachment for unit of active faction
			if (activefaction() != end() && factionId == activefaction()->info())
			{
				// search for command block of unit
				datablock::itor cmd = block;
				for (cmd++; cmd != end() && cmd->depth() > block->depth(); cmd++)
					if (cmd->type() == block_type::TYPE_COMMANDS)
						break;				// found

				// add att_commands to command block
				if (cmd != end() && cmd->type() == block_type::TYPE_COMMANDS && !cmd->attachment())
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
		if (block->type() == block_type::TYPE_FACTION)
			m_factions[block->info()] = block;
		// alliance as placeholder-faction
        if (block->type() == block_type::TYPE_ALLIANCE)
			if (faction(block->info()) == end())
				m_factions[block->info()] = block;
		// .. and islands to island list
		if (block->type() == block_type::TYPE_ISLAND)
			m_islands[block->info()] = block;

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

				if (activefaction() != end())
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
					|| block->value(TYPE_FACTION) == "")
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

	for (block = blocks().begin(); block != end(); block++)
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
				if (island != end())
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
			if (neighbour != end())
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
