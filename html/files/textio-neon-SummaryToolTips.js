﻿NDSummary.OnToolTipsLoaded("File:textio.neon",{307:"<div class=\"NDToolTip TFile LNeon\"><div class=\"TTSummary\">Functions for general purpose text input and output with files.</div></div>",309:"<div class=\"NDToolTip TEnumeration LNeon\"><div class=\"TTSummary\">Mode to use when opening a file.</div></div>",312:"<div class=\"NDToolTip TType LNeon\"><div class=\"TTSummary\">Opaque type representing a text file on disk.</div></div>",314:"<div class=\"NDToolTip TException LNeon\"><div class=\"TTSummary\">General exception for errors raised by this module.</div></div>",315:"<div class=\"NDToolTip TException LNeon\"><div class=\"TTSummary\">An invalid (NIL) file was used.</div></div>",316:"<div class=\"NDToolTip TException LNeon\"><div class=\"TTSummary\">File was unable to be opened.</div></div>",317:"<div class=\"NDToolTip TException LNeon\"><div class=\"TTSummary\">An error occurred while writing to a file.</div></div>",319:"<div class=\"NDToolTip TVariable LNeon\"><div class=\"TTSummary\">The standard input file.</div></div>",320:"<div class=\"NDToolTip TVariable LNeon\"><div class=\"TTSummary\">The standard output file.</div></div>",321:"<div class=\"NDToolTip TVariable LNeon\"><div class=\"TTSummary\">The standard error file.</div></div>",323:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype323\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">DECLARE NATIVE FUNCTION close(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">f</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">TextFile</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">)</div></div></div></div><div class=\"TTSummary\">Close a file.</div></div>",324:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype324\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/3/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/3/2\">DECLARE NATIVE FUNCTION open(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">name</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String,</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"2/2/3/3\" data-NarrowGridArea=\"3/1/4/2\" style=\"grid-area:2/2/3/3\">mode</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"2/3/3/4\" data-NarrowGridArea=\"3/2/4/3\" style=\"grid-area:2/3/3/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"2/4/3/5\" data-NarrowGridArea=\"3/3/4/4\" style=\"grid-area:2/4/3/5\">Mode</div><div class=\"PAfterParameters NegativeLeftSpaceOnWide\" data-WideGridArea=\"2/5/3/6\" data-NarrowGridArea=\"4/1/5/5\" style=\"grid-area:2/5/3/6\">): OpenResult</div></div></div></div><div class=\"TTSummary\">Open a file with the given name and mode.</div></div>",325:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype325\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">DECLARE NATIVE FUNCTION readLine(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">f</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">TextFile</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">): ReadLineResult</div></div></div></div><div class=\"TTSummary\">Read a line of text from a file.</div></div>",326:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype326\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">DECLARE NATIVE FUNCTION seekEnd(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">f</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">TextFile</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">)</div></div></div></div><div class=\"TTSummary\">Seek to the end of a file.</div></div>",327:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype327\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">DECLARE NATIVE FUNCTION seekStart(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">f</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">TextFile</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">)</div></div></div></div><div class=\"TTSummary\">Seek to the start of a file.</div></div>",328:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype328\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">DECLARE NATIVE FUNCTION truncate(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">f</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">TextFile</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">)</div></div></div></div><div class=\"TTSummary\">Truncate the file at the current position.</div></div>",329:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype329\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/3/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/3/2\">DECLARE NATIVE FUNCTION writeLine(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">f</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">TextFile,</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"2/2/3/3\" data-NarrowGridArea=\"3/1/4/2\" style=\"grid-area:2/2/3/3\">s</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"2/3/3/4\" data-NarrowGridArea=\"3/2/4/3\" style=\"grid-area:2/3/3/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"2/4/3/5\" data-NarrowGridArea=\"3/3/4/4\" style=\"grid-area:2/4/3/5\">String</div><div class=\"PAfterParameters NegativeLeftSpaceOnWide\" data-WideGridArea=\"2/5/3/6\" data-NarrowGridArea=\"4/1/5/5\" style=\"grid-area:2/5/3/6\">)</div></div></div></div><div class=\"TTSummary\">Write a string, followed by a newline, to a file.</div></div>"});