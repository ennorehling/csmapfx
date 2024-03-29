#define _CRT_SECURE_NO_WARNINGS // for sscanf
#include "datafile.h"

#include "version.h"
#include "fxhelper.h"
#include "terrain.h"

#include <climits>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef _MSC_VER
#include <windows.h>
#include <stringapiset.h>
#endif

#define HELP_GUARD 16

datafile::datafile() : m_factionId(0), m_recruitment(0), m_turn(-1), m_activefaction(m_blocks.end())
{
	m_cmds.modified = false;
}

const char* UTF8BOM = "\xEF\xBB\xBF";

static bool skip_bom(std::ifstream& file)
{
    // skip BOM, if any
    for (int i = 0; i != 3; ++i) {
        int c = file.get();
        if ((char)c != UTF8BOM[i]) {
            file.seekg(0);
            return false;
        }
    }
    return true;
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

FXString datafile::getVersion()
{
    if (m_version.empty()) {
        datablock::citor block;
        for (block = m_blocks.begin(); block != m_blocks.end(); block++)
        {
            // set turn number to that found in version block
            if (block->type() == block_type::TYPE_VERSION) {
                m_version = block->value("Build");
                break;
            }
        }
    }
    return m_version;
}

int datafile::compareVersions(const FXString &lhs, const FXString &rhs)
{
    return strcmp(lhs.text(), rhs.text());
}

int datafile::turn()
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

int datafile::getFactionIdForUnit(const datablock* unit) const
{
    FXASSERT(unit->type() == block_type::TYPE_UNIT);
    if (unit->valueInt(TYPE_TRAITOR, 0) != 0) {
        return (int)special_faction::TRAITOR;
    }
    return unit->valueInt(TYPE_FACTION, (int)special_faction::ANONYMOUS);
}

FXString datafile::getFactionName(int factionId)
{
    FXString name;
    datablock::itor faction;
    if (factionId >= 0 && getFaction(faction, factionId)) {
        datablock* facPtr = &*faction;
        name = facPtr->value(TYPE_FACTIONNAME);
        if (facPtr->info() < 0)
        {
            if (name.empty()) {
                name.assign("Parteigetarnt");
            }
        }
        else
        {
            FXString fid = facPtr->id();
            if (name.empty()) {
                name.format("Partei %s (%s)", fid.text(), fid.text());
            }
            else {
                name += " (";
                name += fid;
                name += ')';
            }
        }
    }
    else {
        datablock block;
        block.infostr(FXStringVal(factionId));
        if (factionId == (int)special_faction::ANONYMOUS) {
            name.assign("Parteigetarnt");
        }
        else {
            name.assign(L"Verr\u00e4ter");
        }
    }
    return name;
}

datablock* datafile::getMessageTarget(const datablock& msg)
{
    int uid;
    datablock::itor select;
    uid = msg.getReference(block_type::TYPE_UNIT);
    if (uid > 0 && getUnit(select, uid)) {
        return &*select;
    }
    uid = msg.getReference(block_type::TYPE_SHIP);
    if (uid > 0 && getShip(select, uid)) {
        return &*select;
    }
    uid = msg.getReference(block_type::TYPE_BUILDING);
    if (uid > 0 && getBuilding(select, uid)) {
        return &*select;
    }
    FXString loc = msg.value("region");
    if (!loc.empty()) {
        int x, y, plane;
        x = FXIntVal(loc.section(' ', 0));
        y = FXIntVal(loc.section(' ', 1));
        plane = FXIntVal(loc.section(' ', 2));
        if (getRegion(select, x, y, plane)) {
            return &*select;
        }
    }
    return &*m_blocks.begin();
}

void datafile::openFile(const char* filename, std::ifstream& file, std::ios::openmode mode)
{
#ifdef _MSC_VER
    WCHAR pf[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, filename, -1, pf, MAX_PATH);
    std::wstring wname(pf);
    file.open(wname, mode);
#else
    file.open(filename, mode);
#endif
}
// loads file, parses it and returns number of block
bool datafile::load(const FXString& filename, FXString & outError)
{
    if (filename.empty()) {
        return false;
    }

    std::ifstream file;
    openFile(filename.text(), file);
	if (!file.is_open())
	{
		outError.assign(L"Datei konnte nicht ge\u00f6ffnet werden.");
        return false;
	}

    m_blocks.clear();
    
    datablock* block = NULL, newblock;
    datakey key;
    bool utf8 = true;
    skip_bom(file);
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line.back() == '\r') {
            line.pop_back();
        }
        const char* str = line.c_str();

        // erster versuch: enthaelt die Zeile einen datakey?
        if (key.parse(str, block ? block->type() : block_type::TYPE_UNKNOWN, utf8))
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
                        block->addKey(key);		// need textual representation of terrain type

                        terrain = datablock::parseSpecialTerrain(key.value());
                    }

                    block->terrain(terrain);
                    break;
                default:
                    block->addKey(key);			// appends "Value";Key - tags
                }
            }
        }
        // zweiter versuch: enthaelt die Zeile einen datablock-header?
        else if (newblock.parse(str))
        {
            m_blocks.push_back(newblock);		// appends BLOCK Info - tags
            block = &m_blocks.back();
        }
        else if (block) {
            /* fix for a bug introduced by version 1.2.7 */
            const char* semi = strchr(str, ';');
            if (semi) {
                key.key(FXString(semi + 1), block->type());
                key.value(FXString(str, semi - str));
                block->addKey(key);
            }
        }
    }

    if (m_blocks.empty())
    {
        outError.assign(L"Die Datei konnte nicht gelesen werden.\nM\u00f6glicherweise wird das Format nicht unterst\u00fctzt.");
        return false;
    }

    if (m_blocks.front().type() != block_type::TYPE_VERSION)
    {
        outError.assign("Die Datei hat das falsche Format.");
        return false;
    }
	return true;
}

// saves file
int datafile::save(const char* filename, map_type map_filter, std::set<datablock*> const * const selection)
{
	if (!filename)
		return 0;

	// open file for writing
	std::ostringstream file;

	FXFileStream filestr;

    filestr.open(filename, FXStreamSave);

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

        if (block->info() < 0)
        {
            // PARTEI -1 verhindern
            continue;
        }
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
            if (type == block_type::TYPE_REGION) {
                if (selection && !selection->empty()) {
                    datablock* region = &*block;
                    if (selection->find(region) == selection->end()) {
                        continue;
                    }
                }
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

		if (type == block_type::TYPE_REGION)
		{
            file << '\"' << block->terrainString().text() << "\";Terrain" << std::endl;
        }
		// Konfiguration-Block anpassen und Charset auf ISO-8859-1 setzen
		else if (type == block_type::TYPE_VERSION)
		{
            block->setKey(key_type::TYPE_KONFIGURATION, getConfigurationName(map_filter));
		}
		else if (type == block_type::TYPE_UNIT)
		{
			if (isConfirmed(*block))	// TYPE_ORDERS_CONFIRMED
				file << "1;ejcOrdersConfirmed" << std::endl;
		}
		else if (type == block_type::TYPE_COMMANDS)
		{
			if (att_commands* cmds = static_cast<att_commands*>(block->attachment()))
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
        datakey::list_type::const_iterator tags = block->data().begin();
        datakey::list_type::const_iterator iend = block->data().end();

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

void datafile::mergeBlock(datablock::itor& block, const datablock::itor& parent, datablock::itor& end)
{
    int parent_depth = parent->depth();
    block_type type = block->type();
    int info = block->info();
    datablock::itor insert = std::next(parent);
    bool found = false;
    for (datablock::itor child = insert; child != end; ++child) {
        if (child->depth() == parent_depth) {
            end = child;
            break;
        }
        if (child->type() == type) {
            if (insert->type() != type) {
                insert = child;
            }
            if (child->info() == info) {
                // we already have a newer report for this same block
                found = true;
                break;
            }
        }
    }
    if (!found) {
        // we do not have this kind of block
        m_blocks.insert(insert, *block);
    }
}

void datafile::findOffset(datafile* new_cr, int* x_offset, int* y_offset) const
{
    // first, create an index of all regions with an Id in this report:
    blockhash_t regionIndex;
    for (regions_map::const_iterator it = m_regions.begin(); it != m_regions.end(); ++it)
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
    for (auto &block : new_cr->m_blocks)
    {
        if (block.type() == block_type::TYPE_REGION) {
            int id = block.valueInt(TYPE_ID, -1);
            int plane = block.info();
            if (plane == 0) {
                blockhash_t::const_iterator match = regionIndex.find(id);
                if (match != regionIndex.end()) {
                    const datablock::itor &other = (*match).second;
                    /**
                     * NB: drifting icebergs move around the map and keep their ID!
                     **/
                    const datakey *k1 = block.valueKey(TYPE_TERRAIN);
                    const datakey *k2 = other->valueKey(TYPE_TERRAIN);
                    if (k1 && k2 && k1->value() == k2->value()) {
                        int xo = other->x() - block.x();
                        int yo = other->y() - block.y();
                        if (xo != 0 || yo != 0) {
                            if (x_offset) *x_offset = xo;
                            if (y_offset) *y_offset = yo;
                        }
                        return;
                    }
                }
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
    for (datablock::itor old_r = old_cr->m_blocks.begin(); old_r != old_end;)
    {
        // handle only regions
        if (old_r->type() == block_type::TYPE_REGION)
        {
            int x = old_r->x();
            int y = old_r->y();
            int plane = old_r->info();
            if (plane == 0) {
                x += x_offset;
                y += y_offset;
            }

            datablock::itor new_r;
            if (getRegion(new_r, x, y, old_r->info()))            // add some info to old cr (island names)
            {
                bool is_seen = !new_r->hasKey(TYPE_VISIBILITY);
                copy_children = false;
                if (const datakey* islandkey = old_r->valueKey(TYPE_ISLAND))
                {
                    if (!new_r->valueKey(TYPE_ISLAND))
                    {
                        if (islandkey && !islandkey->isInt())        // add only Vorlage-style islands (easier)
                            new_r->addKey(*islandkey);
                    }
                }
                if (!is_seen) {
                    // old region contained good data that we may want to keep
                    for (const datakey& key : old_r->data())
                    {
                        if (key.type() == TYPE_NAME || key.type() == TYPE_TERRAIN) {
                            continue;
                        }
                        if (!new_r->hasKey(key.type())) {
                            new_r->addKey(key);
                        }
                    }
                    // copy child blocks if we don't have them
                    int depth = old_r->depth();
                    datablock::itor new_end = m_blocks.end();
                    for (old_r++; old_r != old_end && old_r->type() != block_type::TYPE_REGION; ++old_r)
                    {
                        if (old_r->depth() == depth + 1) {
                            block_type type = old_r->type();
                            if (!datafile::isEphemeral(type)) {
                                mergeBlock(old_r, new_r, new_end);
                            }
                        }
                    }
                    if (old_r == old_end) {
                        break;
                    }
                    if (old_r->type() != block_type::TYPE_REGION)
                    {
                        ++old_r;
                    }
                }
                else {
                    ++old_r;
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
			default:
				break;
            }
        }
        ++old_r;
    }
    createHashTables(); 
    if (old_cr->getActiveFactionId() != getActiveFactionId()) {
        if (m_password.empty()) {
            m_password = old_cr->m_password;
        }
    }
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
FXString decodeLine(const std::string& line, bool& checkUTF8, bool& utf8)
{
    // check if line is REGION or EINHEIT command
    if (checkUTF8) {
        if (!isUTF8(line.c_str(), line.length())) {
            checkUTF8 = false;
            utf8 = false;
        }
    }
    if (!utf8) {
        return iso2utf(line.c_str(), line.length());
    }
    return FXString(line.c_str());
}

static FXString regionCommand(const datablock &region)
{
    FXString result = "REGION " + FXStringVal(region.x()) + "," + FXStringVal(region.y());
    if (region.info()) {
        result += "," + FXStringVal(region.info());
    }
    result += " ; " + region.value(TYPE_NAME);
    result += " (" + region.terrainString() + ")";
    return result;
}

// loads command file and attaches the commands to the units
int datafile::loadCmds(const FXString& filename)
{
    bool utf8 = true, checkUTF8 = true; // file might not be UTF8
    if (filename.empty())
        return 0;

    if (m_factionId == 0)
        throw std::runtime_error("Kein Report geladen, kann Befehlsdatei nicht einlesen.");


    // load plain text file
    std::ifstream file;
    openFile(filename.text(), file);
    if (!file.is_open()) {
        throw std::runtime_error(FXString(L"Datei konnte nicht ge\u00f6ffnet werden.").text());
    }
    if (skip_bom(file)) {
        checkUTF8 = false;
        utf8 = true;
    }
    std::string line;
    while (std::getline(file, line)) 
    {
        if (line.empty()) continue;
        if (line.back() == '\r') {
            line.pop_back();
        }
        const char* ptr = line.c_str();
        // skip indentation
        while (*ptr && isspace(*ptr))
            ptr++;

        // when not empty, break
        if (*ptr && *ptr != ';') {
            // found first non-empty line
            FXString header(ptr);
            if (header.section(' ', 0) != "ERESSEA" && header.section(' ', 0) != "PARTEI") {
                throw std::runtime_error(FXString(L"Keine g\u00fcltige Befehlsdatei.").text());
            }
            // parse header line:
            // ERESSEA ioen "PASSWORT"
            FXString id36 = header.section(' ', 1);
            char* endptr = nullptr;
            int factionId = strtol(id36.text(), &endptr, 36);

            if (endptr && *endptr)		// id36 string has to be consumed by strtol
                throw std::runtime_error((L"Keine g\u00fcltige Parteinummer: " + id36).text());
            if (factionId != m_factionId)
                throw std::runtime_error((L"Die Befehle sind f\u00fcr eine andere Partei: " + id36).text());

            FXString password = header.section(' ', 2);

            FXint pos = password.find('"', 0);
            while (pos >= 0) {
                password.erase(pos);
                pos = password.find('"', pos);
            }
            if (!password.empty()) {
                m_password = password;
            }
            break;
        }
    }

    // consider command file as correct, read in commands
	m_cmds.prefix_lines.clear();
	m_cmds.region_lines.clear();

    int headerindent = 0, indent = 0;
	// process lines
    FXString cmd, str;
	while(std::getline(file, line)) {
        if (line.empty()) continue;
        if (line.back() == '\r') {
            line.pop_back();
        }
        // strip the line
        if (!line.empty()) {
            str = decodeLine(line, checkUTF8, utf8);
            indent = str.find_first_not_of("\t ");
            if (!str.trim().empty()) {}
            cmd = str.before(' ');
            cmd.upper();

            if (cmd == "REGION" || cmd == "EINHEIT") {
                break;
            }
            // add line to prefix
            m_cmds.prefix_lines.push_back(str);
        }
    }

	// check for end of file:
    if (!file.good())
		throw std::runtime_error("Keine Befehle gefunden.");

	att_commands region_list;
	att_commands* cmds_list = NULL;
	std::vector<int> unit_order;
    datablock::itor block, region = m_blocks.end();
    int unitId = 0;

    while (file.good())
    {
        if (cmd == "REGION" || cmd == "EINHEIT")
        {
            FXString param = str.section(' ', 1);
            headerindent = indent;

            if (cmd == "EINHEIT")
            {
                unitId = strtol(param.text(), nullptr, 36);

                if (!getUnit(block, unitId))
                {
                    throw std::runtime_error(("Einheit nicht gefunden: " + str).text());
                }
                if (region != m_blocks.end()) {
                    Coordinates coor(region->x(), region->y(), region->info());
                    if (!hasChild(region, block)) {
                        if (!getParent(region, block)) {
                            throw std::runtime_error(("Einheit in falscher Region: " + str).text());
                        }
                        coor = Coordinates(region->x(), region->y(), region->info());
                        region_list.header = regionCommand(*region);
                    }
                    m_cmds.region_lines[coor] = region_list;
                    region_list.clear();

                    // add to order list for units of this region
                    unit_order.push_back(unitId);
                    region = m_blocks.end();
                }
                setConfirmed(block, false); // TODO: cumbersome, but reading orders without a `; bestaetigt` comment must do this.
                cmds_list = nullptr;
                datablock::itor cmdb;
                if (getCommands(cmdb, block)) {
                    if (att_commands* cmds = static_cast<att_commands*>(cmdb->attachment())) {
                        cmds_list = cmds;
                    }
                }

                if (!cmds_list) {
                    throw std::runtime_error(("Einheit hat keinen Befehlsblock: " + str).text());
                }
            }
            else {
                unitId = 0;
                if (cmd == "REGION")
                {
                    FXString xstr = param.section(',', 0);
                    FXString ystr = param.section(',', 1);
                    FXString zstr = param.section(',', 2);

                    int x = strtol(xstr.text(), nullptr, 10);
                    int y = strtol(ystr.text(), nullptr, 10);
                    int z = strtol(zstr.text(), nullptr, 10);

                    if (!getRegion(region, x, y, z))
                    {
                        throw std::runtime_error(("Region nicht gefunden: " + param).text());
                    }
                    unit_order.clear();
                    cmds_list = &region_list;
                }
            }
            if (cmds_list) {
                cmds_list->prefix_lines.clear();
                cmds_list->commands.clear();
                cmds_list->postfix_lines.clear();
                cmds_list->header = str;
            }
        }
        else if (cmds_list)
        {
            if (!str.empty()) {
                if (str.left(1) == ";") {
                    cmd = str.section(' ', 1);
                    if (unitId != 0 && flatten(cmd.lower()) == "bestaetigt") {
                        // don't add "; bestaetigt" comment, just set confirmed flag
                        setConfirmed(block, true);
                    }
                    else if (indent > headerindent) {
                        cmds_list->addCommand(str);
                    }
                    else if (cmds_list->commands.empty()) {
                        cmds_list->prefix_lines.push_back(str);
                    }
                    else {
                        // add to postfix if it follows some commands
                        cmds_list->postfix_lines.push_back(str);
                    }
                }
                else {
                    cmds_list->addCommand(str);
                }
            }
        }

        // strip the line
        if (!std::getline(file, line)) {
            break;
        }
        if (!line.empty()) {
            if (line.back() == '\r') {
                line.pop_back();
            }
        }
        // check if line is REGION oder EINHEIT command
        str = decodeLine(line, checkUTF8, utf8);
        indent = str.find_first_not_of("\t ");
        cmd = str.trim().before(' ');
        cmd.upper();
        if (flatten(cmd) == "naechster") break;
    }
    cmdfilename(filename);
    modifiedCmds(false);
    return true;
}

void datafile::writeCmds(std::ostream &out, const att_commands *cmds)
{
    out << cmds->header.text() << "\n";

    // output prefix lines
    for (const FXString &itor : cmds->prefix_lines)
        out << itor.text() << "\n";

    // output changed commands
    for (const FXString &itor : cmds->commands)
        out << "    " << itor.text() << "\n";

    // output postfix lines
    out << "\n";
    for (const FXString &itor : cmds->postfix_lines)
        out << itor.text() << "\n";

}

// saves command file
int datafile::saveCmds(const FXString& filename, const FXString& password, bool stripped)
{
	if (filename.empty())
		return -1;

    if (m_factionId == 0)
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
	
	out << " " << FXStringValEx(m_factionId, 36) << " ";
	
	out << "\"" << password << "\"\n";
    
	// output prefix lines
	if (!stripped && !m_cmds.prefix_lines.empty())
	{
		for (const FXString& line : m_cmds.prefix_lines)
			out << line.text() << "\n";
	}
	else
	{
		out << "\n";
		out << "; ECHECK -v4.7 -l -w3 -r" << recruitment() << "\n";
		out << "\n";
		out << "; " << CSMAP_APP_TITLE_VERSION << "\n";
		out << "\n";
	}

	std::vector<int> *unit_order = nullptr;
	for (datablock::itor region = m_blocks.end(), block = m_blocks.begin(); block != m_blocks.end(); block++)
	{
        if (block->type() == block_type::TYPE_REGION)
        {
            region = block;
        }
        else if (block->type() == block_type::TYPE_UNIT)
		{
			if (block->valueInt(TYPE_FACTION) != m_factionId)
				continue;

            datablock::itor cmdb;
            if (!getCommands(cmdb, block))
            {
                out << "  ; Einheit " << block->id() << " hat keinen Befehlsblock!\n";
                continue;
            }

            if (region != m_blocks.end()) {
                Coordinates koord(region->x(), region->y(), region->info());
                auto it = m_cmds.region_lines.find(koord);
                if (it != m_cmds.region_lines.end())
                {
                    att_commands &cmds = (*it).second;
                    writeCmds(out, &cmds);
                }
                else {
                    // did not write a REGION header yet
                    out << regionCommand(*region) << std::endl;

                    if (int salary = region->valueInt("Lohn")) {
                        out << "; ECheck Lohn " << salary << std::endl << std::endl;
                    }
                }
                region = m_blocks.end();
            }

            // unit has command block
            //  EINHEIT wz5t;  Botschafter des Konzils [1,146245$,Beqwx(1/3)] kaempft nicht

            att_commands *attcmds = static_cast<att_commands *>(cmdb->attachment());
            if (attcmds && !attcmds->header.empty())
                out << attcmds->header.text() << "\n";
            else
            {
                // get amount of silber from GEGENSTAENDE block
                int silver = 0;

                datablock::itor items = block;
                for (items++; items != m_blocks.end() && items->depth() > block->depth(); items++)
                    if (items->type() == block_type::TYPE_ITEMS)
                    {
                        silver = items->valueInt(TYPE_SILVER);
                        break;
                    }

                // output unit header
                out << "EINHEIT " << block->id();

                out << ";  " << block->getName();

                out << " " << "[" << block->valueInt(TYPE_NUMBER) << "," << silver << "$]";

                out << "\n";
            }

            // output attachment (changed) or default commands
            if (attcmds)
            {
                if (isConfirmed(*block))
                    out << "; bestaetigt\n";

                // output prefix lines
                for (const FXString &itor : attcmds->prefix_lines)
                    out << itor.text() << "\n";

                // output changed commands
                {
                    for (const FXString &itor : attcmds->commands)
                        out << "    " << itor.text() << "\n";
                }

                out << "\n";
                // output postfix lines
                for (const FXString &itor : attcmds->postfix_lines)
                    out << itor.text() << "\n";
            }
            else
            {
                // output default commands
                const datakey::list_type &list = cmdb->data();

                for (const datakey &itor : list)
                    out << "    " << itor.value() << "\n";
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
	regions_map::const_iterator region;
	region = m_regions.find(Coordinates(x, y, plane));

	if (region == m_regions.end())
		return m_blocks.end();

	return region->second;
}

datablock::itor datafile::battle(int x, int y, int plane)
{ 
	regions_map::const_iterator block;
    block = m_battles.find(Coordinates(x, y, plane));

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
    regions_map::const_iterator block;
    block  = m_battles.find(Coordinates(x, y, plane));
    return (block != m_battles.end());
}

bool datafile::hasRegion(int x, int y, int plane) const
{
    regions_map::const_iterator region;
    region = m_regions.find(Coordinates(x, y, plane));
    return (region != m_regions.end());
}

void datafile::addRegion(const datablock& block)
{
    Coordinates coor(block.x(), block.y(), block.info());
    datablock::itor back = m_blocks.insert(m_blocks.end(), block);
    m_regions.insert(std::make_pair(coor, back));
}

bool datafile::deleteRegion(const datablock& block)
{
    datablock::itor first;
    if (!getRegion(first, block)) {
        return false;
    }
    eraseRegion(first);
    return true;
}

void datafile::deleteRegions(std::set<datablock*>& regions)
{
    std::set<datablock*>::iterator itor;
    for (itor = regions.begin(); itor != regions.end(); itor++)
    {
        datablock* region = *itor;
        // delete this region
        deleteRegion(*region);
    }
    createHashTables();
}

datablock::itor datafile::group(int id)
{ 
    blockhash_t::const_iterator unit = m_groups.find(id);

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
    blockhash_t::const_iterator unit = m_units.find(id);

	if (unit == m_units.end()) {
        return m_blocks.end();
    }

	return unit->second;
}

bool datafile::getFirst(datablock::itor& out, block_type type)
{
    for (datablock::itor it = m_blocks.begin(); it != m_blocks.end(); ++it)
    {
        if (it->type() == type) {
            out = it;
            return true;
        }
    }
    return false;
}

bool datafile::getNext(datablock::itor& iter, block_type type)
{
    for (datablock::itor it = std::next(iter); it != m_blocks.end(); ++it)
    {
        if (type == it->type()) {
            iter = it;
            return true;
        }
        if (it->depth() < iter->depth()) {
            break;
        }
    }
    return false;
}

bool datafile::getParent(datablock::itor& out, const datablock::itor& child)
{
    for (datablock::itor parent = child; parent != m_blocks.begin(); --parent) {
        if (parent->depth() < child->depth()) {
            out = parent;
            return true;
        }
    }
    return false;
}

bool datafile::hasChild(const datablock::itor &parent, const datablock::itor &child)
{
    int depth = parent->depth();
    for (datablock::itor itor = std::next(parent); itor != m_blocks.end() && itor->depth() > depth; ++itor)
    {
        if (itor == child) {
            return true;
        }
    }
    return false;
}

bool datafile::getChild(datablock::itor& out, const datablock::itor& parent, block_type type)
{
    int depth = parent->depth();
    for (datablock::itor itor = std::next(parent); itor != m_blocks.end() && itor->depth() > depth; ++itor)
    {
        if (itor->type() == type) {
            out = itor;
            return true;
        }
    }
    return false;
}

bool datafile::getCommands(datablock::itor& out, const datablock::itor& unit)
{
    FXASSERT(unit->type() == block_type::TYPE_UNIT);
    return getChild(out, unit, block_type::TYPE_COMMANDS);
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
    blockhash_t::const_iterator faction = m_factions.find(id);

	if (faction == m_factions.end())
		return m_blocks.end();

	return faction->second;
}

bool datafile::getFaction(datablock::itor& out, int id)
{
    blockhash_t::const_iterator faction = m_factions.find(id);
    if (faction != m_factions.end()) {
        out = faction->second;
        return true;
    }
    return false;
}

bool datafile::hasFaction(int id)
{
    blockhash_t::const_iterator faction = m_factions.find(id);
    return faction != m_factions.end();
}

datablock::itor datafile::building(int id)
{
    blockhash_t::const_iterator building = m_buildings.find(id);

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
    blockhash_t::const_iterator ship = m_ships.find(id);

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
    blockhash_t::const_iterator island = m_islands.find(id);

	if (island == m_islands.end())
		return m_blocks.end();

	return island->second;
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

FXString datafile::unitName(const datablock& unit, bool verbose)
{
    FXASSERT(unit.type() == block_type::TYPE_UNIT);
    if (verbose) {
        FXString label;
        int uid = unit.info();
        int fid = unit.valueInt(TYPE_FACTION);
        datablock::itor faction_owner;
        if (getFaction(faction_owner, fid)) {
            label.format("%s (%s), %s (%s)",
                unit.value(TYPE_NAME).text(),
                FXStringValEx(uid, 36).text(),
                faction_owner->value(TYPE_FACTIONNAME).text(),
                FXStringValEx(fid, 36).text()
            );
        }
        else {
            label.format("%s (%s), Unbekannt (%s)",
                unit.value(TYPE_NAME).text(),
                FXStringValEx(uid, 36).text(),
                FXStringValEx(fid, 36).text()
            );
        }
        return label;
    }
    return unit.getName();
}

FXString datafile::regionCoordinates(const datablock& block)
{
    FXASSERT(block.type() == block_type::TYPE_REGION);
    FXString coor;
    coor.format("%d,%d", block.x(), block.y());
    return coor;
}

void datafile::setConfirmed(datablock::itor& unit, bool confirmed)
{
    FXASSERT(unit->type() == block_type::TYPE_UNIT);

    // TODO: two key-searches, possible optimization
    if (confirmed != isConfirmed(*unit))
    {
        att_region* stats = nullptr;
        datablock::itor region;
        if (getParent(region, unit)) {
            stats = static_cast<att_region*>(region->attachment());
        }
        if (confirmed) {
            unit->setKey(TYPE_ORDERS_CONFIRMED, 1);
            if (stats) --stats->unconfirmed;
        }
        else {
            unit->removeKey(TYPE_ORDERS_CONFIRMED);
            if (stats) ++stats->unconfirmed;
        }
    }
}

bool datafile::isConfirmed(const datablock& block) const
{
    if (block.type() == block_type::TYPE_REGION) {
        if (att_region* stats = static_cast<att_region*>(block.attachment())) {
            return stats->unconfirmed == 0;
        }
    }
    else if (block.type() == block_type::TYPE_UNIT) {
        if (block.valueInt(TYPE_FACTION) == getActiveFactionId()) {
            return block.valueInt(TYPE_ORDERS_CONFIRMED) != 0;
        }
    }
    return true;
}

const FXRectangle& datafile::getContentSize(int visiblePlane)
{
    auto it = contentSizes.find(visiblePlane);
    if (it != contentSizes.end()) {
        return (*it).second;
    }
    // auf 'unmoegliche' Werte initialisieren
    FXshort min_x = SHRT_MAX;
    FXshort max_x = SHRT_MIN;

    FXshort min_y = SHRT_MAX;
    FXshort max_y = SHRT_MIN;

    for (const datablock &block : m_blocks)
    {
        // handle only regions
        if (block.type() != block_type::TYPE_REGION)
            continue;

        // handle only the actually visible plane
        if (block.info() != visiblePlane)
            continue;

        FXint scr_x = block.x() * 2 + block.y();
        FXint scr_y = -block.y();

        if (scr_x < min_x)	min_x = scr_x;
        if (scr_y < min_y)	min_y = scr_y;

        if (scr_x + 2 > max_x) max_x = scr_x + 2;
        if (scr_y + 1 > max_y)	max_y = scr_y + 1;
    }

    auto ins = contentSizes.insert(std::map<int, FXRectangle>::value_type(visiblePlane, FXRectangle{
        min_x, min_y,
        FXshort(max_x - min_x),
        FXshort(max_y - min_y) }));
    return (*ins.first).second;
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

static int barHeight2(int people) {
    int log = static_cast<int>(log2(people * 4 + 1));
    return log;
}

void datafile::floodIslandNames()
{
    // islands
    std::list<datablock *> floodislands;		// regions whose island names flood the island

    for (auto& it : m_regions)
    {
        datablock * block = &*it.second;
        if (block->type() != block_type::TYPE_REGION)
            continue;

        // get regions name
        if (const datakey* islandkey = block->valueKey(TYPE_ISLAND))
        {
            FXString name;
            FXint islandId = FXIntVal(islandkey->value());

            if (islandId > 0)		// Magellan-style: integer-Island-tags and ISLAND blocks with names
            {
                datablock::itor island = this->island(islandId);
                if (island != m_blocks.end()) {
                    name = island->value(TYPE_NAME);
                }
            }
            else						// Vorlage-style: string-Island-tags that flood the island
            {
                name = islandkey->value();

                floodislands.push_back(block);		// Vorlage-style floods the islands
            }

            if (!name.empty()) {
                att_region* stats = static_cast<att_region*>(block->attachment());
                if (!stats) {
                    stats = new att_region;
                    block->attachment(stats);
                }

                stats->island = name;
            }
        }
    }

    // flood island names. add regions that get a name to list so that they also flood.
    int offsets[][2] = { {0,1}, {0,-1}, {1,0}, {-1,0}, {1,-1}, {-1,1} };

    for (datablock* block : floodislands)
    {
        FXString name;
        if (att_region* stats = static_cast<att_region*>(block->attachment()))
            name = stats->island;

        int x = block->x(), y = block->y(), z = block->info();

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

                att_region* stats = static_cast<att_region*>(neighbour->attachment());
                if (!stats) {
                    stats = new att_region;
                    neighbour->attachment(stats);
                }

                if (stats->island.empty()) {
                    stats->island = name;

                    floodislands.push_back(&*neighbour);
                }
            }
        }
    }
}

void datafile::createIslands()
{
    m_islands.clear();
    for(datablock::itor block = m_blocks.begin(); block != m_blocks.end(); ++block) {
        if (block->type() == block_type::TYPE_ISLAND) {
            m_islands[block->info()] = block;
        }
    }
    floodIslandNames();
}

bool datafile::isEphemeral(block_type type)
{
    switch (type) {
    case block_type::TYPE_UNIT:
    case block_type::TYPE_DURCHREISE:
    case block_type::TYPE_DURCHSCHIFFUNG:
    case block_type::TYPE_SHIP:
    case block_type::TYPE_MESSAGE:
        return true;
    default:
        return false;
    }
}
datablock::itor datafile::eraseRegion(const datablock::itor& first)
{
    // delete all blocks until next region.
    datablock::itor last = std::next(first);
    while (last != m_blocks.end()) {
        // block is in region?
        if (last->depth() <= first->depth()) {
            return eraseBlocks(first, last);
        }
        ++last;
    }
    return eraseBlocks(first, m_blocks.end());
}

datablock::itor datafile::eraseBlocks(const datablock::itor& begin, const datablock::itor& end)
{
    for (datablock::itor block = begin; block != end; ++block) {
        if (block->type() == block_type::TYPE_REGION) {
            Coordinates coor(block->x(), block->y(), block->info());
            regions_map::const_iterator it = m_regions.find(coor);
            if (it != m_regions.end()) {
                m_regions.erase(it);
            }
        }
    }
    return m_blocks.erase(begin, end);
}

static void setRegionStats(datablock *regionPtr, int region_own, int region_ally, int region_enemy, int unconfirmed)
{
    regionPtr->setFlags(region_own ? datablock::FLAG_REGION_SEEN : 0);
    int own_log = barHeight2(region_own);
    int ally_log = barHeight2(region_ally);
    int enemy_log = barHeight2(region_enemy);

    // generate new style flag information. log_2(people) = 1 to 13
    if (own_log || ally_log || enemy_log || unconfirmed)
    {
        att_region *stats = new att_region;
        regionPtr->attachment(stats);
        stats->unconfirmed = unconfirmed;
        stats->people.reserve(enemy_log ? 3 : 2);
        stats->people.push_back(own_log / 13.0f);
        stats->people.push_back(ally_log / 13.0f);
        if (enemy_log) {
            stats->people.push_back(enemy_log / 13.0f);
        }
    }
}

void datafile::updateHashTables(const datablock::itor& start)
{
    datablock::itor block;
    datablock* regionPtr = NULL;
    int unconfirmed = 0;
    int region_own = 0;
    int region_ally = 0;
    int region_enemy = 0;

    m_activefaction = m_blocks.end();
    m_factionId = 0;
    m_recruitment = 0;
    m_turn = 0;

    datablock::itor insertFaction = m_blocks.end();
    for (block = m_blocks.begin(); block != m_blocks.end(); block++)
    {
        // set turn number to that found in version block
        if (block->type() == block_type::TYPE_VERSION)
            m_turn = block->valueInt(TYPE_TURN, m_turn);

        // set faction as active faction (for ally-state)
        else if (block->type() == block_type::TYPE_FACTION)
        {
            if (m_factionId == 0) {
                if (/*block->value(TYPE_TYPE) != "" ||*/ block->value(TYPE_OPTIONS) != "")
                {
                    m_activefaction = block;		// set active faction here
                    m_factionId = block->info();

                    // get turn from faction block if VERSION block has none
                    if (!m_turn)
                        m_turn = block->valueInt(TYPE_TURN, m_turn);
                    if (!m_recruitment)
                        m_recruitment = block->valueInt(TYPE_RECRUITMENTCOST, m_recruitment);
                    break;
                }
            }
        }
    }

    std::map<int, int> allied_status;	// what factions do we have HELP stati set to?

    // continue to evaluate ALLIANCE blocks for active faction
    block = m_activefaction;
    if (block != m_blocks.end())
    {
        int factionDepth = m_activefaction->depth();
        for (++block; block != m_blocks.end() && block->depth() > factionDepth; ++block)
        {
            if (block->type() == block_type::TYPE_ALLIANCE) {
                int status = block->valueInt("Status", 0);
                allied_status[block->info()] = status;
            }
        }
        insertFaction = block;
    }

    std::set<int> unit_got_taxes;		// units that got taxes (MSG id 1264208711); the regions will get a coins icon
    for (block = start; block != m_blocks.end(); ++block)
    {
        // add battle to list
        if (block->type() == block_type::TYPE_BATTLE)
        {
            m_battles[Coordinates(block->x(), block->y(), block->info())] = block;
        }
        // add region to region list
        else if (block->type() == block_type::TYPE_REGION)
        {
            if (regionPtr) {
                setRegionStats(regionPtr, region_own, region_ally, region_enemy, unconfirmed);
            }

            regionPtr = &*block;
            regionPtr->flags(regionPtr->flags() & (datablock::FLAG_BLOCKID_BIT0 | datablock::FLAG_BLOCKID_BIT1));	// unset all flags except BLOCKID flags
            unconfirmed = region_own = region_ally = region_enemy = 0;

            if (block->value(TYPE_VISIBILITY) == "lighthouse")
                regionPtr->setFlags(datablock::FLAG_LIGHTHOUSE);		// region is seen by lighthouse
            else if (block->value(TYPE_VISIBILITY) == "travel")
                regionPtr->setFlags(datablock::FLAG_TRAVEL);			// region is seen by traveling throu

            m_regions[Coordinates(block->x(), block->y(), block->info())] = block;

            // get region owner (E3 only)
            if (m_activefaction != m_blocks.end())
            {
                int ownerId = block->valueInt("owner", -1);
                if (ownerId == m_factionId)
                    regionPtr->setFlags(datablock::FLAG_REGION_OWN);
                else if (allied_status[ownerId] & HELP_GUARD) {
                    regionPtr->setFlags(datablock::FLAG_REGION_ALLY);
                }
                else if (ownerId != -1)
                    regionPtr->setFlags(datablock::FLAG_REGION_ENEMY);
            }
        }

        // add ships and buildings to their lists
        else if (block->type() == block_type::TYPE_SHIP)
            m_ships[block->info()] = block;
        else if (block->type() == block_type::TYPE_BUILDING)
            m_buildings[block->info()] = block;

        // generate list of units that got a "got taxes" message (E3 only)
        else if (block->type() == block_type::TYPE_MESSAGE) {
            int type = block->valueInt(TYPE_MSG_TYPE);
            if (type == 1264208711)
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

            if (!hasFaction(factionId))
            {
                datablock faction;
                faction.string("PARTEI");
                faction.infostr(FXStringVal(factionId));

                FXString name;
                if (factionId == 0 || factionId == 666)	// Monster (0) and the new Monster (ii)
                {
                    name.assign("Monster");
                }
                else {
                    name.format("Partei %s", FXStringValEx(factionId, 36).text());
                }
                datakey key;
                key.key("Parteiname", block->type());
                key.value(name);
                faction.addKey(key);
                m_factions[factionId] = m_blocks.insert(insertFaction, faction);
            }

            // set attachment for unit of active faction
            else if (factionId == m_factionId)
            {
                datablock::itor cmd;
                if (getCommands(cmd, block))
                {
                    // add att_commands to command block
                    if (!cmd->attachment())
                    {
                        cmd->attachment(new att_commands(*cmd));
                    }
                }
            }
        }

        // .. and factions to faction list
        else if (block->type() == block_type::TYPE_FACTION) {
            m_factions[block->info()] = block;
        }
        else if (block->type() == block_type::TYPE_ALLIANCE) {
            // alliance as placeholder-faction
            if (!hasFaction(block->info())) {
                m_factions[block->info()] = block;
            }
        }
        // .. and islands to island list
        else if (block->type() == block_type::TYPE_ISLAND) {
            m_islands[block->info()] = block;
        }

        if (regionPtr)
        {
            if (block->type() == block_type::TYPE_UNIT)
            {
                const datablock* unitPtr = &*block;
                regionPtr->setFlags(datablock::FLAG_TROOPS);			// region has units

                // count persons
                int number = block->valueInt(TYPE_NUMBER, 0);

                enum class owner_t
                {
                    UNIT_ENEMY,
                    UNIT_OWN,
                    UNIT_ALLY,
                } owner = owner_t::UNIT_ENEMY;

                if (m_factionId != 0)
                {
                    int factionId = getFactionIdForUnit(unitPtr);
                    if (factionId > 0)
                    {
                        if (factionId == m_factionId)
                        {
                            region_own += number;
                            number = 0;
                            owner = owner_t::UNIT_OWN;
                            if (!isConfirmed(*block)) {
                                ++unconfirmed;
                            }
                        }
                        else if (allied_status[factionId] != 0)
                        {
                            region_ally += number;
                            owner = owner_t::UNIT_ALLY;
                            number = 0;
                        }
                    }
                }
                region_enemy += number;

                if (block->valueInt("bewacht") == 1)
                {
                    if (owner == owner_t::UNIT_OWN)
                        regionPtr->setFlags(datablock::FLAG_GUARD_OWN);
                    else if (owner == owner_t::UNIT_ALLY)
                        regionPtr->setFlags(datablock::FLAG_GUARD_ALLY);
                    else
                        regionPtr->setFlags(datablock::FLAG_GUARD_ENEMY);
                }

                if (unit_got_taxes.find(block->info()) != unit_got_taxes.end())
                    regionPtr->setFlags(datablock::FLAG_REGION_TAXES);

                if (block->value(TYPE_FACTION) == "0")
                    regionPtr->setFlags(datablock::FLAG_MONSTER);		// Monster (0) in region
                if (block->value(TYPE_FACTION) == "666")
                    regionPtr->setFlags(datablock::FLAG_MONSTER);		// Monster (ii) in region
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
                        regionPtr->setFlags(datablock::FLAG_MONSTER);		// monsters in the region
                    else if (block->value(TYPE_TYPE) == "Seeschlangen")
                        regionPtr->setFlags(datablock::FLAG_SEASNAKE);		// sea snake in region
                    else if (block->value(TYPE_TYPE) == "Jungdrachen" ||
                        block->value(TYPE_TYPE) == "Drachen" ||
                        block->value(TYPE_TYPE) == "Wyrme")
                        regionPtr->setFlags(datablock::FLAG_DRAGON);		// a dragon is in the region!
                }
            }
            else if (block->type() == block_type::TYPE_DURCHSCHIFFUNG)
                regionPtr->setFlags(datablock::FLAG_SHIPTRAVEL);		// region has travelled by ship
            else if (block->type() == block_type::TYPE_BUILDING)
            {
                regionPtr->setFlags(datablock::FLAG_CASTLE);			// region has a building

                if (block->value(TYPE_TYPE) == "Wurmloch")
                    regionPtr->setFlags(datablock::FLAG_WORMHOLE);		// a wormhole is in the region!
            }
            else if (block->type() == block_type::TYPE_SHIP)
                regionPtr->setFlags(datablock::FLAG_SHIP);				// region has ships inside
            else if (block->type() == block_type::TYPE_BORDER)
            {
                const FXString& type = block->value("typ");

                if (type == "Strasse" || type == "Stra\u00dfe" || type == FXString(L"Stra\u00dfe"))	// TO FIX!
                {												// region has street in some direction
                    int direction = block->valueInt("richtung");
                    if (direction >= 0 && direction <= 5)
                    {
                        if (block->valueInt("prozent") == 100)			// region has a complete street
                            regionPtr->setFlags(datablock::FLAG_STREET << direction);
                        else											// region has incomplete street
                            regionPtr->setFlags(datablock::FLAG_STREET_UNDONE << direction);
                    }
                }
            }
        }
    }
    if (regionPtr) {
        setRegionStats(regionPtr, region_own, region_ally, region_enemy, unconfirmed);
    }
}

void datafile::createHashTables()
{
    contentSizes.clear();
    createHierarchy();
    updateHashTables(m_blocks.begin());
    floodIslandNames();
}

void datafile::parseMessages()
{
    for (auto& it : m_regions) {
        att_region* att = static_cast<att_region *>(it.second->attachment());
        if (att) {
            att->learncost = 0;
            memset(att->income, 0, sizeof(att->income));
        }
    }
    if (hasActiveFaction()) {
        datablock::itor it;
        if (getChild(it, m_activefaction, block_type::TYPE_MESSAGE)) {
            do {
                datablock& block = *it++;
                int type = block.valueInt(TYPE_MSG_TYPE);
                if (type == 1784377885) {
                    // Passwort wurde ge�ndert
                    m_password = block.value("value");
                }
                else if (type != 771334452 && type != 443066738) {
                    continue;
                }
                const datakey::list_type& list = block.data();
                int amount = 0, mode = 0;
                datablock* reg = nullptr;

                for (datakey::list_type::const_iterator itor = list.begin(); itor != list.end(); itor++)
                {
                    key_type key = itor->type();
                    if (key == key_type::TYPE_MSG_REGION) {
                        int x, y, z;
                        int num = sscanf(itor->value().text(), "%d %d %d", &x, &y, &z);
                        if (num >= 2) {
                            if (num < 3) {
                                z = 0;
                            }
                            datablock::itor match;
                            if (getRegion(match, x, y, z)) {
                                reg = &*match;
                            }
                        }

                    }
                    else if (key == key_type::TYPE_MSG_AMOUNT) {
                        amount = itor->getInt();
                    }
                    else if (key == key_type::TYPE_MSG_COST) {
                        amount = itor->getInt();
                    }
                    else if (key == key_type::TYPE_MSG_MODE) {
                        mode = itor->getInt();
                    }
                }

                if (reg) {
                    att_region* att = static_cast<att_region*>(reg->attachment());
                    if (!att) {
                        reg->attachment(att = new att_region());
                    }

                    if (mode < INCOME_MISC || mode >= INCOME_MAX) {
                        mode = INCOME_MISC;
                    }

                    if (type == 443066738)	// Ausgaben fuer teure Talente (+Akademie)
                        att->learncost += amount;
                    else // Einnahmen
                        att->income[mode] += amount;
                }
            } while (it != m_blocks.end() && it->type() == block_type::TYPE_MESSAGE);
        }
    }
}

void datafile::SelectionState::transfer(datafile* old_cr, datafile* new_cr, int x_offset, int y_offset)
{
    FXASSERT(old_cr && new_cr);
    if (selected) {
        if (selected & UNKNOWN_REGION) {
            if (sel_plane == 0) {
                sel_x += x_offset;
                sel_y += y_offset;
            }
        }
        if (selected & REGION) {
            if (!new_cr->getRegion(region, *region)) {
                selected -= REGION;
            }
        }
        if (selected & FACTION) {
            if (!new_cr->getFaction(faction, faction->info())) {
                selected -= FACTION;
            }
        }
        if (selected & UNIT) {
            if (!new_cr->getUnit(unit, unit->info())) {
                selected -= UNIT;
            }
        }
        if (selected & SHIP) {
            if (!new_cr->getShip(ship, ship->info())) {
                selected -= SHIP;
            }
        }
        if (selected & BUILDING) {
            if (!new_cr->getBuilding(building, building->info())) {
                selected -= BUILDING;
            }
        }
        if (!regionsSelected.empty()) {
            std::set<datablock*> sel;
            for (datablock* r : regionsSelected) {
                datablock::itor match;
                if (new_cr->getRegion(match, *r)) {
                    sel.insert(&*match);
                }
            }
            regionsSelected = sel;
        }
        ++selChange;
    }
}

void datafile::SelectionState::clear()
{
    selected = 0;
    regionsSelected.clear();
}
