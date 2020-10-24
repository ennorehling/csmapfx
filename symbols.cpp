#include "symbols.h"

// application icon
#include "csicon.data"

// small multipurpose symbols
#include "symbols/symbols.data"

// 16x16 symbols for toolbar
#include "symbols/toolbar.data"

// 64x64 terrain symbols for map
#include "terrain/terrain.data"

// 16x16 symbols for terrain and factions in regionlist
#include "terrain/symbols.data"
#include "bullets/bullets.data"

// other data
#include "infodlg.data"
#include "infodlg.e3.data"

namespace data
{
	const unsigned int infodlg_data_size = sizeof(infodlg_data);
	const unsigned int infodlg_data_e3_size = sizeof(infodlg_data_e3);

    const unsigned char *terrains[] =
    {
        undefined,
        ocean,
        swamp,
        plains,
        desert,
        forest,
        highland,
        mountain,
        glacier,
        volcano,
        activevolcano,
        packice,
        iceberg,
        icefloe,
        floor,
        wall,
        firewall,
        mahlstrom
    };

}
