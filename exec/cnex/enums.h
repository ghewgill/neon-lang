#ifndef ENUMS_H
#define ENUMS_H

// ToDo: Should these be defined as static const uint32_t's?

#define ENUM_Mode_read              0
#define ENUM_Mode_write             1

#define ENUM_SeekBase_absolute      0
#define ENUM_SeekBase_relative      1
#define ENUM_SeekBase_fromEnd       2

#define ENUM_Platform_posix         0
#define ENUM_Platform_win32         1

#define ENUM_FileType_normal        0
#define ENUM_FileType_directory     1
#define ENUM_FileType_special       2

#define CHOICE_FindResult_notfound  0
#define CHOICE_FindResult_index     1

#define CHOICE_OpenResult_file      0
#define CHOICE_OpenResult_error     1

#define CHOICE_ReadLineResult_line  0
#define CHOICE_ReadLineResult_eof   1
#define CHOICE_ReadLineResult_error 2

#define CHOICE_RecvResult_data  0
#define CHOICE_RecvResult_eof   1
#define CHOICE_RecvResult_error 2

#endif
