#include "ast.h"

#include <array>
#include <assert.h>
#include <fstream>

#include "support.h"

namespace cli {

//#include "functions_compile_cli.inc"

std::vector<uint8_t> &operator<<(std::vector<uint8_t> &a, uint8_t u8)
{
    a.push_back(u8);
    return a;
}

std::vector<uint8_t> &operator<<(std::vector<uint8_t> &a, int16_t s16)
{
    a.push_back((s16      ) & 0xff);
    a.push_back((s16 >>  8) & 0xff);
    return a;
}

std::vector<uint8_t> &operator<<(std::vector<uint8_t> &a, uint16_t u16)
{
    a.push_back((u16      ) & 0xff);
    a.push_back((u16 >>  8) & 0xff);
    return a;
}

std::vector<uint8_t> &operator<<(std::vector<uint8_t> &a, uint32_t u32)
{
    a.push_back((u32      ) & 0xff);
    a.push_back((u32 >>  8) & 0xff);
    a.push_back((u32 >> 16) & 0xff);
    a.push_back((u32 >> 24) & 0xff);
    return a;
}

std::vector<uint8_t> &operator<<(std::vector<uint8_t> &a, uint64_t u64)
{
    a.push_back((u64      ) & 0xff);
    a.push_back((u64 >>  8) & 0xff);
    a.push_back((u64 >> 16) & 0xff);
    a.push_back((u64 >> 24) & 0xff);
    a.push_back((u64 >> 32) & 0xff);
    a.push_back((u64 >> 40) & 0xff);
    a.push_back((u64 >> 48) & 0xff);
    a.push_back((u64 >> 56) & 0xff);
    return a;
}

std::vector<uint8_t> &operator<<(std::vector<uint8_t> &a, const std::vector<uint8_t> &b)
{
    std::copy(b.begin(), b.end(), std::back_inserter(a));
    return a;
}

std::vector<uint8_t> &operator<<(std::vector<uint8_t> &a, const std::string &s)
{
    std::copy(s.begin(), s.end(), std::back_inserter(a));
    a << static_cast<uint8_t>(0);
    return a;
}

const uint8_t MS_DOS_header[128] = {
    0x4d,0x5a,0x90,0x00,0x03,0x00,0x00,0x00,
    0x04,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,
    0xb8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,
    0x0e,0x1f,0xba,0x0e,0x00,0xb4,0x09,0xcd,
    0x21,0xb8,0x01,0x4c,0xcd,0x21,0x54,0x68,
    0x69,0x73,0x20,0x70,0x72,0x6f,0x67,0x72,
    0x61,0x6d,0x20,0x63,0x61,0x6e,0x6e,0x6f,
    0x74,0x20,0x62,0x65,0x20,0x72,0x75,0x6e,
    0x20,0x69,0x6e,0x20,0x44,0x4f,0x53,0x20,
    0x6d,0x6f,0x64,0x65,0x2e,0x0d,0x0d,0x0a,
    0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

const uint16_t IMAGE_FILE_EXECUTABLE_IMAGE  = 0x0002;
const uint16_t IMAGE_FILE_32BIT_MACHINE     = 0x0100;

const uint16_t IMAGE_SUBSYSTEM_WINDOWS_CUI  = 0x3;

const uint16_t IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE = 0x0040;
const uint16_t IMAGE_DLLCHARACTERISTICS_NX_COMPAT    = 0x0100;
const uint16_t IMAGE_DLLCHARACTERISTICS_NO_SEH       = 0x0400;

const uint32_t IMG_SCN_CNT_CODE                 = 0x00000020;
const uint32_t IMG_SCN_CNT_INITIALIZED_DATA     = 0x00000040;
//const uint32_t IMG_SCN_CNT_UNINITIALIZED_DATA   = 0x00000080;
const uint32_t IMG_SCN_MEM_DISCARDABLE          = 0x02000000;
const uint32_t IMG_SCN_MEM_EXECUTE              = 0x20000000;
const uint32_t IMG_SCN_MEM_READ                 = 0x40000000;
//const uint32_t IMG_SCN_MEM_WRITE                = 0x80000000;

const uint32_t COMIMAGE_FLAGS_ILONLY = 0x00000001;

const uint16_t AssemblyFlags_Retargetable = 0x0100;

const uint16_t MethodAttributes_MemberAccess_Public = 0x0006;
const uint16_t MethodAttributes_Static              = 0x0010;

const uint32_t TypeAttributes_Visibility_Public = 0x00000001;

//const uint8_t ELEMENT_TYPE_END      = 0x00;
const uint8_t ELEMENT_TYPE_VOID     = 0x01;
const uint8_t ELEMENT_TYPE_STRING   = 0x0e;
const uint8_t ELEMENT_TYPE_OBJECT   = 0x1c;
const uint8_t ELEMENT_TYPE_SZARRAY  = 0x1d;

const uint8_t op_call  = 0x28;
const uint8_t op_ret   = 0x2A;
const uint8_t op_ldstr = 0x72;

inline uint32_t MemberRefParent_TypeRef(uint32_t i) {
    return static_cast<uint32_t>((i << 3) | 1);
}

inline uint32_t ResolutionScope_AssemblyRef(uint32_t i) {
    return static_cast<uint32_t>((i << 2) | 2);
}

struct PE_file_header {
    PE_file_header()
      : magic(0x4550),
        machine(0x14c),
        number_of_sections(0),
        time_date_stamp(0),
        pointer_to_symbol_table(0),
        number_of_symbols(0),
        optional_header_size(28+68+128),
        characteristics(IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_32BIT_MACHINE)
    {}
    uint32_t magic;
    uint16_t machine;
    uint16_t number_of_sections;
    uint32_t time_date_stamp;
    uint32_t pointer_to_symbol_table;
    uint32_t number_of_symbols;
    uint16_t optional_header_size;
    uint16_t characteristics;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << magic;
        r << machine;
        r << number_of_sections;
        r << time_date_stamp;
        r << pointer_to_symbol_table;
        r << number_of_symbols;
        r << optional_header_size;
        r << characteristics;
        return r;
    }
};

struct PE_header_standard_fields {
    PE_header_standard_fields()
      : magic(0x10b),
        lmajor(6),
        lminor(0),
        code_size(0),
        initialized_data_size(0),
        uninitialized_data_size(0),
        entry_point_rva(0),
        base_of_code(0),
        base_of_data(0)
    {}
    uint16_t magic;
    uint8_t lmajor;
    uint8_t lminor;
    uint32_t code_size;
    uint32_t initialized_data_size;
    uint32_t uninitialized_data_size;
    uint32_t entry_point_rva;
    uint32_t base_of_code;
    uint32_t base_of_data;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << magic;
        r << lmajor;
        r << lminor;
        r << code_size;
        r << initialized_data_size;
        r << uninitialized_data_size;
        r << entry_point_rva;
        r << base_of_code;
        r << base_of_data;
        return r;
    }
};

struct PE_header_Windows_NT_specific_fields {
    PE_header_Windows_NT_specific_fields()
      : image_base(0x400000),
        section_alignment(0x2000),
        file_alignment(0x200),
        os_major(4),
        os_minor(0),
        user_major(0),
        user_minor(0),
        subsys_major(4),
        subsys_minor(0),
        reserved(0),
        image_size(0),
        header_size(0),
        file_checksum(0),
        subsystem(IMAGE_SUBSYSTEM_WINDOWS_CUI),
        dll_flags(IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE | IMAGE_DLLCHARACTERISTICS_NX_COMPAT | IMAGE_DLLCHARACTERISTICS_NO_SEH),
        stack_reserve_size(0x100000),
        stack_commit_size(0x1000),
        heap_reserve_size(0x100000),
        heap_commit_size(0x1000),
        loader_flags(0),
        number_of_data_directories(0x10)
    {}

    uint32_t image_base;
    uint32_t section_alignment;
    uint32_t file_alignment;
    uint16_t os_major;
    uint16_t os_minor;
    uint16_t user_major;
    uint16_t user_minor;
    uint16_t subsys_major;
    uint16_t subsys_minor;
    uint32_t reserved;
    uint32_t image_size;
    uint32_t header_size;
    uint32_t file_checksum;
    uint16_t subsystem;
    uint16_t dll_flags;
    uint32_t stack_reserve_size;
    uint32_t stack_commit_size;
    uint32_t heap_reserve_size;
    uint32_t heap_commit_size;
    uint32_t loader_flags;
    uint32_t number_of_data_directories;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << image_base;
        r << section_alignment;
        r << file_alignment;
        r << os_major;
        r << os_minor;
        r << user_major;
        r << user_minor;
        r << subsys_major;
        r << subsys_minor;
        r << reserved;
        r << image_size;
        r << header_size;
        r << file_checksum;
        r << subsystem;
        r << dll_flags;
        r << stack_reserve_size;
        r << stack_commit_size;
        r << heap_reserve_size;
        r << heap_commit_size;
        r << loader_flags;
        r << number_of_data_directories;
        return r;
    }
};

struct PE_header_data_directories {
    PE_header_data_directories()
      : export_table_rva(0),
        export_table_size(0),
        import_table_rva(0),
        import_table_size(0),
        resource_table_rva(0),
        resource_table_size(0),
        exception_table_rva(0),
        exception_table_size(0),
        certificate_table_rva(0),
        certificate_table_size(0),
        base_relocation_table_rva(0),
        base_relocation_table_size(0),
        debug_rva(0),
        debug_size(0),
        copyright_rva(0),
        copyright_size(0),
        global_ptr_rva(0),
        global_ptr_size(0),
        tls_table_rva(0),
        tls_table_size(0),
        load_config_table_rva(0),
        load_config_table_size(0),
        bound_import_rva(0),
        bound_import_size(0),
        iat_rva(0),
        iat_size(0),
        delay_import_descriptor_rva(0),
        delay_import_descriptor_size(0),
        cli_header_rva(0),
        cli_header_size(0),
        reserved_rva(0),
        reserved_size(0)
    {}

    uint32_t export_table_rva;
    uint32_t export_table_size;
    uint32_t import_table_rva;
    uint32_t import_table_size;
    uint32_t resource_table_rva;
    uint32_t resource_table_size;
    uint32_t exception_table_rva;
    uint32_t exception_table_size;
    uint32_t certificate_table_rva;
    uint32_t certificate_table_size;
    uint32_t base_relocation_table_rva;
    uint32_t base_relocation_table_size;
    uint32_t debug_rva;
    uint32_t debug_size;
    uint32_t copyright_rva;
    uint32_t copyright_size;
    uint32_t global_ptr_rva;
    uint32_t global_ptr_size;
    uint32_t tls_table_rva;
    uint32_t tls_table_size;
    uint32_t load_config_table_rva;
    uint32_t load_config_table_size;
    uint32_t bound_import_rva;
    uint32_t bound_import_size;
    uint32_t iat_rva;
    uint32_t iat_size;
    uint32_t delay_import_descriptor_rva;
    uint32_t delay_import_descriptor_size;
    uint32_t cli_header_rva;
    uint32_t cli_header_size;
    uint32_t reserved_rva;
    uint32_t reserved_size;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << export_table_rva;
        r << export_table_size;
        r << import_table_rva;
        r << import_table_size;
        r << resource_table_rva;
        r << resource_table_size;
        r << exception_table_rva;
        r << exception_table_size;
        r << certificate_table_rva;
        r << certificate_table_size;
        r << base_relocation_table_rva;
        r << base_relocation_table_size;
        r << debug_rva;
        r << debug_size;
        r << copyright_rva;
        r << copyright_size;
        r << global_ptr_rva;
        r << global_ptr_size;
        r << tls_table_rva;
        r << tls_table_size;
        r << load_config_table_rva;
        r << load_config_table_size;
        r << bound_import_rva;
        r << bound_import_size;
        r << iat_rva;
        r << iat_size;
        r << delay_import_descriptor_rva;
        r << delay_import_descriptor_size;
        r << cli_header_rva;
        r << cli_header_size;
        r << reserved_rva;
        r << reserved_size;
        return r;
    }
};

struct PE_optional_header {
    PE_optional_header()
      : standard_fields(),
        NT_specific_fields(),
        data_directories()
    {}
    PE_header_standard_fields standard_fields;
    PE_header_Windows_NT_specific_fields NT_specific_fields;
    PE_header_data_directories data_directories;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << standard_fields.serialize();
        r << NT_specific_fields.serialize();
        r << data_directories.serialize();
        return r;
    }
};

struct Section_header {
    Section_header()
      : Name(),
        VirtualSize(0),
        VirtualAddress(0),
        SizeOfRawData(0),
        PointerToRawData(0),
        PointerToRelocations(0),
        PointerToLinenumbers(0),
        NumberOfRelocations(0),
        NumberOfLinenumbers(0),
        Characteristics(0)
    {}

    std::string Name;
    uint32_t VirtualSize;
    uint32_t VirtualAddress;
    uint32_t SizeOfRawData;
    uint32_t PointerToRawData;
    uint32_t PointerToRelocations;
    uint32_t PointerToLinenumbers;
    uint16_t NumberOfRelocations;
    uint16_t NumberOfLinenumbers;
    uint32_t Characteristics;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        assert(Name.length() <= 8);
        for (size_t i = 0; i < 8; i++) {
            if (i < Name.length()) {
                r << static_cast<uint8_t>(Name.at(i));
            } else {
                r << static_cast<uint8_t>(0);
            }
        }
        r << VirtualSize;
        r << VirtualAddress;
        r << SizeOfRawData;
        r << PointerToRawData;
        r << PointerToRelocations;
        r << PointerToLinenumbers;
        r << NumberOfRelocations;
        r << NumberOfLinenumbers;
        r << Characteristics;
        return r;
    }
};

struct Section {
    Section()
      : header(),
        data()
    {}

    Section_header header;
    std::vector<uint8_t> data;
};

struct Import_table {
    Import_table()
      : ImportLookupTable(0),
        DateTimeStamp(0),
        ForwarderChain(0),
        Name(0),
        ImportAddressTable(0)
    {}
    uint32_t ImportLookupTable;
    uint32_t DateTimeStamp;
    uint32_t ForwarderChain;
    uint32_t Name;
    uint32_t ImportAddressTable;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << ImportLookupTable;
        r << DateTimeStamp;
        r << ForwarderChain;
        r << Name;
        r << ImportAddressTable;
        return r;
    }
};

struct Import_lookup_table {
    Import_lookup_table(): HintNameTableRVA(0) {}
    uint32_t HintNameTableRVA;
    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << HintNameTableRVA;
        return r;
    }
};

struct Image_Resource_Directory {
    Image_Resource_Directory()
      : Characteristics(0),
        TimeDateStamp(0),
        MajorVersion(0),
        MinorVersion(0),
        NumberOfNamedEntries(0),
        NumberOfIdEntries(0)
    {}

    uint32_t Characteristics;
    uint32_t TimeDateStamp;
    uint16_t MajorVersion;
    uint16_t MinorVersion;
    uint16_t NumberOfNamedEntries;
    uint16_t NumberOfIdEntries;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << Characteristics;
        r << TimeDateStamp;
        r << MajorVersion;
        r << MinorVersion;
        r << NumberOfNamedEntries;
        r << NumberOfIdEntries;
        return r;
    }
};

struct CLI_header {
    CLI_header()
      : cb(72),
        MajorRuntimeVersion(2),
        MinorRuntimeVersion(0),
        MetaDataRVA(0),
        MetaDataSize(0),
        Flags(COMIMAGE_FLAGS_ILONLY),
        EntryPointToken(0),
        ResourcesRVA(0),
        ResourcesSize(0),
        StrongNameSignatureRVA(0),
        StrongNameSignatureSize(0),
        CodeManagerTableRVA(0),
        CodeManagerTableSize(0),
        VTableFixupsRVA(0),
        VTableFixupsSize(0),
        ExportAddressTableJumpsRVA(0),
        ExportAddressTableJumpsSize(0),
        ManagedNativeHeaderRVA(0),
        ManagedNativeHeaderSize(0)
    {}

    uint32_t cb;
    uint16_t MajorRuntimeVersion;
    uint16_t MinorRuntimeVersion;
    uint32_t MetaDataRVA;
    uint32_t MetaDataSize;
    uint32_t Flags;
    uint32_t EntryPointToken;
    uint32_t ResourcesRVA;
    uint32_t ResourcesSize;
    uint32_t StrongNameSignatureRVA;
    uint32_t StrongNameSignatureSize;
    uint32_t CodeManagerTableRVA;
    uint32_t CodeManagerTableSize;
    uint32_t VTableFixupsRVA;
    uint32_t VTableFixupsSize;
    uint32_t ExportAddressTableJumpsRVA;
    uint32_t ExportAddressTableJumpsSize;
    uint32_t ManagedNativeHeaderRVA;
    uint32_t ManagedNativeHeaderSize;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << cb;
        r << MajorRuntimeVersion;
        r << MinorRuntimeVersion;
        r << MetaDataRVA;
        r << MetaDataSize;
        r << Flags;
        r << EntryPointToken;
        r << ResourcesRVA;
        r << ResourcesSize;
        r << StrongNameSignatureRVA;
        r << StrongNameSignatureSize;
        r << CodeManagerTableRVA;
        r << CodeManagerTableSize;
        r << VTableFixupsRVA;
        r << VTableFixupsSize;
        r << ExportAddressTableJumpsRVA;
        r << ExportAddressTableJumpsSize;
        r << ManagedNativeHeaderRVA;
        r << ManagedNativeHeaderSize;
        return r;
    }
};

struct Stream_header {
    explicit Stream_header(const std::string &name)
      : Offset(0),
        Size(0),
        Name(name)
    {}

    uint32_t Offset;
    uint32_t Size;
    std::string Name;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << Offset;
        r << Size;
        uint32_t x = static_cast<uint32_t>((Name.length() + 1 + 3) & ~3);
        r << Name;
        for (size_t i = Name.length() + 1; i < x; i++) {
            r << static_cast<uint8_t>(0);
        }
        return r;
    }
};

struct Metadata_root {
    Metadata_root()
      : Signature(0x424a5342),
        MajorVersion(1),
        MinorVersion(1),
        Reserved(0),
        Version("v4.0.30319"),
        Flags(0)
    {}

    uint32_t Signature;
    uint16_t MajorVersion;
    uint16_t MinorVersion;
    uint32_t Reserved;
    std::string Version;
    uint16_t Flags;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << Signature;
        r << MajorVersion;
        r << MinorVersion;
        r << Reserved;
        uint32_t x = static_cast<uint32_t>((Version.length() + 1 + 3) & ~3);
        r << x;
        r << Version;
        for (size_t i = Version.length() + 1; i < x; i++) {
            r << static_cast<uint8_t>(0);
        }
        r << Flags;
        return r;
    }
};

struct Assembly {
    static const uint8_t Number = 0x20;
    Assembly()
      : HashAlgId(0),
        MajorVersion(0),
        MinorVersion(0),
        BuildNumber(0),
        RevisionNumber(0),
        Flags(0),
        PublicKey(0),
        Name(0),
        Culture(0)
    {}

    uint32_t HashAlgId;
    uint16_t MajorVersion;
    uint16_t MinorVersion;
    uint16_t BuildNumber;
    uint16_t RevisionNumber;
    uint32_t Flags;
    uint32_t PublicKey;
    uint32_t Name;
    uint32_t Culture;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        r << HashAlgId;
        r << MajorVersion;
        r << MinorVersion;
        r << BuildNumber;
        r << RevisionNumber;
        r << Flags;
        r << PublicKey;
        r << Name;
        r << Culture;
        return r;
    }
};

struct AssemblyOS {
    static const uint8_t Number = 0x22;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct AssemblyProcessor {
    static const uint8_t Number = 0x21;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct AssemblyRef {
    static const uint8_t Number = 0x23;
    AssemblyRef(uint32_t name, uint32_t publickeyortoken, uint32_t flags)
      : MajorVersion(0),
        MinorVersion(0),
        BuildNumber(0),
        RevisionNumber(0),
        Flags(flags),
        PublicKeyOrToken(publickeyortoken),
        Name(name),
        Culture(0),
        HashValue(0)
    {}

    uint16_t MajorVersion;
    uint16_t MinorVersion;
    uint16_t BuildNumber;
    uint16_t RevisionNumber;
    uint32_t Flags;
    uint32_t PublicKeyOrToken;
    uint32_t Name;
    uint32_t Culture;
    uint32_t HashValue;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        r << MajorVersion;
        r << MinorVersion;
        r << BuildNumber;
        r << RevisionNumber;
        r << Flags;
        r << PublicKeyOrToken;
        r << Name;
        r << Culture;
        r << HashValue;
        return r;
    }
};

struct AssemblyRefOS {
    static const uint8_t Number = 0x25;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct AssemblyRefProcessor {
    static const uint8_t Number = 0x24;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct ClassLayout {
    static const uint8_t Number = 0x0F;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct Constant {
    static const uint8_t Number = 0x0B;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct CustomAttribute {
    static const uint8_t Number = 0x0C;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct DeclSecurity {
    static const uint8_t Number = 0x0E;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct EventMap {
    static const uint8_t Number = 0x12;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct Event {
    static const uint8_t Number = 0x14;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct ExportedType {
    static const uint8_t Number = 0x27;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct Field {
    static const uint8_t Number = 0x04;
    Field()
      : Flags(0),
        Name(0),
        Signature(0)
    {}

    uint16_t Flags;
    uint32_t Name;
    uint32_t Signature;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        r << Flags;
        r << Name;
        r << Signature;
        return r;
    }
};

struct FieldLayout {
    static const uint8_t Number = 0x10;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct FieldMarshal {
    static const uint8_t Number = 0x0D;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct FieldRVA {
    static const uint8_t Number = 0x1D;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct File {
    static const uint8_t Number = 0x26;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct GenericParam {
    static const uint8_t Number = 0x2A;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct GenericParamConstraint {
    static const uint8_t Number = 0x2C;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct ImplMap {
    static const uint8_t Number = 0x1C;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct InterfaceImpl {
    static const uint8_t Number = 0x09;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct ManifestResource {
    static const uint8_t Number = 0x28;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct MemberRef {
    static const uint8_t Number = 0x0A;
    MemberRef(uint32_t parent, uint32_t name, uint32_t signature)
      : Parent(parent),
        Name(name),
        Signature(signature)
    {}

    uint32_t Parent;
    uint32_t Name;
    uint32_t Signature;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        r << static_cast<uint16_t>(Parent); // TODO: coded index adaptive size
        r << Name;
        r << Signature;
        return r;
    }
};

struct MethodDef {
    static const uint8_t Number = 0x06;
    MethodDef()
      : RVA(0),
        ImplFlags(0),
        MethodAttributes(0),
        Name(0),
        Signature(0),
        ParamList(0)
    {}

    uint32_t RVA;
    uint16_t ImplFlags;
    uint16_t MethodAttributes;
    uint32_t Name;
    uint32_t Signature;
    uint32_t ParamList;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &bigtable) const {
        std::vector<uint8_t> r;
        r << RVA;
        r << ImplFlags;
        r << MethodAttributes;
        r << Name;
        r << Signature;
        if (bigtable[/*Param::Number*/ 0x08]) {
            r << ParamList;
        } else {
            r << static_cast<uint16_t>(ParamList);
        }
        return r;
    }
};

struct MethodImpl {
    static const uint8_t Number = 0x19;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct MethodSemantics {
    static const uint8_t Number = 0x18;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct MethodSpec {
    static const uint8_t Number = 0x2B;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct Module {
    static const uint8_t Number = 0x00;
    Module()
      : Generation(0),
        Name(0),
        Mvid(0),
        EncId(0),
        EncBaseId(0)
    {}

    uint16_t Generation;
    uint32_t Name;
    uint32_t Mvid;
    uint32_t EncId;
    uint32_t EncBaseId;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        r << Generation;
        r << Name;
        r << Mvid;
        r << EncId;
        r << EncBaseId;
        return r;
    }
};

struct ModuleRef {
    static const uint8_t Number = 0x1A;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct NestedClass {
    static const uint8_t Number = 0x29;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct Param {
    static const uint8_t Number = 0x08;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct Property {
    static const uint8_t Number = 0x17;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct PropertyMap {
    static const uint8_t Number = 0x15;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct StandAloneSig {
    static const uint8_t Number = 0x11;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct TypeDef {
    static const uint8_t Number = 0x02;
    TypeDef()
      : Flags(0),
        TypeName(0),
        TypeNamespace(0),
        Extends(0),
        FieldList(0),
        MethodList(0)
    {}

    uint32_t Flags;
    uint32_t TypeName;
    uint32_t TypeNamespace;
    uint32_t Extends;
    uint32_t FieldList;
    uint32_t MethodList;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &bigtable) const {
        std::vector<uint8_t> r;
        r << Flags;
        r << TypeName;
        r << TypeNamespace;
        r << static_cast<uint16_t>(Extends); // TODO: coded index adaptive size
        if (bigtable[Field::Number]) {
            r << FieldList;
        } else {
            r << static_cast<uint16_t>(FieldList);
        }
        if (bigtable[MethodDef::Number]) {
            r << MethodList;
        } else {
            r << static_cast<uint16_t>(MethodList);
        }
        return r;
    }
};

struct TypeRef {
    static const uint8_t Number = 0x01;
    TypeRef(uint32_t resolutionscope, uint32_t type_name, uint32_t typenamespace)
      : ResolutionScope(resolutionscope),
        TypeName(type_name),
        TypeNamespace(typenamespace)
    {}

    uint32_t ResolutionScope;
    uint32_t TypeName;
    uint32_t TypeNamespace;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        r << static_cast<uint16_t>(ResolutionScope); // TODO: coded index adaptive size
        r << TypeName;
        r << TypeNamespace;
        return r;
    }
};

struct TypeSpec {
    static const uint8_t Number = 0x1B;

    std::vector<uint8_t> serialize(const std::array<bool, 64> &) const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct Metadata_tables {
    Metadata_tables()
      : Reserved1(0),
        MajorVersion(2),
        MinorVersion(0),
        HeapSizes(0x7),
        Reserved2(1),
        //Valid(0),
        Sorted(0),
        Module_Table(),
        TypeRef_Table(),
        TypeDef_Table(),
        Field_Table(),
        MethodDef_Table(),
        Param_Table(),
        InterfaceImpl_Table(),
        MemberRef_Table(),
        Constant_Table(),
        CustomAttribute_Table(),
        FieldMarshal_Table(),
        DeclSecurity_Table(),
        ClassLayout_Table(),
        FieldLayout_Table(),
        StandAloneSig_Table(),
        EventMap_Table(),
        Event_Table(),
        PropertyMap_Table(),
        Property_Table(),
        MethodSemantics_Table(),
        MethodImpl_Table(),
        ModuleRef_Table(),
        TypeSpec_Table(),
        ImplMap_Table(),
        FieldRVA_Table(),
        Assembly_Table(),
        AssemblyProcessor_Table(),
        AssemblyOS_Table(),
        AssemblyRef_Table(),
        AssemblyRefProcessor_Table(),
        AssemblyRefOS_Table(),
        File_Table(),
        ExportedType_Table(),
        ManifestResource_Table(),
        NestedClass_Table(),
        GenericParam_Table(),
        MethodSpec_Table(),
        GenericParamConstraint_Table()
    {}

    uint32_t Reserved1;
    uint8_t MajorVersion;
    uint8_t MinorVersion;
    uint8_t HeapSizes;
    uint8_t Reserved2;
    //uint64_t Valid;
    uint64_t Sorted;
    std::vector<Module> Module_Table;                                   // 0x00
    std::vector<TypeRef> TypeRef_Table;                                 // 0x01
    std::vector<TypeDef> TypeDef_Table;                                 // 0x02
    std::vector<Field> Field_Table;                                     // 0x04
    std::vector<MethodDef> MethodDef_Table;                             // 0x06
    std::vector<Param> Param_Table;                                     // 0x08
    std::vector<InterfaceImpl> InterfaceImpl_Table;                     // 0x09
    std::vector<MemberRef> MemberRef_Table;                             // 0x0A
    std::vector<Constant> Constant_Table;                               // 0x0B
    std::vector<CustomAttribute> CustomAttribute_Table;                 // 0x0C
    std::vector<FieldMarshal> FieldMarshal_Table;                       // 0x0D
    std::vector<DeclSecurity> DeclSecurity_Table;                       // 0x0E
    std::vector<ClassLayout> ClassLayout_Table;                         // 0x0F
    std::vector<FieldLayout> FieldLayout_Table;                         // 0x10
    std::vector<StandAloneSig> StandAloneSig_Table;                     // 0x11
    std::vector<EventMap> EventMap_Table;                               // 0x12
    std::vector<Event> Event_Table;                                     // 0x14
    std::vector<PropertyMap> PropertyMap_Table;                         // 0x15
    std::vector<Property> Property_Table;                               // 0x17
    std::vector<MethodSemantics> MethodSemantics_Table;                 // 0x18
    std::vector<MethodImpl> MethodImpl_Table;                           // 0x19
    std::vector<ModuleRef> ModuleRef_Table;                             // 0x1A
    std::vector<TypeSpec> TypeSpec_Table;                               // 0x1B
    std::vector<ImplMap> ImplMap_Table;                                 // 0x1C
    std::vector<FieldRVA> FieldRVA_Table;                               // 0x1D
    std::vector<Assembly> Assembly_Table;                               // 0x20
    std::vector<AssemblyProcessor> AssemblyProcessor_Table;             // 0x21
    std::vector<AssemblyOS> AssemblyOS_Table;                           // 0x22
    std::vector<AssemblyRef> AssemblyRef_Table;                         // 0x23
    std::vector<AssemblyRefProcessor> AssemblyRefProcessor_Table;       // 0x24
    std::vector<AssemblyRefOS> AssemblyRefOS_Table;                     // 0x25
    std::vector<File> File_Table;                                       // 0x26
    std::vector<ExportedType> ExportedType_Table;                       // 0x27
    std::vector<ManifestResource> ManifestResource_Table;               // 0x28
    std::vector<NestedClass> NestedClass_Table;                         // 0x29
    std::vector<GenericParam> GenericParam_Table;                       // 0x2A
    std::vector<MethodSpec> MethodSpec_Table;                           // 0x2B
    std::vector<GenericParamConstraint> GenericParamConstraint_Table;   // 0x2C

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << Reserved1;
        r << MajorVersion;
        r << MinorVersion;
        r << HeapSizes;
        r << Reserved2;
        uint64_t valid = 0;
        std::array<bool, 64> bigtable;
        update_flags(valid, bigtable, Module_Table);
        update_flags(valid, bigtable, TypeRef_Table);
        update_flags(valid, bigtable, TypeDef_Table);
        update_flags(valid, bigtable, Field_Table);
        update_flags(valid, bigtable, MethodDef_Table);
        update_flags(valid, bigtable, Param_Table);
        update_flags(valid, bigtable, InterfaceImpl_Table);
        update_flags(valid, bigtable, MemberRef_Table);
        update_flags(valid, bigtable, Constant_Table);
        update_flags(valid, bigtable, CustomAttribute_Table);
        update_flags(valid, bigtable, FieldMarshal_Table);
        update_flags(valid, bigtable, DeclSecurity_Table);
        update_flags(valid, bigtable, ClassLayout_Table);
        update_flags(valid, bigtable, FieldLayout_Table);
        update_flags(valid, bigtable, StandAloneSig_Table);
        update_flags(valid, bigtable, EventMap_Table);
        update_flags(valid, bigtable, Event_Table);
        update_flags(valid, bigtable, PropertyMap_Table);
        update_flags(valid, bigtable, Property_Table);
        update_flags(valid, bigtable, MethodSemantics_Table);
        update_flags(valid, bigtable, MethodImpl_Table);
        update_flags(valid, bigtable, ModuleRef_Table);
        update_flags(valid, bigtable, TypeSpec_Table);
        update_flags(valid, bigtable, ImplMap_Table);
        update_flags(valid, bigtable, FieldRVA_Table);
        update_flags(valid, bigtable, Assembly_Table);
        update_flags(valid, bigtable, AssemblyProcessor_Table);
        update_flags(valid, bigtable, AssemblyOS_Table);
        update_flags(valid, bigtable, AssemblyRef_Table);
        update_flags(valid, bigtable, AssemblyRefProcessor_Table);
        update_flags(valid, bigtable, AssemblyRefOS_Table);
        update_flags(valid, bigtable, File_Table);
        update_flags(valid, bigtable, ExportedType_Table);
        update_flags(valid, bigtable, ManifestResource_Table);
        update_flags(valid, bigtable, NestedClass_Table);
        update_flags(valid, bigtable, GenericParam_Table);
        update_flags(valid, bigtable, MethodSpec_Table);
        update_flags(valid, bigtable, GenericParamConstraint_Table);
        r << valid;
        r << Sorted;
        if (not Module_Table.empty())                r << static_cast<uint32_t>(Module_Table.size());
        if (not TypeRef_Table.empty())               r << static_cast<uint32_t>(TypeRef_Table.size());
        if (not TypeDef_Table.empty())               r << static_cast<uint32_t>(TypeDef_Table.size());
        if (not Field_Table.empty())                 r << static_cast<uint32_t>(Field_Table.size());
        if (not MethodDef_Table.empty())             r << static_cast<uint32_t>(MethodDef_Table.size());
        if (not Param_Table.empty())                 r << static_cast<uint32_t>(Param_Table.size());
        if (not InterfaceImpl_Table.empty())         r << static_cast<uint32_t>(InterfaceImpl_Table.size());
        if (not MemberRef_Table.empty())             r << static_cast<uint32_t>(MemberRef_Table.size());
        if (not Constant_Table.empty())              r << static_cast<uint32_t>(Constant_Table.size());
        if (not CustomAttribute_Table.empty())       r << static_cast<uint32_t>(CustomAttribute_Table.size());
        if (not FieldMarshal_Table.empty())          r << static_cast<uint32_t>(FieldMarshal_Table.size());
        if (not DeclSecurity_Table.empty())          r << static_cast<uint32_t>(DeclSecurity_Table.size());
        if (not ClassLayout_Table.empty())           r << static_cast<uint32_t>(ClassLayout_Table.size());
        if (not FieldLayout_Table.empty())           r << static_cast<uint32_t>(FieldLayout_Table.size());
        if (not StandAloneSig_Table.empty())         r << static_cast<uint32_t>(StandAloneSig_Table.size());
        if (not EventMap_Table.empty())              r << static_cast<uint32_t>(EventMap_Table.size());
        if (not Event_Table.empty())                 r << static_cast<uint32_t>(Event_Table.size());
        if (not PropertyMap_Table.empty())           r << static_cast<uint32_t>(PropertyMap_Table.size());
        if (not Property_Table.empty())              r << static_cast<uint32_t>(Property_Table.size());
        if (not MethodSemantics_Table.empty())       r << static_cast<uint32_t>(MethodSemantics_Table.size());
        if (not MethodImpl_Table.empty())            r << static_cast<uint32_t>(MethodImpl_Table.size());
        if (not ModuleRef_Table.empty())             r << static_cast<uint32_t>(ModuleRef_Table.size());
        if (not TypeSpec_Table.empty())              r << static_cast<uint32_t>(TypeSpec_Table.size());
        if (not ImplMap_Table.empty())               r << static_cast<uint32_t>(ImplMap_Table.size());
        if (not FieldRVA_Table.empty())              r << static_cast<uint32_t>(FieldRVA_Table.size());
        if (not Assembly_Table.empty())              r << static_cast<uint32_t>(Assembly_Table.size());
        if (not AssemblyProcessor_Table.empty())     r << static_cast<uint32_t>(AssemblyProcessor_Table.size());
        if (not AssemblyOS_Table.empty())            r << static_cast<uint32_t>(AssemblyOS_Table.size());
        if (not AssemblyRef_Table.empty())           r << static_cast<uint32_t>(AssemblyRef_Table.size());
        if (not AssemblyRefProcessor_Table.empty())  r << static_cast<uint32_t>(AssemblyRefProcessor_Table.size());
        if (not AssemblyRefOS_Table.empty())         r << static_cast<uint32_t>(AssemblyRefOS_Table.size());
        if (not File_Table.empty())                  r << static_cast<uint32_t>(File_Table.size());
        if (not ExportedType_Table.empty())          r << static_cast<uint32_t>(ExportedType_Table.size());
        if (not ManifestResource_Table.empty())      r << static_cast<uint32_t>(ManifestResource_Table.size());
        if (not NestedClass_Table.empty())           r << static_cast<uint32_t>(NestedClass_Table.size());
        if (not GenericParam_Table.empty())          r << static_cast<uint32_t>(GenericParam_Table.size());
        if (not MethodSpec_Table.empty())            r << static_cast<uint32_t>(MethodSpec_Table.size());
        if (not GenericParamConstraint_Table.empty())r << static_cast<uint32_t>(GenericParamConstraint_Table.size());
        for (auto x: Module_Table) { r << x.serialize(bigtable); }
        for (auto x: TypeRef_Table) { r << x.serialize(bigtable); }
        for (auto x: TypeDef_Table) { r << x.serialize(bigtable); }
        for (auto x: Field_Table) { r << x.serialize(bigtable); }
        for (auto x: MethodDef_Table) { r << x.serialize(bigtable); }
        for (auto x: Param_Table) { r << x.serialize(bigtable); }
        for (auto x: InterfaceImpl_Table) { r << x.serialize(bigtable); }
        for (auto x: MemberRef_Table) { r << x.serialize(bigtable); }
        for (auto x: Constant_Table) { r << x.serialize(bigtable); }
        for (auto x: CustomAttribute_Table) { r << x.serialize(bigtable); }
        for (auto x: FieldMarshal_Table) { r << x.serialize(bigtable); }
        for (auto x: DeclSecurity_Table) { r << x.serialize(bigtable); }
        for (auto x: ClassLayout_Table) { r << x.serialize(bigtable); }
        for (auto x: FieldLayout_Table) { r << x.serialize(bigtable); }
        for (auto x: StandAloneSig_Table) { r << x.serialize(bigtable); }
        for (auto x: EventMap_Table) { r << x.serialize(bigtable); }
        for (auto x: Event_Table) { r << x.serialize(bigtable); }
        for (auto x: PropertyMap_Table) { r << x.serialize(bigtable); }
        for (auto x: Property_Table) { r << x.serialize(bigtable); }
        for (auto x: MethodSemantics_Table) { r << x.serialize(bigtable); }
        for (auto x: MethodImpl_Table) { r << x.serialize(bigtable); }
        for (auto x: ModuleRef_Table) { r << x.serialize(bigtable); }
        for (auto x: TypeSpec_Table) { r << x.serialize(bigtable); }
        for (auto x: ImplMap_Table) { r << x.serialize(bigtable); }
        for (auto x: FieldRVA_Table) { r << x.serialize(bigtable); }
        for (auto x: Assembly_Table) { r << x.serialize(bigtable); }
        for (auto x: AssemblyProcessor_Table) { r << x.serialize(bigtable); }
        for (auto x: AssemblyOS_Table) { r << x.serialize(bigtable); }
        for (auto x: AssemblyRef_Table) { r << x.serialize(bigtable); }
        for (auto x: AssemblyRefProcessor_Table) { r << x.serialize(bigtable); }
        for (auto x: AssemblyRefOS_Table) { r << x.serialize(bigtable); }
        for (auto x: File_Table) { r << x.serialize(bigtable); }
        for (auto x: ExportedType_Table) { r << x.serialize(bigtable); }
        for (auto x: ManifestResource_Table) { r << x.serialize(bigtable); }
        for (auto x: NestedClass_Table) { r << x.serialize(bigtable); }
        for (auto x: GenericParam_Table) { r << x.serialize(bigtable); }
        for (auto x: MethodSpec_Table) { r << x.serialize(bigtable); }
        for (auto x: GenericParamConstraint_Table) { r << x.serialize(bigtable); };
        return r;
    }

    template<typename T> static void update_flags(uint64_t &valid, std::array<bool, 64> &bigtable, const std::vector<T> &table) {
        if (not table.empty()) {
            valid |= (1LL << T::Number);
        }
        bigtable[T::Number] = table.size() > 0x10000;
    }
};

struct Metadata {
    Metadata()
      : root(),
        Strings_Stream("#Strings"),
        Userstring_Stream("#US"),
        Blob_Stream("#Blob"),
        Guid_Stream("#GUID"),
        Tables_Stream("#~"),
        StringsData(),
        UserstringData(),
        BlobData(),
        GuidData(),
        Tables()
    {
        StringsData.push_back(0);
        UserstringData.push_back(0);
        BlobData.push_back(0);
    }

    Metadata_root root;
    Stream_header Strings_Stream;
    Stream_header Userstring_Stream;
    Stream_header Blob_Stream;
    Stream_header Guid_Stream;
    Stream_header Tables_Stream;
    std::vector<uint8_t> StringsData;
    std::vector<uint8_t> UserstringData;
    std::vector<uint8_t> BlobData;
    std::vector<uint8_t> GuidData;
    Metadata_tables Tables;

    uint32_t String(const std::string &s) {
        uint32_t r = static_cast<uint32_t>(StringsData.size());
        StringsData << s;
        return r;
    }

    uint32_t Userstring(const std::string &s) {
        uint32_t r = static_cast<uint32_t>(UserstringData.size());
        // TODO: proper convert to UTF-16
        assert(s.length() <= 0x3f);
        UserstringData.push_back((2*s.length() + 1) & 0xff); // TODO: encode length
        for (auto c: s) {
            UserstringData << static_cast<uint16_t>(c);
        }
        UserstringData.push_back(0);
        return 0x70000000 | r;
    }

    uint32_t Blob(const std::vector<uint8_t> &a) {
        uint32_t r = static_cast<uint32_t>(BlobData.size());
        assert(a.size() <= 0x7f);
        BlobData.push_back(a.size() & 0xff); // TODO: encode length
        BlobData << a;
        return r;
    }

    uint32_t Guid(const char *g) {
        uint32_t r = static_cast<uint32_t>(1 + GuidData.size() / 16);
        for (int i = 0; i < 16; i++) {
            GuidData.push_back(*g++);
        }
        return r;
    }

    uint32_t AssemblyRefIndex(const AssemblyRef &ar) {
        Tables.AssemblyRef_Table.push_back(ar);
        return static_cast<uint32_t>(Tables.AssemblyRef_Table.size());
    }
    uint32_t AssemblyRefToken(const AssemblyRef &ar) {
        return static_cast<uint32_t>((AssemblyRef::Number << 24) | AssemblyRefIndex(ar));
    }

    uint32_t MemberRefIndex(const MemberRef &mr) {
        Tables.MemberRef_Table.push_back(mr);
        return static_cast<uint32_t>(Tables.MemberRef_Table.size());
    }
    uint32_t MemberRefToken(const MemberRef &mr) {
        return static_cast<uint32_t>((MemberRef::Number << 24) | MemberRefIndex(mr));
    }

    uint32_t TypeRefIndex(const TypeRef &tr) {
        Tables.TypeRef_Table.push_back(tr);
        return static_cast<uint32_t>(Tables.TypeRef_Table.size());
    }
    uint32_t TypeRefToken(const TypeRef &tr) {
        return static_cast<uint32_t>((TypeRef::Number << 24) | TypeRefIndex(tr));
    }

    void calculate_offsets() {
        size_t offset = root.serialize().size() + sizeof(uint16_t);
        offset += Strings_Stream.serialize().size();
        offset += Userstring_Stream.serialize().size();
        offset += Blob_Stream.serialize().size();
        offset += Guid_Stream.serialize().size();
        offset += Tables_Stream.serialize().size();
        Strings_Stream.Offset = static_cast<uint32_t>(offset);
        Strings_Stream.Size = static_cast<uint32_t>(StringsData.size());
        offset += Strings_Stream.Size;
        Userstring_Stream.Offset = static_cast<uint32_t>(offset);
        Userstring_Stream.Size = static_cast<uint32_t>(UserstringData.size());
        offset += Userstring_Stream.Size;
        Blob_Stream.Offset = static_cast<uint32_t>(offset);
        Blob_Stream.Size = static_cast<uint32_t>(BlobData.size());
        offset += Blob_Stream.Size;
        Guid_Stream.Offset = static_cast<uint32_t>(offset);
        Guid_Stream.Size = static_cast<uint32_t>(GuidData.size());
        offset += Guid_Stream.Size;
        Tables_Stream.Offset = static_cast<uint32_t>(offset);
        Tables_Stream.Size = static_cast<uint32_t>(Tables.serialize().size());
        offset += Tables_Stream.Size;
    }

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << root.serialize();
        r << static_cast<uint16_t>(5);
        r << Strings_Stream.serialize();
        r << Userstring_Stream.serialize();
        r << Blob_Stream.serialize();
        r << Guid_Stream.serialize();
        r << Tables_Stream.serialize();
        r << StringsData;
        r << UserstringData;
        r << BlobData;
        r << GuidData;
        r << Tables.serialize();
        return r;
    }
};

std::vector<uint8_t> compressUnsigned(uint32_t x)
{
    assert(x <= 0x7f);
    std::vector<uint8_t> r;
    r.push_back(x & 0xff);
    return r;
}

class MethodDefSig {
public:
    MethodDefSig() {}

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        // TODO: this is only appropriate for void Main(string[])
        r << static_cast<uint8_t>(0);
        r << compressUnsigned(1);
        r << ELEMENT_TYPE_VOID;
        r << ELEMENT_TYPE_SZARRAY << ELEMENT_TYPE_STRING;
        return r;
    }
};

class MethodRefSig_WriteLine {
public:
    MethodRefSig_WriteLine() {}

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        // TODO: this is only appropriate for void WriteLine(object)
        r << static_cast<uint8_t>(0);
        r << compressUnsigned(1);
        r << ELEMENT_TYPE_VOID;
        r << ELEMENT_TYPE_OBJECT;
        return r;
    }
};

class MethodRefSig_makeString {
public:
    MethodRefSig_makeString() {}

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        // TODO: this is only appropriate for void WriteLine(object)
        r << static_cast<uint8_t>(0);
        r << compressUnsigned(1);
        r << ELEMENT_TYPE_OBJECT;
        r << ELEMENT_TYPE_STRING;
        return r;
    }
};

struct MethodHeader {
    MethodHeader()
      : Flags(0x3),
        Size(3),
        MaxStack(0),
        CodeSize(0),
        LocalVarSigTok(0)
    {}

    uint16_t Flags;
    uint8_t Size;
    uint16_t MaxStack;
    uint32_t CodeSize;
    uint32_t LocalVarSigTok;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << static_cast<uint16_t>(Flags | (Size << 12));
        r << MaxStack;
        r << CodeSize;
        r << LocalVarSigTok;
        return r;
    }
};

class ExecutableFile {
public:
    ExecutableFile(const std::string &path, const std::string &name): path(path), name(name), poh(), text(), reloc(), md() {
        poh.standard_fields.base_of_code = 0x2000;
        poh.NT_specific_fields.header_size = 0x200;
    }
    ExecutableFile(const ExecutableFile &) = delete;
    ExecutableFile &operator=(const ExecutableFile &) = delete;
    const std::string path;
    const std::string name;
    PE_optional_header poh;
    Section text;
    //Section rsrc;
    Section reloc;
    Metadata md;

    std::vector<uint8_t> finalize() {
        Import_lookup_table ilt;
        ilt.HintNameTableRVA = static_cast<uint32_t>(poh.standard_fields.base_of_code + text.data.size());
        text.data << static_cast<uint16_t>(0);
        text.data << "_CorExeMain";

        while (text.data.size() % 4 != 0) {
            text.data << static_cast<uint8_t>(0);
        }
        Import_table it;
        it.ImportLookupTable = static_cast<uint32_t>(poh.standard_fields.base_of_code + text.data.size());
        text.data << ilt.serialize();
        text.data << Import_lookup_table().serialize();
        it.Name = static_cast<uint32_t>(poh.standard_fields.base_of_code + text.data.size());
        text.data << "mscoree.dll";
        while (text.data.size() % 4 != 0) {
            text.data << static_cast<uint8_t>(0);
        }
        it.ImportAddressTable = static_cast<uint32_t>(poh.standard_fields.base_of_code + text.data.size());
        poh.data_directories.iat_rva = it.ImportAddressTable;
        poh.data_directories.iat_size = static_cast<uint32_t>(8);
        text.data << ilt.serialize();
        text.data << Import_lookup_table().serialize();
        while (text.data.size() % 4 != 0) {
            text.data << static_cast<uint8_t>(0);
        }
        poh.data_directories.import_table_rva = static_cast<uint32_t>(poh.standard_fields.base_of_code + text.data.size());
        text.data << it.serialize();
        text.data << Import_table().serialize();
        poh.data_directories.import_table_size = static_cast<uint32_t>(poh.standard_fields.base_of_code + text.data.size() - poh.data_directories.import_table_rva);

        Assembly assembly;
        assembly.Name = md.String("hello");
        md.Tables.Assembly_Table.push_back(assembly);
        uint32_t entry_point_index = static_cast<uint32_t>(md.Tables.MethodDef_Table.size());
        CLI_header ch;
        ch.cb = 72;
        while (text.data.size() % 4 != 0) {
            text.data << static_cast<uint8_t>(0);
        }
        ch.MetaDataRVA = static_cast<uint32_t>(poh.standard_fields.base_of_code + text.data.size());
        md.calculate_offsets();
        text.data << md.serialize();
        ch.MetaDataSize = static_cast<uint32_t>(poh.standard_fields.base_of_code + text.data.size() - ch.MetaDataRVA);
        ch.EntryPointToken = static_cast<uint32_t>((MethodDef::Number << 24) | entry_point_index);
        while (text.data.size() % 4 != 0) {
            text.data << static_cast<uint8_t>(0);
        }
        poh.data_directories.cli_header_rva = static_cast<uint32_t>(poh.standard_fields.base_of_code + text.data.size());
        text.data << ch.serialize();
        poh.data_directories.cli_header_size = static_cast<uint32_t>(poh.standard_fields.base_of_code + text.data.size() - poh.data_directories.cli_header_rva);
        text.data << std::vector<uint8_t>(64); // Add padding that mono seems to expect.

        poh.standard_fields.entry_point_rva = static_cast<uint32_t>(poh.standard_fields.base_of_code + text.data.size());
        text.data << static_cast<uint8_t>(0xff);
        text.data << static_cast<uint8_t>(0x25);
        text.data << poh.NT_specific_fields.image_base + it.ImportAddressTable;
        //uint32_t code_size_rounded = (text.data.size() + 0x200-1) & ~0x1ff;
        //poh.standard_fields.code_size = code_size_rounded;
        //uint32_t code_section_rounded = (code_size_rounded + 0x2000-1) & ~0x1fff;

        text.header.Name = ".text";
        text.header.Characteristics = IMG_SCN_CNT_CODE | IMG_SCN_MEM_EXECUTE | IMG_SCN_MEM_READ;

        //rsrc.header.Name = ".rsrc";
        //rsrc.header.Characteristics = IMG_SCN_CNT_INITIALIZED_DATA | IMG_SCN_MEM_READ;
        //rsrc.data << Image_Resource_Directory().serialize();

        reloc.header.Name = ".reloc";
        reloc.header.Characteristics = IMG_SCN_CNT_INITIALIZED_DATA | IMG_SCN_MEM_READ | IMG_SCN_MEM_DISCARDABLE;
        reloc.data << static_cast<uint32_t>(0x2000);
        reloc.data << static_cast<uint32_t>(12);
        reloc.data << static_cast<uint16_t>(0x3000 | (poh.standard_fields.entry_point_rva + 2 - poh.standard_fields.base_of_code));
        reloc.data << static_cast<uint16_t>(0);

        std::vector<Section *> sections {&text, &reloc};
        uint32_t virtual_address = poh.standard_fields.base_of_code;
        uint32_t pointer_to_raw_data = 0x200;
        for (auto s: sections) {
            uint32_t data_size_rounded = (s->data.size() + 0x200-1) & ~0x1ff;
            if (s->header.Name == ".text") {
                poh.standard_fields.code_size = data_size_rounded;
            } else if (s->header.Name == ".rsrc") {
                poh.standard_fields.base_of_data = virtual_address;
                poh.standard_fields.initialized_data_size += data_size_rounded;
                poh.data_directories.resource_table_rva = virtual_address;
                poh.data_directories.resource_table_size = static_cast<uint32_t>(s->data.size());
            } else if (s->header.Name == ".reloc") {
                poh.standard_fields.initialized_data_size += data_size_rounded;
                poh.data_directories.base_relocation_table_rva = virtual_address;
                poh.data_directories.base_relocation_table_size = static_cast<uint32_t>(s->data.size());
            }
            s->header.VirtualSize = static_cast<uint32_t>(s->data.size());
            s->header.VirtualAddress = virtual_address;
            s->header.SizeOfRawData = data_size_rounded;
            s->header.PointerToRawData = pointer_to_raw_data;
            virtual_address += (s->data.size() + 0x2000-1) & ~0x1fff;
            pointer_to_raw_data += data_size_rounded;
        }

        std::vector<uint8_t> r;
        std::copy(MS_DOS_header, MS_DOS_header+sizeof(MS_DOS_header), std::back_inserter(r));
        PE_file_header pfh;
        pfh.number_of_sections = static_cast<uint16_t>(sections.size());
        r << pfh.serialize();
        poh.NT_specific_fields.image_size = virtual_address;
        r << poh.serialize();
        for (auto s: sections) {
            r << s->header.serialize();
        }
        for (auto s: sections) {
            while (r.size() % 0x200 != 0) {
                r << static_cast<uint8_t>(0);
            }
            r << s->data;
        }
        while (r.size() % 0x200 != 0) {
            r << static_cast<uint8_t>(0);
        }
        return r;
    }
};

class ClassContext {
public:
    void unimplemented(const std::string &info) {
        internal_error(info);
    }
};

class Context {
public:
    class Label {
        friend class Context;
    public:
        Label(): fixups(), target(UINT16_MAX) {}
    private:
        std::vector<uint16_t> fixups;
        uint16_t target;
    public:
        uint16_t get_target() {
            if (target == UINT16_MAX) {
                internal_error("Label::get_target");
            }
            return target;
        }
    };
    class LoopLabels {
    public:
        LoopLabels(): exit(nullptr), next(nullptr) {}
        LoopLabels(Label *exit, Label *next): exit(exit), next(next) {}
        Label *exit;
        Label *next;
    };
public:
    Context(ExecutableFile &ef, const std::string &name): ef(ef), md(ef.md), name(name), method(), code(), label_exit(), loop_labels() {
        method.RVA = static_cast<uint32_t>(ef.poh.standard_fields.base_of_code + ef.text.data.size());
    }
    Context(const Context &) = delete;
    Context &operator=(const Context &) = delete;
    ExecutableFile &ef;
    Metadata &md;
    std::string name;
    MethodDef method;
    std::vector<uint8_t> code;
    Label label_exit;
    std::map<size_t, LoopLabels> loop_labels;

    Label create_label() {
        return Label();
    }
    void emit_jump(uint8_t, Label &) {
        unimplemented("emit_jump");
    }
    void jump_target(Label &) {
        unimplemented("emit_jump");
    }
    void add_loop_labels(unsigned int loop_id, Label &exit, Label &next) {
        loop_labels[loop_id] = LoopLabels(&exit, &next);
    }
    void remove_loop_labels(unsigned int loop_id) {
        loop_labels.erase(loop_id);
    }
    Label &get_exit_label(unsigned int loop_id) {
        if (loop_labels.find(loop_id) == loop_labels.end()) {
            internal_error("loop_id not found");
        }
        return *loop_labels[loop_id].exit;
    }
    Label &get_next_label(unsigned int loop_id) {
        if (loop_labels.find(loop_id) == loop_labels.end()) {
            internal_error("loop_id not found");
        }
        return *loop_labels[loop_id].next;
    }
    void unimplemented(const std::string &info) {
        internal_error(info);
    }
    std::vector<uint8_t> finalize() {
        std::vector<uint8_t> r;
        MethodHeader mh;
        mh.MaxStack = 8;
        mh.CodeSize = static_cast<uint32_t>(code.size());
        r << mh.serialize();
        r << code;
        method.MethodAttributes = MethodAttributes_MemberAccess_Public | MethodAttributes_Static;
        method.Name = md.String(name);
        method.Signature = md.Blob(MethodDefSig().serialize());
        method.ParamList = static_cast<uint16_t>(1 + md.Tables.Param_Table.size());
        md.Tables.MethodDef_Table.push_back(method);
        return r;
    }
};

class Type;
class Variable;
class Expression;
class Statement;

static std::map<const ast::Type *, Type *> g_type_cache;
static std::map<const ast::Variable *, Variable *> g_variable_cache;
static std::map<const ast::Expression *, Expression *> g_expression_cache;
static std::map<const ast::Statement *, Statement *> g_statement_cache;

class Type {
public:
    Type(const ast::Type *t, const std::string &classname, const std::string &jtype = ""): classname(classname), jtype(jtype.empty() ? "L" + classname + ";" : jtype) {
        g_type_cache[t] = this;
    }
    Type(const Type &) = delete;
    Type &operator=(const Type &) = delete;
    virtual ~Type() {}
    const std::string classname;
    const std::string jtype;
    virtual void generate_class(Context &) const {}
    virtual void generate_default(Context &context) const = 0;
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const { internal_error("Type::generate_call"); }
};

Type *transform(const ast::Type *t);

class Variable {
public:
    explicit Variable(const ast::Variable *v): type(transform(v->type)) {
        g_variable_cache[v] = this;
    }
    Variable(const Variable &) = delete;
    Variable &operator=(const Variable &) = delete;
    virtual ~Variable() {}
    const Type *type;
    virtual void generate_decl(ClassContext &context, bool exported) const = 0;
    virtual void generate_load(Context &context) const = 0;
    virtual void generate_store(Context &context) const = 0;
    virtual void generate_call(Context &context, const std::vector<const Expression *> &args) const = 0;
};

Variable *transform(const ast::Variable *v);

class Expression {
public:
    explicit Expression(const ast::Expression *node): type(transform(node->type)) {
        g_expression_cache[node] = this;
    }
    Expression(const Expression &) = delete;
    Expression &operator=(const Expression &) = delete;
    virtual ~Expression() {}
    const Type *type;
    virtual void generate(Context &context) const = 0;
    virtual void generate_call(Context &context, const std::vector<const Expression *> &) const = 0;
    virtual void generate_store(Context &context) const = 0;
};

Expression *transform(const ast::Expression *e);

class Statement {
public:
    explicit Statement(const ast::Statement *s) {
        g_statement_cache[s] = this;
    }
    virtual ~Statement() {}
    virtual void generate(Context &context) const = 0;
};

Statement *transform(const ast::Statement *s);

class TypeNothing: public Type {
public:
    explicit TypeNothing(const ast::TypeNothing *tn): Type(tn, "V", "V"), tn(tn) {}
    TypeNothing(const TypeNothing &) = delete;
    TypeNothing &operator=(const TypeNothing &) = delete;
    const ast::TypeNothing *tn;
    virtual void generate_default(Context &) const override { internal_error("TypeNothing"); }
};

class TypeDummy: public Type {
public:
    explicit TypeDummy(const ast::TypeDummy *td): Type(td, ""), td(td) {}
    TypeDummy(const TypeDummy &) = delete;
    TypeDummy &operator=(const TypeDummy &) = delete;
    const ast::TypeDummy *td;
    virtual void generate_default(Context &) const override { internal_error("TypeDummy"); }
};

class TypeBoolean: public Type {
public:
    explicit TypeBoolean(const ast::TypeBoolean *tb): Type(tb, "java/lang/Boolean"), tb(tb) {}
    TypeBoolean(const TypeBoolean &) = delete;
    TypeBoolean &operator=(const TypeBoolean &) = delete;
    const ast::TypeBoolean *tb;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeBoolean");
    }
};

class TypeNumber: public Type {
public:
    explicit TypeNumber(const ast::TypeNumber *tn): Type(tn, "neon/type/Number"), tn(tn) {}
    TypeNumber(const TypeNumber &) = delete;
    TypeNumber &operator=(const TypeNumber &) = delete;
    const ast::TypeNumber *tn;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeNumber");
    }
};

class TypeString: public Type {
public:
    explicit TypeString(const ast::TypeString *ts): Type(ts, "java/lang/String"), ts(ts) {}
    TypeString(const TypeString &) = delete;
    TypeString &operator=(const TypeString &) = delete;
    const ast::TypeString *ts;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeString");
    }
};

class TypeBytes: public Type {
public:
    explicit TypeBytes(const ast::TypeBytes *tb): Type(tb, "[B", "[B"), tb(tb) {}
    TypeBytes(const TypeBytes &) = delete;
    TypeBytes &operator=(const TypeBytes &) = delete;
    const ast::TypeBytes *tb;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeBytes");
    }
};

class TypeObject: public Type {
public:
    explicit TypeObject(const ast::TypeObject *to): Type(to, "object"), to(to) {}
    TypeObject(const TypeObject &) = delete;
    TypeObject &operator=(const TypeObject &) = delete;
    const ast::TypeObject *to;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeObject");
    }
};

class TypeFunction: public Type {
public:
    explicit TypeFunction(const ast::TypeFunction *tf): Type(tf, ""), tf(tf), returntype(transform(tf->returntype)), paramtypes() {
        for (auto p: tf->params) {
            paramtypes.push_back(std::make_pair(p->mode, transform(p->type)));
        }
    }
    TypeFunction(const TypeFunction &) = delete;
    TypeFunction &operator=(const TypeFunction &) = delete;
    const ast::TypeFunction *tf;
    const Type *returntype;
    std::vector<std::pair<ast::ParameterType::Mode, const Type *>> paramtypes;
    virtual void generate_default(Context &) const override { internal_error("TypeFunction"); }
};

class TypeArray: public Type {
public:
    explicit TypeArray(const ast::TypeArray *ta): Type(ta, "neon/type/Array"), ta(ta), elementtype(transform(ta->elementtype)) {}
    TypeArray(const TypeArray &) = delete;
    TypeArray &operator=(const TypeArray &) = delete;
    const ast::TypeArray *ta;
    const Type *elementtype;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeAray");
    }
};

class TypeDictionary: public Type {
public:
    explicit TypeDictionary(const ast::TypeDictionary *td): Type(td, "java/util/HashMap"), td(td), elementtype(transform(td->elementtype)) {}
    TypeDictionary(const TypeDictionary &) = delete;
    TypeDictionary &operator=(const TypeDictionary &) = delete;
    const ast::TypeDictionary *td;
    const Type *elementtype;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeDictionary");
    }
};

class TypeRecord: public Type {
public:
    explicit TypeRecord(const ast::TypeRecord *tr): Type(tr, tr->module + "$" + tr->name), tr(tr), field_types() {
        for (auto f: tr->fields) {
            field_types.push_back(transform(f.type));
        }
    }
    TypeRecord(const TypeRecord &) = delete;
    TypeRecord &operator=(const TypeRecord &) = delete;
    const ast::TypeRecord *tr;
    std::vector<const Type *> field_types;

    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeRecord");
    }
};

class TypePointer: public Type {
public:
    explicit TypePointer(const ast::TypePointer *tp): Type(tp, tp->reftype != nullptr ? transform(tp->reftype)->classname : "java/lang/Object"), tp(tp) {}
    TypePointer(const TypePointer &) = delete;
    TypePointer &operator=(const TypePointer &) = delete;
    const ast::TypePointer *tp;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypePointer");
    }
};

class TypeFunctionPointer: public Type {
public:
    explicit TypeFunctionPointer(const ast::TypeFunctionPointer *fp): Type(fp, "java/lang/reflect/Method"), fp(fp), functype(dynamic_cast<const TypeFunction *>(transform(fp->functype))) {}
    TypeFunctionPointer(const TypeFunctionPointer &) = delete;
    TypeFunctionPointer &operator=(const TypeFunctionPointer &) = delete;
    const ast::TypeFunctionPointer *fp;
    const TypeFunction *functype;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeFunctionPointer");
    }
    virtual void generate_call(Context &context, const std::vector<const Expression *> &) const override {
        context.unimplemented("TypeFunctionPointer");
    }
};

class TypeEnum: public Type {
public:
    explicit TypeEnum(const ast::TypeEnum *te): Type(te, te->module + "$" + te->name), te(te) {}
    TypeEnum(const TypeEnum &) = delete;
    TypeEnum &operator=(const TypeEnum &) = delete;
    const ast::TypeEnum *te;

    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeEnum");
    }
};

class TypeChoice: public Type {
public:
    explicit TypeChoice(const ast::TypeChoice *tc): Type(tc, tc->module + "$" + tc->name), tc(tc) {}
    TypeChoice(const TypeChoice &) = delete;
    TypeChoice &operator=(const TypeChoice &) = delete;
    const ast::TypeChoice *tc;

    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeChoice");
    }
};

class PredefinedVariable: public Variable {
public:
    explicit PredefinedVariable(const ast::PredefinedVariable *pv): Variable(pv), pv(pv) {}
    PredefinedVariable(const PredefinedVariable &) = delete;
    PredefinedVariable &operator=(const PredefinedVariable &) = delete;
    const ast::PredefinedVariable *pv;

    virtual void generate_decl(ClassContext &, bool) const override {}
    virtual void generate_load(Context &context) const override {
        context.unimplemented("PredefinedVariable");
    }
    virtual void generate_store(Context &) const override { internal_error("PredefinedVariable"); }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("PredefinedVariable"); }
};

class ModuleVariable: public Variable {
public:
    explicit ModuleVariable(const ast::ModuleVariable *mv): Variable(mv), mv(mv) {}
    ModuleVariable(const ModuleVariable &) = delete;
    ModuleVariable &operator=(const ModuleVariable &) = delete;
    const ast::ModuleVariable *mv;

    virtual void generate_decl(ClassContext &, bool) const override {}
    virtual void generate_load(Context &context) const override {
        context.unimplemented("ModuleVariable");
    }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("ModuleVariable");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ModuleVariable"); }
};

class GlobalVariable: public Variable {
public:
    explicit GlobalVariable(const ast::GlobalVariable *gv): Variable(gv), gv(gv), name() {}
    GlobalVariable(const GlobalVariable &) = delete;
    GlobalVariable &operator=(const GlobalVariable &) = delete;
    const ast::GlobalVariable *gv;
    mutable std::string name;

    virtual void generate_decl(ClassContext &context, bool) const override {
        context.unimplemented("GlobalVariable");
    }
    virtual void generate_load(Context &context) const override {
        context.unimplemented("GlobalVariable");
    }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("GlobalVariable");
    }
    virtual void generate_call(Context &context, const std::vector<const Expression *> &) const override {
        context.unimplemented("GlobalVariable");
    }
};

class LocalVariable: public Variable {
public:
    explicit LocalVariable(const ast::LocalVariable *lv): Variable(lv), lv(lv), index(-1) {}
    LocalVariable(const LocalVariable &) = delete;
    LocalVariable &operator=(const LocalVariable &) = delete;
    const ast::LocalVariable *lv;
    mutable int index; // TODO

    virtual void generate_decl(ClassContext &, bool) const override {}
    virtual void generate_load(Context &context) const override {
        context.unimplemented("LocalVariable");
    }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("LocalVariable");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("LocalVariable"); }
};

class FunctionParameter: public Variable {
public:
    explicit FunctionParameter(const ast::FunctionParameter *fp, int index): Variable(fp), fp(fp), index(index) {}
    FunctionParameter(const FunctionParameter &) = delete;
    FunctionParameter &operator=(const FunctionParameter &) = delete;
    const ast::FunctionParameter *fp;
    const int index;

    virtual void generate_decl(ClassContext &, bool) const override { internal_error("FunctionParameter"); }
    virtual void generate_load(Context &context) const override {
        context.unimplemented("FunctionParameter");
    }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("FunctionParameter");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("FunctionParameter"); }
};

class ConstantBooleanExpression: public Expression {
public:
    explicit ConstantBooleanExpression(const ast::ConstantBooleanExpression *cbe): Expression(cbe), cbe(cbe) {}
    ConstantBooleanExpression(const ConstantBooleanExpression &) = delete;
    ConstantBooleanExpression &operator=(const ConstantBooleanExpression &) = delete;
    const ast::ConstantBooleanExpression *cbe;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConstantBooleanExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantBooleanExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantBooleanExpression"); }
};

class ConstantNumberExpression: public Expression {
public:
    explicit ConstantNumberExpression(const ast::ConstantNumberExpression *cne): Expression(cne), cne(cne) {}
    ConstantNumberExpression(const ConstantNumberExpression &) = delete;
    ConstantNumberExpression &operator=(const ConstantNumberExpression &) = delete;
    const ast::ConstantNumberExpression *cne;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConstantNumberExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantNumberExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantNumberExpression"); }
};

class ConstantStringExpression: public Expression {
public:
    explicit ConstantStringExpression(const ast::ConstantStringExpression *cse): Expression(cse), cse(cse) {}
    ConstantStringExpression(const ConstantStringExpression &) = delete;
    ConstantStringExpression &operator=(const ConstantStringExpression &) = delete;
    const ast::ConstantStringExpression *cse;

    virtual void generate(Context &context) const override {
        context.code << op_ldstr << context.md.Userstring(cse->value.str());
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantStringExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantStringExpression"); }
};

class ConstantBytesExpression: public Expression {
public:
    explicit ConstantBytesExpression(const ast::ConstantBytesExpression *cbe): Expression(cbe), cbe(cbe) {}
    ConstantBytesExpression(const ConstantBytesExpression &) = delete;
    ConstantBytesExpression &operator=(const ConstantBytesExpression &) = delete;
    const ast::ConstantBytesExpression *cbe;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConstantBytesExpression");
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantBytesExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantBytesExpression"); }
};

class ConstantEnumExpression: public Expression {
public:
    explicit ConstantEnumExpression(const ast::ConstantEnumExpression *cee): Expression(cee), cee(cee), type(dynamic_cast<const TypeEnum *>(transform(cee->type))) {}
    ConstantEnumExpression(const ConstantEnumExpression &) = delete;
    ConstantEnumExpression &operator=(const ConstantEnumExpression &) = delete;
    const ast::ConstantEnumExpression *cee;
    const TypeEnum *type;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConstantEnumExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantEnumExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantEnumExpression"); }
};

class ConstantChoiceExpression: public Expression {
public:
    explicit ConstantChoiceExpression(const ast::ConstantChoiceExpression *cce): Expression(cce), cce(cce), type(dynamic_cast<const TypeChoice *>(transform(cce->type))) {}
    ConstantChoiceExpression(const ConstantChoiceExpression &) = delete;
    ConstantChoiceExpression &operator=(const ConstantChoiceExpression &) = delete;
    const ast::ConstantChoiceExpression *cce;
    const TypeChoice *type;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConstantChoiceExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantChoiceExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantChoiceExpression"); }
};

class ConstantNilExpression: public Expression {
public:
    explicit ConstantNilExpression(const ast::ConstantNilExpression *cne): Expression(cne), cne(cne) {}
    ConstantNilExpression(const ConstantNilExpression &) = delete;
    ConstantNilExpression &operator=(const ConstantNilExpression &) = delete;
    const ast::ConstantNilExpression *cne;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConstantNilExpression");
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantNilExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantNilExpression"); }
};

class ConstantNowhereExpression: public Expression {
public:
    explicit ConstantNowhereExpression(const ast::ConstantNowhereExpression *cne): Expression(cne), cne(cne) {}
    ConstantNowhereExpression(const ConstantNowhereExpression &) = delete;
    ConstantNowhereExpression &operator=(const ConstantNowhereExpression &) = delete;
    const ast::ConstantNowhereExpression *cne;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConstantNowhereExpression");
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantNowhereExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantNowhereExpression"); }
};

class ArrayLiteralExpression: public Expression {
public:
    explicit ArrayLiteralExpression(const ast::ArrayLiteralExpression *ale): Expression(ale), ale(ale), elements() {
        for (auto e: ale->elements) {
            elements.push_back(transform(e));
        }
    }
    ArrayLiteralExpression(const ArrayLiteralExpression &) = delete;
    ArrayLiteralExpression &operator=(const ArrayLiteralExpression &) = delete;
    const ast::ArrayLiteralExpression *ale;
    std::vector<const Expression *> elements;

    virtual void generate(Context &context) const override {
        context.unimplemented("ArrayLiteralExpression");
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ArrayLiteralExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ArrayLiteralExpression"); }
};

class DictionaryLiteralExpression: public Expression {
public:
    explicit DictionaryLiteralExpression(const ast::DictionaryLiteralExpression *dle): Expression(dle), dle(dle), dict() {
        for (auto d: dle->dict) {
            dict[d.first.str()] = transform(d.second);
        }
    }
    DictionaryLiteralExpression(const DictionaryLiteralExpression &) = delete;
    DictionaryLiteralExpression &operator=(const DictionaryLiteralExpression &) = delete;
    const ast::DictionaryLiteralExpression *dle;
    std::map<std::string, const Expression *> dict;

    virtual void generate(Context &context) const override {
        context.unimplemented("DictionaryLiteralExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DictionaryLiteralExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DictionaryLiteralExpression"); }
};

class RecordLiteralExpression: public Expression {
public:
    explicit RecordLiteralExpression(const ast::RecordLiteralExpression *rle): Expression(rle), rle(rle), type(dynamic_cast<TypeRecord *>(transform(rle->type))), values() {
        for (auto v: rle->values) {
            values.push_back(transform(v));
        }
    }
    RecordLiteralExpression(const RecordLiteralExpression &) = delete;
    RecordLiteralExpression &operator=(const RecordLiteralExpression &) = delete;
    const ast::RecordLiteralExpression *rle;
    const TypeRecord *type;
    std::vector<const Expression *> values;

    virtual void generate(Context &context) const override {
        context.unimplemented("RecordLiteralExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("RecordLiteralExpression"); }
    virtual void generate_store(Context &) const override { internal_error("RecordLiteralExpression"); }
};

class NewClassExpression: public Expression {
public:
    explicit NewClassExpression(const ast::NewClassExpression *nce): Expression(nce), nce(nce), value(transform(nce->value)), type(dynamic_cast<const TypeRecord *>(transform(dynamic_cast<const ast::TypeValidPointer *>(nce->type)->reftype))) {}
    NewClassExpression(const NewClassExpression &) = delete;
    NewClassExpression &operator=(const NewClassExpression &) = delete;
    const ast::NewClassExpression *nce;
    const Expression *value;
    const TypeRecord *type;

    virtual void generate(Context &context) const override {
        context.unimplemented("NewClassExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("NewClassExpression"); }
    virtual void generate_store(Context &) const override { internal_error("NewClassExpression"); }
};

class UnaryMinusExpression: public Expression {
public:
    explicit UnaryMinusExpression(const ast::UnaryMinusExpression *ume): Expression(ume), ume(ume), value(transform(ume->value)) {}
    UnaryMinusExpression(const UnaryMinusExpression &) = delete;
    UnaryMinusExpression &operator=(const UnaryMinusExpression &) = delete;
    const ast::UnaryMinusExpression *ume;
    const Expression *value;

    virtual void generate(Context &context) const override {
        context.unimplemented("UnaryMinusExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("UnaryMinusExpression"); }
    virtual void generate_store(Context &) const override { internal_error("UnaryMinusExpression"); }
};

class LogicalNotExpression: public Expression {
public:
    explicit LogicalNotExpression(const ast::LogicalNotExpression *lne): Expression(lne), lne(lne), value(transform(lne->value)) {}
    LogicalNotExpression(const LogicalNotExpression &) = delete;
    LogicalNotExpression &operator=(const LogicalNotExpression &) = delete;
    const ast::LogicalNotExpression *lne;
    const Expression *value;

    virtual void generate(Context &context) const override {
        context.unimplemented("LogicalNotExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("LogicalNotExpression"); }
    virtual void generate_store(Context &) const override { internal_error("LogicalNotExpression"); }
};

class ConditionalExpression: public Expression {
public:
    explicit ConditionalExpression(const ast::ConditionalExpression *ce): Expression(ce), ce(ce), condition(transform(ce->condition)), left(transform(ce->left)), right(transform(ce->right)) {}
    ConditionalExpression(const ConditionalExpression &) = delete;
    ConditionalExpression &operator=(const ConditionalExpression &) = delete;
    const ast::ConditionalExpression *ce;
    const Expression *condition;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConditionalExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConditionalExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConditionalExpression"); }
};

class TryExpressionTrap {
public:
    explicit TryExpressionTrap(const ast::TryTrap *tt): tt(tt), name(transform(tt->name)), handler(), gives(transform(dynamic_cast<const ast::Expression *>(tt->handler))) {
        const ast::ExceptionHandlerStatement *h = dynamic_cast<const ast::ExceptionHandlerStatement *>(tt->handler);
        if (h != nullptr) {
            for (auto s: h->statements) {
                handler.push_back(transform(s));
            }
        }
    }
    TryExpressionTrap(const TryExpressionTrap &) = delete;
    TryExpressionTrap &operator=(const TryExpressionTrap &) = delete;
    const ast::TryTrap *tt;
    const Variable *name;
    std::vector<const Statement *> handler;
    const Expression *gives;
};

class TryExpression: public Expression {
public:
    explicit TryExpression(const ast::TryExpression *te): Expression(te), te(te), expr(transform(te->expr)), catches() {
        for (auto &t: te->catches) {
            catches.push_back(new TryExpressionTrap(&t));
        }
    }
    TryExpression(const TryExpression &) = delete;
    TryExpression &operator=(const TryExpression &) = delete;
    const ast::TryExpression *te;
    const Expression *expr;
    std::vector<const TryExpressionTrap *> catches;

    virtual void generate(Context &context) const override {
        context.unimplemented("TryExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("TryExpression"); }
    virtual void generate_store(Context &) const override { internal_error("TryExpression"); }
};

class DisjunctionExpression: public Expression {
public:
    explicit DisjunctionExpression(const ast::DisjunctionExpression *de): Expression(de), de(de), left(transform(de->left)), right(transform(de->right)) {}
    DisjunctionExpression(const DisjunctionExpression &) = delete;
    DisjunctionExpression &operator=(const DisjunctionExpression &) = delete;
    const ast::DisjunctionExpression *de;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("DisjunctionExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DisjunctionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DisjunctionExpression"); }
};

class ConjunctionExpression: public Expression {
public:
    explicit ConjunctionExpression(const ast::ConjunctionExpression *ce): Expression(ce), ce(ce), left(transform(ce->left)), right(transform(ce->right)) {}
    ConjunctionExpression(const ConjunctionExpression &) = delete;
    ConjunctionExpression &operator=(const ConjunctionExpression &) = delete;
    const ast::ConjunctionExpression *ce;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConjunctionExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConjunctionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConjunctionExpression"); }
};

class ArrayInExpression: public Expression {
public:
    explicit ArrayInExpression(const ast::ArrayInExpression *aie): Expression(aie), aie(aie), left(transform(aie->left)), right(transform(aie->right)) {}
    ArrayInExpression(const ArrayInExpression &) = delete;
    ArrayInExpression &operator=(const ArrayInExpression &) = delete;
    const ast::ArrayInExpression *aie;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("ArrayInExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ArrayInExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ArrayInExpression"); }
};

class DictionaryInExpression: public Expression {
public:
    explicit DictionaryInExpression(const ast::DictionaryInExpression *die): Expression(die), die(die), left(transform(die->left)), right(transform(die->right)) {}
    DictionaryInExpression(const DictionaryInExpression &) = delete;
    DictionaryInExpression &operator=(const DictionaryInExpression &) = delete;
    const ast::DictionaryInExpression *die;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("DictionaryInExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DictionaryInExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DictionaryInExpression"); }
};

class ComparisonExpression: public Expression {
public:
    explicit ComparisonExpression(const ast::ComparisonExpression *ce): Expression(ce), ce(ce), left(transform(ce->left)), right(transform(ce->right)) {}
    ComparisonExpression(const ComparisonExpression &) = delete;
    ComparisonExpression &operator=(const ComparisonExpression &) = delete;
    const ast::ComparisonExpression *ce;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        right->generate(context);
        generate_comparison(context);
    }
    virtual void generate_comparison(Context &context) const = 0;

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ComparisonExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ComparisonExpression"); }
};

class ChainedComparisonExpression: public Expression {
public:
    explicit ChainedComparisonExpression(const ast::ChainedComparisonExpression *cce): Expression(cce), cce(cce), comps() {
        for (auto c: cce->comps) {
            const ComparisonExpression *ce = dynamic_cast<const ComparisonExpression *>(transform(c));
            if (ce == nullptr) {
                internal_error("ChainedComparisonExpression");
            }
            comps.push_back(ce);
        }
    }
    ChainedComparisonExpression(const ChainedComparisonExpression &) = delete;
    ChainedComparisonExpression &operator=(const ChainedComparisonExpression &) = delete;
    const ast::ChainedComparisonExpression *cce;
    std::vector<const ComparisonExpression *> comps;

    virtual void generate(Context &context) const override {
        context.unimplemented("ChainedComparisonExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ChainedComparisonExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ChainedComparisonExpression"); }
};

class BooleanComparisonExpression: public ComparisonExpression {
public:
    explicit BooleanComparisonExpression(const ast::BooleanComparisonExpression *bce): ComparisonExpression(bce), bce(bce) {}
    BooleanComparisonExpression(const BooleanComparisonExpression &) = delete;
    BooleanComparisonExpression &operator=(const BooleanComparisonExpression &) = delete;
    const ast::BooleanComparisonExpression *bce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("BooleanComparisonExpression");
    }
};

class NumericComparisonExpression: public ComparisonExpression {
public:
    explicit NumericComparisonExpression(const ast::NumericComparisonExpression *nce): ComparisonExpression(nce), nce(nce) {}
    NumericComparisonExpression(const NumericComparisonExpression &) = delete;
    NumericComparisonExpression &operator=(const NumericComparisonExpression &) = delete;
    const ast::NumericComparisonExpression *nce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("NumericComparisonExpression");
    }
};

class EnumComparisonExpression: public ComparisonExpression {
public:
    explicit EnumComparisonExpression(const ast::EnumComparisonExpression *ece): ComparisonExpression(ece), ece(ece) {}
    EnumComparisonExpression(const EnumComparisonExpression &) = delete;
    EnumComparisonExpression &operator=(const EnumComparisonExpression &) = delete;
    const ast::EnumComparisonExpression *ece;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("EnumComparisonExpression");
    }
};

class StringComparisonExpression: public ComparisonExpression {
public:
    explicit StringComparisonExpression(const ast::StringComparisonExpression *sce): ComparisonExpression(sce), sce(sce) {}
    StringComparisonExpression(const StringComparisonExpression &) = delete;
    StringComparisonExpression &operator=(const StringComparisonExpression &) = delete;
    const ast::StringComparisonExpression *sce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("StringComparisonExpression");
    }
};

class BytesComparisonExpression: public ComparisonExpression {
public:
    explicit BytesComparisonExpression(const ast::BytesComparisonExpression *bce): ComparisonExpression(bce), bce(bce) {}
    BytesComparisonExpression(const BytesComparisonExpression &) = delete;
    BytesComparisonExpression &operator=(const BytesComparisonExpression &) = delete;
    const ast::BytesComparisonExpression *bce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("BytesComparisonExpression");
    }
};

class ArrayComparisonExpression: public ComparisonExpression {
public:
    explicit ArrayComparisonExpression(const ast::ArrayComparisonExpression *ace): ComparisonExpression(ace), ace(ace) {}
    ArrayComparisonExpression(const ArrayComparisonExpression &) = delete;
    ArrayComparisonExpression &operator=(const ArrayComparisonExpression &) = delete;
    const ast::ArrayComparisonExpression *ace;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("ArrayComparisonExpression");
    }
};

class DictionaryComparisonExpression: public ComparisonExpression {
public:
    explicit DictionaryComparisonExpression(const ast::DictionaryComparisonExpression *dce): ComparisonExpression(dce), dce(dce) {}
    DictionaryComparisonExpression(const DictionaryComparisonExpression &) = delete;
    DictionaryComparisonExpression &operator=(const DictionaryComparisonExpression &) = delete;
    const ast::DictionaryComparisonExpression *dce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("DictionaryComparisonExpression");
    }
};

class RecordComparisonExpression: public ComparisonExpression {
public:
    explicit RecordComparisonExpression(const ast::RecordComparisonExpression *rce): ComparisonExpression(rce), rce(rce) {}
    RecordComparisonExpression(const RecordComparisonExpression &) = delete;
    RecordComparisonExpression &operator=(const RecordComparisonExpression &) = delete;
    const ast::RecordComparisonExpression *rce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("RecordComparisonExpression");
    }
};

class PointerComparisonExpression: public ComparisonExpression {
public:
    explicit PointerComparisonExpression(const ast::PointerComparisonExpression *pce): ComparisonExpression(pce), pce(pce) {}
    PointerComparisonExpression(const PointerComparisonExpression &) = delete;
    PointerComparisonExpression &operator=(const PointerComparisonExpression &) = delete;
    const ast::PointerComparisonExpression *pce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("PointerComparisonExpression");
    }
};

class ValidPointerExpression: public PointerComparisonExpression {
public:
    explicit ValidPointerExpression(const ast::ValidPointerExpression *vpe): PointerComparisonExpression(vpe), vpe(vpe), var(transform(vpe->var)) {}
    ValidPointerExpression(const ValidPointerExpression &) = delete;
    ValidPointerExpression &operator=(const ValidPointerExpression &) = delete;
    const ast::ValidPointerExpression *vpe;
    const Variable *var;

    virtual void generate(Context &context) const override {
        context.unimplemented("ValidPointerExpression");
    }
};

class FunctionPointerComparisonExpression: public ComparisonExpression {
public:
    explicit FunctionPointerComparisonExpression(const ast::FunctionPointerComparisonExpression *fpce): ComparisonExpression(fpce), fpce(fpce) {}
    FunctionPointerComparisonExpression(const FunctionPointerComparisonExpression &) = delete;
    FunctionPointerComparisonExpression &operator=(const FunctionPointerComparisonExpression &) = delete;
    const ast::FunctionPointerComparisonExpression *fpce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("FunctionPointerComparisonExpression");
    }
};

class AdditionExpression: public Expression {
public:
    explicit AdditionExpression(const ast::AdditionExpression *ae): Expression(ae), ae(ae), left(transform(ae->left)), right(transform(ae->right)) {}
    AdditionExpression(const AdditionExpression &) = delete;
    AdditionExpression &operator=(const AdditionExpression &) = delete;
    const ast::AdditionExpression *ae;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("AdditionExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("AdditionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("AdditionExpression"); }
};

class SubtractionExpression: public Expression {
public:
    explicit SubtractionExpression(const ast::SubtractionExpression *se): Expression(se), se(se), left(transform(se->left)), right(transform(se->right)) {}
    SubtractionExpression(const SubtractionExpression &) = delete;
    SubtractionExpression &operator=(const SubtractionExpression &) = delete;
    const ast::SubtractionExpression *se;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("SubtractionExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("SubtractionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("SubtractionExpression"); }
};

class MultiplicationExpression: public Expression {
public:
    explicit MultiplicationExpression(const ast::MultiplicationExpression *me): Expression(me), me(me), left(transform(me->left)), right(transform(me->right)) {}
    MultiplicationExpression(const MultiplicationExpression &) = delete;
    MultiplicationExpression &operator=(const MultiplicationExpression &) = delete;
    const ast::MultiplicationExpression *me;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("MultiplicationExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("MultiplicationExpression"); }
    virtual void generate_store(Context &) const override { internal_error("MultiplicationExpression"); }
};

class DivisionExpression: public Expression {
public:
    explicit DivisionExpression(const ast::DivisionExpression *de): Expression(de), de(de), left(transform(de->left)), right(transform(de->right)) {}
    DivisionExpression(const DivisionExpression &) = delete;
    DivisionExpression &operator=(const DivisionExpression &) = delete;
    const ast::DivisionExpression *de;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("DivisionExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DivisionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DivisionExpression"); }
};

class ModuloExpression: public Expression {
public:
    explicit ModuloExpression(const ast::ModuloExpression *me): Expression(me), me(me), left(transform(me->left)), right(transform(me->right)) {}
    ModuloExpression(const ModuloExpression &) = delete;
    ModuloExpression &operator=(const ModuloExpression &) = delete;
    const ast::ModuloExpression *me;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("ModuloExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ModuloExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ModuloExpression"); }
};

class ExponentiationExpression: public Expression {
public:
    explicit ExponentiationExpression(const ast::ExponentiationExpression *ee): Expression(ee), ee(ee), left(transform(ee->left)), right(transform(ee->right)) {}
    ExponentiationExpression(const ExponentiationExpression &) = delete;
    ExponentiationExpression &operator=(const ExponentiationExpression &) = delete;
    const ast::ExponentiationExpression *ee;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("ExponentiationExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ExponentiationExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ExponentiationExpression"); }
};

class DummyExpression: public Expression {
public:
    explicit DummyExpression(const ast::DummyExpression *de): Expression(de), de(de) {}
    DummyExpression(const DummyExpression &) = delete;
    DummyExpression &operator=(const DummyExpression &) = delete;
    const ast::DummyExpression *de;

    virtual void generate(Context &) const override { internal_error("DummyExpression"); }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DummyExpression"); }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("DummyExpression");
    }
};

class ArrayReferenceIndexExpression: public Expression {
public:
    explicit ArrayReferenceIndexExpression(const ast::ArrayReferenceIndexExpression *arie): Expression(arie), arie(arie), array(transform(arie->array)), index(transform(arie->index)) {}
    ArrayReferenceIndexExpression(const ArrayReferenceIndexExpression &) = delete;
    ArrayReferenceIndexExpression &operator=(const ArrayReferenceIndexExpression &) = delete;
    const ast::ArrayReferenceIndexExpression *arie;
    const Expression *array;
    const Expression *index;

    virtual void generate(Context &context) const override {
        context.unimplemented("ArrayReferenceIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ArrayReferenceIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("ArrayReferenceIndexExpression");
    }
};

class ArrayValueIndexExpression: public Expression {
public:
    explicit ArrayValueIndexExpression(const ast::ArrayValueIndexExpression *avie): Expression(avie), avie(avie), array(transform(avie->array)), index(transform(avie->index)) {}
    ArrayValueIndexExpression(const ArrayValueIndexExpression &) = delete;
    ArrayValueIndexExpression &operator=(const ArrayValueIndexExpression &) = delete;
    const ast::ArrayValueIndexExpression *avie;
    const Expression *array;
    const Expression *index;

    virtual void generate(Context &context) const override {
        context.unimplemented("ArrayValueIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ArrayValueIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("ArrayValueIndexExpression");
    }
};

class DictionaryReferenceIndexExpression: public Expression {
public:
    explicit DictionaryReferenceIndexExpression(const ast::DictionaryReferenceIndexExpression *drie): Expression(drie), drie(drie), dictionary(transform(drie->dictionary)), index(transform(drie->index)) {}
    DictionaryReferenceIndexExpression(const DictionaryReferenceIndexExpression &) = delete;
    DictionaryReferenceIndexExpression &operator=(const DictionaryReferenceIndexExpression &) = delete;
    const ast::DictionaryReferenceIndexExpression *drie;
    const Expression *dictionary;
    const Expression *index;

    virtual void generate(Context &context) const override {
        context.unimplemented("DictionaryReferenceIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DictionaryReferenceIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("DictionaryReferenceIndexExpression");
    }
};

class DictionaryValueIndexExpression: public Expression {
public:
    explicit DictionaryValueIndexExpression(const ast::DictionaryValueIndexExpression *dvie): Expression(dvie), dvie(dvie), dictionary(transform(dvie->dictionary)), index(transform(dvie->index)) {}
    DictionaryValueIndexExpression(const DictionaryValueIndexExpression &) = delete;
    DictionaryValueIndexExpression &operator=(const DictionaryValueIndexExpression &) = delete;
    const ast::DictionaryValueIndexExpression *dvie;
    const Expression *dictionary;
    const Expression *index;

    virtual void generate(Context &context) const override {
        context.unimplemented("DictionaryValueIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DictionaryValueIndexExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DictionaryValueIndexExpression"); }
};

class StringReferenceIndexExpression: public Expression {
public:
    explicit StringReferenceIndexExpression(const ast::StringReferenceIndexExpression *srie): Expression(srie), srie(srie), ref(transform(srie->ref)), index(transform(srie->index)) {}
    StringReferenceIndexExpression(const StringReferenceIndexExpression &) = delete;
    StringReferenceIndexExpression &operator=(const StringReferenceIndexExpression &) = delete;
    const ast::StringReferenceIndexExpression *srie;
    const Expression *ref;
    const Expression *index;

    virtual void generate(Context &context) const override {
        context.unimplemented("StringReferenceIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("StringReferenceIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("StringReferenceIndexExpression");
    }
};

class StringValueIndexExpression: public Expression {
public:
    explicit StringValueIndexExpression(const ast::StringValueIndexExpression *svie): Expression(svie), svie(svie), str(transform(svie->str)), index(transform(svie->index)) {}
    StringValueIndexExpression(const StringValueIndexExpression &) = delete;
    StringValueIndexExpression &operator=(const StringValueIndexExpression &) = delete;
    const ast::StringValueIndexExpression *svie;
    const Expression *str;
    const Expression *index;

    virtual void generate(Context &context) const override {
        context.unimplemented("StringValueIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("StringValueIndexExpression"); }
    virtual void generate_store(Context&) const override { internal_error("StringValueIndexExpression"); }
};

class StringReferenceRangeIndexExpression: public Expression {
public:
    explicit StringReferenceRangeIndexExpression(const ast::StringReferenceRangeIndexExpression *srie): Expression(srie), srie(srie), ref(transform(srie->ref)), first(transform(srie->first)), last(transform(srie->last)) {}
    StringReferenceRangeIndexExpression(const StringReferenceRangeIndexExpression &) = delete;
    StringReferenceRangeIndexExpression &operator=(const StringReferenceRangeIndexExpression &) = delete;
    const ast::StringReferenceRangeIndexExpression *srie;
    const Expression *ref;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        context.unimplemented("StringReferenceRangeIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("StringReferenceRangeIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("StringReferenceRangeIndexExpression");
    }
};

class StringValueRangeIndexExpression: public Expression {
public:
    explicit StringValueRangeIndexExpression(const ast::StringValueRangeIndexExpression *svie): Expression(svie), svie(svie), str(transform(svie->str)), first(transform(svie->first)), last(transform(svie->last)) {}
    StringValueRangeIndexExpression(const StringValueRangeIndexExpression &) = delete;
    StringValueRangeIndexExpression &operator=(const StringValueRangeIndexExpression &) = delete;
    const ast::StringValueRangeIndexExpression *svie;
    const Expression *str;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        context.unimplemented("StringValueRangeIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("StringValueRangeIndexExpression"); }
    virtual void generate_store(Context&) const override { internal_error("StringValueRangeIndexExpression"); }
};

class BytesReferenceIndexExpression: public Expression {
public:
    explicit BytesReferenceIndexExpression(const ast::BytesReferenceIndexExpression *brie): Expression(brie), brie(brie), ref(transform(brie->ref)), index(transform(brie->index)) {}
    BytesReferenceIndexExpression(const BytesReferenceIndexExpression &) = delete;
    BytesReferenceIndexExpression &operator=(const BytesReferenceIndexExpression &) = delete;
    const ast::BytesReferenceIndexExpression *brie;
    const Expression *ref;
    const Expression *index;

    virtual void generate(Context &context) const override {
        context.unimplemented("BytesReferenceIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("BytesReferenceIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("BytesReferenceIndexExpression");
    }
};

class BytesValueIndexExpression: public Expression {
public:
    explicit BytesValueIndexExpression(const ast::BytesValueIndexExpression *bvie): Expression(bvie), bvie(bvie), data(transform(bvie->str)), index(transform(bvie->index)) {}
    BytesValueIndexExpression(const BytesValueIndexExpression &) = delete;
    BytesValueIndexExpression &operator=(const BytesValueIndexExpression &) = delete;
    const ast::BytesValueIndexExpression *bvie;
    const Expression *data;
    const Expression *index;

    virtual void generate(Context &context) const override {
        context.unimplemented("BytesValueIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("BytesValueIndexExpression"); }
    virtual void generate_store(Context &) const override { internal_error("BytesValueIndexExpression"); }
};

class BytesReferenceRangeIndexExpression: public Expression {
public:
    explicit BytesReferenceRangeIndexExpression(const ast::BytesReferenceRangeIndexExpression *brie): Expression(brie), brie(brie), ref(transform(brie->ref)), first(transform(brie->first)), last(transform(brie->last)) {}
    BytesReferenceRangeIndexExpression(const BytesReferenceRangeIndexExpression &) = delete;
    BytesReferenceRangeIndexExpression &operator=(const BytesReferenceRangeIndexExpression &) = delete;
    const ast::BytesReferenceRangeIndexExpression *brie;
    const Expression *ref;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        context.unimplemented("BytesReferenceRangeIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("BytesReferenceRangeIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("BytesReferenceRangeIndexExpression");
    }
};

class BytesValueRangeIndexExpression: public Expression {
public:
    explicit BytesValueRangeIndexExpression(const ast::BytesValueRangeIndexExpression *bvie): Expression(bvie), bvie(bvie), data(transform(bvie->str)), first(transform(bvie->first)), last(transform(bvie->last)) {}
    BytesValueRangeIndexExpression(const BytesValueRangeIndexExpression &) = delete;
    BytesValueRangeIndexExpression &operator=(const BytesValueRangeIndexExpression &) = delete;
    const ast::BytesValueRangeIndexExpression *bvie;
    const Expression *data;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        context.unimplemented("BytesValueRangeIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("BytesValueRangeIndexExpression"); }
    virtual void generate_store(Context &) const override { internal_error("BytesValueRangeIndexExpression"); }
};

class RecordReferenceFieldExpression: public Expression {
public:
    explicit RecordReferenceFieldExpression(const ast::RecordReferenceFieldExpression *rrfe): Expression(rrfe), rrfe(rrfe), ref(transform(rrfe->ref)), rectype(dynamic_cast<const TypeRecord *>(transform(rrfe->ref->type))), fieldtype(transform(rrfe->type)) {}
    RecordReferenceFieldExpression(const RecordReferenceFieldExpression &) = delete;
    RecordReferenceFieldExpression &operator=(const RecordReferenceFieldExpression &) = delete;
    const ast::RecordReferenceFieldExpression *rrfe;
    const Expression *ref;
    const TypeRecord *rectype;
    const Type *fieldtype;

    virtual void generate(Context &context) const override {
        context.unimplemented("RecordReferenceFieldExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("RecordReferenceFieldExpression"); }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("RecordReferenceFieldExpression");
    }
};

class RecordValueFieldExpression: public Expression {
public:
    explicit RecordValueFieldExpression(const ast::RecordValueFieldExpression *rvfe): Expression(rvfe), rvfe(rvfe), rec(transform(rvfe->rec)), rectype(dynamic_cast<const TypeRecord *>(transform(rvfe->rec->type))), fieldtype(transform(rvfe->type)) {}
    RecordValueFieldExpression(const RecordValueFieldExpression &) = delete;
    RecordValueFieldExpression &operator=(const RecordValueFieldExpression &) = delete;
    const ast::RecordValueFieldExpression *rvfe;
    const Expression *rec;
    const TypeRecord *rectype;
    const Type *fieldtype;

    virtual void generate(Context &context) const override {
        context.unimplemented("RecordValueFieldExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("RecordValueFieldExpression"); }
    virtual void generate_store(Context &) const override { internal_error("RecordValueFieldExpression"); }
};

class ArrayReferenceRangeExpression: public Expression {
public:
    explicit ArrayReferenceRangeExpression(const ast::ArrayReferenceRangeExpression *arre): Expression(arre), arre(arre), ref(transform(arre->ref)), first(transform(arre->first)), last(transform(arre->last)) {}
    ArrayReferenceRangeExpression(const ArrayReferenceRangeExpression &) = delete;
    ArrayReferenceRangeExpression &operator=(const ArrayReferenceRangeExpression &) = delete;
    const ast::ArrayReferenceRangeExpression *arre;
    const Expression *ref;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        context.unimplemented("ArrayReferenceRangeExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ArrayReferenceRangeExpression"); }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("ArrayReferenceRangeExpression");
    }
};

class ArrayValueRangeExpression: public Expression {
public:
    explicit ArrayValueRangeExpression(const ast::ArrayValueRangeExpression *avre): Expression(avre), avre(avre), array(transform(avre->array)), first(transform(avre->first)), last(transform(avre->last)) {}
    ArrayValueRangeExpression(const ArrayValueRangeExpression &) = delete;
    ArrayValueRangeExpression &operator=(const ArrayValueRangeExpression &) = delete;
    const ast::ArrayValueRangeExpression *avre;
    const Expression *array;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        context.unimplemented("ArrayValueRangeExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ArrayValueRangeExpression"); }
    virtual void generate_store(Context&) const override { internal_error("ArrayValueRangeExpression"); }
};

class ChoiceReferenceExpression: public Expression {
public:
    explicit ChoiceReferenceExpression(const ast::ChoiceReferenceExpression *ccre): Expression(ccre), ccre(ccre) {}
    ChoiceReferenceExpression(const ChoiceReferenceExpression &) = delete;
    ChoiceReferenceExpression &operator=(const ChoiceReferenceExpression &) = delete;
    const ast::ChoiceReferenceExpression *ccre;

    virtual void generate(Context &context) const override {
        context.unimplemented("ChoiceReferenceExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ChoiceReferenceExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ChoiceReferenceExpression"); }
};

class PointerDereferenceExpression: public Expression {
public:
    explicit PointerDereferenceExpression(const ast::PointerDereferenceExpression *pde): Expression(pde), pde(pde), ptr(transform(pde->ptr)) {}
    PointerDereferenceExpression(const PointerDereferenceExpression &) = delete;
    PointerDereferenceExpression &operator=(const PointerDereferenceExpression &) = delete;
    const ast::PointerDereferenceExpression *pde;
    const Expression *ptr;

    virtual void generate(Context &context) const override {
        context.unimplemented("PointerDereferenceExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("PointerDereferenceExpression"); }
    virtual void generate_store(Context&) const override { internal_error("PointerDereferenceExpression"); }
};

class VariableExpression: public Expression {
public:
    explicit VariableExpression(const ast::VariableExpression *ve): Expression(ve), ve(ve), var(transform(ve->var)) {}
    VariableExpression(const VariableExpression &) = delete;
    VariableExpression &operator=(const VariableExpression &) = delete;
    const ast::VariableExpression *ve;
    const Variable *var;

    virtual void generate(Context &context) const override {
        var->generate_load(context);
    }

    virtual void generate_call(Context &context, const std::vector<const Expression *> &args) const override {
        var->generate_call(context, args);
    }
    virtual void generate_store(Context &context) const override {
        var->generate_store(context);
    }
};

class FunctionCall: public Expression {
public:
    explicit FunctionCall(const ast::FunctionCall *fc): Expression(fc), fc(fc), func(transform(fc->func)), args() {
        for (auto a: fc->args) {
            args.push_back(transform(a));
        }
    }
    FunctionCall(const FunctionCall &) = delete;
    FunctionCall &operator=(const FunctionCall &) = delete;
    const ast::FunctionCall *fc;
    const Expression *func;
    std::vector<const Expression *> args;

    virtual void generate(Context &context) const override {
        func->generate_call(context, args);
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("FunctionCall"); }
    virtual void generate_store(Context &) const override { internal_error("FunctionCall"); }
};

class NullStatement: public Statement {
public:
    explicit NullStatement(const ast::NullStatement *ns): Statement(ns), ns(ns) {}
    NullStatement(const NullStatement &) = delete;
    NullStatement &operator=(const NullStatement &) = delete;
    const ast::NullStatement *ns;

    virtual void generate(Context &) const override {}
};

class DeclarationStatement: public Statement {
public:
    explicit DeclarationStatement(const ast::DeclarationStatement *ds): Statement(ds), ds(ds), decl(transform(ds->decl)) {}
    DeclarationStatement(const DeclarationStatement &) = delete;
    DeclarationStatement &operator=(const DeclarationStatement &) = delete;
    const ast::DeclarationStatement *ds;
    const Variable *decl;

    virtual void generate(Context &context) const override {
        context.unimplemented("DeclarationStatement");
    }
};

class AssertStatement: public Statement {
public:
    explicit AssertStatement(const ast::AssertStatement *as): Statement(as), as(as), statements(), expr(transform(as->expr)) {
        for (auto s: as->statements) {
            statements.push_back(transform(s));
        }
    }
    AssertStatement(const AssertStatement &) = delete;
    AssertStatement &operator=(const AssertStatement &) = delete;
    const ast::AssertStatement *as;
    std::vector<const Statement *> statements;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        context.unimplemented("AssertStatement");
    }
};

class AssignmentStatement: public Statement {
public:
    explicit AssignmentStatement(const ast::AssignmentStatement *as): Statement(as), as(as), variables(), expr(transform(as->expr)) {
        for (auto v: as->variables) {
            variables.push_back(transform(v));
        }
    }
    AssignmentStatement(const AssignmentStatement &) = delete;
    AssignmentStatement &operator=(const AssignmentStatement &) = delete;
    const ast::AssignmentStatement *as;
    std::vector<const Expression *> variables;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        expr->generate(context);
        assert(variables.size() == 1);
        //for (size_t i = 0; i < variables.size() - 1; i++) {
        //    context.code << op_dup;
        //}
        for (auto v: variables) {
             v->generate_store(context);
        }
    }
};

class ExpressionStatement: public Statement {
public:
    explicit ExpressionStatement(const ast::ExpressionStatement *es): Statement(es), es(es), expr(transform(es->expr)) {}
    ExpressionStatement(const ExpressionStatement &) = delete;
    ExpressionStatement &operator=(const ExpressionStatement &) = delete;
    const ast::ExpressionStatement *es;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        expr->generate(context);
    }
};

class CompoundStatement: public Statement {
public:
    explicit CompoundStatement(const ast::CompoundStatement *cs): Statement(cs), cs(cs), statements() {
        for (auto s: cs->statements) {
            statements.push_back(transform(s));
        }
    }
    CompoundStatement(const CompoundStatement &) = delete;
    CompoundStatement &operator=(const CompoundStatement &) = delete;
    const ast::CompoundStatement *cs;
    std::vector<const Statement *> statements;

    virtual void generate(Context &context) const override {
        for (auto s: statements) {
            s->generate(context);
        }
    }
};

class BaseLoopStatement: public CompoundStatement {
public:
    explicit BaseLoopStatement(const ast::BaseLoopStatement *bls): CompoundStatement(bls), bls(bls), prologue(), tail() {
        for (auto s: bls->prologue) {
            prologue.push_back(transform(s));
        }
        for (auto s: bls->tail) {
            tail.push_back(transform(s));
        }
    }
    BaseLoopStatement(const BaseLoopStatement &) = delete;
    BaseLoopStatement &operator=(const BaseLoopStatement &) = delete;
    const ast::BaseLoopStatement *bls;
    std::vector<const Statement *> prologue;
    std::vector<const Statement *> tail;

    virtual void generate(Context &context) const override {
        context.unimplemented("BaseLoopStatement");
    }
};

class CaseStatement: public Statement {
public:
    class WhenCondition {
    public:
        WhenCondition() {}
        WhenCondition(const WhenCondition &) = delete;
        WhenCondition &operator=(const WhenCondition &) = delete;
        virtual ~WhenCondition() {}
        virtual void generate(Context &context, Context::Label &label_true, Context::Label &label_false) const = 0;
    };
    class ComparisonWhenCondition: public WhenCondition {
    public:
        ComparisonWhenCondition(ast::ComparisonExpression::Comparison comp, const Expression *expr): comp(comp), expr(expr) {}
        ComparisonWhenCondition(const ComparisonWhenCondition &) = delete;
        ComparisonWhenCondition &operator=(const ComparisonWhenCondition &) = delete;
        ast::ComparisonExpression::Comparison comp;
        const Expression *expr;
        virtual void generate(Context &context, Context::Label &, Context::Label &) const override {
            context.unimplemented("ComparisonWhenCondition");
        }
    };
    class RangeWhenCondition: public WhenCondition {
    public:
        RangeWhenCondition(const Expression *low_expr, const Expression *high_expr): low_expr(low_expr), high_expr(high_expr) {}
        RangeWhenCondition(const RangeWhenCondition &) = delete;
        RangeWhenCondition &operator=(const RangeWhenCondition &) = delete;
        const Expression *low_expr;
        const Expression *high_expr;
        virtual void generate(Context &context, Context::Label &, Context::Label &) const override {
            context.unimplemented("RangeWhenCondition");
        }
    };
    explicit CaseStatement(const ast::CaseStatement *cs): Statement(cs), cs(cs), expr(transform(cs->expr)), clauses() {
        for (auto &c: cs->clauses) {
            std::vector<const WhenCondition *> whens;
            for (auto w: c.first) {
                auto *cwc = dynamic_cast<const ast::CaseStatement::ComparisonWhenCondition *>(w);
                auto *rwc = dynamic_cast<const ast::CaseStatement::RangeWhenCondition *>(w);
                if (cwc != nullptr) {
                    whens.push_back(new ComparisonWhenCondition(cwc->comp, transform(cwc->expr)));
                } else if (rwc != nullptr) {
                    whens.push_back(new RangeWhenCondition(transform(rwc->low_expr), transform(rwc->high_expr)));
                } else {
                    internal_error("CaseStatement");
                }
            }
            std::vector<const Statement *> statements;
            for (auto s: c.second) {
                statements.push_back(transform(s));
            }
            clauses.push_back(std::make_pair(whens, statements));
        }
    }
    CaseStatement(const CaseStatement &) = delete;
    CaseStatement &operator=(const CaseStatement &) = delete;
    const ast::CaseStatement *cs;
    const Expression *expr;
    std::vector<std::pair<std::vector<const WhenCondition *>, std::vector<const Statement *>>> clauses;

    virtual void generate(Context &context) const override {
        context.unimplemented("CaseStatement");
    }
};

class ExitStatement: public Statement {
public:
    explicit ExitStatement(const ast::ExitStatement *es): Statement(es), es(es) {}
    ExitStatement(const ExitStatement &) = delete;
    ExitStatement &operator=(const ExitStatement &) = delete;
    const ast::ExitStatement *es;

    void generate(Context &context) const override {
        context.unimplemented("ExitStatement");
    }
};

class NextStatement: public Statement {
public:
    explicit NextStatement(const ast::NextStatement *ns): Statement(ns), ns(ns) {}
    NextStatement(const NextStatement &) = delete;
    NextStatement &operator=(const NextStatement &) = delete;
    const ast::NextStatement *ns;

    virtual void generate(Context &context) const override {
        context.unimplemented("NextStatement");
    }
};

class TryStatementTrap {
public:
    explicit TryStatementTrap(const ast::TryTrap *tt): tt(tt), name(transform(tt->name)), handler() {
        for (auto s: dynamic_cast<const ast::ExceptionHandlerStatement *>(tt->handler)->statements) {
            handler.push_back(transform(s));
        }
    }
    TryStatementTrap(const TryStatementTrap &) = delete;
    TryStatementTrap &operator=(const TryStatementTrap &) = delete;
    const ast::TryTrap *tt;
    const Variable *name;
    std::vector<const Statement *> handler;
};

class TryStatement: public Statement {
public:
    explicit TryStatement(const ast::TryStatement *ts): Statement(ts), ts(ts), statements(), catches() {
        for (auto s: ts->statements) {
            statements.push_back(transform(s));
        }
        for (auto &t: ts->catches) {
            catches.push_back(new TryStatementTrap(&t));
        }
    }
    TryStatement(const TryStatement &) = delete;
    TryStatement &operator=(const TryStatement &) = delete;
    const ast::TryStatement *ts;
    std::vector<const Statement *> statements;
    std::vector<const TryStatementTrap *> catches;

    virtual void generate(Context &context) const override {
        context.unimplemented("TryStatement");
    }
};

class ReturnStatement: public Statement {
public:
    explicit ReturnStatement(const ast::ReturnStatement *rs): Statement(rs), rs(rs), expr(transform(rs->expr)) {}
    ReturnStatement(const ReturnStatement &) = delete;
    ReturnStatement &operator=(const ReturnStatement &) = delete;
    const ast::ReturnStatement *rs;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        context.unimplemented("ReturnStatement");
    }
};

class IncrementStatement: public Statement {
public:
    explicit IncrementStatement(const ast::IncrementStatement *is): Statement(is), is(is), ref(transform(is->ref)) {}
    IncrementStatement(const IncrementStatement &) = delete;
    IncrementStatement &operator=(const IncrementStatement &) = delete;
    const ast::IncrementStatement *is;
    const Expression *ref;

    virtual void generate(Context &context) const override {
        context.unimplemented("IncrementStatement");
    }
};

class IfStatement: public Statement {
public:
    explicit IfStatement(const ast::IfStatement *is): Statement(is), is(is), condition_statements(), else_statements() {
        for (auto cs: is->condition_statements) {
            std::vector<const Statement *> statements;
            for (auto s: cs.second) {
                statements.push_back(transform(s));
            }
            condition_statements.push_back(std::make_pair(transform(cs.first), statements));
        }
        for (auto s: is->else_statements) {
            else_statements.push_back(transform(s));
        }
    }
    IfStatement(const IfStatement &) = delete;
    IfStatement &operator=(const IfStatement &) = delete;
    const ast::IfStatement *is;
    std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    std::vector<const Statement *> else_statements;

    void generate(Context &context) const override {
        context.unimplemented("IfStatement");
    }
};

class RaiseStatement: public Statement {
public:
    explicit RaiseStatement(const ast::RaiseStatement *rs): Statement(rs), rs(rs) {}
    RaiseStatement(const RaiseStatement &) = delete;
    RaiseStatement &operator=(const RaiseStatement &) = delete;
    const ast::RaiseStatement *rs;

    virtual void generate(Context &context) const override {
        context.unimplemented("RaiseStatement");
    }
};

class ResetStatement: public Statement {
public:
    explicit ResetStatement(const ast::ResetStatement *rs): Statement(rs), rs(rs) {}
    ResetStatement(const ResetStatement &) = delete;
    ResetStatement &operator=(const ResetStatement &) = delete;
    const ast::ResetStatement *rs;

    virtual void generate(Context &) const override {
    }
};

class Function: public Variable {
public:
    explicit Function(const ast::Function *f): Variable(f), f(f), statements(), params(), signature(), out_count(0) {
        // Need to transform the function parameters before transforming
        // the code that might use them (statements).
        signature = "(";
        int i = 0;
        for (auto p: f->params) {
            FunctionParameter *q = new FunctionParameter(p, i);
            params.push_back(q);
            g_variable_cache[p] = q;
            signature.append(q->type->jtype);
            if (q->fp->mode == ast::ParameterType::Mode::INOUT || q->fp->mode == ast::ParameterType::Mode::OUT) {
                out_count++;
            }
            i++;
        }
        signature.append(")");
        if (out_count > 0) {
            signature.append("[Ljava/lang/Object;");
        } else {
            signature.append(dynamic_cast<const TypeFunction *>(type)->returntype->jtype);
        }
        for (auto s: f->statements) {
            statements.push_back(transform(s));
        }
    }
    Function(const Function &) = delete;
    Function &operator=(const Function &) = delete;
    const ast::Function *f;
    std::vector<const Statement *> statements;
    std::vector<FunctionParameter *> params;
    std::string signature;
    int out_count;

    virtual void generate_decl(ClassContext &context, bool) const override {
        context.unimplemented("Function");
    }
    virtual void generate_load(Context &context) const override {
        context.unimplemented("Function");
    }
    virtual void generate_store(Context &) const override { internal_error("Function"); }
    virtual void generate_call(Context &context, const std::vector<const Expression *> &) const override {
        context.unimplemented("Function");
    }
};

class PredefinedFunction: public Variable {
public:
    explicit PredefinedFunction(const ast::PredefinedFunction *pf): Variable(pf), pf(pf), out_count(0) {
        const ast::TypeFunction *tf = dynamic_cast<const ast::TypeFunction *>(pf->type);
        for (auto p: tf->params) {
            if (p->mode == ast::ParameterType::Mode::INOUT || p->mode == ast::ParameterType::Mode::OUT) {
                out_count++;
            }
        }
    }
    PredefinedFunction(const PredefinedFunction &) = delete;
    PredefinedFunction &operator=(const PredefinedFunction &) = delete;
    const ast::PredefinedFunction *pf;
    int out_count;

    virtual void generate_decl(ClassContext &, bool) const override {}
    virtual void generate_load(Context &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_store(Context &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_call(Context &context, const std::vector<const Expression *> &args) const override {
        for (auto a: args) {
            a->generate(context);
        }
        if (pf->name == "print") {
            context.code << op_call << context.md.MemberRefToken(MemberRef(
                MemberRefParent_TypeRef(context.md.TypeRefIndex(TypeRef(
                    ResolutionScope_AssemblyRef(context.md.AssemblyRefIndex(AssemblyRef(
                        context.md.String("Neon"),
                        0, // TODO: not sure where to get this or whether it's necessary
                        AssemblyFlags_Retargetable
                    ))),
                    context.md.String("Global"),
                    context.md.String("Neon")))),
                context.md.String("print"),
                context.md.Blob(MethodRefSig_WriteLine().serialize())
            ));
        } else if (pf->name == "object__makeString") {
            context.code << op_call << context.md.MemberRefToken(MemberRef(
                MemberRefParent_TypeRef(context.md.TypeRefIndex(TypeRef(
                    ResolutionScope_AssemblyRef(context.md.AssemblyRefIndex(AssemblyRef(
                        context.md.String("Neon"),
                        0, // TODO: not sure where to get this or whether it's necessary
                        AssemblyFlags_Retargetable
                    ))),
                    context.md.String("Global"),
                    context.md.String("Neon")))),
                context.md.String("object__makeString"),
                context.md.Blob(MethodRefSig_makeString().serialize())
            ));
        } else {
            internal_error("predefined function: " + pf->name);
        }
    }
};

class ModuleFunction: public Variable {
public:
    explicit ModuleFunction(const ast::ModuleFunction *mf): Variable(mf), mf(mf), signature(), out_count(0) {
        const TypeFunction *functype = dynamic_cast<const TypeFunction *>(transform(mf->type));
        signature = "(";
        int i = 0;
        for (auto p: functype->paramtypes) {
            signature.append(p.second->jtype);
            if (p.first == ast::ParameterType::Mode::INOUT || p.first == ast::ParameterType::Mode::OUT) {
                out_count++;
            }
            i++;
        }
        signature.append(")");
        if (out_count > 0) {
            signature.append("[Ljava/lang/Object;");
        } else {
            signature.append(functype->returntype->jtype);
        }
    }
    ModuleFunction(const ModuleFunction &) = delete;
    ModuleFunction &operator=(const ModuleFunction &) = delete;
    const ast::ModuleFunction *mf;
    std::string signature;
    int out_count;

    virtual void generate_decl(ClassContext &, bool) const override {}
    virtual void generate_load(Context &) const override { internal_error("ModuleFunction"); }
    virtual void generate_store(Context &) const override { internal_error("ModuleFunction"); }
    virtual void generate_call(Context &context, const std::vector<const Expression *> &) const override {
        context.unimplemented("ModuleFunction");
    }
};

class Program {
public:
    Program(CompilerSupport *support, const ast::Program *program): support(support), program(program), statements() {
        for (auto s: program->statements) {
            statements.push_back(transform(s));
        }
    }
    Program(const Program &) = delete;
    Program &operator=(const Program &) = delete;
    virtual ~Program() {}
    CompilerSupport *support;
    const ast::Program *program;
    std::vector<const Statement *> statements;

    virtual void generate() const {
        std::string path;
        std::string::size_type i = program->source_path.find_last_of("/\\:");
        if (i != std::string::npos) {
            path = program->source_path.substr(0, i + 1);
        }
        ExecutableFile exe(path, program->module_name);
        Module mod;
        mod.Name = exe.md.String("hello");
        mod.Mvid = exe.md.Guid("1234567890123456");
        exe.md.Tables.Module_Table.push_back(mod);
        TypeDef td;
        td.Flags = TypeAttributes_Visibility_Public;
        td.TypeName = exe.md.String("<Module>");
        //td.Extends = 
        td.FieldList = static_cast<uint16_t>(1 + exe.md.Tables.Field_Table.size());
        td.MethodList = static_cast<uint16_t>(1 + exe.md.Tables.MethodDef_Table.size());
        exe.md.Tables.TypeDef_Table.push_back(td);
        Context context(exe, "Main");
        for (auto s: statements) {
            s->generate(context);
        }
        context.code << op_ret;
        exe.text.data << context.finalize();
        std::vector<uint8_t> data = exe.finalize();
        support->writeOutput(path + program->module_name + ".exe", data);
    }
};

class TypeTransformer: public ast::IAstVisitor {
public:
    TypeTransformer(): r(nullptr) {}
    TypeTransformer(const TypeTransformer &) = delete;
    TypeTransformer &operator=(const TypeTransformer &) = delete;
    Type *retval() { if (r == nullptr) internal_error("TypeTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *node) { r = new TypeNothing(node); }
    virtual void visit(const ast::TypeDummy *node) { r = new TypeDummy(node); }
    virtual void visit(const ast::TypeBoolean *node) { r = new TypeBoolean(node); }
    virtual void visit(const ast::TypeNumber *node) { r = new TypeNumber(node); }
    virtual void visit(const ast::TypeString *node) { r = new TypeString(node); }
    virtual void visit(const ast::TypeBytes *node) { r = new TypeBytes(node); }
    virtual void visit(const ast::TypeObject *node) { r = new TypeObject(node); }
    virtual void visit(const ast::TypeFunction *node) { r = new TypeFunction(node); }
    virtual void visit(const ast::TypeArray *node) { r = new TypeArray(node); }
    virtual void visit(const ast::TypeDictionary *node) { r = new TypeDictionary(node); }
    virtual void visit(const ast::TypeRecord *node) { r = new TypeRecord(node); }
    virtual void visit(const ast::TypeClass *node) { r = new TypeRecord(node); }
    virtual void visit(const ast::TypePointer *node) { r = new TypePointer(node); }
    virtual void visit(const ast::TypeInterfacePointer *) { internal_error("TypeInterfacePointer"); }
    virtual void visit(const ast::TypeFunctionPointer *node) { r = new TypeFunctionPointer(node); }
    virtual void visit(const ast::TypeEnum *node) { r = new TypeEnum(node); }
    virtual void visit(const ast::TypeChoice *node) { r = new TypeChoice(node); }
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::TypeInterface *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *) {}
    virtual void visit(const ast::ModuleVariable *) {}
    virtual void visit(const ast::GlobalVariable *) {}
    virtual void visit(const ast::ExternalGlobalVariable *) {}
    virtual void visit(const ast::LocalVariable *) {}
    virtual void visit(const ast::FunctionParameter *) {}
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Interface *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *) {}
    virtual void visit(const ast::ConstantNumberExpression *) {}
    virtual void visit(const ast::ConstantStringExpression *) {}
    virtual void visit(const ast::ConstantBytesExpression *) {}
    virtual void visit(const ast::ConstantEnumExpression *) {}
    virtual void visit(const ast::ConstantChoiceExpression *) {}
    virtual void visit(const ast::ConstantNilExpression *) {}
    virtual void visit(const ast::ConstantNowhereExpression *) {}
    virtual void visit(const ast::ConstantNilObject *) {}
    virtual void visit(const ast::TypeConversionExpression *) {}
    virtual void visit(const ast::ArrayLiteralExpression *) {}
    virtual void visit(const ast::DictionaryLiteralExpression *) {}
    virtual void visit(const ast::RecordLiteralExpression *) {}
    virtual void visit(const ast::ClassLiteralExpression *) {}
    virtual void visit(const ast::NewClassExpression *) {}
    virtual void visit(const ast::UnaryMinusExpression *) {}
    virtual void visit(const ast::LogicalNotExpression *) {}
    virtual void visit(const ast::ConditionalExpression *) {}
    virtual void visit(const ast::TryExpression *) {}
    virtual void visit(const ast::DisjunctionExpression *) {}
    virtual void visit(const ast::ConjunctionExpression *) {}
    virtual void visit(const ast::TypeTestExpression *) {}
    virtual void visit(const ast::ChoiceTestExpression *) {}
    virtual void visit(const ast::ArrayInExpression *) {}
    virtual void visit(const ast::DictionaryInExpression *) {}
    virtual void visit(const ast::ChainedComparisonExpression *) {}
    virtual void visit(const ast::BooleanComparisonExpression *) {}
    virtual void visit(const ast::NumericComparisonExpression *) {}
    virtual void visit(const ast::EnumComparisonExpression *) {}
    virtual void visit(const ast::StringComparisonExpression *) {}
    virtual void visit(const ast::BytesComparisonExpression *) {}
    virtual void visit(const ast::ArrayComparisonExpression *) {}
    virtual void visit(const ast::DictionaryComparisonExpression *) {}
    virtual void visit(const ast::RecordComparisonExpression *) {}
    virtual void visit(const ast::PointerComparisonExpression *) {}
    virtual void visit(const ast::ValidPointerExpression *) {}
    virtual void visit(const ast::FunctionPointerComparisonExpression *) {}
    virtual void visit(const ast::AdditionExpression *) {}
    virtual void visit(const ast::SubtractionExpression *) {}
    virtual void visit(const ast::MultiplicationExpression *) {}
    virtual void visit(const ast::DivisionExpression *) {}
    virtual void visit(const ast::ModuloExpression *) {}
    virtual void visit(const ast::ExponentiationExpression *) {}
    virtual void visit(const ast::DummyExpression *) {}
    virtual void visit(const ast::ArrayReferenceIndexExpression *) {}
    virtual void visit(const ast::ArrayValueIndexExpression *) {}
    virtual void visit(const ast::DictionaryReferenceIndexExpression *) {}
    virtual void visit(const ast::DictionaryValueIndexExpression *) {}
    virtual void visit(const ast::StringReferenceIndexExpression *) {}
    virtual void visit(const ast::StringValueIndexExpression *) {}
    virtual void visit(const ast::StringReferenceRangeIndexExpression *) {}
    virtual void visit(const ast::StringValueRangeIndexExpression *) {}
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::BytesReferenceRangeIndexExpression *) {}
    virtual void visit(const ast::BytesValueRangeIndexExpression *) {}
    virtual void visit(const ast::ObjectSubscriptExpression *) {}
    virtual void visit(const ast::RecordReferenceFieldExpression *) {}
    virtual void visit(const ast::RecordValueFieldExpression *) {}
    virtual void visit(const ast::ArrayReferenceRangeExpression *) {}
    virtual void visit(const ast::ArrayValueRangeExpression *) {}
    virtual void visit(const ast::ChoiceReferenceExpression *) {}
    virtual void visit(const ast::PointerDereferenceExpression *) {}
    virtual void visit(const ast::ConstantExpression *) {}
    virtual void visit(const ast::VariableExpression *) {}
    virtual void visit(const ast::InterfaceMethodExpression *) {}
    virtual void visit(const ast::InterfacePointerConstructor *) {}
    virtual void visit(const ast::InterfacePointerDeconstructor *) {}
    virtual void visit(const ast::FunctionCall *) {}
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *) {}
    virtual void visit(const ast::TypeDeclarationStatement *) {}
    virtual void visit(const ast::DeclarationStatement *) {}
    virtual void visit(const ast::ExceptionHandlerStatement *) {}
    virtual void visit(const ast::AssertStatement *) {}
    virtual void visit(const ast::AssignmentStatement *) {}
    virtual void visit(const ast::ExpressionStatement *) {}
    virtual void visit(const ast::ReturnStatement *) {}
    virtual void visit(const ast::IncrementStatement *) {}
    virtual void visit(const ast::IfStatement *) {}
    virtual void visit(const ast::BaseLoopStatement *) {}
    virtual void visit(const ast::CaseStatement *) {}
    virtual void visit(const ast::ExitStatement *) {}
    virtual void visit(const ast::NextStatement *) {}
    virtual void visit(const ast::TryStatement *) {}
    virtual void visit(const ast::RaiseStatement *) {}
    virtual void visit(const ast::ResetStatement *) {}
    virtual void visit(const ast::Function *) {}
    virtual void visit(const ast::PredefinedFunction *) {}
    virtual void visit(const ast::ExtensionFunction *) {}
    virtual void visit(const ast::ModuleFunction *) {}
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Type *r;
};

class VariableTransformer: public ast::IAstVisitor {
public:
    VariableTransformer(): r(nullptr) {}
    VariableTransformer(const VariableTransformer &) = delete;
    VariableTransformer &operator=(const VariableTransformer &) = delete;
    Variable *retval() { if (r == nullptr) internal_error("VariableTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *) {}
    virtual void visit(const ast::TypeDummy *) {}
    virtual void visit(const ast::TypeBoolean *) {}
    virtual void visit(const ast::TypeNumber *) {}
    virtual void visit(const ast::TypeString *) {}
    virtual void visit(const ast::TypeBytes *) {}
    virtual void visit(const ast::TypeObject *) {}
    virtual void visit(const ast::TypeFunction *) {}
    virtual void visit(const ast::TypeArray *) {}
    virtual void visit(const ast::TypeDictionary *) {}
    virtual void visit(const ast::TypeRecord *) {}
    virtual void visit(const ast::TypeClass *) {}
    virtual void visit(const ast::TypePointer *) {}
    virtual void visit(const ast::TypeInterfacePointer *) {}
    virtual void visit(const ast::TypeFunctionPointer *) {}
    virtual void visit(const ast::TypeEnum *) {}
    virtual void visit(const ast::TypeChoice *) {}
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::TypeInterface *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *node) { r = new PredefinedVariable(node); }
    virtual void visit(const ast::ModuleVariable *node) { r = new ModuleVariable(node); }
    virtual void visit(const ast::GlobalVariable *node) { r = new GlobalVariable(node); }
    virtual void visit(const ast::ExternalGlobalVariable *) { internal_error("ExternalGlobalVariable"); }
    virtual void visit(const ast::LocalVariable *node) { r = new LocalVariable(node); }
    virtual void visit(const ast::FunctionParameter *) { /*r = new FunctionParameter(node);*/ }
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Interface *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *) {}
    virtual void visit(const ast::ConstantNumberExpression *) {}
    virtual void visit(const ast::ConstantStringExpression *) {}
    virtual void visit(const ast::ConstantBytesExpression *) {}
    virtual void visit(const ast::ConstantEnumExpression *) {}
    virtual void visit(const ast::ConstantChoiceExpression *) {}
    virtual void visit(const ast::ConstantNilExpression *) {}
    virtual void visit(const ast::ConstantNowhereExpression *) {}
    virtual void visit(const ast::ConstantNilObject *) {}
    virtual void visit(const ast::TypeConversionExpression *) {}
    virtual void visit(const ast::ArrayLiteralExpression *) {}
    virtual void visit(const ast::DictionaryLiteralExpression *) {}
    virtual void visit(const ast::RecordLiteralExpression *) {}
    virtual void visit(const ast::NewClassExpression *) {}
    virtual void visit(const ast::UnaryMinusExpression *) {}
    virtual void visit(const ast::LogicalNotExpression *) {}
    virtual void visit(const ast::ConditionalExpression *) {}
    virtual void visit(const ast::ClassLiteralExpression *) {}
    virtual void visit(const ast::TryExpression *) {}
    virtual void visit(const ast::DisjunctionExpression *) {}
    virtual void visit(const ast::ConjunctionExpression *) {}
    virtual void visit(const ast::TypeTestExpression *) {}
    virtual void visit(const ast::ChoiceTestExpression *) {}
    virtual void visit(const ast::ArrayInExpression *) {}
    virtual void visit(const ast::DictionaryInExpression *) {}
    virtual void visit(const ast::ChainedComparisonExpression *) {}
    virtual void visit(const ast::BooleanComparisonExpression *) {}
    virtual void visit(const ast::NumericComparisonExpression *) {}
    virtual void visit(const ast::EnumComparisonExpression *) {}
    virtual void visit(const ast::StringComparisonExpression *) {}
    virtual void visit(const ast::BytesComparisonExpression *) {}
    virtual void visit(const ast::ArrayComparisonExpression *) {}
    virtual void visit(const ast::DictionaryComparisonExpression *) {}
    virtual void visit(const ast::RecordComparisonExpression *) {}
    virtual void visit(const ast::PointerComparisonExpression *) {}
    virtual void visit(const ast::ValidPointerExpression *) {}
    virtual void visit(const ast::FunctionPointerComparisonExpression *) {}
    virtual void visit(const ast::AdditionExpression *) {}
    virtual void visit(const ast::SubtractionExpression *) {}
    virtual void visit(const ast::MultiplicationExpression *) {}
    virtual void visit(const ast::DivisionExpression *) {}
    virtual void visit(const ast::ModuloExpression *) {}
    virtual void visit(const ast::ExponentiationExpression *) {}
    virtual void visit(const ast::DummyExpression *) {}
    virtual void visit(const ast::ArrayReferenceIndexExpression *) {}
    virtual void visit(const ast::ArrayValueIndexExpression *) {}
    virtual void visit(const ast::DictionaryReferenceIndexExpression *) {}
    virtual void visit(const ast::DictionaryValueIndexExpression *) {}
    virtual void visit(const ast::StringReferenceIndexExpression *) {}
    virtual void visit(const ast::StringValueIndexExpression *) {}
    virtual void visit(const ast::StringReferenceRangeIndexExpression *) {}
    virtual void visit(const ast::StringValueRangeIndexExpression *) {}
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::BytesReferenceRangeIndexExpression *) {}
    virtual void visit(const ast::BytesValueRangeIndexExpression *) {}
    virtual void visit(const ast::ObjectSubscriptExpression *) {}
    virtual void visit(const ast::RecordReferenceFieldExpression *) {}
    virtual void visit(const ast::RecordValueFieldExpression *) {}
    virtual void visit(const ast::ArrayReferenceRangeExpression *) {}
    virtual void visit(const ast::ArrayValueRangeExpression *) {}
    virtual void visit(const ast::ChoiceReferenceExpression *) {}
    virtual void visit(const ast::PointerDereferenceExpression *) {}
    virtual void visit(const ast::ConstantExpression *) {}
    virtual void visit(const ast::VariableExpression *) {}
    virtual void visit(const ast::InterfaceMethodExpression *) {}
    virtual void visit(const ast::InterfacePointerConstructor *) {}
    virtual void visit(const ast::InterfacePointerDeconstructor *) {}
    virtual void visit(const ast::FunctionCall *) {}
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *) {}
    virtual void visit(const ast::TypeDeclarationStatement *) {}
    virtual void visit(const ast::DeclarationStatement *) {}
    virtual void visit(const ast::ExceptionHandlerStatement *) {}
    virtual void visit(const ast::AssertStatement *) {}
    virtual void visit(const ast::AssignmentStatement *) {}
    virtual void visit(const ast::ExpressionStatement *) {}
    virtual void visit(const ast::ReturnStatement *) {}
    virtual void visit(const ast::IncrementStatement *) {}
    virtual void visit(const ast::IfStatement *) {}
    virtual void visit(const ast::BaseLoopStatement *) {}
    virtual void visit(const ast::CaseStatement *) {}
    virtual void visit(const ast::ExitStatement *) {}
    virtual void visit(const ast::NextStatement *) {}
    virtual void visit(const ast::TryStatement *) {}
    virtual void visit(const ast::RaiseStatement *) {}
    virtual void visit(const ast::ResetStatement *) {}
    virtual void visit(const ast::Function *node) { r = new Function(node); }
    virtual void visit(const ast::PredefinedFunction *node) { r = new PredefinedFunction(node); }
    virtual void visit(const ast::ExtensionFunction *) { internal_error("ExtensionFunction"); }
    virtual void visit(const ast::ModuleFunction *node) { r = new ModuleFunction(node); }
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Variable *r;
};

class ExpressionTransformer: public ast::IAstVisitor {
public:
    ExpressionTransformer(): r(nullptr) {}
    ExpressionTransformer(const ExpressionTransformer &) = delete;
    ExpressionTransformer &operator=(const ExpressionTransformer &) = delete;
    Expression *retval() { if (r == nullptr) internal_error("ExpressionTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *) {}
    virtual void visit(const ast::TypeDummy *) {}
    virtual void visit(const ast::TypeBoolean *) {}
    virtual void visit(const ast::TypeNumber *) {}
    virtual void visit(const ast::TypeString *) {}
    virtual void visit(const ast::TypeBytes *) {}
    virtual void visit(const ast::TypeObject *) {}
    virtual void visit(const ast::TypeFunction *) {}
    virtual void visit(const ast::TypeArray *) {}
    virtual void visit(const ast::TypeDictionary *) {}
    virtual void visit(const ast::TypeRecord *) {}
    virtual void visit(const ast::TypeClass *) {}
    virtual void visit(const ast::TypePointer *) {}
    virtual void visit(const ast::TypeInterfacePointer *) {}
    virtual void visit(const ast::TypeFunctionPointer *) {}
    virtual void visit(const ast::TypeEnum *) {}
    virtual void visit(const ast::TypeChoice *) {}
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::TypeInterface *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *) {}
    virtual void visit(const ast::ModuleVariable *) {}
    virtual void visit(const ast::GlobalVariable *) {}
    virtual void visit(const ast::ExternalGlobalVariable *) {}
    virtual void visit(const ast::LocalVariable *) {}
    virtual void visit(const ast::FunctionParameter *) {}
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Interface *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *node) { r = new ConstantBooleanExpression(node); }
    virtual void visit(const ast::ConstantNumberExpression *node) { r = new ConstantNumberExpression(node); }
    virtual void visit(const ast::ConstantStringExpression *node) { r = new ConstantStringExpression(node); }
    virtual void visit(const ast::ConstantBytesExpression *node) { r = new ConstantBytesExpression(node); }
    virtual void visit(const ast::ConstantEnumExpression *node) { r = new ConstantEnumExpression(node); }
    virtual void visit(const ast::ConstantChoiceExpression *node) { r = new ConstantChoiceExpression(node); }
    virtual void visit(const ast::ConstantNilExpression *node) { r = new ConstantNilExpression(node); }
    virtual void visit(const ast::ConstantNowhereExpression *node) { r = new ConstantNowhereExpression(node); }
    virtual void visit(const ast::ConstantNilObject *) { internal_error("ConstantNilObject"); }
    virtual void visit(const ast::TypeConversionExpression *) { internal_error("TypeConversionExpression"); }
    virtual void visit(const ast::ArrayLiteralExpression *node) { r = new ArrayLiteralExpression(node); }
    virtual void visit(const ast::DictionaryLiteralExpression *node) { r = new DictionaryLiteralExpression(node); }
    virtual void visit(const ast::RecordLiteralExpression *node) { r = new RecordLiteralExpression(node); }
    virtual void visit(const ast::ClassLiteralExpression *) { internal_error("ClassLiteralExpression"); }
    virtual void visit(const ast::NewClassExpression *node) { r =  new NewClassExpression(node); }
    virtual void visit(const ast::UnaryMinusExpression *node) { r = new UnaryMinusExpression(node); }
    virtual void visit(const ast::LogicalNotExpression *node) { r = new LogicalNotExpression(node); }
    virtual void visit(const ast::ConditionalExpression *node) { r = new ConditionalExpression(node); }
    virtual void visit(const ast::TryExpression *node) { r = new TryExpression(node); }
    virtual void visit(const ast::DisjunctionExpression *node) { r = new DisjunctionExpression(node); }
    virtual void visit(const ast::ConjunctionExpression *node) { r = new ConjunctionExpression(node); }
    virtual void visit(const ast::TypeTestExpression *) { internal_error("TypeTestExpression"); }
    virtual void visit(const ast::ChoiceTestExpression *) { internal_error("ChoiceTestExpression"); }
    virtual void visit(const ast::ArrayInExpression *node) { r = new ArrayInExpression(node); }
    virtual void visit(const ast::DictionaryInExpression *node) { r =  new DictionaryInExpression(node); }
    virtual void visit(const ast::ChainedComparisonExpression *node) { r = new ChainedComparisonExpression(node); }
    virtual void visit(const ast::BooleanComparisonExpression *node) { r = new BooleanComparisonExpression(node); }
    virtual void visit(const ast::NumericComparisonExpression *node) { r = new NumericComparisonExpression(node); }
    virtual void visit(const ast::EnumComparisonExpression *node) { r = new EnumComparisonExpression(node); }
    virtual void visit(const ast::StringComparisonExpression *node) { r = new StringComparisonExpression(node); }
    virtual void visit(const ast::BytesComparisonExpression *node) { r = new BytesComparisonExpression(node); }
    virtual void visit(const ast::ArrayComparisonExpression *node) { r = new ArrayComparisonExpression(node); }
    virtual void visit(const ast::DictionaryComparisonExpression *node) { r = new DictionaryComparisonExpression(node); }
    virtual void visit(const ast::RecordComparisonExpression *node) { r = new RecordComparisonExpression(node); }
    virtual void visit(const ast::PointerComparisonExpression *node) { r = new PointerComparisonExpression(node); }
    virtual void visit(const ast::ValidPointerExpression *node) { r = new ValidPointerExpression(node); }
    virtual void visit(const ast::FunctionPointerComparisonExpression *node) { r = new FunctionPointerComparisonExpression(node); }
    virtual void visit(const ast::AdditionExpression *node) { r = new AdditionExpression(node); }
    virtual void visit(const ast::SubtractionExpression *node) { r = new SubtractionExpression(node); }
    virtual void visit(const ast::MultiplicationExpression *node) { r = new MultiplicationExpression(node); }
    virtual void visit(const ast::DivisionExpression *node) { r = new DivisionExpression(node); }
    virtual void visit(const ast::ModuloExpression *node) { r = new ModuloExpression(node); }
    virtual void visit(const ast::ExponentiationExpression *node) { r = new ExponentiationExpression(node); }
    virtual void visit(const ast::DummyExpression *node) { r = new DummyExpression(node); }
    virtual void visit(const ast::ArrayReferenceIndexExpression *node) { r = new ArrayReferenceIndexExpression(node); }
    virtual void visit(const ast::ArrayValueIndexExpression *node) { r = new ArrayValueIndexExpression(node); }
    virtual void visit(const ast::DictionaryReferenceIndexExpression *node) { r = new DictionaryReferenceIndexExpression(node); }
    virtual void visit(const ast::DictionaryValueIndexExpression *node) { r = new DictionaryValueIndexExpression(node); }
    virtual void visit(const ast::StringReferenceIndexExpression *node) { r = new StringReferenceIndexExpression(node); }
    virtual void visit(const ast::StringValueIndexExpression *node) { r = new StringValueIndexExpression(node); }
    virtual void visit(const ast::StringReferenceRangeIndexExpression *node) { r = new StringReferenceRangeIndexExpression(node); }
    virtual void visit(const ast::StringValueRangeIndexExpression *node) { r = new StringValueRangeIndexExpression(node); }
    virtual void visit(const ast::BytesReferenceIndexExpression *node) { r = new BytesReferenceIndexExpression(node); }
    virtual void visit(const ast::BytesValueIndexExpression *node) { r = new BytesValueIndexExpression(node); }
    virtual void visit(const ast::BytesReferenceRangeIndexExpression *node) { r = new BytesReferenceRangeIndexExpression(node); }
    virtual void visit(const ast::BytesValueRangeIndexExpression *node) { r = new BytesValueRangeIndexExpression(node); }
    virtual void visit(const ast::ObjectSubscriptExpression *) { internal_error("ObjectSubscriptExpression"); }
    virtual void visit(const ast::RecordReferenceFieldExpression *node) { r = new RecordReferenceFieldExpression(node); }
    virtual void visit(const ast::RecordValueFieldExpression *node) { r = new RecordValueFieldExpression(node); }
    virtual void visit(const ast::ArrayReferenceRangeExpression *node) { r = new ArrayReferenceRangeExpression(node); }
    virtual void visit(const ast::ArrayValueRangeExpression *node) { r = new ArrayValueRangeExpression(node); }
    virtual void visit(const ast::ChoiceReferenceExpression *node) { r = new ChoiceReferenceExpression(node); }
    virtual void visit(const ast::PointerDereferenceExpression *node) { r =  new PointerDereferenceExpression(node); }
    virtual void visit(const ast::ConstantExpression *node) { r = transform(node->constant->value); }
    virtual void visit(const ast::VariableExpression *node) { r = new VariableExpression(node); }
    virtual void visit(const ast::InterfaceMethodExpression *) { internal_error("InterfaceMethodExpression"); }
    virtual void visit(const ast::InterfacePointerConstructor *) { internal_error("InterfacePointerConstructor"); }
    virtual void visit(const ast::InterfacePointerDeconstructor *) { internal_error("InterfacePointerDeconstructor"); }
    virtual void visit(const ast::FunctionCall *node) { r = new FunctionCall(node); }
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *) {}
    virtual void visit(const ast::TypeDeclarationStatement *) {}
    virtual void visit(const ast::DeclarationStatement *) {}
    virtual void visit(const ast::ExceptionHandlerStatement *) {}
    virtual void visit(const ast::AssertStatement *) {}
    virtual void visit(const ast::AssignmentStatement *) {}
    virtual void visit(const ast::ExpressionStatement *) {}
    virtual void visit(const ast::ReturnStatement *) {}
    virtual void visit(const ast::IncrementStatement *) {}
    virtual void visit(const ast::IfStatement *) {}
    virtual void visit(const ast::BaseLoopStatement *) {}
    virtual void visit(const ast::CaseStatement *) {}
    virtual void visit(const ast::ExitStatement *) {}
    virtual void visit(const ast::NextStatement *) {}
    virtual void visit(const ast::TryStatement *) {}
    virtual void visit(const ast::RaiseStatement *) {}
    virtual void visit(const ast::ResetStatement *) {}
    virtual void visit(const ast::Function *) {}
    virtual void visit(const ast::PredefinedFunction *) {}
    virtual void visit(const ast::ExtensionFunction *) {}
    virtual void visit(const ast::ModuleFunction *) {}
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Expression *r;
};

class StatementTransformer: public ast::IAstVisitor {
public:
    StatementTransformer(): r(nullptr) {}
    StatementTransformer(const StatementTransformer &) = delete;
    StatementTransformer &operator=(const StatementTransformer &) = delete;
    Statement *retval() { if (r == nullptr) internal_error("StatementTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *) {}
    virtual void visit(const ast::TypeDummy *) {}
    virtual void visit(const ast::TypeBoolean *) {}
    virtual void visit(const ast::TypeNumber *) {}
    virtual void visit(const ast::TypeString *) {}
    virtual void visit(const ast::TypeBytes *) {}
    virtual void visit(const ast::TypeObject *) {}
    virtual void visit(const ast::TypeFunction *) {}
    virtual void visit(const ast::TypeArray *) {}
    virtual void visit(const ast::TypeDictionary *) {}
    virtual void visit(const ast::TypeRecord *) {}
    virtual void visit(const ast::TypeClass *) {}
    virtual void visit(const ast::TypePointer *) {}
    virtual void visit(const ast::TypeInterfacePointer *) {}
    virtual void visit(const ast::TypeFunctionPointer *) {}
    virtual void visit(const ast::TypeEnum *) {}
    virtual void visit(const ast::TypeChoice *) {}
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::TypeInterface *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *) {}
    virtual void visit(const ast::ModuleVariable *) {}
    virtual void visit(const ast::GlobalVariable *) {}
    virtual void visit(const ast::ExternalGlobalVariable *) {}
    virtual void visit(const ast::LocalVariable *) {}
    virtual void visit(const ast::FunctionParameter *) {}
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Interface *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *) {}
    virtual void visit(const ast::ConstantNumberExpression *) {}
    virtual void visit(const ast::ConstantStringExpression *) {}
    virtual void visit(const ast::ConstantBytesExpression *) {}
    virtual void visit(const ast::ConstantEnumExpression *) {}
    virtual void visit(const ast::ConstantChoiceExpression *) {}
    virtual void visit(const ast::ConstantNilExpression *) {}
    virtual void visit(const ast::ConstantNowhereExpression *) {}
    virtual void visit(const ast::ConstantNilObject *) {}
    virtual void visit(const ast::TypeConversionExpression *) {}
    virtual void visit(const ast::ArrayLiteralExpression *) {}
    virtual void visit(const ast::DictionaryLiteralExpression *) {}
    virtual void visit(const ast::RecordLiteralExpression *) {}
    virtual void visit(const ast::ClassLiteralExpression *) {}
    virtual void visit(const ast::NewClassExpression *) {}
    virtual void visit(const ast::UnaryMinusExpression *) {}
    virtual void visit(const ast::LogicalNotExpression *) {}
    virtual void visit(const ast::ConditionalExpression *) {}
    virtual void visit(const ast::TryExpression *) {}
    virtual void visit(const ast::DisjunctionExpression *) {}
    virtual void visit(const ast::ConjunctionExpression *) {}
    virtual void visit(const ast::TypeTestExpression *) {}
    virtual void visit(const ast::ChoiceTestExpression *) {}
    virtual void visit(const ast::ArrayInExpression *) {}
    virtual void visit(const ast::DictionaryInExpression *) {}
    virtual void visit(const ast::ChainedComparisonExpression *) {}
    virtual void visit(const ast::BooleanComparisonExpression *) {}
    virtual void visit(const ast::NumericComparisonExpression *) {}
    virtual void visit(const ast::EnumComparisonExpression *) {}
    virtual void visit(const ast::StringComparisonExpression *) {}
    virtual void visit(const ast::BytesComparisonExpression *) {}
    virtual void visit(const ast::ArrayComparisonExpression *) {}
    virtual void visit(const ast::DictionaryComparisonExpression *) {}
    virtual void visit(const ast::RecordComparisonExpression *) {}
    virtual void visit(const ast::PointerComparisonExpression *) {}
    virtual void visit(const ast::ValidPointerExpression *) {}
    virtual void visit(const ast::FunctionPointerComparisonExpression *) {}
    virtual void visit(const ast::AdditionExpression *) {}
    virtual void visit(const ast::SubtractionExpression *) {}
    virtual void visit(const ast::MultiplicationExpression *) {}
    virtual void visit(const ast::DivisionExpression *) {}
    virtual void visit(const ast::ModuloExpression *) {}
    virtual void visit(const ast::ExponentiationExpression *) {}
    virtual void visit(const ast::DummyExpression *) {}
    virtual void visit(const ast::ArrayReferenceIndexExpression *) {}
    virtual void visit(const ast::ArrayValueIndexExpression *) {}
    virtual void visit(const ast::DictionaryReferenceIndexExpression *) {}
    virtual void visit(const ast::DictionaryValueIndexExpression *) {}
    virtual void visit(const ast::StringReferenceIndexExpression *) {}
    virtual void visit(const ast::StringValueIndexExpression *) {}
    virtual void visit(const ast::StringReferenceRangeIndexExpression *) {}
    virtual void visit(const ast::StringValueRangeIndexExpression *) {}
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::BytesReferenceRangeIndexExpression *) {}
    virtual void visit(const ast::BytesValueRangeIndexExpression *) {}
    virtual void visit(const ast::ObjectSubscriptExpression *) {}
    virtual void visit(const ast::RecordReferenceFieldExpression *) {}
    virtual void visit(const ast::RecordValueFieldExpression *) {}
    virtual void visit(const ast::ArrayReferenceRangeExpression *) {}
    virtual void visit(const ast::ArrayValueRangeExpression *) {}
    virtual void visit(const ast::ChoiceReferenceExpression *) {}
    virtual void visit(const ast::PointerDereferenceExpression *) {}
    virtual void visit(const ast::ConstantExpression *) {}
    virtual void visit(const ast::VariableExpression *) {}
    virtual void visit(const ast::InterfaceMethodExpression *) {}
    virtual void visit(const ast::InterfacePointerConstructor *) {}
    virtual void visit(const ast::InterfacePointerDeconstructor *) {}
    virtual void visit(const ast::FunctionCall *) {}
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *node) { r = new NullStatement(node); }
    virtual void visit(const ast::TypeDeclarationStatement *) { internal_error("TypeDeclarationStatement"); }
    virtual void visit(const ast::DeclarationStatement *node) { r = new DeclarationStatement(node); }
    virtual void visit(const ast::ExceptionHandlerStatement *) { internal_error("ExceptionHandlerStatement" ); }
    virtual void visit(const ast::AssertStatement *node) { r = new AssertStatement(node); }
    virtual void visit(const ast::AssignmentStatement *node) { r = new AssignmentStatement(node); }
    virtual void visit(const ast::ExpressionStatement *node) { r = new ExpressionStatement(node); }
    virtual void visit(const ast::ReturnStatement *node) { r = new ReturnStatement(node); }
    virtual void visit(const ast::IncrementStatement *node) { r =  new IncrementStatement(node); }
    virtual void visit(const ast::IfStatement *node) { r = new IfStatement(node); }
    virtual void visit(const ast::BaseLoopStatement *node) { r = new BaseLoopStatement(node); }
    virtual void visit(const ast::CaseStatement *node) { r = new CaseStatement(node); }
    virtual void visit(const ast::ExitStatement *node) { r = new ExitStatement(node); }
    virtual void visit(const ast::NextStatement *node) { r = new NextStatement(node); }
    virtual void visit(const ast::TryStatement *node) { r = new TryStatement(node); }
    virtual void visit(const ast::RaiseStatement *node) { r = new RaiseStatement(node); }
    virtual void visit(const ast::ResetStatement *node) { r = new ResetStatement(node); }
    virtual void visit(const ast::Function *) {}
    virtual void visit(const ast::PredefinedFunction *) {}
    virtual void visit(const ast::ExtensionFunction *) {}
    virtual void visit(const ast::ModuleFunction *) {}
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Statement *r;
};

Type *transform(const ast::Type *t)
{
    if (t == nullptr) {
        return nullptr;
    }
    //fprintf(stderr, "transform type %s %s\n", typeid(*t).name(), t->text().c_str());
    auto i = g_type_cache.find(t);
    if (i != g_type_cache.end()) {
        return i->second;
    }
    TypeTransformer tt;
    t->accept(&tt);
    return tt.retval();
}

Variable *transform(const ast::Variable *v)
{
    if (v == nullptr) {
        return nullptr;
    }
    //fprintf(stderr, "transform variable %s %s\n", typeid(*v).name(), v->text().c_str());
    auto i = g_variable_cache.find(v);
    if (i != g_variable_cache.end()) {
        return i->second;
    }
    VariableTransformer vt;
    v->accept(&vt);
    return vt.retval();
}

Expression *transform(const ast::Expression *e)
{
    if (e == nullptr) {
        return nullptr;
    }
    //fprintf(stderr, "transform expression %s %s\n", typeid(*e).name(), e->text().c_str());
    auto i = g_expression_cache.find(e);
    if (i != g_expression_cache.end()) {
        return i->second;
    }
    ExpressionTransformer et;
    e->accept(&et);
    return et.retval();
}

Statement *transform(const ast::Statement *s)
{
    if (s == nullptr) {
        return nullptr;
    }
    //fprintf(stderr, "transform statement %s %s\n", typeid(*s).name(), s->text().c_str());
    auto i = g_statement_cache.find(s);
    if (i != g_statement_cache.end()) {
        return i->second;
    }
    StatementTransformer st;
    s->accept(&st);
    return st.retval();
}

} // namespace cli

void compile_cli(CompilerSupport *support, const ast::Program *p, std::string /*output*/, std::map<std::string, std::string> /*options*/)
{
    cli::g_type_cache.clear();
    cli::g_variable_cache.clear();
    cli::g_expression_cache.clear();
    cli::g_statement_cache.clear();

    cli::Program *ct = new cli::Program(support, p);
    ct->generate();
}
