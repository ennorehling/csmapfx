#include "fxkeys.h"
#include "commands.h"

// *********************************************************************************************************
// *** FXCommands implementation

FXDEFMAP(FXCommands) MessageMap[]=
{ 
	//________Message_Type_____________________ID_______________Message_Handler_______ 
	FXMAPFUNC(SEL_COMMAND,			FXCommands::ID_UPDATE,			FXCommands::onMapChange), 
    FXMAPFUNC(SEL_COMMAND,          FXCommands::ID_UNIT_NEXT,		FXCommands::onNextUnit),
    FXMAPFUNC(SEL_COMMAND,          FXCommands::ID_UNIT_PREV,		FXCommands::onPrevUnit),
    FXMAPFUNC(SEL_COMMAND,          FXCommands::ID_UNIT_CONFIRM,    FXCommands::onConfirmUnit),
    FXMAPFUNC(SEL_COMMAND,          FXCommands::ID_UNIT_ADD,        FXCommands::onCreateUnit),

    FXMAPFUNC(SEL_UPDATE,			FXCommands::ID_UNIT_CONFIRM,    FXCommands::updConfirmed),
    FXMAPFUNC(SEL_UPDATE,			FXCommands::ID_UNIT_ADD,       FXCommands::updCommandable),

    FXMAPFUNC(SEL_KEYPRESS,			0,								FXCommands::onKeyPress),
    FXMAPFUNC(SEL_UPDATE,			0,								FXCommands::onUpdate),

    FXMAPFUNC(SEL_UPDATE,			FXCommands::ID_ROWCOL,			FXCommands::updRowCol),
};

FXIMPLEMENT(FXCommands,FXText,MessageMap, ARRAYNUMBER(MessageMap))

FXCommands::FXCommands(FXComposite* p, FXObject* tgt,FXSelector sel, FXuint opts, FXint x,FXint y,FXint w,FXint h)
		: FXText(p, tgt,sel, opts, x,y,w,h)
{
	// initial visible rows & tab stop
	setVisibleRows(6);
	setTabColumns(4);

	// init variables
	mapFile = NULL;

	// set text color styles
	textStyles.resize(19);
	for(size_t i = 0; i < textStyles.size(); i++)
	{
		// set ALL the fields
		textStyles[i].normalForeColor = getApp()->getForeColor();
		textStyles[i].normalBackColor = getApp()->getBackColor();
		textStyles[i].selectForeColor = getApp()->getSelforeColor();
		textStyles[i].selectBackColor = getApp()->getSelbackColor();
		textStyles[i].hiliteForeColor = getApp()->getHiliteColor();
		textStyles[i].hiliteBackColor = FXRGB(255, 128, 128); // from FXText.cpp
		textStyles[i].activeBackColor = getApp()->getBackColor();
		textStyles[i].style = 0; // no underline, italic, bold
	}

														// [0] no style (normal text)

	textStyles[0].normalForeColor = FXRGB(0,0,0);		// [1] schwarz (dunkel)
	textStyles[1].normalForeColor = FXRGB(0,0,0x80);	// [2] blue
	textStyles[2].normalForeColor = FXRGB(0,0x80,0);	// [3] green
	textStyles[3].normalForeColor = FXRGB(0,0x80,0x80);	// [4] cyan
	textStyles[4].normalForeColor = FXRGB(0x80,0,0);	// [5] rot
	textStyles[5].normalForeColor = FXRGB(0x80,0,0x80);	// [6] violett
	textStyles[6].normalForeColor = FXRGB(0x80,0x80,0);	// [7] gelb
	textStyles[7].normalForeColor = FXRGB(0x80,0x80,0x80);	// [8] grau

	textStyles[8].normalForeColor = FXRGB(0xc0,0xc0,0xc0);	// [9] grau (hell)
	textStyles[9].normalForeColor = FXRGB(0,0,0xff);		// [10] blue
	textStyles[10].normalForeColor = FXRGB(0,0xff,0);		// [11] green
	textStyles[11].normalForeColor = FXRGB(0,0xff,0xff);	// [12] cyan
	textStyles[12].normalForeColor = FXRGB(0xff,0,0);		// [13] rot
	textStyles[13].normalForeColor = FXRGB(0xff,0,0xff);	// [14] violett
	textStyles[14].normalForeColor = FXRGB(0xff,0xff,0);	// [15] gelb
	textStyles[15].normalForeColor = FXRGB(0xff,0xff,0xff);	// [16] grau

	textStyles[16].style = STYLE_BOLD;						// [17] fett
	textStyles[17].style = STYLE_UNDERLINE;					// [18] unterstrichen

	textStyles[18].normalForeColor = FXRGB(0,0x80,0);		// [19] bold dark green
	textStyles[18].style = STYLE_BOLD;

	setStyled();
	setHiliteStyles(&textStyles[0]);
}

void FXCommands::create()
{
	FXText::create();
	disable();
	setBackColor(getApp()->getBaseColor());
}

FXCommands::~FXCommands()
{
}

void FXCommands::setMapFile(datafile *f)
{
    mapFile = f;
}

void FXCommands::connectMap(FXCSMap* map_)
{
	map = map_;
}

long FXCommands::onPrevUnit(FXObject *, FXSelector, void *)
{
	if (!mapFile)
		return 0;

	datablock::citor end = mapFile->blocks().end();
	datablock::citor begin = mapFile->blocks().begin();
	datablock::itor unit;
	if (selection.selected & selection.UNIT)
		unit = std::prev(selection.unit);
	else if (selection.selected & selection.REGION)
		unit = std::prev(selection.region);
    else
		return 0;

	for (; unit != begin; --unit)
	{
		if (unit->type() != block_type::TYPE_UNIT)
			continue;

		// search command block
		datablock::itor block = unit;
		for (block++; block != end && block->depth() > unit->depth(); block++)
			if (block->type() == block_type::TYPE_COMMANDS)
				break;				// found

		if (block != end && block->type() == block_type::TYPE_COMMANDS)
		{
			bool unconfirmed = true;

			if (att_commands* cmds = dynamic_cast<att_commands*>(block->attachment()))
				unconfirmed = !cmds->confirmed;

			if (unconfirmed)
			{
				// send selectionchange, select this unit
	
                selection.selected = selection.UNIT;
                selection.unit = unit;
                updateSelection();
				
				flags |= FLAG_UPDATE;
				forceRefresh();
				setFocus();
				return 1;
			}
		}
	}

	// no next unit found: beep
	getApp()->beep();
	return 1;
}

long FXCommands::onNextUnit(FXObject *, FXSelector, void *)
{
	if (!mapFile)
		return 0;

	datablock::citor end = mapFile->blocks().end();
	datablock::itor unit;
	if (selection.selected & selection.UNIT)
		unit = std::next(selection.unit);
	else if (selection.selected & selection.REGION)
		unit = std::next(selection.region);
    else
        return 0;

	for (; unit != end; ++unit)
	{
		if (unit->type() != block_type::TYPE_UNIT)
			continue;

		// search command block
		datablock::citor block = unit;
		for (block++; block != end && block->depth() > unit->depth(); block++)
			if (block->type() == block_type::TYPE_COMMANDS)
				break;				// found

		if (block != end && block->type() == block_type::TYPE_COMMANDS)
		{
			bool unconfirmed = true;

			if (att_commands* cmds = dynamic_cast<att_commands*>(block->attachment()))
				unconfirmed = !cmds->confirmed;

			if (unconfirmed)
			{
                selection.selected = selection.UNIT;
                selection.unit = unit;
                updateSelection();

				// force update and set focus
				flags |= FLAG_UPDATE;
				forceRefresh();
				setFocus();
				return 1;
			}
		}
	}

	// no next unit found: beep
	getApp()->beep();
	return 0;
}

void FXCommands::makeTemp()
{
	if (!isEnabled())
	{
		getApp()->beep();
		return;
	}

	appendText("MACHE TEMP " + getFreeTemp() + "\n\t");
	int row = getNumRows()-1;
	appendText("\nENDE\n");
	setCursorRow(row);
	setCursorPos(lineEnd(getCursorPos()), 1);
	makePositionVisible(getCursorPos());
	
	highlightText();
	setModified(true);
	
	setFocus();
}

FXString FXCommands::getFreeTemp()
{
    static int i = 1;			// TODO: Check if tempnumber is free
	return FXStringVal(i++);
}

long FXCommands::onConfirmUnit(FXObject*, FXSelector, void*)
{
    setConfirmed(!getConfirmed());
    return 1;
}

long FXCommands::onCreateUnit(FXObject*, FXSelector, void*)
{
    makeTemp();
    return 1;
}

void FXCommands::setConfirmed(bool confirmed)
{
	if (!mapFile)
		return;

	if (selection.selected & selection.UNIT)
	{
		datablock::itor unit = selection.unit;

		// search for command block of unit
		datablock::itor end = mapFile->blocks().end();
		datablock::itor block = unit;
		for (block++; block != end && block->depth() > unit->depth(); block++)
			if (block->type() == block_type::TYPE_COMMANDS)
				break;				// found

		if (block != end && block->type() == block_type::TYPE_COMMANDS)
		{
			att_commands* cmds = dynamic_cast<att_commands*>(block->attachment());
			if (cmds)
                cmds->confirmed = confirmed;
			else if (confirmed)			// don't need to add block if it will not be confirmed
			{
				// copy commands of selected unit
				block->attachment(cmds = new att_commands);
				cmds->confirmed = confirmed;

				const datakey::list_type &list = block->data();

                for (datakey::list_type::const_iterator itor = list.begin(); itor != list.end(); itor++)
                {
                    cmds->commands.push_back((*itor).value());
                }
			}
		}
	}

	// send selectionchange
    updateSelection();
}

int FXCommands::getConfirmed()
{
	if (!mapFile)
		return -1;

	if (selection.selected & selection.UNIT)
	{
		datablock::itor unit = selection.unit;

		// search for command block of unit
		datablock::itor end = mapFile->blocks().end();
		datablock::itor block = unit;
		for (block++; block != end && block->depth() > unit->depth(); block++)
			if (block->type() == block_type::TYPE_COMMANDS)
				break;				// found

		if (block != end && block->type() == block_type::TYPE_COMMANDS)
		{
			if (att_commands* cmds = dynamic_cast<att_commands*>(block->attachment()))
				return cmds->confirmed;

			return 0;	// no changed commands, so not confirmed
		}
        return 1;
	}

	return -1;	// no unit with command block
}

long FXCommands::onMapChange(FXObject* sender, FXSelector, void* ptr)
{
	datafile::SelectionState *pstate = (datafile::SelectionState*)ptr;

    // any data changed, so need to update list?
	if (selection.fileChange != pstate->fileChange || selection.selChange != pstate->selChange)
	{
        selection = *pstate;

        setText("");
		setModified(false);
		disable();

		if (mapFile && selection.selected & selection.UNIT)
		{
            getApp()->beginWaitCursor();
            datablock::itor unit = selection.unit;

			// search for command block of unit
			datablock::itor end = mapFile->blocks().end();
			datablock::itor block = unit;
			for (block++; block != end && block->depth() > unit->depth(); block++)
				if (block->type() == block_type::TYPE_COMMANDS)
					break;				// found

			if (block != end && block->type() == block_type::TYPE_COMMANDS)
			{
				att_commands* cmds = dynamic_cast<att_commands*>(block->attachment());
				if (!cmds)
				{
					// copy commands of selected unit
					block->attachment(cmds = new att_commands);

					const datakey::list_type &list = block->data();

                    for (datakey::list_type::const_iterator itor = list.begin(); itor != list.end(); itor++) {
                        cmds->commands.push_back((*itor).value());
                    }
				}

				// show commands of selected unit
				for (att_commands::cmdlist_t::iterator itor = cmds->commands.begin(); itor != cmds->commands.end(); itor++)
				{
					appendText(itor->text(), itor->length());
					appendText("\n", 1);
				}

				// parse & highlight text and enable the widget
				highlightText();
				mapShowRoute();
				setModified(false);
				enable();
			}
            getApp()->endWaitCursor();
        }

		if (isEnabled())
			setBackColor(getApp()->getBackColor());
		else
			setBackColor(getApp()->getBaseColor());
    }
    return 1;
}

long FXCommands::onKeyPress(FXObject* sender,FXSelector sel,void* ptr)
{
	FXEvent* event=(FXEvent*)ptr;
	if (!isEnabled())
		return FXText::onKeyPress(sender, sel, ptr);
	else if (event->code == KEY_Return || event->code == KEY_KP_Enter)
	{
		// Keeps the indentation level, when you go to the next line
		int curs = getCursorPos();
		int begin = lineStart(curs);

		FXString line;
		extractText(line, begin, curs-begin);

		int pos = line.find_first_not_of(" \t");
		if (pos != -1)
			line.erase(pos, line.length() - pos);

		if (!line.empty())
		{
			long res = FXText::onKeyPress(sender, sel, ptr);
			if (res)
				insertText(lineStart(getCursorPos()), line);
			return res;
		}
	}
	else if (event->code == KEY_Tab || event->code == KEY_KP_Tab)
	{
		int curs = getCursorPos();
		int begin = lineStart(curs);
		int end = lineEnd(curs);

		FXString line;
		extractText(line, begin, curs-begin);

		if (line.find_first_not_of(" \t") != -1)
		{
			// the cursor is after some text.
			extractText(line, begin, curs-begin);

			line.upper();
			if (line == "MACHE TEMP" || line == "MACH TEMP")
			{
				removeText(begin, end-begin);

				insertText(begin, "MACHE TEMP " + getFreeTemp() + "\n\t");
				int pos = lineEnd(nextLine(begin));
				insertText(pos, "\nENDE\n");
				setCursorPos(pos, 1);
				makePositionVisible(getCursorPos());
				
				highlightText();
				setModified(true);
				return 1;
			}

			//FXMessageBox::information(getApp(), MBOX_OK, "Tab", line.text());
			//return 1;

			getApp()->beep();
			return 1;
		}
	}

	return FXText::onKeyPress(sender, sel, ptr);
}

long FXCommands::onUpdate(FXObject* sender,FXSelector sel,void* ptr)
{
	if (isModified())
	{
		highlightText();
		saveCommands();
	}
	mapShowRoute();
	return FXText::onUpdate(sender, sel, ptr);
}

long FXCommands::updConfirmed(FXObject* sender, FXSelector, void*) {
    FXCheckButton * btn = (FXCheckButton *)sender;
    int conf = getConfirmed();
    if (conf == -1) {
        btn->disable();
    }
    else {
        btn->enable();
    }
    btn->setCheck(conf == 1);
    return 1;
}

long FXCommands::updCommandable(FXObject* sender, FXSelector, void*) {
    FXCheckButton * btn = (FXCheckButton *)sender;
    isEnabled() ? btn->enable() : btn->disable();
    return 1;
}

long FXCommands::updRowCol(FXObject* sender,FXSelector,void*)
{
	if (!sender->isMemberOf(&FXTextField::metaClass))
        return 0;

	FXString txt;

	if (routeLength)
	{
		FXString len = FXStringVal(routeLength);
		while (len.length() < 2)
			len = " " + len;

		txt = "Reg " + len + " ";
	}

	if (hasFocus())
	{
		FXString row = FXStringVal(getCursorRow()+1);
		FXString col = FXStringVal(getCursorColumn()+1);

		while (row.length() < 2)
			row = " " + row;
		while (col.length() < 2)
			col = " " + col;

		txt += "Ln " + row + " Cl " + col;
	}

	static_cast<FXTextField*>(sender)->setText(txt);
	return 1;
}

void FXCommands::saveCommands()
{
    if (selection.selected & selection.UNIT)
    {
        // search for command block of unit
        datablock::itor iend = mapFile->blocks().end();
        datablock::itor iblock = selection.unit;
        for (iblock++; iblock != iend && iblock->depth() > selection.unit->depth(); iblock++) {
            if (iblock->type() == block_type::TYPE_COMMANDS) {
                if (att_commands* cmds = dynamic_cast<att_commands*>(iblock->attachment()))
                {
                    cmds->commands.clear();

                    int begin = 0, end;
                    do
                    {
                        end = lineEnd(begin);

                        FXString str;
                        extractText(str, begin, end - begin);
                        cmds->commands.push_back(str);

                        begin = nextLine(end);
                    } while (begin != end);

                    // remove trailing empty lines
                    while (!cmds->commands.empty())
                    {
                        FXString line = cmds->commands.back();
                        if (line.simplify().length())
                            break;
                        cmds->commands.pop_back();
                    }
                }
                setModified(false);
                // notify application that commands were modified
                if (!mapFile->modifiedCmds())
                {
                    mapFile->modifiedCmds(true);
                    // FIXME: only need to change the modified flag, not the selection!
                    updateSelection();
                }

                break;
            }
        }
    }
}

void FXCommands::mapShowRoute()
{
	if (!map)
	{
		routeLength = 0;
		return;
	}

	// if ROUTE or NACH, send to map
	int begin = lineStart(getCursorPos());
	int end = lineEnd(begin);

	FXString cmd, line;
	extractText(line, begin, end-begin);
	line.trim();

	cmd = line.before(' ');
	cmd.upper();
	
	if (cmd == "NACH" || cmd == "ROUTE")
	{
		routeLength = map->sendRouteCmds(line, 0);
		return;
	}

	for (begin = 0, end = 1; begin != end; begin = nextLine(end))
	{
		end = lineEnd(begin);
		extractText(line, begin, end-begin);

		line.trim();
		if (line.empty())
			continue;

		cmd = line.before(' ');
		cmd.upper();

		if (cmd == "NACH" || cmd == "ROUTE")
		{
			routeLength = map->sendRouteCmds(line, 0);
			return;
		}
	}
		
	map->sendRouteCmds("", 0);
	routeLength = 0;
}

void FXCommands::updateSelection()
{
    getShell()->handle(this, FXSEL(SEL_COMMAND, ID_UPDATE), &selection);
}

void FXCommands::highlightText()
{
	const int color_normal = 0;
	const int color_comment = 3;		// Kommentare: dark green
	const int color_comment_bold = 19;	// Kommentare: bold dark green
	const int color_error = 13;			// Fehler: hell-rot
	const int color_command = 10;		// Befehl: hell-blau

	// for each line
	for (int begin = 0, end = 1; begin != end; begin = nextLine(end))
	{
		end = lineEnd(begin);
		FXString line;
		extractText(line, begin, end-begin);

		int start = line.find_first_not_of(" \t");		// first non-whitespace
		if (start == -1)
			continue;

		changeStyle(begin, end-begin, color_normal);				// reset style
		
		int comment = line.find_first_of(';', start);
		
		if (line.mid(start, 2) == "//")
		{
			changeStyle(begin+start, 2, color_comment);
			comment = start + 3;

			int pos = line.find_first_not_of(" \t", comment);
			if (pos >= comment && line.mid(pos, 4) == "TODO")
			{
				changeStyle(begin+pos, 4, color_comment_bold);	// mark "TODO"
				comment = pos+4;
			}

			if (line.mid(start+2, 1) != " ")
			{
				changeStyle(begin+start+2, end-begin-start-2, color_error);	// mark errors
				comment = end;
			}
		}
		else
		{
			int pos = line.find_first_of(" \t", start);

			if (pos == -1)
				pos = end-begin;

            changeStyle(begin+start, pos-start, color_command);
		}

		// mark comments
		if (comment >= 0 && comment <= end-begin)
			changeStyle(begin+comment, end-begin-comment, color_comment);
	}
}
