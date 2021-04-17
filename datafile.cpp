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

#ifdef HAVE_BZ2LIB_H
#include <FXBZFileStream.h>
#endif

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

/*static*/ FXint datakey::parseType(const FXString& type)
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
	if (type == "ejcOrdersConfirmed")
		return TYPE_ORDERS_CONFIRMED;
	if (type == "charset")
		return TYPE_CHARSET;
	
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
	else if (type() == TYPE_ORDERS_CONFIRMED)
		return "ejcOrdersConfirmed";
	else if (type() == TYPE_CHARSET)
		return "charset";

	return "";
}

FXint datakey::getInt() const
{
	return strtol(m_value.text(), NULL, 10);
}

// parses str and returns created datakey object or NULL pointer
FXbool datakey::parse(FXchar* str)
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

	FXint length;
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
	
	value(val);						// Wert speichern

	return true;
}

// converts strings from file-charset (iso8859-15) to utf8
void datakey::iso2utf()
{
	m_key = ::iso2utf(m_key);
	m_value = ::iso2utf(m_value);
}

void datakey::utf2iso()
{
	m_key = ::utf2iso(m_key);
	m_value = ::utf2iso(m_value);
}

// =============================
// === datablock implementation

/*static*/ datablock::blocknames datablock::BLOCKNAMES[] =
{
	{ TYPE_VERSION, "VERSION" },
	{ TYPE_OPTIONS, "OPTIONEN" },
	{ TYPE_FACTION, "PARTEI" },
	{ TYPE_GROUP, "GRUPPE" },
	{ TYPE_ALLIANCE, "ALLIANZ" },
	{ TYPE_REGION, "REGION" },
	{ TYPE_ISLAND, "ISLAND" },
	{ TYPE_SCHEMEN, "SCHEMEN" },
	{ TYPE_RESOURCE, "RESOURCE" },
	{ TYPE_PRICES, "PREISE" },
	{ TYPE_DURCHREISE, "DURCHREISE" },
	{ TYPE_DURCHSCHIFFUNG, "DURCHSCHIFFUNG" },
	{ TYPE_BORDER, "GRENZE" },
	{ TYPE_BORDER, "BORDER" },
	{ TYPE_SHIP, "SCHIFF" },
	{ TYPE_BUILDING, "BURG" },
	{ TYPE_UNIT, "EINHEIT" },
	{ TYPE_UNITMESSAGES, "EINHEITSBOTSCHAFTEN" },
	{ TYPE_TALENTS, "TALENTE" },
	{ TYPE_SPELLS, "SPRUECHE" },
	{ TYPE_COMBATSPELL, "KAMPFZAUBER" },
	{ TYPE_ZAUBER, "ZAUBER" },
	{ TYPE_KOMPONENTEN, "KOMPONENTEN" },
	{ TYPE_TRANK, "TRANK" },
	{ TYPE_ZUTATEN, "ZUTATEN" },
	{ TYPE_ITEMS, "GEGENSTAENDE" },
	{ TYPE_COMMANDS, "COMMANDS" },
	{ TYPE_EFFECTS, "EFFECTS" },
	{ TYPE_MESSAGE, "MESSAGE" },
	{ TYPE_BATTLE, "BATTLE" },
	{ TYPE_MESSAGETYPE, "MESSAGETYPE" },
	{ TYPE_TRANSLATION, "TRANSLATION" },
	{ 0, NULL }
};

/*static*/ FXint datablock::parseType(const FXString& type)
{
	// region moved to top (performance)
	if (type == "REGION")
		return TYPE_REGION;

    for (int i = 0; BLOCKNAMES[i].name ; i++)
		if (type == BLOCKNAMES[i].name)
			return BLOCKNAMES[i].id;
	
	return TYPE_UNKNOWN;
}

const FXString datablock::terrainString() const
{
	const FXString type = value(datakey::TYPE_TERRAIN);
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

/*static*/ FXint datablock::parseTerrain(const FXString& terrain)
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

/*static*/ FXint datablock::parseSpecialTerrain(const FXString& terrain)
{
	// (terrain that uses image of another terrain)

	// this terrain types should be kept as text,
	// so if you resave the file, it's exact terrain type (_Aktiver_ Vulkan) 
	// could be saved.

	return data::TERRAIN_UNKNOWN;
}

/*static*/ FXString datablock::planeName(FXint plane)
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
FXbool datablock::parse(FXchar* str)
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

datablock::datablock() : m_type(0), m_info(0), m_x(0),m_y(0), m_terrain(0), m_flags(0), m_depth(0), m_attachment(0)
{
}

datablock::~datablock()
{
	delete m_attachment;
}

void datablock::string(const FXString& s)
{
	m_type = parseType(s);

	if (m_type == TYPE_UNKNOWN)
		m_string = s;
	else
		m_string.clear();
}

const FXString datablock::string() const
{
	if (m_type == TYPE_UNKNOWN)
		return m_string;

    for (int i = 0; BLOCKNAMES[i].name; i++)
		if (m_type == BLOCKNAMES[i].id)
			return BLOCKNAMES[i].name;

	return "";
}

// return info() as base36
FXString datablock::id() const
{
	FXchar buf[35], b36[] = "0123456789abcdefghijkLmnopqrstuvwxyz";
	FXchar *p=buf+34;
	FXint nn=(FXint)info();
	FXbool negativ = false;
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

void datablock::terrain(FXint terrain)
{
	m_terrain = terrain;
}

void datablock::flags(FXint flags)
{
	m_flags = flags;
}

void datablock::setFlags(FXint flags)
{
	m_flags |= flags;
}

void datablock::depth(FXint depth)
{
	m_depth = depth;
}

void datablock::attachment(::attachment* attach)
{
	delete m_attachment;		// free old attachment and set new
	m_attachment = attach;
}

const FXString datablock::value(const FXchar* key) const
{
	for(datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
		if (srch->key() == key)
			return srch->value();

	return "";
}

const FXString datablock::value(FXint key) const
{
	for(datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
		if (srch->type() == key)
			return srch->value();

	return "";
}

FXint datablock::valueInt(const FXchar* key, FXint def /* = 0 */) const
{
	for(datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
		if (srch->key() == key)
			return srch->getInt();

	return def;
}

FXint datablock::valueInt(FXint key, FXint def /* = 0 */) const
{
	for(datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
		if (srch->type() == key)
			return srch->getInt();

	return def;
}

const datakey* datablock::valueKey(FXint key) const
{
	for(datakey::list_type::const_iterator srch = m_data.begin(); srch != m_data.end(); srch++)
		if (srch->type() == key)
			return &*srch;

	return NULL;
}

void datablock::iso2utf()
{
	m_string = ::FXString(m_string);
}

void datablock::utf2iso()
{
	m_string = ::utf2iso(m_string);
}

// ===========================
// === datafile implementation

datafile::datafile() : m_cmds(), m_recruitment(0), m_turn(0), m_activefaction(end())
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

const FXchar* UTF8BOM = "\xEF\xBB\xBF";

// loads file, parses it and returns number of block
FXint datafile::load(const FXchar* filename)
{
	this->filename(filename);
	this->utf8cr(false);

	if (!filename)
		return 0;

	FXString data;

	FXString filename_str = filename;
	if (filename_str.length() >= 7 && filename_str.right(7) == ".cr.bz2")
	{
#ifdef HAVE_BZ2LIB_H
            // load bzip2 packed CR
		FXBZFileStream file;
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
#endif
	}
	else if (filename_str.length() >= 4 && filename_str.right(4) == ".xml")
	{
		this->filename("");
		throw std::runtime_error("Dateiformat nicht implementiert: XML.");
	}
	else
	{
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
	}

	// ...

	m_blocks.clear();
	datablock *block = NULL, newblock;
	datakey key;
	FXchar *ptr, *next = &data[0];

	// check for utf8 BOM: EF BB BF
	bool utf8mode = utf8cr();
	if (!strncmp(next, UTF8BOM, strlen(UTF8BOM)))
	{
		utf8cr(true);
		utf8mode = true;
        next += strlen(UTF8BOM);
	}

	while(*(ptr = next))
	{
		// strip the line
		next = getNextLine(ptr);

		// erster versuch: enth\u00e4lt die Zeile einen datakey?
		if (key.parse(ptr))
		{
			if (!utf8mode)
				key.iso2utf();

			if (block)
			{
				if (key.type() == datakey::TYPE_TERRAIN)
				{
					FXint terrain = datablock::parseTerrain(key.value());

					if (terrain == data::TERRAIN_UNKNOWN)
					{
						block->data().push_back(key);		// need textual representation of terrain type
					
						terrain = datablock::parseSpecialTerrain(key.value());
					}
					
					block->terrain(terrain);
				}
				else
					block->data().push_back(key);			// appends "Value";Key - tags

				// check if charset set
				if (key.type() == datakey::TYPE_CHARSET && block->type() == datablock::TYPE_VERSION)
				{
					FXString charset = key.value();
					charset.upper();

					if (charset == "UTF-8" || charset == "UTF8")
					{
						utf8cr(true);
						utf8mode = true;
					}
				}
			}
		}
		// zweiter versuch: enth\u00e4lt die Zeile einen datablock-header?
		else if (newblock.parse(ptr))
		{
			if (!utf8mode)
				newblock.iso2utf();

			m_blocks.push_back(newblock);		// appends BLOCK Info - tags
			block = &m_blocks.back();
		}
	}

	createHierarchy();		// set depth field of blocks
	createHashTables();		// creates hash tables and set region flags

	return blocks().size();
}

// saves file
FXint datafile::save(const FXchar* filename, bool replace, bool merge_commands /*= false*/)
{
	if (!filename)
		return 0;

	// Soll \u00fcberschrieben werden? Wenn nicht, pr\u00fcfen, ob Datei vorhanden
	if (replace == false)
	{
		FXFileStream filestr;
		filestr.open(filename, FXStreamLoad);
		if (filestr.status() == FXStreamOK)
		{
			filestr.close();
			return -2;		// Datei existiert schon, Fehler!
		}
	}

	// Datei zum Schreiben \u00f6ffnen
	std::ostringstream file;

	FXFileStream plainfile;
#ifdef HAVE_BZ2LIB_H
        FXBZFileStream bzip2file;
#endif
	FXStream *filestr_ptr;

	FXString filename_str = filename;
    if (filename_str.length() >= 4 && filename_str.right(4) == ".xml")
    {
        return -1;
    }
#ifdef HAVE_BZ2LIB_H
    else if (filename_str.length() >= 7 && filename_str.right(7) == ".cr.bz2")
    {
        bzip2file.open(filename, FXStreamSave);
		filestr_ptr = &bzip2file;
    }
#endif
	else
	{
		plainfile.open(filename, FXStreamSave);
		filestr_ptr = &plainfile;
	}

	FXStream& filestr = *filestr_ptr;
	if (filestr.status() != FXStreamOK)
	{
		return -1;
	}

	// Alles ok soweit...
	this->filename(filename);
	//this->utf8cr(false);

	const bool utf8mode = utf8cr();

	if (utf8mode)
		file << UTF8BOM;

	const datablock::itor end = blocks().end();
	for (datablock::itor block = blocks().begin(); block != end; block++)
	{
		bool hideKeys = false;

        // Blocknamen + ID-Nummern ausgeben
		file << block->string();

		// VERSION auf mindestens 64 hochsetzen
		if (block->type() == datablock::TYPE_VERSION)
			if (block->info() < 64)
				block->infostr("64");

		if (block->type() == datablock::TYPE_REGION || block->type() == datablock::TYPE_BATTLE ||
				block->x() != 0 || block->y() != 0)
			file << ' ' << block->x() << ' ' << block->y();
		
		if (block->type() == datablock::TYPE_COMBATSPELL || block->info())
			file << ' ' << block->info();

		file << std::endl;

		// Name-Tag und Terrain-Tag ausgeben
		if (block->type() == datablock::TYPE_REGION)
		{
			FXString name = block->value(datakey::TYPE_NAME);
			if (!utf8mode)
				name = utf2iso(name);
			if (!name.empty())
			{
				file << '\"';

				for (int i = 0; i < name.length(); i++)
				{
					FXchar c = name[i];

					if (c == '\\' || c == '\"')
						file << '\\';

					if (c == '\n')
						file << "\\n";
					else
						file << c;
				}

				file << "\";Name" << std::endl;
			}

			if (utf8mode)
				file << '\"' << block->terrainString().text() << "\";Terrain" << std::endl;
			else
				file << '\"' << block->terrainString() << "\";Terrain" << std::endl;
		}
		// Konfiguration-Block anpassen und Charset auf ISO-8859-1 setzen
		else if (block->type() == datablock::TYPE_VERSION)
		{
			bool charsettag = false;
			datakey charset;
			charset.key("charset");
			if (utf8mode)
				charset.value("UTF-8");
			else
				charset.value("ISO-8859-1");

			for (datakey::itor tags = block->data().begin(); tags != block->data().end(); tags++)
			{
				if (tags->type() == datakey::TYPE_KONFIGURATION)
				{
                    tags->value(CSMAP_APP_TITLE_VERSION);
				}
				if (tags->type() == datakey::TYPE_CHARSET)
				{
					tags->value(charset.value());
					charsettag = true;
				}
			}

            if (!charsettag)
				block->data().insert(block->data().begin(), charset);
		}
		else if (block->type() == datablock::TYPE_UNIT && merge_commands)
		{
			// search for command block of unit
			datablock::itor cmd = block;
			for (cmd++; cmd != end && cmd->depth() > block->depth(); cmd++)
				if (cmd->type() == datablock::TYPE_COMMANDS)
					break;				// found

			bool confirmed = block->valueInt(datakey::TYPE_ORDERS_CONFIRMED) != 0;

			if (cmd != end && cmd->type() == datablock::TYPE_COMMANDS)
			{
				// att_commands' confirmed attribute overwrites the tag
				if (att_commands* cmds = dynamic_cast<att_commands*>(cmd->attachment()))
					confirmed = cmds->confirmed;
			}

			if (confirmed)	// datakey::TYPE_ORDERS_CONFIRMED
				file << "1;ejcOrdersConfirmed" << std::endl;
		}
		else if (block->type() == datablock::TYPE_COMMANDS && merge_commands)
		{
			if (att_commands* cmds = dynamic_cast<att_commands*>(block->attachment()))
			{
				hideKeys = true;		// hide original commands

				for (att_commands::cmdlist_t::iterator it = cmds->commands.begin(); it != cmds->commands.end(); it++)
				{
					file << '\"';

					FXString value = *it;
					if (!utf8mode)
						value = utf2iso(value);


					/*
					\\ ist ein \, \" ist ein ", einzelen " sind Anfang und Ende 
					von Strings, \n ist ein Zeilenumbruch, (\x ist x, falls keine Sonderregel)
					*/
					for (int i = 0; i < value.length(); i++)
					{
						FXchar c = value[i];

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
			// ;Terrain ignorieren. Wird an ;Name angehangen... (s.u.)
			if (block->type() == datablock::TYPE_REGION)
			{
				if (tags->type() == datakey::TYPE_TERRAIN || tags->type() == datakey::TYPE_NAME)
					continue;
			}
			else if (block->type() == datablock::TYPE_UNIT && merge_commands)
			{
				if (tags->type() == datakey::TYPE_ORDERS_CONFIRMED)
					continue;		// will be set above
			}

			FXString value = tags->value();
			if (!utf8mode)
				value = utf2iso(value);

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
					FXchar c = value[i];

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
				if (!utf8mode)
					file << ';' << tags->key();	// als iso/latin1 ausgeben
				else
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
FXint datafile::loadCmds(const FXchar* filename)
{
	cmdfilename("");
	modifiedCmds(false);

	if (!filename)
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
	FXchar *ptr, *next = &data[0];

	// check for utf8 BOM: EF BB BF
	bool utf8mode = false;
	if (!strncmp(next, UTF8BOM, strlen(UTF8BOM)))
	{
		utf8mode = true;
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
		throw std::runtime_error("Keine g\u00fcltige Befehlsdatei.");

	if (!utf8mode)
		line = FXString(line);

	// parse header line:
	// ERESSEA ioen "PASSWORT"
	FXString id36 = line.section(' ', 1);
	FXString passwd = line.section(' ', 2);

	char* endptr;
	int factionId = strtol(id36.text(), &endptr, 36);

    if (endptr && *endptr)		// id36 string has to be consumed by strtol
		throw std::runtime_error(("Keine g\u00fcltige Parteinummer: " + id36).text());
	if (factionId != activefaction()->info())
		throw std::runtime_error(("Die Befehle sind f\u00fcr eine andere Partei: " + id36).text());
	if (passwd.length() < 2 || passwd.left(1) != "\"" || passwd.right(1) != "\"")
		throw std::runtime_error(("Das Passwort muss in Anf\u00fchrungszeichen gesetzt werden: " + passwd).text());

    // consider command file as correct, read in commands
	m_cmds.prefix_lines.clear();
	m_cmds.region_lines.clear();
	m_cmds.region_order.clear();
	password(passwd);

	FXString cmd;
	int headerindent = 0, indent = 0;

	// process lines
	while(*(ptr = next))
	{
		// strip the line
		next = getNextLine(ptr);

		// check if line is REGION or EINHEIT command
		line = ptr;
		if (!utf8mode)
			line = FXString(line);

		line.substitute("\t", "    ");
		indent = line.find_first_not_of(' ');
		cmd = line.trim().before(' ');
		cmd.upper();

		if (cmd == "REGION" || cmd == "EINHEIT")
			break;

		// add line to prefix
		m_cmds.prefix_lines.push_back(line);
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
				cmds_list = NULL;

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
				else
					throw std::runtime_error(("Einheit nicht gefunden: " + line).text());

				for (; block != end() && block->depth() > unitDepth; block++)
					if (block->type() == datablock::TYPE_COMMANDS)
						break;				// found

				if (block != end() && block->type() == datablock::TYPE_COMMANDS)
				{
					if (att_commands* cmds = dynamic_cast<att_commands*>(block->attachment()))
						cmds_list = cmds;
				}
				else
					throw std::runtime_error(("Einheit hat keinen Befehlsblock: " + line).text());
			}

			if (cmds_list)
			{
				cmds_list->confirmed = false;
				cmds_list->prefix_lines.clear();
				cmds_list->commands.clear();
				cmds_list->postfix_lines.clear();
				cmds_list->header = line;
			}
		}
		else if (cmds_list)
		{
			if (line.left(1) == ";")
			{
				cmd = line.after(';');
				cmd.trim().lower();
			}

			if (flatten(cmd) == "bestaetigt")
			{
				// don't add "; bestaetigt" comment, just set confirmed flag
				cmds_list->confirmed = true;
			}
			// when normal command or comment indented as command
			else if ((!line.empty() && line.left(1) != ";") || indent > headerindent)
			{
				// move all postfix comments into command block
				if (!cmds_list->postfix_lines.empty())
				{
					std::copy(cmds_list->postfix_lines.begin(), cmds_list->postfix_lines.end(),
							std::back_inserter(cmds_list->commands));
                    cmds_list->postfix_lines.clear();
				}

				indent -= headerindent + 2;
				while (indent >= 4) indent -= 4, line = "\t" + line;
				while (indent >= 1) indent -= 1, line = " " + line;

                // then append command
				cmds_list->commands.push_back(line);
			}
			// seems to be a comment
			else
			{
				// add to prefix if no other command is found,
				if (cmds_list->commands.empty())
					cmds_list->prefix_lines.push_back(line);
				else	// add to postfix if it follows some commands
					cmds_list->postfix_lines.push_back(line);
			}
		}

		if (!*(ptr = next))
			break;

		// strip the line
		next = getNextLine(ptr);

		// check if line is REGION oder EINHEIT command
		line = ptr;
		if (!utf8mode)
			line = FXString(line);

		line.substitute("\t", "    ");
		indent = line.find_first_not_of(' ');
		cmd = line.trim().before(' ');
		cmd.upper();

	} while (flatten(cmd) != "naechster");

	cmdfilename(filename);
	return true;
}

class LineCounter
{
public:
	LineCounter(std::ostream& target, int lineWidth) : m_target(target), m_width(lineWidth), m_stripped(false), m_row(0)
	{}

	~LineCounter() { flush(); }

	void stripped(bool strip) { m_stripped = strip; }
	unsigned row() const { return m_row; }

	template <typename TYPE>
	LineCounter& operator<<(const TYPE& data)
	{
		m_stream << data;
        *this << m_stream.str();
		m_stream.str("");
		return *this;
	}

	LineCounter& operator<<(const std::string& str)
	{
		for (size_t i = 0; i < str.size(); i++)
            *this << str[i];
		return *this;
	}

	LineCounter& operator<<(char c)
	{
		m_line += c;
        if (c == '\n')
		{
			flush();
			m_row++;
		}
		return *this;
	}

private:
	// don't call it by hand!
	void flush()
	{
		std::string indent;
		int width = m_width;

		bool linefeed = false, comment = false, stringlit = false;
		if (m_line.size() && m_line[m_line.size() - 1] == '\n')
		{
			m_line.erase(m_line.size() - 1, 1);
			linefeed = true;
		}

		size_t pos = m_line.find_first_not_of(" \t");
		if (pos > 0 || pos == std::string::npos)
		{
			indent = m_line.substr(0, pos);
			m_line.erase(0, pos);

			// strip off comments and indentation in stripped-mode
			if (m_stripped)
			{
				indent.clear();
				if (!m_line.empty() && m_line.substr(0, 1) == ";")
				{
					bool stripComment = true;

					size_t start = m_line.find_first_not_of("; \t"), end = std::string::npos;
					if (start != std::string::npos)
						end = m_line.find_first_of(" \t", start);
					if (end != std::string::npos && flatten(m_line.substr(start, end-start)) == "echeck")
							stripComment = false;

					if (stripComment)
						m_line.clear();
				}
			}
/*
			// replace 4 spaces by '\t'
			for (pos = indent.find_first_not_of(' '); (pos == std::string::npos) ? indent.size() > 3 : pos > 3; pos = indent.find_first_not_of(' '))
				indent.replace(0, 4, "\t");

			for (pos = indent.find('\t'); pos != std::string::npos; pos = indent.find('\t', pos+1))
				width -= 3;		// count tabs as 4 spaces
*/
			width -= indent.size();
		}

		if (width < 4)
		{
			indent.clear();
			width = 4;
		}

		size_t i = 0, size = m_line.size();

		if (!size && m_stripped)
			linefeed = false;

		while (i < size)
		{
            m_target << indent;
			int col = width - 2;

			if (comment)
				m_target << "; ", col-=2;
			
            for (; i < size && col > 0; i++, col--)
			{
				char c = m_line[i];
				m_target << c;

				if (c == ';' && !stringlit && !comment)
					comment = true;
				else if(c == '\"' && !comment)
					stringlit = !stringlit;
			}

			if (col <= 0)
			{
				// if we need a "line break backslash" and a space, print it
				if (!comment && (i+2 < size) && (m_line[i+1] == ' ' || m_line[i+1] == '\t'))
				{
					if (m_line[i] == ' ' || m_line[i] == '\t')
						m_target << m_line[i++];
					m_target << '\\';
				}
				else
				{
					if (i < size)
						m_target << m_line[i++];

					// if we need a "line break backslash", print it
					if (!comment && (i+1 < size))
						m_target << '\\';
					else if (i < size)	// else, print another char
						m_target << m_line[i++];
				}
			}

			if (i == 0 && !m_line.empty())		// when line width is to short, avoid endless loop
				throw std::runtime_error("Zeilenbreite ist zu klein.");

			if (i < size)
				m_target << '\n', m_row++;
		}

		m_line.clear();

		if (linefeed)
			m_target << '\n';
	}

private:
	LineCounter(const LineCounter&);
	void operator=(const LineCounter&);

	std::ostream& m_target;		// where to put the data
	unsigned m_width;			// how wide are the lines?
	bool m_stripped;			// strip off comments and indentation

	std::ostringstream m_stream;
	std::string m_line;			// current line
	unsigned m_row;				// current row
};

// saves command file
FXint datafile::saveCmds(const FXchar* filename, bool stripped, bool replace)
{
	if (!filename)
		return -1;

    if (activefaction() == end())
		return -1;

	// Soll \u00fcberschrieben werden? Wenn nicht, pr\u00fcfen, ob Datei vorhanden
	if (replace == false)
	{
		FXFileStream filestr;
		filestr.open(filename, FXStreamLoad);
		if (filestr.status() == FXStreamOK)
		{
			filestr.close();
			return -2;		// Datei existiert schon, Fehler!
		}
	}

	// Datei zum Schreiben \u00f6ffnen
	std::ofstream file(filename);
	if (!file.is_open())
		return -1;

	LineCounter out(file, 75);
	out.stripped(stripped);

	// Alles ok soweit...
	cmdfilename(filename);
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
	
	if (password().empty())	
		out << "\"HIER-PASSWORT\"" << "\n";
	else
		out << password() << "\n";
    
	// output prefix lines
	if (!m_cmds.prefix_lines.empty())
	{
		for (std::vector<FXString>::iterator itor = m_cmds.prefix_lines.begin(); itor != m_cmds.prefix_lines.end(); itor++)
			out << " " << *itor << "\n";
	}
	else
	{
		out << "\n";
		out << " ; ECHECK -l -w4 -r" << recruitment() << "\n";
		out << "\n";
		out << " ; " << CSMAP_APP_TITLE_VERSION << "\n";
		out << "\n";
	}

	// loop through regions and units and complete the ordering lists
	int factionId = activefaction()->info();

	std::vector<int> *unit_order = NULL;
	for (datablock::itor region = end(), block = blocks().begin(); block != end(); block++)
	{
		if (block->type() == datablock::TYPE_REGION)
			region = block;
		else if (block->type() == datablock::TYPE_UNIT)
		{
			if (block->valueInt(datakey::TYPE_FACTION) != factionId)
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

					cmds->header += " ; " + region->value(datakey::TYPE_NAME);
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

		out << " " << cmds->header << "\n";

		// output prefix lines
		for (att_commands::cmdlist_t::iterator itor = cmds->prefix_lines.begin(); itor != cmds->prefix_lines.end(); itor++)
			out << " " << *itor << "\n";

		// output changed commands
		for (att_commands::cmdlist_t::iterator itor = cmds->commands.begin(); itor != cmds->commands.end(); itor++)
			out << "  " << *itor << "\n";

		// output postfix lines
		for (att_commands::cmdlist_t::iterator itor = cmds->postfix_lines.begin(); itor != cmds->postfix_lines.end(); itor++)
			out << " " << *itor << "\n";

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
				if (cmdb->type() == datablock::TYPE_COMMANDS)
					break;				// found

			if (cmdb == end() || cmdb->type() != datablock::TYPE_COMMANDS)
			{
				out << "  ; Einheit " << unit->id() << " hat keinen Befehlsblock!\n";
				continue;
			}

			// unit has command block
			//  EINHEIT wz5t;  Botschafter des Konzils [1,146245$,Beqwx(1/3)] k\u00e4mpft nicht

			att_commands* attcmds = dynamic_cast<att_commands*>(cmdb->attachment());
			if (attcmds && !attcmds->header.empty())
				out << "  " << attcmds->header << "\n";
			else
			{
				// get amount of silber from GEGENSTAENDE block
				int silver = 0;
		
				datablock::itor items = unit;
				for (items++; items != end() && items->depth() > unit->depth(); items++)
					if (items->type() == datablock::TYPE_ITEMS)
					{
						silver = items->valueInt(datakey::TYPE_SILVER);
						break;
					}

				// output unit header
				out << "  EINHEIT " << unit->id();

				out << ";  " << unit->value(datakey::TYPE_NAME);
			
				out << " " << "[" << unit->valueInt(datakey::TYPE_NUMBER) << "," << silver << "$]";

				out << "\n";
			}

			//out << "  ; Debug: EINHEIT ist in Zeile: " << out.row() << "\n";

			// output attachment (changed) or default commands
			if (attcmds)
			{
				if (attcmds->confirmed)
					out << "  ; bestaetigt\n";

				// *** SOME DEBUGGING ***
				//bool EdEKaravelle = false;
				//bool EdETrireme = false;
				//if (regord->first.x == 3 && regord->first.y == -1 && regord->first.z == 0)
				//{
				//	FXint shipId = unit->valueInt("Schiff");
				//	if (shipId)
				//	{
				//		datablock::itor ship = this->ship(shipId);
				//		if (ship != end() && *uid == ship->valueInt("Kapitaen"))
				//		{
				//			if (ship->value("Typ") == "Karavelle")
				//				EdEKaravelle = true;
				//			else if (ship->value("Typ") == "Trireme")
				//				EdETrireme = true;

				//			//out << "  ; auf Schiff " << ship->value("Name") << " (" << ship->id() << ")\n";
				//		}
				//	}
				//}

				// output prefix lines
				for (att_commands::cmdlist_t::iterator itor = attcmds->prefix_lines.begin(); itor != attcmds->prefix_lines.end(); itor++)
					out << "  " << *itor << "\n";

				// output changed commands
				//if (EdEKaravelle)
				//{
				//	out << "  ; auf EdEKaravelle\n";
				//	out << "    @reserviere 3000 Silber\n";
				//}
				//else if (EdETrireme)
				//{
				//	out << "  ; auf EdETrireme\n";
				//	out << "    @reserviere 2000 Silber\n";
				//}
				//else
				{
					for (att_commands::cmdlist_t::iterator itor = attcmds->commands.begin(); itor != attcmds->commands.end(); itor++)
						out << "   " << *itor << "\n";
				}

				// output postfix lines
				for (att_commands::cmdlist_t::iterator itor = attcmds->postfix_lines.begin(); itor != attcmds->postfix_lines.end(); itor++)
					out << "  " << *itor << "\n";
			}		
			else
			{
				// output default commands
				datakey::list_type &list = cmdb->data();

				for (datakey::itor itor = list.begin(); itor != list.end(); itor++)
					out << " " << itor->value() << "\n";
			}
		}
	}

	out << " NAECHSTER\n";
	return true;
}

// block hash tables
// -----------------
datablock::itor datafile::region(FXint x, FXint y, FXint plane)
{ 
	std::map<koordinates, datablock::itor>::iterator region;
	region = m_regions.find(koordinates(x, y, plane));

	if (region == m_regions.end())
		return end();

	return region->second;
}
datablock::itor datafile::unit(FXint id)
{ 
	std::map<FXint, datablock::itor>::iterator unit = m_units.find(id);

	if (unit == m_units.end())
		return end();

	return unit->second;
}
datablock::itor datafile::faction(FXint id)
{
	std::map<FXint, datablock::itor>::iterator faction = m_factions.find(id);

	if (faction == m_factions.end())
		return end();

	return faction->second;
}
datablock::itor datafile::building(FXint id)
{ 
	std::map<FXint, datablock::itor>::iterator building = m_buildings.find(id);

	if (building == m_buildings.end())
		return end();

	return building->second;
}
datablock::itor datafile::ship(FXint id)
{ 
	std::map<FXint, datablock::itor>::iterator ship = m_ships.find(id);

	if (ship == m_ships.end())
		return end();

	return ship->second;
}

datablock::itor datafile::island(FXint id)
{ 
	std::map<FXint, datablock::itor>::iterator island = m_islands.find(id);

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
	typedef std::vector<int> stack;
	typedef std::set<int> tset;
	typedef std::map<int, tset> types_to_tset;

	types_to_tset enclosed;				// map of PARENT-BLOCK -> set of CHILD_BLOCKs
	
	// fill map
	for (int type = datablock::TYPE_UNKNOWN+1; type != datablock::TYPE_LAST; type++)
		enclosed[type].insert(datablock::TYPE_UNKNOWN);

	enclosed[datablock::TYPE_VERSION].insert(datablock::TYPE_BATTLE);
		enclosed[datablock::TYPE_BATTLE].insert(datablock::TYPE_MESSAGE);
	enclosed[datablock::TYPE_VERSION].insert(datablock::TYPE_MESSAGETYPE);
	enclosed[datablock::TYPE_VERSION].insert(datablock::TYPE_ISLAND);
	enclosed[datablock::TYPE_VERSION].insert(datablock::TYPE_TRANSLATION);
	enclosed[datablock::TYPE_VERSION].insert(datablock::TYPE_REGION);
		enclosed[datablock::TYPE_REGION].insert(datablock::TYPE_SCHEMEN);
		enclosed[datablock::TYPE_REGION].insert(datablock::TYPE_BORDER);
		enclosed[datablock::TYPE_REGION].insert(datablock::TYPE_RESOURCE);
		enclosed[datablock::TYPE_REGION].insert(datablock::TYPE_ITEMS);
		enclosed[datablock::TYPE_REGION].insert(datablock::TYPE_UNIT);
			enclosed[datablock::TYPE_UNIT].insert(datablock::TYPE_EFFECTS);
			enclosed[datablock::TYPE_UNIT].insert(datablock::TYPE_COMMANDS);
			enclosed[datablock::TYPE_UNIT].insert(datablock::TYPE_ITEMS);
			enclosed[datablock::TYPE_UNIT].insert(datablock::TYPE_SPELLS);
			enclosed[datablock::TYPE_UNIT].insert(datablock::TYPE_COMBATSPELL);
			enclosed[datablock::TYPE_UNIT].insert(datablock::TYPE_UNITMESSAGES);
			enclosed[datablock::TYPE_UNIT].insert(datablock::TYPE_TALENTS);
		enclosed[datablock::TYPE_REGION].insert(datablock::TYPE_BUILDING);
			enclosed[datablock::TYPE_BUILDING].insert(datablock::TYPE_EFFECTS);
		enclosed[datablock::TYPE_REGION].insert(datablock::TYPE_DURCHREISE);
		enclosed[datablock::TYPE_REGION].insert(datablock::TYPE_DURCHSCHIFFUNG);
		enclosed[datablock::TYPE_REGION].insert(datablock::TYPE_SHIP);
			enclosed[datablock::TYPE_SHIP].insert(datablock::TYPE_EFFECTS);
		enclosed[datablock::TYPE_REGION].insert(datablock::TYPE_PRICES);
		enclosed[datablock::TYPE_REGION].insert(datablock::TYPE_EFFECTS);
		enclosed[datablock::TYPE_REGION].insert(datablock::TYPE_MESSAGE);
	enclosed[datablock::TYPE_FACTION].insert(datablock::TYPE_ZAUBER);
		enclosed[datablock::TYPE_ZAUBER].insert(datablock::TYPE_KOMPONENTEN);
	enclosed[datablock::TYPE_VERSION].insert(datablock::TYPE_TRANK);
		enclosed[datablock::TYPE_TRANK].insert(datablock::TYPE_ZUTATEN);
	enclosed[datablock::TYPE_VERSION].insert(datablock::TYPE_FACTION);
		enclosed[datablock::TYPE_FACTION].insert(datablock::TYPE_GROUP);
			enclosed[datablock::TYPE_GROUP].insert(datablock::TYPE_ALLIANCE);
		enclosed[datablock::TYPE_FACTION].insert(datablock::TYPE_ALLIANCE);
		enclosed[datablock::TYPE_FACTION].insert(datablock::TYPE_BATTLE);
		enclosed[datablock::TYPE_FACTION].insert(datablock::TYPE_MESSAGE);
		enclosed[datablock::TYPE_FACTION].insert(datablock::TYPE_OPTIONS);
		enclosed[datablock::TYPE_FACTION].insert(datablock::TYPE_ITEMS);

	// build hierarchy
	stack parents;						// parent block types. size() == depth in hierarchy.

	for (datablock::itor block = blocks().begin(); block != end(); block++)
	{
		int type = block->type();

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

	std::map<int, bool> allied_status;	// what factions do we have HELP stati set to?
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
		if (block->type() == datablock::TYPE_VERSION)
			m_turn = block->valueInt(datakey::TYPE_TURN, m_turn);

		// set faction as active faction (for ally-state)
		if (block->type() == datablock::TYPE_FACTION)
		{
			if (m_activefaction == end())
				if (/*block->value(datakey::TYPE_TYPE) != "" ||*/ block->value(datakey::TYPE_OPTIONS) != "")
				{
					m_activefaction = block;		// set active faction here

					// get turn from faction block if VERSION block has none
					if (!m_turn)
						m_turn = block->valueInt(datakey::TYPE_TURN, m_turn);
					if (!m_recruitment)
						m_recruitment = block->valueInt(datakey::TYPE_RECRUITMENTCOST, m_recruitment);
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
			if (block->type() == datablock::TYPE_ALLIANCE)
				allied_status[block->info()] = true;
		}
	}

	for (block = blocks().begin(); block != end(); block++)
	{
        // add region to region list
        if (block->type() == datablock::TYPE_REGION)
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

			if (block->value(datakey::TYPE_VISIBILITY) == "lighthouse")
				region->setFlags(datablock::FLAG_LIGHTHOUSE);		// region is seen by lighthouse
			else if (block->value(datakey::TYPE_VISIBILITY) == "travel")
				region->setFlags(datablock::FLAG_TRAVEL);			// region is seen by traveling throu

			// put newest region into map
			if (m_regions.find(koordinates(block->x(), block->y(), block->info())) !=
					m_regions.end())
			{
				int region_turn = block->valueInt(datakey::TYPE_TURN, m_turn);

				datablock::itor oldregion = m_regions[koordinates(block->x(),block->y(), block->info())];

				int old_region_turn = oldregion->valueInt(datakey::TYPE_TURN, m_turn);

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
				else if (allied_status[ownerId])
					region->setFlags(datablock::FLAG_REGION_ALLY);
				else if (ownerId != -1)
					region->setFlags(datablock::FLAG_REGION_ENEMY);
			}
		}

        // add ships and buildings to their lists
        if (block->type() == datablock::TYPE_SHIP)
			m_ships[block->info()] = block;
        else if (block->type() == datablock::TYPE_BUILDING)
			m_buildings[block->info()] = block;

		// generate list of units that got a "got taxes" message (E3 only)
        if (block->type() == datablock::TYPE_MESSAGE)
			if (block->value("type") == "1264208711")
			{
				//std::set<int> unit_got_taxes;
				int unitId = block->valueInt("unit", -1);
				unit_got_taxes.insert(unitId);
			}

        // add units to unit list
        if (block->type() == datablock::TYPE_UNIT)
		{
			m_units[block->info()] = block;

			int factionId = block->valueInt(datakey::TYPE_FACTION, -1);

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
					if (cmd->type() == datablock::TYPE_COMMANDS)
						break;				// found

				// add att_commands to command block
				if (cmd != end() && cmd->type() == datablock::TYPE_COMMANDS && !cmd->attachment())
				{
					att_commands* cmds = new att_commands;
					cmd->attachment(cmds);
					
					cmds->confirmed = block->valueInt(datakey::TYPE_ORDERS_CONFIRMED) != 0;

					datakey::list_type &list = cmd->data();

					for (datakey::itor itor = list.begin(); itor != list.end(); itor++)
						cmds->commands.push_back(itor->value());
				}
			}
		}

		// .. and factions to faction list
		if (block->type() == datablock::TYPE_FACTION)
			m_factions[block->info()] = block;
		// alliance as placeholder-faction
        if (block->type() == datablock::TYPE_ALLIANCE)
			if (faction(block->info()) == end())
				m_factions[block->info()] = block;
		// .. and islands to island list
		if (block->type() == datablock::TYPE_ISLAND)
			m_islands[block->info()] = block;

		if (region)
		{
			if (block->type() == datablock::TYPE_UNIT)
			{
				region->setFlags(datablock::FLAG_TROOPS);			// region has units

				// count persons
				int number = block->valueInt(datakey::TYPE_NUMBER, 0);

				enum
				{
					UNIT_ENEMY,
					UNIT_OWN,
					UNIT_ALLY,
				} owner = UNIT_ENEMY;

				if (activefaction() != end())
				{
					int factionId = block->valueInt(datakey::TYPE_FACTION, -1);
					if (factionId == activefaction()->info())
					{
						region_own += number;
						owner = UNIT_OWN;
					}
					else if (allied_status[factionId])
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

				if (block->value(datakey::TYPE_FACTION) == "0")
					region->setFlags(datablock::FLAG_MONSTER);		// Monster (0) in region
				if (block->value(datakey::TYPE_FACTION) == "666")
					region->setFlags(datablock::FLAG_MONSTER);		// Monster (ii) in region
				if (block->valueInt(datakey::TYPE_FACTION) == 0 || block->valueInt(datakey::TYPE_FACTION) == 666
					|| block->value(datakey::TYPE_FACTION) == "")
				{
					if (block->value(datakey::TYPE_TYPE) == "Skelette" ||
							block->value(datakey::TYPE_TYPE) == "Skelettherren" ||
							block->value(datakey::TYPE_TYPE) == "Zombies" ||
							block->value(datakey::TYPE_TYPE) == "Juju-Zombies" ||
							block->value(datakey::TYPE_TYPE) == "Ghoule" ||
							block->value(datakey::TYPE_TYPE) == "Ghaste" ||
							block->value(datakey::TYPE_TYPE) == "Ents" ||
							block->value(datakey::TYPE_TYPE) == "Bauern" ||
							block->value(datakey::TYPE_TYPE) == FXString(L"Hirnt\u00f6ter"))
						region->setFlags(datablock::FLAG_MONSTER);		// monsters in the region
					else if (block->value(datakey::TYPE_TYPE) == "Seeschlangen")
						region->setFlags(datablock::FLAG_SEASNAKE);		// sea snake in region
					else if (block->value(datakey::TYPE_TYPE) == "Jungdrachen" ||
							block->value(datakey::TYPE_TYPE) == "Drachen" ||
							block->value(datakey::TYPE_TYPE) == "Wyrme")
						region->setFlags(datablock::FLAG_DRAGON);		// a dragon is in the region!
				}
			}
			else if (block->type() == datablock::TYPE_DURCHSCHIFFUNG)
				region->setFlags(datablock::FLAG_SHIPTRAVEL);		// region has travelled by ship
			else if (block->type() == datablock::TYPE_BUILDING)
			{
				region->setFlags(datablock::FLAG_CASTLE);			// region has a building
				
				if (block->value(datakey::TYPE_TYPE) == "Wurmloch")
					region->setFlags(datablock::FLAG_WORMHOLE);		// a wormhole is in the region!
			}
			else if (block->type() == datablock::TYPE_SHIP)
				region->setFlags(datablock::FLAG_SHIP);				// region has ships inside
			else if (block->type() == datablock::TYPE_BORDER)
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
        if (block->type() != datablock::TYPE_REGION)
			continue;
        
		// get regions name
		if (const datakey* islandkey = block->valueKey(datakey::TYPE_ISLAND))
		{
			FXString name;

			if (islandkey->isInt())		// Magellan-style: integer-Island-tags and ISLAND blocks with names
			{
				datablock::itor island = this->island(islandkey->getInt());
				if (island != end())
					name = island->value(datakey::TYPE_NAME);
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

		FXint x = (*itor)->x(), y = (*itor)->y(), z = (*itor)->info();

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
