﻿NDSummary.OnToolTipsLoaded("File:file.neon",{462:"<div class=\"NDToolTip TFile LNeon\"><div class=\"TTSummary\">Functions for working with files and directories.</div></div>",464:"<div class=\"NDToolTip TType LNeon\"><div class=\"TTSummary\">Various types of directory entries.</div></div>",465:"<div class=\"NDToolTip TType LNeon\"><div class=\"TTSummary\">Holds file information returned by getInfo.</div></div>",467:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype467\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/3/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/3/2\">DECLARE NATIVE FUNCTION copy(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">filename</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String,</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"2/2/3/3\" data-NarrowGridArea=\"3/1/4/2\" style=\"grid-area:2/2/3/3\">destination</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"2/3/3/4\" data-NarrowGridArea=\"3/2/4/3\" style=\"grid-area:2/3/3/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"2/4/3/5\" data-NarrowGridArea=\"3/3/4/4\" style=\"grid-area:2/4/3/5\">String</div><div class=\"PAfterParameters NegativeLeftSpaceOnWide\" data-WideGridArea=\"2/5/3/6\" data-NarrowGridArea=\"4/1/5/5\" style=\"grid-area:2/5/3/6\">): FileResult</div></div></div></div><div class=\"TTSummary\">Copies a file to a new destination.&nbsp; If the destination file already exists, a FileException.Exists is raised, and the file is not copied.</div></div>",468:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype468\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/3/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/3/2\">DECLARE NATIVE FUNCTION copyOverwriteIfExists(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">filename</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String,</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"2/2/3/3\" data-NarrowGridArea=\"3/1/4/2\" style=\"grid-area:2/2/3/3\">destination</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"2/3/3/4\" data-NarrowGridArea=\"3/2/4/3\" style=\"grid-area:2/3/3/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"2/4/3/5\" data-NarrowGridArea=\"3/3/4/4\" style=\"grid-area:2/4/3/5\">String</div><div class=\"PAfterParameters NegativeLeftSpaceOnWide\" data-WideGridArea=\"2/5/3/6\" data-NarrowGridArea=\"4/1/5/5\" style=\"grid-area:2/5/3/6\">): FileResult</div></div></div></div><div class=\"TTSummary\">Copies a file to a new destination.&nbsp; If the destination file exists, it will be overwritten.&nbsp; If the destination file does not exist, it will be created.</div></div>",469:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype469\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">DECLARE NATIVE FUNCTION <span class=\"SHKeyword\">delete</span>(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">filename</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">): FileResult</div></div></div></div><div class=\"TTSummary\">Delete a file. This function does not raise an exception if the file does not exist.</div></div>",470:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype470\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">DECLARE NATIVE FUNCTION exists(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">filename</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">): Boolean</div></div></div></div><div class=\"TTSummary\">Check to see whether a name exists in the filesystem.&nbsp; The name may refer to either a file or a directory.</div></div>",471:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype471\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">DECLARE NATIVE FUNCTION files(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">path</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">): Array&lt;String&gt;</div></div></div></div><div class=\"TTSummary\">Given a path name, return an array containing the names of the files in that directory.</div></div>",472:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype472\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/4/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/4/2\">FUNCTION filesByWildcard(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">path</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String,</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"2/2/3/3\" data-NarrowGridArea=\"3/1/4/2\" style=\"grid-area:2/2/3/3\">pattern</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"2/3/3/4\" data-NarrowGridArea=\"3/2/4/3\" style=\"grid-area:2/3/3/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"2/4/3/5\" data-NarrowGridArea=\"3/3/4/4\" style=\"grid-area:2/4/3/5\">String,</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"3/2/4/3\" data-NarrowGridArea=\"4/1/5/2\" style=\"grid-area:3/2/4/3\">recursive</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"3/3/4/4\" data-NarrowGridArea=\"4/2/5/3\" style=\"grid-area:3/3/4/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"3/4/4/5\" data-NarrowGridArea=\"4/3/5/4\" style=\"grid-area:3/4/4/5\">Boolean DEFAULT FALSE</div><div class=\"PAfterParameters\" data-WideGridArea=\"3/5/4/6\" data-NarrowGridArea=\"5/1/6/5\" style=\"grid-area:3/5/4/6\">): Array&lt;String&gt;</div></div></div></div><div class=\"TTSummary\">Given a path name and a wildcard pattern, return an array containing the names of the files in the path which match the wildcard. If &lt;recursive&gt; is true, then search through all subdirectories of &lt;path&gt; for names matching the pattern.</div></div>",473:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype473\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">DECLARE NATIVE FUNCTION getInfo(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">name</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">): FileInfoResult</div></div></div></div><div class=\"TTSummary\">Given a file name, return the file information.</div></div>",474:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype474\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">DECLARE NATIVE FUNCTION isDirectory(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">path</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">): Boolean</div></div></div></div><div class=\"TTSummary\">Similar to exists, but only returns TRUE if the path actually is a directory.</div></div>",475:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype475\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">DECLARE NATIVE FUNCTION mkdir(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">path</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">): FileResult</div></div></div></div><div class=\"TTSummary\">Create a new directory with the given name.</div></div>",476:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype476\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">FUNCTION pathJoin(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">paths</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String...</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">): String</div></div></div></div><div class=\"TTSummary\">Join components of a path using the &lt;Separator&gt; characters appropriate for the OS.</div></div>",477:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype477\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"6\" data-NarrowColumnCount=\"5\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/4/2\" data-NarrowGridArea=\"1/1/2/6\" style=\"grid-area:1/1/4/2\">FUNCTION pathSplit(</div><div class=\"PName\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">path</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"2/4/3/5\" style=\"grid-area:1/5/2/6\">String,</div><div class=\"PModifierQualifier InFirstParameterColumn\" data-WideGridArea=\"2/2/3/3\" data-NarrowGridArea=\"3/1/4/2\" style=\"grid-area:2/2/3/3\">OUT&nbsp;</div><div class=\"PName\" data-WideGridArea=\"2/3/3/4\" data-NarrowGridArea=\"3/2/4/3\" style=\"grid-area:2/3/3/4\">directory,</div><div class=\"PName\" data-WideGridArea=\"3/3/4/4\" data-NarrowGridArea=\"4/2/5/3\" style=\"grid-area:3/3/4/4\">name</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"3/4/4/5\" data-NarrowGridArea=\"4/3/5/4\" style=\"grid-area:3/4/4/5\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"3/5/4/6\" data-NarrowGridArea=\"4/4/5/5\" style=\"grid-area:3/5/4/6\">String</div><div class=\"PAfterParameters NegativeLeftSpaceOnWide\" data-WideGridArea=\"3/6/4/7\" data-NarrowGridArea=\"5/1/6/6\" style=\"grid-area:3/6/4/7\">)</div></div></div></div><div class=\"TTSummary\">Split a path into directory and name parts.</div></div>",478:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype478\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">DECLARE NATIVE FUNCTION readBytes(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">filename</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">): BytesResult</div></div></div></div><div class=\"TTSummary\">Read the contents of a file into Bytes.</div></div>",479:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype479\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">DECLARE NATIVE FUNCTION readLines(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">filename</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">): LinesResult</div></div></div></div><div class=\"TTSummary\">Read the lines of a file into an array of Strings.</div></div>",480:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype480\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">FUNCTION removeDirectoryTree(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">path</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">): FileResult</div></div></div></div><div class=\"TTSummary\">Remove directory tree recursively.</div></div>",481:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype481\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/2/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/2/2\">DECLARE NATIVE FUNCTION removeEmptyDirectory(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">path</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String</div><div class=\"PAfterParameters\" data-WideGridArea=\"1/5/2/6\" data-NarrowGridArea=\"3/1/4/5\" style=\"grid-area:1/5/2/6\">): FileResult</div></div></div></div><div class=\"TTSummary\">Remove an empty directory.</div></div>",482:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype482\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/3/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/3/2\">DECLARE NATIVE FUNCTION rename(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">oldname</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String,</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"2/2/3/3\" data-NarrowGridArea=\"3/1/4/2\" style=\"grid-area:2/2/3/3\">newname</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"2/3/3/4\" data-NarrowGridArea=\"3/2/4/3\" style=\"grid-area:2/3/3/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"2/4/3/5\" data-NarrowGridArea=\"3/3/4/4\" style=\"grid-area:2/4/3/5\">String</div><div class=\"PAfterParameters NegativeLeftSpaceOnWide\" data-WideGridArea=\"2/5/3/6\" data-NarrowGridArea=\"4/1/5/5\" style=\"grid-area:2/5/3/6\">): FileResult</div></div></div></div><div class=\"TTSummary\">Rename a file. This function can also be used to move a file from one directory to another.</div></div>",483:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype483\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/3/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/3/2\">FUNCTION wildcardMatch(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">pattern</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String,</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"2/2/3/3\" data-NarrowGridArea=\"3/1/4/2\" style=\"grid-area:2/2/3/3\">name</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"2/3/3/4\" data-NarrowGridArea=\"3/2/4/3\" style=\"grid-area:2/3/3/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"2/4/3/5\" data-NarrowGridArea=\"3/3/4/4\" style=\"grid-area:2/4/3/5\">String</div><div class=\"PAfterParameters NegativeLeftSpaceOnWide\" data-WideGridArea=\"2/5/3/6\" data-NarrowGridArea=\"4/1/5/5\" style=\"grid-area:2/5/3/6\">): Boolean</div></div></div></div><div class=\"TTSummary\">Return TRUE if the given name matches the wildcard pattern. The wildcard characters are:</div></div>",484:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype484\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/3/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/3/2\">DECLARE NATIVE FUNCTION writeBytes(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">filename</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String,</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"2/2/3/3\" data-NarrowGridArea=\"3/1/4/2\" style=\"grid-area:2/2/3/3\">data</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"2/3/3/4\" data-NarrowGridArea=\"3/2/4/3\" style=\"grid-area:2/3/3/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"2/4/3/5\" data-NarrowGridArea=\"3/3/4/4\" style=\"grid-area:2/4/3/5\">Bytes</div><div class=\"PAfterParameters NegativeLeftSpaceOnWide\" data-WideGridArea=\"2/5/3/6\" data-NarrowGridArea=\"4/1/5/5\" style=\"grid-area:2/5/3/6\">): FileResult</div></div></div></div><div class=\"TTSummary\">Write a complete file from data in Bytes.</div></div>",485:"<div class=\"NDToolTip TFunction LNeon\"><div id=\"NDPrototype485\" class=\"NDPrototype WideForm\"><div class=\"PSection PParameterSection PascalStyle\"><div class=\"PParameterCells\" data-WideColumnCount=\"5\" data-NarrowColumnCount=\"4\"><div class=\"PBeforeParameters\" data-WideGridArea=\"1/1/3/2\" data-NarrowGridArea=\"1/1/2/5\" style=\"grid-area:1/1/3/2\">DECLARE NATIVE FUNCTION writeLines(</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"1/2/2/3\" data-NarrowGridArea=\"2/1/3/2\" style=\"grid-area:1/2/2/3\">filename</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"1/3/2/4\" data-NarrowGridArea=\"2/2/3/3\" style=\"grid-area:1/3/2/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"1/4/2/5\" data-NarrowGridArea=\"2/3/3/4\" style=\"grid-area:1/4/2/5\">String,</div><div class=\"PName InFirstParameterColumn\" data-WideGridArea=\"2/2/3/3\" data-NarrowGridArea=\"3/1/4/2\" style=\"grid-area:2/2/3/3\">data</div><div class=\"PTypeNameSeparator\" data-WideGridArea=\"2/3/3/4\" data-NarrowGridArea=\"3/2/4/3\" style=\"grid-area:2/3/3/4\">:&nbsp;</div><div class=\"PType InLastParameterColumn\" data-WideGridArea=\"2/4/3/5\" data-NarrowGridArea=\"3/3/4/4\" style=\"grid-area:2/4/3/5\">Array&lt;String&gt;</div><div class=\"PAfterParameters\" data-WideGridArea=\"2/5/3/6\" data-NarrowGridArea=\"4/1/5/5\" style=\"grid-area:2/5/3/6\">): FileResult</div></div></div></div><div class=\"TTSummary\">Write a complete file from lines of text in an array.</div></div>"});