#ifndef _CSMAP_SYMBOLS
#define _CSMAP_SYMBOLS

//#include <FXICOIcon.h>

namespace data
{

	// application icon
	extern const unsigned char csmap[];

	// small multipurpose symbols
	extern const unsigned char small_minus[];
	extern const unsigned char small_plus[];
	extern const unsigned char small_x[];

	// symbols displayed on the map
	extern const unsigned char troopsunknown[];
	extern const unsigned char troopally[];
	extern const unsigned char troopenemy[];
	extern const unsigned char ship[];
	extern const unsigned char castle[];
    extern const unsigned char castle_owned[];
    extern const unsigned char coins[];
	extern const unsigned char eye[];
	extern const unsigned char eye_mixed[];
	extern const unsigned char shiptravel[];
	extern const unsigned char lighthouse[];
	extern const unsigned char travel[];
	extern const unsigned char monster[];
	extern const unsigned char seasnake[];
	extern const unsigned char dragon[];
	extern const unsigned char wormhole[];

	// street symbols
	extern const unsigned char street_no[];
	extern const unsigned char street_nw[];
	extern const unsigned char street_o[];
	extern const unsigned char street_so[];
	extern const unsigned char street_sw[];
	extern const unsigned char street_w[];

	extern const unsigned char street_undone_no[];
	extern const unsigned char street_undone_nw[];
	extern const unsigned char street_undone_o[];
	extern const unsigned char street_undone_so[];
	extern const unsigned char street_undone_sw[];
	extern const unsigned char street_undone_w[];

	// arrows for travel
	extern const unsigned char redarrow_no[];
	extern const unsigned char redarrow_nw[];
	extern const unsigned char redarrow_o[];
	extern const unsigned char redarrow_so[];
	extern const unsigned char redarrow_sw[];
	extern const unsigned char redarrow_w[];


	// toolbar symbols
	extern const unsigned char tb_close[];
	extern const unsigned char tb_info[];
	extern const unsigned char tb_merge[];
	extern const unsigned char tb_open[];
	extern const unsigned char tb_pointer[];
	extern const unsigned char tb_save[];
	extern const unsigned char tb_select[];

    // 64x64 terrain symbols for map
    const unsigned char* terrain_data(int i);
	extern const unsigned char active[];
	extern const unsigned char selected[];

	// 16x16 symbols for terrain and factions in regionlist
    const unsigned char* terrain_icon(int i);
    
    // colored bullets for factions
	extern const unsigned char black[];
	extern const unsigned char blue[];
	extern const unsigned char cyan[];
	extern const unsigned char gray[];
	extern const unsigned char green[];
	extern const unsigned char orange[];
	extern const unsigned char red[];
	extern const unsigned char yellow[];

	// internal info dialog data
	extern const unsigned char infodlg_data[];
	extern const unsigned int infodlg_data_size;

	extern const unsigned char infodlg_data_e3[];
	extern const unsigned int infodlg_data_e3_size;
};

#endif //_CSMAP_SYMBOLS
