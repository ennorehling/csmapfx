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

    class Textures {
    private:
        struct TextureData {
            std::string str;
            const unsigned char* bitmap;
        } data;
        static Textures* instance;
        TextureData m_terrains[data::TERRAIN_LAST];
        TextureData m_icons[data::TERRAIN_LAST];

    public:
        static Textures* get() {
            if (!instance) {
                instance = new Textures();
            }
            return instance;
        }

        static std::string loadTexture(const char* folder, const char* name)
        {
            unsigned char* result = nullptr;
            std::string filename(folder);
            filename += '/';
            filename += name;
            return loadResourceFile(filename.c_str());
        }

        const unsigned char* getTerrainIcon(int i) {
            if (m_icons[i].bitmap == nullptr) {
                m_icons[i].str = Textures::loadTexture("terrain/icons", terrains[i].filename);
                if (m_icons[i].str.empty())
                {
                    m_icons[i].bitmap = terrainSymbols[i];
                }
                else {
                    m_icons[i].bitmap = (const unsigned char*)m_icons[i].str.c_str();
                }
            }
            return m_icons[i].bitmap;
        }

        const unsigned char *getTerrainData(int i) {
            if (m_terrains[i].bitmap == nullptr) {
                m_terrains[i].str = Textures::loadTexture("terrain", terrains[i].filename);
                if (m_terrains[i].str.empty())
                {
                    // give up, don't try loading again. hack: cast, but never free this!
                    m_terrains[i].bitmap = terrains[i].image;
                }
                else {
                    m_terrains[i].bitmap = (const unsigned char*)m_terrains[i].str.c_str();
                }
            }
            return m_terrains[i].bitmap;
        }
    };

    Textures* Textures::instance;

    const unsigned char* terrain_data(int i) {
        return Textures::get()->getTerrainData(i);
    }

    const unsigned char* terrain_icon(int i) {
        return Textures::get()->getTerrainIcon(i);
    }

}
