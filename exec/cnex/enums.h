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

#define ENUM_Error_alreadyExists    0
#define ENUM_Error_notFound         1
#define ENUM_Error_permissionDenied 2
#define ENUM_Error_other            3

#define CHOICE_FileResult_ok        0
#define CHOICE_FileResult_error     1

#define CHOICE_FileInfoResult_info  0
#define CHOICE_FileInfoResult_error 1

#define CHOICE_BytesResult_data     0
#define CHOICE_BytesResult_error    1

#define CHOICE_LinesResult_lines    0
#define CHOICE_LinesResult_error    1

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

#define CHOICE_DecodeResult_string  0
#define CHOICE_DecodeResult_error   1

#define CHOICE_ParseNumberResult_number 0
#define CHOICE_ParseNumberResult_error  1

#endif
