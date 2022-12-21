#include "symbols.h"

#include "fxhelper.h"

// application icon
#include "csicon.data"

// small multipurpose symbols
#include "symbols/symbols.data"

// 16x16 symbols for toolbar
#include "symbols/toolbar.data"

// 64x64 terrain symbols for map
#include "terrain.data"

// 16x16 symbols for terrain and factions in regionlist
#include "symbols.data"
#include "bullets/bullets.data"

// other data
#include "infodlg.data"
#include "infodlg.e3.data"

#include "terrain.h"

#include <string>
#include <climits>
namespace data
{
	const unsigned int infodlg_data_size = sizeof(infodlg_data);
	const unsigned int infodlg_data_e3_size = sizeof(infodlg_data_e3);

    struct {
        const char* filename;
        const unsigned char* image;
        const unsigned char* icon;
    } terrains[data::TERRAIN_LAST] = {
        { "undefined.gif", undefined },
        { "ocean.gif", ocean },
        { "swamp.gif", swamp },
        { "plains.gif", plains },
        { "desert.gif", desert },
        { "forest.gif", forest },
        { "highland.gif", highland },
        { "mountain.gif", mountain },
        { "glacier.gif", glacier },
        { "volcano.gif", volcano },
        { "activevolcano.gif", activevolcano },
        { "packice.gif", packice },
        { "iceberg.gif", iceberg },
        { "icefloe.gif", icefloe },
        { "corridor.gif", corridor },
        { "wall.gif", wall },
        { "hall.gif", undefined },
        { "fog.gif", undefined },
        { "thickfog.gif", undefined },
        { "firewall.gif", firewall },
        { "mahlstrom.gif", mahlstrom },
    };

    const unsigned char* terrainSymbols[data::TERRAIN_LAST] =
    {
        sym_undefined,
        sym_ocean,
        sym_swamp,
        sym_plains,
        sym_desert,
        sym_forest,
        sym_highland,
        sym_mountain,
        sym_glacier,
        sym_volcano,
        sym_volcano,
        sym_iceberg,
        sym_iceberg,
        sym_iceberg,
        sym_corridor,
        sym_wall,
        sym_hall,
        sym_fog,
        sym_thickfog,
        sym_firewall,
        sym_mahlstrom
    };

    class TextureData {
    private:
        static TextureData* instance;
        unsigned char* m_terrains[data::TERRAIN_LAST];
        unsigned char* m_icons[data::TERRAIN_LAST];

    public:
        ~TextureData() {
            for (int i = 0; i != data::TERRAIN_LAST; ++i) {
                if (m_terrains[i] != undefined) {
                    delete m_terrains[i];
                }
                if (m_icons[i] != sym_undefined) {
                    delete m_icons[i];
                }
            }
        }
        static TextureData* get() {
            if (!instance) {
                instance = new TextureData();
            }
            return instance;
        }

        static unsigned char* loadTexture(const char* folder, const char* name)
        {
            unsigned char* result = nullptr;
            FXString filename(folder);
            filename += '/';
            filename += name;
            return loadResourceFile(filename);
        }

        const unsigned char* getTerrainIcon(int i) {
            if (m_icons[i] == nullptr) {
                if ((m_icons[i] = TextureData::loadTexture("terrain/icons", terrains[i].filename)) == nullptr)
                {
                    // give up, don't try loading again. hack: cast, but never free this!
                    m_icons[i] = (unsigned char*)sym_undefined;
                }
            }
            if (m_icons[i] && m_icons[i] != sym_undefined) {
                return m_icons[i];
            }
            return terrainSymbols[i];
        }

        const unsigned char *getTerrainData(int i) {
            if (m_terrains[i] == nullptr) {
                std::string filename("terrain/");
                if ((m_terrains[i] = TextureData::loadTexture("terrain", terrains[i].filename)) == nullptr)
                {
                    // give up, don't try loading again. hack: cast, but never free this!
                    m_terrains[i] = (unsigned char*)undefined;
                }
            }
            if (m_terrains[i] && m_terrains[i] != undefined) {
                return m_terrains[i];
            }
            return terrains[i].image;
        }
    };

    TextureData* TextureData::instance;

    const unsigned char* terrain_data(int i) {
        return TextureData::get()->getTerrainData(i);
    }

    const unsigned char* terrain_icon(int i) {
        return TextureData::get()->getTerrainIcon(i);
    }

}
