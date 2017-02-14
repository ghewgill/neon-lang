#include "ast.h"

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

const uint32_t IMG_SCN_CNT_CODE                 = 0x00000020;
//const uint32_t IMG_SCN_CNT_INITIALIZED_DATA     = 0x00000040;
//const uint32_t IMG_SCN_CNT_UNINITIALIZED_DATA   = 0x00000080;
const uint32_t IMG_SCN_MEM_EXECUTE              = 0x20000000;
const uint32_t IMG_SCN_MEM_READ                 = 0x40000000;
//const uint32_t IMG_SCN_MEM_WRITE                = 0x80000000;

const uint32_t COMIMAGE_FLAGS_ILONLY = 0x00000001;

const uint16_t MethodAttributes_MemberAccess_Public = 0x0006;
const uint16_t MethodAttributes_Static              = 0x0010;

const uint32_t TypeAttributes_Visibility_Public = 0x00000001;

//const uint8_t ELEMENT_TYPE_END      = 0x00;
const uint8_t ELEMENT_TYPE_VOID     = 0x01;
const uint8_t ELEMENT_TYPE_STRING   = 0x0e;
const uint8_t ELEMENT_TYPE_SZARRAY  = 0x1d;

const uint8_t op_call  = 0x28;
const uint8_t op_ret   = 0x2A;
const uint8_t op_ldstr = 0x72;

inline uint32_t MemberRefParent_TypeRef(uint32_t i) {
    return static_cast<uint32_t>((i << 3) | 0);
}

inline uint32_t ResolutionScope_AssemblyRef(uint32_t i) {
    return static_cast<uint32_t>((i << 2) | 2);
}

struct PE_file_header {
    PE_file_header()
      : magic(0x4550),
        machine(0x14c),
        number_of_sections(1),
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
        os_major(5),
        os_minor(0),
        user_major(0),
        user_minor(0),
        subsys_major(5),
        subsys_minor(0),
        reserved(0),
        image_size(0),
        header_size(0),
        file_checksum(0),
        subsystem(IMAGE_SUBSYSTEM_WINDOWS_CUI),
        dll_flags(0),
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

struct Import_address_table {
    Import_address_table()
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
    Stream_header(const std::string &name)
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
        Version("Standard CLI 2005"),
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

    std::vector<uint8_t> serialize() const {
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

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct AssemblyProcessor {
    static const uint8_t Number = 0x21;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct AssemblyRef {
    static const uint8_t Number = 0x23;
    AssemblyRef(uint32_t name)
      : MajorVersion(0),
        MinorVersion(0),
        BuildNumber(0),
        RevisionNumber(0),
        Flags(0),
        PublicKeyOrToken(0),
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

    std::vector<uint8_t> serialize() const {
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

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct AssemblyRefProcessor {
    static const uint8_t Number = 0x24;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct ClassLayout {
    static const uint8_t Number = 0x0F;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct Constant {
    static const uint8_t Number = 0x0B;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct CustomAttribute {
    static const uint8_t Number = 0x0C;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct DeclSecurity {
    static const uint8_t Number = 0x0E;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct EventMap {
    static const uint8_t Number = 0x12;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct Event {
    static const uint8_t Number = 0x14;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct ExportedType {
    static const uint8_t Number = 0x27;

    std::vector<uint8_t> serialize() const {
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

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << Flags;
        r << Name;
        r << Signature;
        return r;
    }
};

struct FieldLayout {
    static const uint8_t Number = 0x10;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct FieldMarshal {
    static const uint8_t Number = 0x0D;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct FieldRVA {
    static const uint8_t Number = 0x1D;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct File {
    static const uint8_t Number = 0x26;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct GenericParam {
    static const uint8_t Number = 0x2A;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct GenericParamConstraint {
    static const uint8_t Number = 0x2C;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct ImplMap {
    static const uint8_t Number = 0x1C;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct InterfaceImpl {
    static const uint8_t Number = 0x09;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct ManifestResource {
    static const uint8_t Number = 0x28;

    std::vector<uint8_t> serialize() const {
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

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << Parent;
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

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << RVA;
        r << ImplFlags;
        r << MethodAttributes;
        r << Name;
        r << Signature;
        r << ParamList;
        return r;
    }
};

struct MethodImpl {
    static const uint8_t Number = 0x19;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct MethodSemantics {
    static const uint8_t Number = 0x18;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct MethodSpec {
    static const uint8_t Number = 0x2B;

    std::vector<uint8_t> serialize() const {
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

    std::vector<uint8_t> serialize() const {
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

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct NestedClass {
    static const uint8_t Number = 0x29;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct Param {
    static const uint8_t Number = 0x08;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct Property {
    static const uint8_t Number = 0x17;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct PropertyMap {
    static const uint8_t Number = 0x15;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        return r;
    }
};

struct StandAloneSig {
    static const uint8_t Number = 0x11;

    std::vector<uint8_t> serialize() const {
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
    uint16_t Extends;
    uint16_t FieldList;
    uint16_t MethodList;

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << Flags;
        r << TypeName;
        r << TypeNamespace;
        r << Extends;
        r << FieldList;
        r << MethodList;
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

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        r << ResolutionScope;
        r << TypeName;
        r << TypeNamespace;
        return r;
    }
};

struct TypeSpec {
    static const uint8_t Number = 0x1B;

    std::vector<uint8_t> serialize() const {
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
        Sorted(0)
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
        update_valid(valid, Module_Table);
        update_valid(valid, TypeRef_Table);
        update_valid(valid, TypeDef_Table);
        update_valid(valid, Field_Table);
        update_valid(valid, MethodDef_Table);
        update_valid(valid, Param_Table);
        update_valid(valid, InterfaceImpl_Table);
        update_valid(valid, MemberRef_Table);
        update_valid(valid, Constant_Table);
        update_valid(valid, CustomAttribute_Table);
        update_valid(valid, FieldMarshal_Table);
        update_valid(valid, DeclSecurity_Table);
        update_valid(valid, ClassLayout_Table);
        update_valid(valid, FieldLayout_Table);
        update_valid(valid, StandAloneSig_Table);
        update_valid(valid, EventMap_Table);
        update_valid(valid, Event_Table);
        update_valid(valid, PropertyMap_Table);
        update_valid(valid, Property_Table);
        update_valid(valid, MethodSemantics_Table);
        update_valid(valid, MethodImpl_Table);
        update_valid(valid, ModuleRef_Table);
        update_valid(valid, TypeSpec_Table);
        update_valid(valid, ImplMap_Table);
        update_valid(valid, FieldRVA_Table);
        update_valid(valid, Assembly_Table);
        update_valid(valid, AssemblyProcessor_Table);
        update_valid(valid, AssemblyOS_Table);
        update_valid(valid, AssemblyRef_Table);
        update_valid(valid, AssemblyRefProcessor_Table);
        update_valid(valid, AssemblyRefOS_Table);
        update_valid(valid, File_Table);
        update_valid(valid, ExportedType_Table);
        update_valid(valid, ManifestResource_Table);
        update_valid(valid, NestedClass_Table);
        update_valid(valid, GenericParam_Table);
        update_valid(valid, MethodSpec_Table);
        update_valid(valid, GenericParamConstraint_Table);
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
        for (auto x: Module_Table) { r << x.serialize(); }
        for (auto x: TypeRef_Table) { r << x.serialize(); }
        for (auto x: TypeDef_Table) { r << x.serialize(); }
        for (auto x: Field_Table) { r << x.serialize(); }
        for (auto x: MethodDef_Table) { r << x.serialize(); }
        for (auto x: Param_Table) { r << x.serialize(); }
        for (auto x: InterfaceImpl_Table) { r << x.serialize(); }
        for (auto x: MemberRef_Table) { r << x.serialize(); }
        for (auto x: Constant_Table) { r << x.serialize(); }
        for (auto x: CustomAttribute_Table) { r << x.serialize(); }
        for (auto x: FieldMarshal_Table) { r << x.serialize(); }
        for (auto x: DeclSecurity_Table) { r << x.serialize(); }
        for (auto x: ClassLayout_Table) { r << x.serialize(); }
        for (auto x: FieldLayout_Table) { r << x.serialize(); }
        for (auto x: StandAloneSig_Table) { r << x.serialize(); }
        for (auto x: EventMap_Table) { r << x.serialize(); }
        for (auto x: Event_Table) { r << x.serialize(); }
        for (auto x: PropertyMap_Table) { r << x.serialize(); }
        for (auto x: Property_Table) { r << x.serialize(); }
        for (auto x: MethodSemantics_Table) { r << x.serialize(); }
        for (auto x: MethodImpl_Table) { r << x.serialize(); }
        for (auto x: ModuleRef_Table) { r << x.serialize(); }
        for (auto x: TypeSpec_Table) { r << x.serialize(); }
        for (auto x: ImplMap_Table) { r << x.serialize(); }
        for (auto x: FieldRVA_Table) { r << x.serialize(); }
        for (auto x: Assembly_Table) { r << x.serialize(); }
        for (auto x: AssemblyProcessor_Table) { r << x.serialize(); }
        for (auto x: AssemblyOS_Table) { r << x.serialize(); }
        for (auto x: AssemblyRef_Table) { r << x.serialize(); }
        for (auto x: AssemblyRefProcessor_Table) { r << x.serialize(); }
        for (auto x: AssemblyRefOS_Table) { r << x.serialize(); }
        for (auto x: File_Table) { r << x.serialize(); }
        for (auto x: ExportedType_Table) { r << x.serialize(); }
        for (auto x: ManifestResource_Table) { r << x.serialize(); }
        for (auto x: NestedClass_Table) { r << x.serialize(); }
        for (auto x: GenericParam_Table) { r << x.serialize(); }
        for (auto x: MethodSpec_Table) { r << x.serialize(); }
        for (auto x: GenericParamConstraint_Table) { r << x.serialize(); };
        return r;
    }

    template<typename T> static void update_valid(uint64_t &valid, const std::vector<T> &table) {
        if (not table.empty()) {
            valid |= (1LL << T::Number);
        }
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
        uint32_t r = static_cast<uint32_t>(GuidData.size());
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

class MethodDefSig_WriteLine {
public:
    MethodDefSig_WriteLine() {}

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        // TODO: this is only appropriate for void WriteLine(string)
        r << static_cast<uint8_t>(0);
        r << compressUnsigned(1);
        r << ELEMENT_TYPE_VOID;
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
    ExecutableFile(const std::string &path, const std::string &name): path(path), name(name), bytecode() {}
    const std::string path;
    const std::string name;
    std::vector<uint8_t> bytecode;
    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> r;
        std::copy(MS_DOS_header, MS_DOS_header+sizeof(MS_DOS_header), std::back_inserter(r));
        PE_file_header pfh;
        r << pfh.serialize();

        PE_optional_header poh;
        poh.standard_fields.base_of_code = 0x2000;
        poh.NT_specific_fields.image_size = 0x8000; // TODO
        poh.NT_specific_fields.header_size = 0x200;

        std::vector<uint8_t> code_section = bytecode;
        Import_lookup_table ilt;
        ilt.HintNameTableRVA = static_cast<uint32_t>(poh.standard_fields.base_of_code + code_section.size());
        code_section << static_cast<uint16_t>(0);
        code_section << "_CorExeMain";

        Import_address_table iat;
        iat.ImportLookupTable = static_cast<uint32_t>(poh.standard_fields.base_of_code + code_section.size());
        code_section << ilt.serialize();
        code_section << Import_lookup_table().serialize();
        iat.Name = static_cast<uint32_t>(poh.standard_fields.base_of_code + code_section.size());
        code_section << "mscoree.dll";
        poh.data_directories.import_table_rva = static_cast<uint32_t>(poh.standard_fields.base_of_code + code_section.size());
        code_section << iat.serialize();
        code_section << Import_address_table().serialize();
        poh.data_directories.import_table_size = static_cast<uint32_t>(poh.standard_fields.base_of_code + code_section.size() - poh.data_directories.import_table_rva);

        Metadata md;
        Module mod;
        mod.Name = md.String("hello");
        mod.Mvid = md.Guid("1234567890123456");
        md.Tables.Module_Table.push_back(mod);
        TypeDef td;
        td.Flags = TypeAttributes_Visibility_Public;
        td.TypeName = md.String("<Module>");
        //td.Extends = 
        td.FieldList = static_cast<uint16_t>(1 + md.Tables.Field_Table.size());
        td.MethodList = static_cast<uint16_t>(1 + md.Tables.MethodDef_Table.size());
        md.Tables.TypeDef_Table.push_back(td);
        MethodDef main;
        main.RVA = static_cast<uint32_t>(poh.standard_fields.base_of_code + code_section.size());
        MethodHeader mh;
        mh.MaxStack = 8;
        mh.CodeSize = 11;
        code_section << mh.serialize();
        code_section << op_ldstr << md.Userstring("hello world");
        code_section << op_call << md.MemberRefToken(MemberRef(
            MemberRefParent_TypeRef(md.TypeRefIndex(TypeRef(
                ResolutionScope_AssemblyRef(md.AssemblyRefIndex(AssemblyRef(md.String("mscorlib")))),
                md.String("System.Console"),
                0))),
            md.String("WriteLine"),
            md.Blob(MethodDefSig_WriteLine().serialize())
        ));
        code_section << op_ret;
        main.MethodAttributes = MethodAttributes_MemberAccess_Public | MethodAttributes_Static;
        main.Name = md.String("Main");
        main.Signature = md.Blob(MethodDefSig().serialize());
        main.ParamList = static_cast<uint16_t>(1 + md.Tables.Param_Table.size());
        md.Tables.MethodDef_Table.push_back(main);
        CLI_header ch;
        ch.cb = 72;
        while (code_section.size() % 4 != 0) {
            code_section << static_cast<uint8_t>(0);
        }
        ch.MetaDataRVA = static_cast<uint32_t>(poh.standard_fields.base_of_code + code_section.size());
        md.calculate_offsets();
        code_section << md.serialize();
        ch.MetaDataSize = static_cast<uint32_t>(poh.standard_fields.base_of_code + code_section.size() - ch.MetaDataRVA);
        poh.data_directories.cli_header_rva = static_cast<uint32_t>(poh.standard_fields.base_of_code + code_section.size());
        code_section << ch.serialize();
        poh.data_directories.cli_header_size = static_cast<uint32_t>(poh.standard_fields.base_of_code + code_section.size() - poh.data_directories.cli_header_rva);
        code_section << std::vector<uint8_t>(64); // Add padding that mono seems to expect.

        poh.standard_fields.entry_point_rva = static_cast<uint32_t>(poh.standard_fields.base_of_code + code_section.size());
        code_section << static_cast<uint8_t>(0xff);
        code_section << static_cast<uint8_t>(0x25);
        code_section << static_cast<uint32_t>(0x402006);
        poh.standard_fields.code_size = static_cast<uint32_t>(code_section.size());

        r << poh.serialize();
        Section_header sh;
        sh.Name = ".text";
        sh.VirtualSize = static_cast<uint32_t>(code_section.size());
        sh.VirtualAddress = poh.standard_fields.base_of_code;
        sh.SizeOfRawData = static_cast<uint32_t>(code_section.size());
        sh.PointerToRawData = 0x200;
        sh.Characteristics = IMG_SCN_CNT_CODE | IMG_SCN_MEM_EXECUTE | IMG_SCN_MEM_READ;
        r << sh.serialize();
        while (r.size() < 0x200) {
            r << static_cast<uint8_t>(0);
        }
        r << code_section;
        return r;
    }
private:
    ExecutableFile(const ExecutableFile &);
    ExecutableFile &operator=(const ExecutableFile &);
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
    Context(ClassContext &cc): cc(cc), label_exit(), loop_labels() {}
    ClassContext &cc;
    //ExecutableFile &ef;
    //Code_attribute &ca;
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
private:
    Context(const Context &);
    Context &operator=(const Context &);
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
    virtual ~Type() {}
    const std::string classname;
    const std::string jtype;
    virtual void generate_class(Context &) const {}
    virtual void generate_default(Context &context) const = 0;
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const { internal_error("Type::generate_call"); }
private:
    Type(const Type &);
    Type &operator=(const Type &);
};

Type *transform(const ast::Type *t);

class Variable {
public:
    Variable(const ast::Variable *v): type(transform(v->type)) {
        g_variable_cache[v] = this;
    }
    virtual ~Variable() {}
    const Type *type;
    virtual void generate_decl(ClassContext &context, bool exported) const = 0;
    virtual void generate_load(Context &context) const = 0;
    virtual void generate_store(Context &context) const = 0;
    virtual void generate_call(Context &context, const std::vector<const Expression *> &args) const = 0;
private:
    Variable(const Variable &);
    Variable &operator=(const Variable &);
};

Variable *transform(const ast::Variable *v);

class Expression {
public:
    Expression(const ast::Expression *node): type(transform(node->type)) {
        g_expression_cache[node] = this;
    }
    virtual ~Expression() {}
    const Type *type;
    virtual void generate(Context &context) const = 0;
    virtual void generate_call(Context &context, const std::vector<const Expression *> &) const = 0;
    virtual void generate_store(Context &context) const = 0;
private:
    Expression(const Expression &);
    Expression &operator=(const Expression &);
};

Expression *transform(const ast::Expression *e);

class Statement {
public:
    Statement(const ast::Statement *s) {
        g_statement_cache[s] = this;
    }
    virtual ~Statement() {}
    virtual void generate(Context &context) const = 0;
};

Statement *transform(const ast::Statement *s);

class TypeNothing: public Type {
public:
    TypeNothing(const ast::TypeNothing *tn): Type(tn, "V", "V"), tn(tn) {}
    const ast::TypeNothing *tn;
    virtual void generate_default(Context &) const override { internal_error("TypeNothing"); }
private:
    TypeNothing(const TypeNothing &);
    TypeNothing &operator=(const TypeNothing &);
};

class TypeDummy: public Type {
public:
    TypeDummy(const ast::TypeDummy *td): Type(td, ""), td(td) {}
    const ast::TypeDummy *td;
    virtual void generate_default(Context &) const override { internal_error("TypeDummy"); }
private:
    TypeDummy(const TypeDummy &);
    TypeDummy &operator=(const TypeDummy &);
};

class TypeBoolean: public Type {
public:
    TypeBoolean(const ast::TypeBoolean *tb): Type(tb, "java/lang/Boolean"), tb(tb) {}
    const ast::TypeBoolean *tb;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeBoolean");
    }
private:
    TypeBoolean(const TypeBoolean &);
    TypeBoolean &operator=(const TypeBoolean &);
};

class TypeNumber: public Type {
public:
    TypeNumber(const ast::TypeNumber *tn): Type(tn, "neon/type/Number"), tn(tn) {}
    const ast::TypeNumber *tn;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeNumber");
    }
private:
    TypeNumber(const TypeNumber &);
    TypeNumber &operator=(const TypeNumber &);
};

class TypeString: public Type {
public:
    TypeString(const ast::TypeString *ts): Type(ts, "java/lang/String"), ts(ts) {}
    const ast::TypeString *ts;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeString");
    }
private:
    TypeString(const TypeString &);
    TypeString &operator=(const TypeString &);
};

class TypeBytes: public Type {
public:
    TypeBytes(const ast::TypeBytes *tb): Type(tb, "[B", "[B"), tb(tb) {}
    const ast::TypeBytes *tb;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeBytes");
    }
private:
    TypeBytes(const TypeBytes &);
    TypeBytes &operator=(const TypeBytes &);
};

class TypeFunction: public Type {
public:
    TypeFunction(const ast::TypeFunction *tf): Type(tf, ""), tf(tf), returntype(transform(tf->returntype)), paramtypes() {
        for (auto p: tf->params) {
            paramtypes.push_back(std::make_pair(p->mode, transform(p->type)));
        }
    }
    const ast::TypeFunction *tf;
    const Type *returntype;
    std::vector<std::pair<ast::ParameterType::Mode, const Type *>> paramtypes;
    virtual void generate_default(Context &) const override { internal_error("TypeFunction"); }
private:
    TypeFunction(const TypeFunction &);
    TypeFunction &operator=(const TypeFunction &);
};

class TypeArray: public Type {
public:
    TypeArray(const ast::TypeArray *ta): Type(ta, "neon/type/Array"), ta(ta), elementtype(transform(ta->elementtype)) {}
    const ast::TypeArray *ta;
    const Type *elementtype;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeAray");
    }
private:
    TypeArray(const TypeArray &);
    TypeArray &operator=(const TypeArray &);
};

class TypeDictionary: public Type {
public:
    TypeDictionary(const ast::TypeDictionary *td): Type(td, "java/util/HashMap"), td(td), elementtype(transform(td->elementtype)) {}
    const ast::TypeDictionary *td;
    const Type *elementtype;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeDictionary");
    }
private:
    TypeDictionary(const TypeDictionary &);
    TypeDictionary &operator=(const TypeDictionary &);
};

class TypeRecord: public Type {
public:
    TypeRecord(const ast::TypeRecord *tr): Type(tr, tr->module + "$" + tr->name), tr(tr), field_types() {
        for (auto f: tr->fields) {
            field_types.push_back(transform(f.type));
        }
    }
    const ast::TypeRecord *tr;
    std::vector<const Type *> field_types;

    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeRecord");
    }
private:
    TypeRecord(const TypeRecord &);
    TypeRecord &operator=(const TypeRecord &);
};

class TypePointer: public Type {
public:
    TypePointer(const ast::TypePointer *tp): Type(tp, tp->reftype != nullptr ? transform(tp->reftype)->classname : "java/lang/Object"), tp(tp) {}
    const ast::TypePointer *tp;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypePointer");
    }
private:
    TypePointer(const TypePointer &);
    TypePointer &operator=(const TypePointer &);
};

class TypeFunctionPointer: public Type {
public:
    TypeFunctionPointer(const ast::TypeFunctionPointer *fp): Type(fp, "java/lang/reflect/Method"), fp(fp), functype(dynamic_cast<const TypeFunction *>(transform(fp->functype))) {}
    const ast::TypeFunctionPointer *fp;
    const TypeFunction *functype;
    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeFunctionPointer");
    }
    virtual void generate_call(Context &context, const std::vector<const Expression *> &) const override {
        context.unimplemented("TypeFunctionPointer");
    }
private:
    TypeFunctionPointer(const TypeFunctionPointer &);
    TypeFunctionPointer &operator=(const TypeFunctionPointer &);
};

class TypeEnum: public Type {
public:
    TypeEnum(const ast::TypeEnum *te): Type(te, te->module + "$" + te->name), te(te) {}
    const ast::TypeEnum *te;

    virtual void generate_default(Context &context) const override {
        context.unimplemented("TypeEnum");
    }
private:
    TypeEnum(const TypeEnum &);
    TypeEnum &operator=(const TypeEnum &);
};

class PredefinedVariable: public Variable {
public:
    PredefinedVariable(const ast::PredefinedVariable *pv): Variable(pv), pv(pv) {}
    const ast::PredefinedVariable *pv;

    virtual void generate_decl(ClassContext &, bool) const override {}
    virtual void generate_load(Context &context) const override {
        context.unimplemented("PredefinedVariable");
    }
    virtual void generate_store(Context &) const override { internal_error("PredefinedVariable"); }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("PredefinedVariable"); }
private:
    PredefinedVariable(const PredefinedVariable &);
    PredefinedVariable &operator=(const PredefinedVariable &);
};

class ModuleVariable: public Variable {
public:
    ModuleVariable(const ast::ModuleVariable *mv): Variable(mv), mv(mv) {}
    const ast::ModuleVariable *mv;

    virtual void generate_decl(ClassContext &, bool) const override {}
    virtual void generate_load(Context &context) const override {
        context.unimplemented("ModuleVariable");
    }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("ModuleVariable");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ModuleVariable"); }
private:
    ModuleVariable(const ModuleVariable &);
    ModuleVariable &operator=(const ModuleVariable &);
};

class GlobalVariable: public Variable {
public:
    GlobalVariable(const ast::GlobalVariable *gv): Variable(gv), gv(gv), name() {}
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
private:
    GlobalVariable(const GlobalVariable &);
    GlobalVariable &operator=(const GlobalVariable &);
};

class LocalVariable: public Variable {
public:
    LocalVariable(const ast::LocalVariable *lv): Variable(lv), lv(lv), index(-1) {}
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
private:
    LocalVariable(const LocalVariable &);
    LocalVariable &operator=(const LocalVariable &);
};

class FunctionParameter: public Variable {
public:
    FunctionParameter(const ast::FunctionParameter *fp, int index): Variable(fp), fp(fp), index(index) {}
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
private:
    FunctionParameter(const FunctionParameter &);
    FunctionParameter &operator=(const FunctionParameter &);
};

class ConstantBooleanExpression: public Expression {
public:
    ConstantBooleanExpression(const ast::ConstantBooleanExpression *cbe): Expression(cbe), cbe(cbe) {}
    const ast::ConstantBooleanExpression *cbe;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConstantBooleanExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantBooleanExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantBooleanExpression"); }
private:
    ConstantBooleanExpression(const ConstantBooleanExpression &);
    ConstantBooleanExpression &operator=(const ConstantBooleanExpression &);
};

class ConstantNumberExpression: public Expression {
public:
    ConstantNumberExpression(const ast::ConstantNumberExpression *cne): Expression(cne), cne(cne) {}
    const ast::ConstantNumberExpression *cne;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConstantNumberExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantNumberExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantNumberExpression"); }
private:
    ConstantNumberExpression(const ConstantNumberExpression &);
    ConstantNumberExpression &operator=(const ConstantNumberExpression &);
};

class ConstantStringExpression: public Expression {
public:
    ConstantStringExpression(const ast::ConstantStringExpression *cse): Expression(cse), cse(cse) {}
    const ast::ConstantStringExpression *cse;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConstantStringExpression");
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantStringExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantStringExpression"); }
private:
    ConstantStringExpression(const ConstantStringExpression &);
    ConstantStringExpression &operator=(const ConstantStringExpression &);
};

class ConstantBytesExpression: public Expression {
public:
    ConstantBytesExpression(const ast::ConstantBytesExpression *cbe): Expression(cbe), cbe(cbe) {}
    const ast::ConstantBytesExpression *cbe;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConstantBytesExpression");
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantBytesExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantBytesExpression"); }
private:
    ConstantBytesExpression(const ConstantBytesExpression &);
    ConstantBytesExpression &operator=(const ConstantBytesExpression &);
};

class ConstantEnumExpression: public Expression {
public:
    ConstantEnumExpression(const ast::ConstantEnumExpression *cee): Expression(cee), cee(cee), type(dynamic_cast<const TypeEnum *>(transform(cee->type))) {}
    const ast::ConstantEnumExpression *cee;
    const TypeEnum *type;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConstantEnumExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantEnumExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantEnumExpression"); }
private:
    ConstantEnumExpression(const ConstantEnumExpression &);
    ConstantEnumExpression &operator=(const ConstantEnumExpression &);
};

class ConstantNilExpression: public Expression {
public:
    ConstantNilExpression(const ast::ConstantNilExpression *cne): Expression(cne), cne(cne) {}
    const ast::ConstantNilExpression *cne;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConstantNilExpression");
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantNilExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantNilExpression"); }
private:
    ConstantNilExpression(const ConstantNilExpression &);
    ConstantNilExpression &operator=(const ConstantNilExpression &);
};

class ConstantNowhereExpression: public Expression {
public:
    ConstantNowhereExpression(const ast::ConstantNowhereExpression *cne): Expression(cne), cne(cne) {}
    const ast::ConstantNowhereExpression *cne;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConstantNowhereExpression");
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantNowhereExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantNowhereExpression"); }
private:
    ConstantNowhereExpression(const ConstantNowhereExpression &);
    ConstantNowhereExpression &operator=(const ConstantNowhereExpression &);
};

class ArrayLiteralExpression: public Expression {
public:
    ArrayLiteralExpression(const ast::ArrayLiteralExpression *ale): Expression(ale), ale(ale), elements() {
        for (auto e: ale->elements) {
            elements.push_back(transform(e));
        }
    }
    const ast::ArrayLiteralExpression *ale;
    std::vector<const Expression *> elements;

    virtual void generate(Context &context) const override {
        context.unimplemented("ArrayLiteralExpression");
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ArrayLiteralExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ArrayLiteralExpression"); }
private:
    ArrayLiteralExpression(const ArrayLiteralExpression &);
    ArrayLiteralExpression &operator=(const ArrayLiteralExpression &);
};

class DictionaryLiteralExpression: public Expression {
public:
    DictionaryLiteralExpression(const ast::DictionaryLiteralExpression *dle): Expression(dle), dle(dle), dict() {
        for (auto d: dle->dict) {
            dict[d.first] = transform(d.second);
        }
    }
    const ast::DictionaryLiteralExpression *dle;
    std::map<std::string, const Expression *> dict;

    virtual void generate(Context &context) const override {
        context.unimplemented("DictionaryLiteralExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DictionaryLiteralExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DictionaryLiteralExpression"); }
private:
    DictionaryLiteralExpression(const DictionaryLiteralExpression &);
    DictionaryLiteralExpression &operator=(const DictionaryLiteralExpression &);
};

class RecordLiteralExpression: public Expression {
public:
    RecordLiteralExpression(const ast::RecordLiteralExpression *rle): Expression(rle), rle(rle), type(dynamic_cast<TypeRecord *>(transform(rle->type))), values() {
        for (auto v: rle->values) {
            values.push_back(transform(v));
        }
    }
    const ast::RecordLiteralExpression *rle;
    const TypeRecord *type;
    std::vector<const Expression *> values;

    virtual void generate(Context &context) const override {
        context.unimplemented("RecordLiteralExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("RecordLiteralExpression"); }
    virtual void generate_store(Context &) const override { internal_error("RecordLiteralExpression"); }
private:
    RecordLiteralExpression(const RecordLiteralExpression &);
    RecordLiteralExpression &operator=(const RecordLiteralExpression &);
};

class NewClassExpression: public Expression {
public:
    NewClassExpression(const ast::NewClassExpression *nce): Expression(nce), nce(nce), value(transform(nce->value)), type(dynamic_cast<const TypeRecord *>(transform(dynamic_cast<const ast::TypeValidPointer *>(nce->type)->reftype))) {}
    const ast::NewClassExpression *nce;
    const Expression *value;
    const TypeRecord *type;

    virtual void generate(Context &context) const override {
        context.unimplemented("NewClassExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("NewClassExpression"); }
    virtual void generate_store(Context &) const override { internal_error("NewClassExpression"); }
private:
    NewClassExpression(const NewClassExpression &);
    NewClassExpression &operator=(const NewClassExpression &);
};

class UnaryMinusExpression: public Expression {
public:
    UnaryMinusExpression(const ast::UnaryMinusExpression *ume): Expression(ume), ume(ume), value(transform(ume->value)) {}
    const ast::UnaryMinusExpression *ume;
    const Expression *value;

    virtual void generate(Context &context) const override {
        context.unimplemented("UnaryMinusExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("UnaryMinusExpression"); }
    virtual void generate_store(Context &) const override { internal_error("UnaryMinusExpression"); }
private:
    UnaryMinusExpression(const UnaryMinusExpression &);
    UnaryMinusExpression &operator=(const UnaryMinusExpression &);
};

class LogicalNotExpression: public Expression {
public:
    LogicalNotExpression(const ast::LogicalNotExpression *lne): Expression(lne), lne(lne), value(transform(lne->value)) {}
    const ast::LogicalNotExpression *lne;
    const Expression *value;

    virtual void generate(Context &context) const override {
        context.unimplemented("LogicalNotExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("LogicalNotExpression"); }
    virtual void generate_store(Context &) const override { internal_error("LogicalNotExpression"); }
private:
    LogicalNotExpression(const LogicalNotExpression &);
    LogicalNotExpression &operator=(const LogicalNotExpression &);
};

class ConditionalExpression: public Expression {
public:
    ConditionalExpression(const ast::ConditionalExpression *ce): Expression(ce), ce(ce), condition(transform(ce->condition)), left(transform(ce->left)), right(transform(ce->right)) {}
    const ast::ConditionalExpression *ce;
    const Expression *condition;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConditionalExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConditionalExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConditionalExpression"); }
private:
    ConditionalExpression(const ConditionalExpression &);
    ConditionalExpression &operator=(const ConditionalExpression &);
};

class TryExpressionTrap {
public:
    TryExpressionTrap(const ast::TryTrap *tt): tt(tt), name(transform(tt->name)), handler(), gives(transform(dynamic_cast<const ast::Expression *>(tt->handler))) {
        const ast::ExceptionHandlerStatement *h = dynamic_cast<const ast::ExceptionHandlerStatement *>(tt->handler);
        if (h != nullptr) {
            for (auto s: h->statements) {
                handler.push_back(transform(s));
            }
        }
    }
    const ast::TryTrap *tt;
    const Variable *name;
    std::vector<const Statement *> handler;
    const Expression *gives;
private:
    TryExpressionTrap(const TryExpressionTrap &);
    TryExpressionTrap &operator=(const TryExpressionTrap &);
};

class TryExpression: public Expression {
public:
    TryExpression(const ast::TryExpression *te): Expression(te), te(te), expr(transform(te->expr)), catches() {
        for (auto &t: te->catches) {
            catches.push_back(new TryExpressionTrap(&t));
        }
    }
    const ast::TryExpression *te;
    const Expression *expr;
    std::vector<const TryExpressionTrap *> catches;

    virtual void generate(Context &context) const override {
        context.unimplemented("TryExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("TryExpression"); }
    virtual void generate_store(Context &) const override { internal_error("TryExpression"); }
private:
    TryExpression(const TryExpression &);
    TryExpression &operator=(const TryExpression &);
};

class DisjunctionExpression: public Expression {
public:
    DisjunctionExpression(const ast::DisjunctionExpression *de): Expression(de), de(de), left(transform(de->left)), right(transform(de->right)) {}
    const ast::DisjunctionExpression *de;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("DisjunctionExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DisjunctionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DisjunctionExpression"); }
private:
    DisjunctionExpression(const DisjunctionExpression &);
    DisjunctionExpression &operator=(const DisjunctionExpression &);
};

class ConjunctionExpression: public Expression {
public:
    ConjunctionExpression(const ast::ConjunctionExpression *ce): Expression(ce), ce(ce), left(transform(ce->left)), right(transform(ce->right)) {}
    const ast::ConjunctionExpression *ce;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("ConjunctionExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConjunctionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConjunctionExpression"); }
private:
    ConjunctionExpression(const ConjunctionExpression &);
    ConjunctionExpression &operator=(const ConjunctionExpression &);
};

class ArrayInExpression: public Expression {
public:
    ArrayInExpression(const ast::ArrayInExpression *aie): Expression(aie), aie(aie), left(transform(aie->left)), right(transform(aie->right)) {}
    const ast::ArrayInExpression *aie;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("ArrayInExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ArrayInExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ArrayInExpression"); }
private:
    ArrayInExpression(const ArrayInExpression &);
    ArrayInExpression &operator=(const ArrayInExpression &);
};

class DictionaryInExpression: public Expression {
public:
    DictionaryInExpression(const ast::DictionaryInExpression *die): Expression(die), die(die), left(transform(die->left)), right(transform(die->right)) {}
    const ast::DictionaryInExpression *die;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("DictionaryInExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DictionaryInExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DictionaryInExpression"); }
private:
    DictionaryInExpression(const DictionaryInExpression &);
    DictionaryInExpression &operator=(const DictionaryInExpression &);
};

class ComparisonExpression: public Expression {
public:
    ComparisonExpression(const ast::ComparisonExpression *ce): Expression(ce), ce(ce), left(transform(ce->left)), right(transform(ce->right)) {}
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
private:
    ComparisonExpression(const ComparisonExpression &);
    ComparisonExpression &operator=(const ComparisonExpression &);
};

class ChainedComparisonExpression: public Expression {
public:
    ChainedComparisonExpression(const ast::ChainedComparisonExpression *cce): Expression(cce), cce(cce), comps() {
        for (auto c: cce->comps) {
            const ComparisonExpression *ce = dynamic_cast<const ComparisonExpression *>(transform(c));
            if (ce == nullptr) {
                internal_error("ChainedComparisonExpression");
            }
            comps.push_back(ce);
        }
    }
    const ast::ChainedComparisonExpression *cce;
    std::vector<const ComparisonExpression *> comps;

    virtual void generate(Context &context) const override {
        context.unimplemented("ChainedComparisonExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ChainedComparisonExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ChainedComparisonExpression"); }
private:
    ChainedComparisonExpression(const ChainedComparisonExpression &);
    ChainedComparisonExpression &operator=(const ChainedComparisonExpression &);
};

class BooleanComparisonExpression: public ComparisonExpression {
public:
    BooleanComparisonExpression(const ast::BooleanComparisonExpression *bce): ComparisonExpression(bce), bce(bce) {}
    const ast::BooleanComparisonExpression *bce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("BooleanComparisonExpression");
    }
private:
    BooleanComparisonExpression(const BooleanComparisonExpression &);
    BooleanComparisonExpression &operator=(const BooleanComparisonExpression &);
};

class NumericComparisonExpression: public ComparisonExpression {
public:
    NumericComparisonExpression(const ast::NumericComparisonExpression *nce): ComparisonExpression(nce), nce(nce) {}
    const ast::NumericComparisonExpression *nce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("NumericComparisonExpression");
    }
private:
    NumericComparisonExpression(const NumericComparisonExpression &);
    NumericComparisonExpression &operator=(const NumericComparisonExpression &);
};

class EnumComparisonExpression: public ComparisonExpression {
public:
    EnumComparisonExpression(const ast::EnumComparisonExpression *ece): ComparisonExpression(ece), ece(ece) {}
    const ast::EnumComparisonExpression *ece;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("EnumComparisonExpression");
    }
private:
    EnumComparisonExpression(const EnumComparisonExpression &);
    EnumComparisonExpression &operator=(const EnumComparisonExpression &);
};

class StringComparisonExpression: public ComparisonExpression {
public:
    StringComparisonExpression(const ast::StringComparisonExpression *sce): ComparisonExpression(sce), sce(sce) {}
    const ast::StringComparisonExpression *sce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("StringComparisonExpression");
    }
private:
    StringComparisonExpression(const StringComparisonExpression &);
    StringComparisonExpression &operator=(const StringComparisonExpression &);
};

class BytesComparisonExpression: public ComparisonExpression {
public:
    BytesComparisonExpression(const ast::BytesComparisonExpression *bce): ComparisonExpression(bce), bce(bce) {}
    const ast::BytesComparisonExpression *bce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("BytesComparisonExpression");
    }
private:
    BytesComparisonExpression(const BytesComparisonExpression &);
    BytesComparisonExpression &operator=(const BytesComparisonExpression &);
};

class ArrayComparisonExpression: public ComparisonExpression {
public:
    ArrayComparisonExpression(const ast::ArrayComparisonExpression *ace): ComparisonExpression(ace), ace(ace) {}
    const ast::ArrayComparisonExpression *ace;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("ArrayComparisonExpression");
    }
private:
    ArrayComparisonExpression(const ArrayComparisonExpression &);
    ArrayComparisonExpression &operator=(const ArrayComparisonExpression &);
};

class DictionaryComparisonExpression: public ComparisonExpression {
public:
    DictionaryComparisonExpression(const ast::DictionaryComparisonExpression *dce): ComparisonExpression(dce), dce(dce) {}
    const ast::DictionaryComparisonExpression *dce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("DictionaryComparisonExpression");
    }
private:
    DictionaryComparisonExpression(const DictionaryComparisonExpression &);
    DictionaryComparisonExpression &operator=(const DictionaryComparisonExpression &);
};

class RecordComparisonExpression: public ComparisonExpression {
public:
    RecordComparisonExpression(const ast::RecordComparisonExpression *rce): ComparisonExpression(rce), rce(rce) {}
    const ast::RecordComparisonExpression *rce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("RecordComparisonExpression");
    }
private:
    RecordComparisonExpression(const RecordComparisonExpression &);
    RecordComparisonExpression &operator=(const RecordComparisonExpression &);
};

class PointerComparisonExpression: public ComparisonExpression {
public:
    PointerComparisonExpression(const ast::PointerComparisonExpression *pce): ComparisonExpression(pce), pce(pce) {}
    const ast::PointerComparisonExpression *pce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("PointerComparisonExpression");
    }
private:
    PointerComparisonExpression(const PointerComparisonExpression &);
    PointerComparisonExpression &operator=(const PointerComparisonExpression &);
};

class ValidPointerExpression: public PointerComparisonExpression {
public:
    ValidPointerExpression(const ast::ValidPointerExpression *vpe): PointerComparisonExpression(vpe), vpe(vpe), var(transform(vpe->var)) {}
    const ast::ValidPointerExpression *vpe;
    const Variable *var;

    virtual void generate(Context &context) const override {
        context.unimplemented("ValidPointerExpression");
    }
private:
    ValidPointerExpression(const ValidPointerExpression &);
    ValidPointerExpression &operator=(const ValidPointerExpression &);
};

class FunctionPointerComparisonExpression: public ComparisonExpression {
public:
    FunctionPointerComparisonExpression(const ast::FunctionPointerComparisonExpression *fpce): ComparisonExpression(fpce), fpce(fpce) {}
    const ast::FunctionPointerComparisonExpression *fpce;

    virtual void generate_comparison(Context &context) const override {
        context.unimplemented("FunctionPointerComparisonExpression");
    }
private:
    FunctionPointerComparisonExpression(const FunctionPointerComparisonExpression &);
    FunctionPointerComparisonExpression &operator=(const FunctionPointerComparisonExpression &);
};

class AdditionExpression: public Expression {
public:
    AdditionExpression(const ast::AdditionExpression *ae): Expression(ae), ae(ae), left(transform(ae->left)), right(transform(ae->right)) {}
    const ast::AdditionExpression *ae;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("AdditionExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("AdditionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("AdditionExpression"); }
private:
    AdditionExpression(const AdditionExpression &);
    AdditionExpression &operator=(const AdditionExpression &);
};

class SubtractionExpression: public Expression {
public:
    SubtractionExpression(const ast::SubtractionExpression *se): Expression(se), se(se), left(transform(se->left)), right(transform(se->right)) {}
    const ast::SubtractionExpression *se;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("SubtractionExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("SubtractionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("SubtractionExpression"); }
private:
    SubtractionExpression(const SubtractionExpression &);
    SubtractionExpression &operator=(const SubtractionExpression &);
};

class MultiplicationExpression: public Expression {
public:
    MultiplicationExpression(const ast::MultiplicationExpression *me): Expression(me), me(me), left(transform(me->left)), right(transform(me->right)) {}
    const ast::MultiplicationExpression *me;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("MultiplicationExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("MultiplicationExpression"); }
    virtual void generate_store(Context &) const override { internal_error("MultiplicationExpression"); }
private:
    MultiplicationExpression(const MultiplicationExpression &);
    MultiplicationExpression &operator=(const MultiplicationExpression &);
};

class DivisionExpression: public Expression {
public:
    DivisionExpression(const ast::DivisionExpression *de): Expression(de), de(de), left(transform(de->left)), right(transform(de->right)) {}
    const ast::DivisionExpression *de;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("DivisionExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DivisionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DivisionExpression"); }
private:
    DivisionExpression(const DivisionExpression &);
    DivisionExpression &operator=(const DivisionExpression &);
};

class ModuloExpression: public Expression {
public:
    ModuloExpression(const ast::ModuloExpression *me): Expression(me), me(me), left(transform(me->left)), right(transform(me->right)) {}
    const ast::ModuloExpression *me;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("ModuloExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ModuloExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ModuloExpression"); }
private:
    ModuloExpression(const ModuloExpression &);
    ModuloExpression &operator=(const ModuloExpression &);
};

class ExponentiationExpression: public Expression {
public:
    ExponentiationExpression(const ast::ExponentiationExpression *ee): Expression(ee), ee(ee), left(transform(ee->left)), right(transform(ee->right)) {}
    const ast::ExponentiationExpression *ee;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.unimplemented("ExponentiationExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ExponentiationExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ExponentiationExpression"); }
private:
    ExponentiationExpression(const ExponentiationExpression &);
    ExponentiationExpression &operator=(const ExponentiationExpression &);
};

class DummyExpression: public Expression {
public:
    DummyExpression(const ast::DummyExpression *de): Expression(de), de(de) {}
    const ast::DummyExpression *de;

    virtual void generate(Context &) const override { internal_error("DummyExpression"); }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DummyExpression"); }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("DummyExpression");
    }
private:
    DummyExpression(const DummyExpression &);
    DummyExpression &operator=(const DummyExpression &);
};

class ArrayReferenceIndexExpression: public Expression {
public:
    ArrayReferenceIndexExpression(const ast::ArrayReferenceIndexExpression *arie): Expression(arie), arie(arie), array(transform(arie->array)), index(transform(arie->index)) {}
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
private:
    ArrayReferenceIndexExpression(const ArrayReferenceIndexExpression &);
    ArrayReferenceIndexExpression &operator=(const ArrayReferenceIndexExpression &);
};

class ArrayValueIndexExpression: public Expression {
public:
    ArrayValueIndexExpression(const ast::ArrayValueIndexExpression *avie): Expression(avie), avie(avie), array(transform(avie->array)), index(transform(avie->index)) {}
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
private:
    ArrayValueIndexExpression(const ArrayValueIndexExpression &);
    ArrayValueIndexExpression &operator=(const ArrayValueIndexExpression &);
};

class DictionaryReferenceIndexExpression: public Expression {
public:
    DictionaryReferenceIndexExpression(const ast::DictionaryReferenceIndexExpression *drie): Expression(drie), drie(drie), dictionary(transform(drie->dictionary)), index(transform(drie->index)) {}
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
private:
    DictionaryReferenceIndexExpression(const DictionaryReferenceIndexExpression &);
    DictionaryReferenceIndexExpression &operator=(const DictionaryReferenceIndexExpression &);
};

class DictionaryValueIndexExpression: public Expression {
public:
    DictionaryValueIndexExpression(const ast::DictionaryValueIndexExpression *dvie): Expression(dvie), dvie(dvie), dictionary(transform(dvie->dictionary)), index(transform(dvie->index)) {}
    const ast::DictionaryValueIndexExpression *dvie;
    const Expression *dictionary;
    const Expression *index;

    virtual void generate(Context &context) const override {
        context.unimplemented("DictionaryValueIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DictionaryValueIndexExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DictionaryValueIndexExpression"); }
private:
    DictionaryValueIndexExpression(const DictionaryValueIndexExpression &);
    DictionaryValueIndexExpression &operator=(const DictionaryValueIndexExpression &);
};

class StringReferenceIndexExpression: public Expression {
public:
    StringReferenceIndexExpression(const ast::StringReferenceIndexExpression *srie): Expression(srie), srie(srie), ref(transform(srie->ref)), first(transform(srie->first)), last(transform(srie->last)) {}
    const ast::StringReferenceIndexExpression *srie;
    const Expression *ref;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        context.unimplemented("StringReferenceIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("StringReferenceIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("StringReferenceIndexExpression");
    }
private:
    StringReferenceIndexExpression(const StringReferenceIndexExpression &);
    StringReferenceIndexExpression &operator=(const StringReferenceIndexExpression &);
};

class StringValueIndexExpression: public Expression {
public:
    StringValueIndexExpression(const ast::StringValueIndexExpression *svie): Expression(svie), svie(svie), str(transform(svie->str)), first(transform(svie->first)), last(transform(svie->last)) {}
    const ast::StringValueIndexExpression *svie;
    const Expression *str;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        context.unimplemented("StringValueIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("StringValueIndexExpression"); }
    virtual void generate_store(Context&) const override { internal_error("StringValueIndexExpression"); }
private:
    StringValueIndexExpression(const StringValueIndexExpression &);
    StringValueIndexExpression &operator=(const StringValueIndexExpression &);
};

class BytesReferenceIndexExpression: public Expression {
public:
    BytesReferenceIndexExpression(const ast::BytesReferenceIndexExpression *brie): Expression(brie), brie(brie), ref(transform(brie->ref)), first(transform(brie->first)), last(transform(brie->last)) {}
    const ast::BytesReferenceIndexExpression *brie;
    const Expression *ref;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        context.unimplemented("BytesReferenceIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("BytesReferenceIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        context.unimplemented("BytesReferenceIndexExpression");
    }
private:
    BytesReferenceIndexExpression(const BytesReferenceIndexExpression &);
    BytesReferenceIndexExpression &operator=(const BytesReferenceIndexExpression &);
};

class BytesValueIndexExpression: public Expression {
public:
    BytesValueIndexExpression(const ast::BytesValueIndexExpression *bvie): Expression(bvie), bvie(bvie), data(transform(bvie->str)), first(transform(bvie->first)), last(transform(bvie->last)) {}
    const ast::BytesValueIndexExpression *bvie;
    const Expression *data;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        context.unimplemented("BytesValueIndexExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("BytesValueIndexExpression"); }
    virtual void generate_store(Context &) const override { internal_error("BytesValueIndexExpression"); }
private:
    BytesValueIndexExpression(const BytesValueIndexExpression &);
    BytesValueIndexExpression &operator=(const BytesValueIndexExpression &);
};

class RecordReferenceFieldExpression: public Expression {
public:
    RecordReferenceFieldExpression(const ast::RecordReferenceFieldExpression *rrfe): Expression(rrfe), rrfe(rrfe), ref(transform(rrfe->ref)), rectype(dynamic_cast<const TypeRecord *>(transform(rrfe->ref->type))), fieldtype(transform(rrfe->type)) {}
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
private:
    RecordReferenceFieldExpression(const RecordReferenceFieldExpression &);
    RecordReferenceFieldExpression &operator=(const RecordReferenceFieldExpression &);
};

class RecordValueFieldExpression: public Expression {
public:
    RecordValueFieldExpression(const ast::RecordValueFieldExpression *rvfe): Expression(rvfe), rvfe(rvfe), rec(transform(rvfe->rec)), rectype(dynamic_cast<const TypeRecord *>(transform(rvfe->rec->type))), fieldtype(transform(rvfe->type)) {}
    const ast::RecordValueFieldExpression *rvfe;
    const Expression *rec;
    const TypeRecord *rectype;
    const Type *fieldtype;

    virtual void generate(Context &context) const override {
        context.unimplemented("RecordValueFieldExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("RecordValueFieldExpression"); }
    virtual void generate_store(Context &) const override { internal_error("RecordValueFieldExpression"); }
private:
    RecordValueFieldExpression(const RecordValueFieldExpression &);
    RecordValueFieldExpression &operator=(const RecordValueFieldExpression &);
};

class ArrayReferenceRangeExpression: public Expression {
public:
    ArrayReferenceRangeExpression(const ast::ArrayReferenceRangeExpression *arre): Expression(arre), arre(arre), ref(transform(arre->ref)), first(transform(arre->first)), last(transform(arre->last)) {}
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
private:
    ArrayReferenceRangeExpression(const ArrayReferenceRangeExpression &);
    ArrayReferenceRangeExpression &operator=(const ArrayReferenceRangeExpression &);
};

class ArrayValueRangeExpression: public Expression {
public:
    ArrayValueRangeExpression(const ast::ArrayValueRangeExpression *avre): Expression(avre), avre(avre), array(transform(avre->array)), first(transform(avre->first)), last(transform(avre->last)) {}
    const ast::ArrayValueRangeExpression *avre;
    const Expression *array;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        context.unimplemented("ArrayValueRangeExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ArrayValueRangeExpression"); }
    virtual void generate_store(Context&) const override { internal_error("ArrayValueRangeExpression"); }
private:
    ArrayValueRangeExpression(const ArrayValueRangeExpression &);
    ArrayValueRangeExpression &operator=(const ArrayValueRangeExpression &);
};

class PointerDereferenceExpression: public Expression {
public:
    PointerDereferenceExpression(const ast::PointerDereferenceExpression *pde): Expression(pde), pde(pde), ptr(transform(pde->ptr)) {}
    const ast::PointerDereferenceExpression *pde;
    const Expression *ptr;

    virtual void generate(Context &context) const override {
        context.unimplemented("PointerDereferenceExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("PointerDereferenceExpression"); }
    virtual void generate_store(Context&) const override { internal_error("PointerDereferenceExpression"); }
private:
    PointerDereferenceExpression(const PointerDereferenceExpression &);
    PointerDereferenceExpression &operator=(const PointerDereferenceExpression &);
};

class VariableExpression: public Expression {
public:
    VariableExpression(const ast::VariableExpression *ve): Expression(ve), ve(ve), var(transform(ve->var)) {}
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
private:
    VariableExpression(const VariableExpression &);
    VariableExpression &operator=(const VariableExpression &);
};

class FunctionCall: public Expression {
public:
    FunctionCall(const ast::FunctionCall *fc): Expression(fc), fc(fc), func(transform(fc->func)), args() {
        for (auto a: fc->args) {
            args.push_back(transform(a));
        }
    }
    const ast::FunctionCall *fc;
    const Expression *func;
    std::vector<const Expression *> args;

    virtual void generate(Context &context) const override {
        func->generate_call(context, args);
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("FunctionCall"); }
    virtual void generate_store(Context &) const override { internal_error("FunctionCall"); }
private:
    FunctionCall(const FunctionCall &);
    FunctionCall &operator=(const FunctionCall &);
};

class NullStatement: public Statement {
public:
    NullStatement(const ast::NullStatement *ns): Statement(ns), ns(ns) {}
    const ast::NullStatement *ns;

    virtual void generate(Context &) const override {}
private:
    NullStatement(const NullStatement &);
    NullStatement &operator=(const NullStatement &);
};

class DeclarationStatement: public Statement {
public:
    DeclarationStatement(const ast::DeclarationStatement *ds): Statement(ds), ds(ds), decl(transform(ds->decl)) {}
    const ast::DeclarationStatement *ds;
    const Variable *decl;

    virtual void generate(Context &context) const override {
        context.unimplemented("DeclarationStatement");
    }
private:
    DeclarationStatement(const DeclarationStatement &);
    DeclarationStatement &operator=(const DeclarationStatement &);
};

class AssertStatement: public Statement {
public:
    AssertStatement(const ast::AssertStatement *as): Statement(as), as(as), statements(), expr(transform(as->expr)) {
        for (auto s: as->statements) {
            statements.push_back(transform(s));
        }
    }
    const ast::AssertStatement *as;
    std::vector<const Statement *> statements;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        context.unimplemented("AssertStatement");
    }
private:
    AssertStatement(const AssertStatement &);
    AssertStatement &operator=(const AssertStatement &);
};

class AssignmentStatement: public Statement {
public:
    AssignmentStatement(const ast::AssignmentStatement *as): Statement(as), as(as), variables(), expr(transform(as->expr)) {
        for (auto v: as->variables) {
            variables.push_back(transform(v));
        }
    }
    const ast::AssignmentStatement *as;
    std::vector<const Expression *> variables;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        context.unimplemented("AssignmentStatement");
    }
private:
    AssignmentStatement(const AssignmentStatement &);
    AssignmentStatement &operator=(const AssignmentStatement &);
};

class ExpressionStatement: public Statement {
public:
    ExpressionStatement(const ast::ExpressionStatement *es): Statement(es), es(es), expr(transform(es->expr)) {}
    const ast::ExpressionStatement *es;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        expr->generate(context);
    }
private:
    ExpressionStatement(const ExpressionStatement &);
    ExpressionStatement &operator=(const ExpressionStatement &);
};

class CompoundStatement: public Statement {
public:
    CompoundStatement(const ast::CompoundStatement *cs): Statement(cs), cs(cs), statements() {
        for (auto s: cs->statements) {
            statements.push_back(transform(s));
        }
    }
    const ast::CompoundStatement *cs;
    std::vector<const Statement *> statements;

    virtual void generate(Context &context) const override {
        for (auto s: statements) {
            s->generate(context);
        }
    }
private:
    CompoundStatement(const CompoundStatement &);
    CompoundStatement &operator=(const CompoundStatement &);
};

class BaseLoopStatement: public CompoundStatement {
public:
    BaseLoopStatement(const ast::BaseLoopStatement *bls): CompoundStatement(bls), bls(bls), prologue(), tail() {
        for (auto s: bls->prologue) {
            prologue.push_back(transform(s));
        }
        for (auto s: bls->tail) {
            tail.push_back(transform(s));
        }
    }
    const ast::BaseLoopStatement *bls;
    std::vector<const Statement *> prologue;
    std::vector<const Statement *> tail;

    virtual void generate(Context &context) const override {
        context.unimplemented("BaseLoopStatement");
    }
private:
    BaseLoopStatement(const BaseLoopStatement &);
    BaseLoopStatement &operator=(const BaseLoopStatement &);
};

class CaseStatement: public Statement {
public:
    class WhenCondition {
    public:
        WhenCondition() {}
        virtual ~WhenCondition() {}
        virtual void generate(Context &context, Context::Label &label_true, Context::Label &label_false) const = 0;
    private:
        WhenCondition(const WhenCondition &);
        WhenCondition &operator=(const WhenCondition &);
    };
    class ComparisonWhenCondition: public WhenCondition {
    public:
        ComparisonWhenCondition(ast::ComparisonExpression::Comparison comp, const Expression *expr): comp(comp), expr(expr) {}
        ast::ComparisonExpression::Comparison comp;
        const Expression *expr;
        virtual void generate(Context &context, Context::Label &, Context::Label &) const override {
            context.unimplemented("ComparisonWhenCondition");
        }
    private:
        ComparisonWhenCondition(const ComparisonWhenCondition &);
        ComparisonWhenCondition &operator=(const ComparisonWhenCondition &);
    };
    class RangeWhenCondition: public WhenCondition {
    public:
        RangeWhenCondition(const Expression *low_expr, const Expression *high_expr): low_expr(low_expr), high_expr(high_expr) {}
        const Expression *low_expr;
        const Expression *high_expr;
        virtual void generate(Context &context, Context::Label &, Context::Label &) const override {
            context.unimplemented("RangeWhenCondition");
        }
    private:
        RangeWhenCondition(const RangeWhenCondition &);
        RangeWhenCondition &operator=(const RangeWhenCondition &);
    };
    CaseStatement(const ast::CaseStatement *cs): Statement(cs), cs(cs), expr(transform(cs->expr)), clauses() {
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
    const ast::CaseStatement *cs;
    const Expression *expr;
    std::vector<std::pair<std::vector<const WhenCondition *>, std::vector<const Statement *>>> clauses;

    virtual void generate(Context &context) const override {
        context.unimplemented("CaseStatement");
    }
private:
    CaseStatement(const CaseStatement &);
    CaseStatement &operator=(const CaseStatement &);
};

class ExitStatement: public Statement {
public:
    ExitStatement(const ast::ExitStatement *es): Statement(es), es(es) {}
    const ast::ExitStatement *es;

    void generate(Context &context) const override {
        context.unimplemented("ExitStatement");
    }
private:
    ExitStatement(const ExitStatement &);
    ExitStatement &operator=(const ExitStatement &);
};

class NextStatement: public Statement {
public:
    NextStatement(const ast::NextStatement *ns): Statement(ns), ns(ns) {}
    const ast::NextStatement *ns;

    virtual void generate(Context &context) const override {
        context.unimplemented("NextStatement");
    }
private:
    NextStatement(const NextStatement &);
    NextStatement &operator=(const NextStatement &);
};

class TryStatementTrap {
public:
    TryStatementTrap(const ast::TryTrap *tt): tt(tt), name(transform(tt->name)), handler() {
        for (auto s: dynamic_cast<const ast::ExceptionHandlerStatement *>(tt->handler)->statements) {
            handler.push_back(transform(s));
        }
    }
    const ast::TryTrap *tt;
    const Variable *name;
    std::vector<const Statement *> handler;
private:
    TryStatementTrap(const TryStatementTrap &);
    TryStatementTrap &operator=(const TryStatementTrap &);
};

class TryStatement: public Statement {
public:
    TryStatement(const ast::TryStatement *ts): Statement(ts), ts(ts), statements(), catches() {
        for (auto s: ts->statements) {
            statements.push_back(transform(s));
        }
        for (auto &t: ts->catches) {
            catches.push_back(new TryStatementTrap(&t));
        }
    }
    const ast::TryStatement *ts;
    std::vector<const Statement *> statements;
    std::vector<const TryStatementTrap *> catches;

    virtual void generate(Context &context) const override {
        context.unimplemented("TryStatement");
    }
private:
    TryStatement(const TryStatement &);
    TryStatement &operator=(const TryStatement &);
};

class ReturnStatement: public Statement {
public:
    ReturnStatement(const ast::ReturnStatement *rs): Statement(rs), rs(rs), expr(transform(rs->expr)) {}
    const ast::ReturnStatement *rs;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        context.unimplemented("ReturnStatement");
    }
private:
    ReturnStatement(const ReturnStatement &);
    ReturnStatement &operator=(const ReturnStatement &);
};

class IncrementStatement: public Statement {
public:
    IncrementStatement(const ast::IncrementStatement *is): Statement(is), is(is), ref(transform(is->ref)) {}
    const ast::IncrementStatement *is;
    const Expression *ref;

    virtual void generate(Context &context) const override {
        context.unimplemented("IncrementStatement");
    }
private:
    IncrementStatement(const IncrementStatement &);
    IncrementStatement &operator=(const IncrementStatement &);
};

class IfStatement: public Statement {
public:
    IfStatement(const ast::IfStatement *is): Statement(is), is(is), condition_statements(), else_statements() {
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
    const ast::IfStatement *is;
    std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    std::vector<const Statement *> else_statements;

    void generate(Context &context) const override {
        context.unimplemented("IfStatement");
    }
private:
    IfStatement(const IfStatement &);
    IfStatement &operator=(const IfStatement &);
};

class RaiseStatement: public Statement {
public:
    RaiseStatement(const ast::RaiseStatement *rs): Statement(rs), rs(rs) {}
    const ast::RaiseStatement *rs;

    virtual void generate(Context &context) const override {
        context.unimplemented("RaiseStatement");
    }
private:
    RaiseStatement(const RaiseStatement &);
    RaiseStatement &operator=(const RaiseStatement &);
};

class ResetStatement: public Statement {
public:
    ResetStatement(const ast::ResetStatement *rs): Statement(rs), rs(rs) {}
    const ast::ResetStatement *rs;

    virtual void generate(Context &) const override {
    }
private:
    ResetStatement(const ResetStatement &);
    ResetStatement &operator=(const ResetStatement &);
};

class Function: public Variable {
public:
    Function(const ast::Function *f): Variable(f), f(f), statements(), params(), signature(), out_count(0) {
        // Need to transform the function parameters before transforming
        // the code that might use them (statements).
        signature = "(";
        int i = 0;
        for (auto p: f->params) {
            FunctionParameter *q = new FunctionParameter(p, i);
            params.push_back(q);
            g_variable_cache[p] = q;
            signature.append(q->type->jtype);
            if (q->fp->mode == ast::ParameterType::INOUT || q->fp->mode == ast::ParameterType::OUT) {
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
private:
    Function(const Function &);
    Function &operator=(const Function &);
};

class PredefinedFunction: public Variable {
public:
    PredefinedFunction(const ast::PredefinedFunction *pf): Variable(pf), pf(pf), out_count(0) {
        const ast::TypeFunction *tf = dynamic_cast<const ast::TypeFunction *>(pf->type);
        for (auto p: tf->params) {
            if (p->mode == ast::ParameterType::INOUT || p->mode == ast::ParameterType::OUT) {
                out_count++;
            }
        }
    }
    const ast::PredefinedFunction *pf;
    int out_count;

    virtual void generate_decl(ClassContext &, bool) const override {}
    virtual void generate_load(Context &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_store(Context &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_call(Context &context, const std::vector<const Expression *> &) const override {
        context.unimplemented("PredefinedFunction");
    }
private:
    PredefinedFunction(const PredefinedFunction &);
    PredefinedFunction &operator=(const PredefinedFunction &);
};

class ModuleFunction: public Variable {
public:
    ModuleFunction(const ast::ModuleFunction *mf): Variable(mf), mf(mf), signature(), out_count(0) {
        const TypeFunction *functype = dynamic_cast<const TypeFunction *>(transform(mf->type));
        signature = "(";
        int i = 0;
        for (auto p: functype->paramtypes) {
            signature.append(p.second->jtype);
            if (p.first == ast::ParameterType::INOUT || p.first == ast::ParameterType::OUT) {
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
    const ast::ModuleFunction *mf;
    std::string signature;
    int out_count;

    virtual void generate_decl(ClassContext &, bool) const override {}
    virtual void generate_load(Context &) const override { internal_error("ModuleFunction"); }
    virtual void generate_store(Context &) const override { internal_error("ModuleFunction"); }
    virtual void generate_call(Context &context, const std::vector<const Expression *> &) const override {
        context.unimplemented("ModuleFunction");
    }
private:
    ModuleFunction(const ModuleFunction &);
    ModuleFunction &operator=(const ModuleFunction &);
};

class Program {
public:
    Program(CompilerSupport *support, const ast::Program *program): support(support), program(program), statements() {
        for (auto s: program->statements) {
            statements.push_back(transform(s));
        }
    }
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
        std::vector<uint8_t> data = exe.serialize();
        support->writeOutput(path + program->module_name + ".exe", data);
    }
private:
    Program(const Program &);
    Program &operator=(const Program &);
};

class TypeTransformer: public ast::IAstVisitor {
public:
    TypeTransformer(): r(nullptr) {}
    Type *retval() { if (r == nullptr) internal_error("TypeTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *node) { r = new TypeNothing(node); }
    virtual void visit(const ast::TypeDummy *node) { r = new TypeDummy(node); }
    virtual void visit(const ast::TypeBoolean *node) { r = new TypeBoolean(node); }
    virtual void visit(const ast::TypeNumber *node) { r = new TypeNumber(node); }
    virtual void visit(const ast::TypeString *node) { r = new TypeString(node); }
    virtual void visit(const ast::TypeBytes *node) { r = new TypeBytes(node); }
    virtual void visit(const ast::TypeFunction *node) { r = new TypeFunction(node); }
    virtual void visit(const ast::TypeArray *node) { r = new TypeArray(node); }
    virtual void visit(const ast::TypeDictionary *node) { r = new TypeDictionary(node); }
    virtual void visit(const ast::TypeRecord *node) { r = new TypeRecord(node); }
    virtual void visit(const ast::TypeClass *node) { r = new TypeRecord(node); }
    virtual void visit(const ast::TypePointer *node) { r = new TypePointer(node); }
    virtual void visit(const ast::TypeFunctionPointer *node) { r = new TypeFunctionPointer(node); }
    virtual void visit(const ast::TypeEnum *node) { r = new TypeEnum(node); }
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *) {}
    virtual void visit(const ast::ModuleVariable *) {}
    virtual void visit(const ast::GlobalVariable *) {}
    virtual void visit(const ast::ExternalGlobalVariable *) {}
    virtual void visit(const ast::LocalVariable *) {}
    virtual void visit(const ast::FunctionParameter *) {}
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *) {}
    virtual void visit(const ast::ConstantNumberExpression *) {}
    virtual void visit(const ast::ConstantStringExpression *) {}
    virtual void visit(const ast::ConstantBytesExpression *) {}
    virtual void visit(const ast::ConstantEnumExpression *) {}
    virtual void visit(const ast::ConstantNilExpression *) {}
    virtual void visit(const ast::ConstantNowhereExpression *) {}
    virtual void visit(const ast::ArrayLiteralExpression *) {}
    virtual void visit(const ast::DictionaryLiteralExpression *) {}
    virtual void visit(const ast::RecordLiteralExpression *) {}
    virtual void visit(const ast::NewClassExpression *) {}
    virtual void visit(const ast::UnaryMinusExpression *) {}
    virtual void visit(const ast::LogicalNotExpression *) {}
    virtual void visit(const ast::ConditionalExpression *) {}
    virtual void visit(const ast::TryExpression *) {}
    virtual void visit(const ast::DisjunctionExpression *) {}
    virtual void visit(const ast::ConjunctionExpression *) {}
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
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::RecordReferenceFieldExpression *) {}
    virtual void visit(const ast::RecordValueFieldExpression *) {}
    virtual void visit(const ast::ArrayReferenceRangeExpression *) {}
    virtual void visit(const ast::ArrayValueRangeExpression *) {}
    virtual void visit(const ast::PointerDereferenceExpression *) {}
    virtual void visit(const ast::ConstantExpression *) {}
    virtual void visit(const ast::VariableExpression *) {}
    virtual void visit(const ast::FunctionCall *) {}
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *) {}
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
    virtual void visit(const ast::ModuleFunction *) {}
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Type *r;
private:
    TypeTransformer(const TypeTransformer &);
    TypeTransformer &operator=(const TypeTransformer &);
};

class VariableTransformer: public ast::IAstVisitor {
public:
    VariableTransformer(): r(nullptr) {}
    Variable *retval() { if (r == nullptr) internal_error("VariableTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *) {}
    virtual void visit(const ast::TypeDummy *) {}
    virtual void visit(const ast::TypeBoolean *) {}
    virtual void visit(const ast::TypeNumber *) {}
    virtual void visit(const ast::TypeString *) {}
    virtual void visit(const ast::TypeBytes *) {}
    virtual void visit(const ast::TypeFunction *) {}
    virtual void visit(const ast::TypeArray *) {}
    virtual void visit(const ast::TypeDictionary *) {}
    virtual void visit(const ast::TypeRecord *) {}
    virtual void visit(const ast::TypeClass *) {}
    virtual void visit(const ast::TypePointer *) {}
    virtual void visit(const ast::TypeFunctionPointer *) {}
    virtual void visit(const ast::TypeEnum *) {}
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *node) { r = new PredefinedVariable(node); }
    virtual void visit(const ast::ModuleVariable *node) { r = new ModuleVariable(node); }
    virtual void visit(const ast::GlobalVariable *node) { r = new GlobalVariable(node); }
    virtual void visit(const ast::ExternalGlobalVariable *) { internal_error("ExternalGlobalVariable"); }
    virtual void visit(const ast::LocalVariable *node) { r = new LocalVariable(node); }
    virtual void visit(const ast::FunctionParameter *) { /*r = new FunctionParameter(node);*/ }
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *) {}
    virtual void visit(const ast::ConstantNumberExpression *) {}
    virtual void visit(const ast::ConstantStringExpression *) {}
    virtual void visit(const ast::ConstantBytesExpression *) {}
    virtual void visit(const ast::ConstantEnumExpression *) {}
    virtual void visit(const ast::ConstantNilExpression *) {}
    virtual void visit(const ast::ConstantNowhereExpression *) {}
    virtual void visit(const ast::ArrayLiteralExpression *) {}
    virtual void visit(const ast::DictionaryLiteralExpression *) {}
    virtual void visit(const ast::RecordLiteralExpression *) {}
    virtual void visit(const ast::NewClassExpression *) {}
    virtual void visit(const ast::UnaryMinusExpression *) {}
    virtual void visit(const ast::LogicalNotExpression *) {}
    virtual void visit(const ast::ConditionalExpression *) {}
    virtual void visit(const ast::TryExpression *) {}
    virtual void visit(const ast::DisjunctionExpression *) {}
    virtual void visit(const ast::ConjunctionExpression *) {}
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
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::RecordReferenceFieldExpression *) {}
    virtual void visit(const ast::RecordValueFieldExpression *) {}
    virtual void visit(const ast::ArrayReferenceRangeExpression *) {}
    virtual void visit(const ast::ArrayValueRangeExpression *) {}
    virtual void visit(const ast::PointerDereferenceExpression *) {}
    virtual void visit(const ast::ConstantExpression *) {}
    virtual void visit(const ast::VariableExpression *) {}
    virtual void visit(const ast::FunctionCall *) {}
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *) {}
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
    virtual void visit(const ast::ModuleFunction *node) { r = new ModuleFunction(node); }
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Variable *r;
private:
    VariableTransformer(const VariableTransformer &);
    VariableTransformer &operator=(const VariableTransformer &);
};

class ExpressionTransformer: public ast::IAstVisitor {
public:
    ExpressionTransformer(): r(nullptr) {}
    Expression *retval() { if (r == nullptr) internal_error("ExpressionTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *) {}
    virtual void visit(const ast::TypeDummy *) {}
    virtual void visit(const ast::TypeBoolean *) {}
    virtual void visit(const ast::TypeNumber *) {}
    virtual void visit(const ast::TypeString *) {}
    virtual void visit(const ast::TypeBytes *) {}
    virtual void visit(const ast::TypeFunction *) {}
    virtual void visit(const ast::TypeArray *) {}
    virtual void visit(const ast::TypeDictionary *) {}
    virtual void visit(const ast::TypeRecord *) {}
    virtual void visit(const ast::TypeClass *) {}
    virtual void visit(const ast::TypePointer *) {}
    virtual void visit(const ast::TypeFunctionPointer *) {}
    virtual void visit(const ast::TypeEnum *) {}
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *) {}
    virtual void visit(const ast::ModuleVariable *) {}
    virtual void visit(const ast::GlobalVariable *) {}
    virtual void visit(const ast::ExternalGlobalVariable *) {}
    virtual void visit(const ast::LocalVariable *) {}
    virtual void visit(const ast::FunctionParameter *) {}
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *node) { r = new ConstantBooleanExpression(node); }
    virtual void visit(const ast::ConstantNumberExpression *node) { r = new ConstantNumberExpression(node); }
    virtual void visit(const ast::ConstantStringExpression *node) { r = new ConstantStringExpression(node); }
    virtual void visit(const ast::ConstantBytesExpression *node) { r = new ConstantBytesExpression(node); }
    virtual void visit(const ast::ConstantEnumExpression *node) { r = new ConstantEnumExpression(node); }
    virtual void visit(const ast::ConstantNilExpression *node) { r = new ConstantNilExpression(node); }
    virtual void visit(const ast::ConstantNowhereExpression *node) { r = new ConstantNowhereExpression(node); }
    virtual void visit(const ast::ArrayLiteralExpression *node) { r = new ArrayLiteralExpression(node); }
    virtual void visit(const ast::DictionaryLiteralExpression *node) { r = new DictionaryLiteralExpression(node); }
    virtual void visit(const ast::RecordLiteralExpression *node) { r = new RecordLiteralExpression(node); }
    virtual void visit(const ast::NewClassExpression *node) { r =  new NewClassExpression(node); }
    virtual void visit(const ast::UnaryMinusExpression *node) { r = new UnaryMinusExpression(node); }
    virtual void visit(const ast::LogicalNotExpression *node) { r = new LogicalNotExpression(node); }
    virtual void visit(const ast::ConditionalExpression *node) { r = new ConditionalExpression(node); }
    virtual void visit(const ast::TryExpression *node) { r = new TryExpression(node); }
    virtual void visit(const ast::DisjunctionExpression *node) { r = new DisjunctionExpression(node); }
    virtual void visit(const ast::ConjunctionExpression *node) { r = new ConjunctionExpression(node); }
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
    virtual void visit(const ast::BytesReferenceIndexExpression *node) { r = new BytesReferenceIndexExpression(node); }
    virtual void visit(const ast::BytesValueIndexExpression *node) { r = new BytesValueIndexExpression(node); }
    virtual void visit(const ast::RecordReferenceFieldExpression *node) { r = new RecordReferenceFieldExpression(node); }
    virtual void visit(const ast::RecordValueFieldExpression *node) { r = new RecordValueFieldExpression(node); }
    virtual void visit(const ast::ArrayReferenceRangeExpression *node) { r = new ArrayReferenceRangeExpression(node); }
    virtual void visit(const ast::ArrayValueRangeExpression *node) { r = new ArrayValueRangeExpression(node); }
    virtual void visit(const ast::PointerDereferenceExpression *node) { r =  new PointerDereferenceExpression(node); }
    virtual void visit(const ast::ConstantExpression *node) { r = transform(node->constant->value); }
    virtual void visit(const ast::VariableExpression *node) { r = new VariableExpression(node); }
    virtual void visit(const ast::FunctionCall *node) { r = new FunctionCall(node); }
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *) {}
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
    virtual void visit(const ast::ModuleFunction *) {}
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Expression *r;
private:
    ExpressionTransformer(const ExpressionTransformer &);
    ExpressionTransformer &operator=(const ExpressionTransformer &);
};

class StatementTransformer: public ast::IAstVisitor {
public:
    StatementTransformer(): r(nullptr) {}
    Statement *retval() { if (r == nullptr) internal_error("StatementTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *) {}
    virtual void visit(const ast::TypeDummy *) {}
    virtual void visit(const ast::TypeBoolean *) {}
    virtual void visit(const ast::TypeNumber *) {}
    virtual void visit(const ast::TypeString *) {}
    virtual void visit(const ast::TypeBytes *) {}
    virtual void visit(const ast::TypeFunction *) {}
    virtual void visit(const ast::TypeArray *) {}
    virtual void visit(const ast::TypeDictionary *) {}
    virtual void visit(const ast::TypeRecord *) {}
    virtual void visit(const ast::TypeClass *) {}
    virtual void visit(const ast::TypePointer *) {}
    virtual void visit(const ast::TypeFunctionPointer *) {}
    virtual void visit(const ast::TypeEnum *) {}
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *) {}
    virtual void visit(const ast::ModuleVariable *) {}
    virtual void visit(const ast::GlobalVariable *) {}
    virtual void visit(const ast::ExternalGlobalVariable *) {}
    virtual void visit(const ast::LocalVariable *) {}
    virtual void visit(const ast::FunctionParameter *) {}
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *) {}
    virtual void visit(const ast::ConstantNumberExpression *) {}
    virtual void visit(const ast::ConstantStringExpression *) {}
    virtual void visit(const ast::ConstantBytesExpression *) {}
    virtual void visit(const ast::ConstantEnumExpression *) {}
    virtual void visit(const ast::ConstantNilExpression *) {}
    virtual void visit(const ast::ConstantNowhereExpression *) {}
    virtual void visit(const ast::ArrayLiteralExpression *) {}
    virtual void visit(const ast::DictionaryLiteralExpression *) {}
    virtual void visit(const ast::RecordLiteralExpression *) {}
    virtual void visit(const ast::NewClassExpression *) {}
    virtual void visit(const ast::UnaryMinusExpression *) {}
    virtual void visit(const ast::LogicalNotExpression *) {}
    virtual void visit(const ast::ConditionalExpression *) {}
    virtual void visit(const ast::TryExpression *) {}
    virtual void visit(const ast::DisjunctionExpression *) {}
    virtual void visit(const ast::ConjunctionExpression *) {}
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
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::RecordReferenceFieldExpression *) {}
    virtual void visit(const ast::RecordValueFieldExpression *) {}
    virtual void visit(const ast::ArrayReferenceRangeExpression *) {}
    virtual void visit(const ast::ArrayValueRangeExpression *) {}
    virtual void visit(const ast::PointerDereferenceExpression *) {}
    virtual void visit(const ast::ConstantExpression *) {}
    virtual void visit(const ast::VariableExpression *) {}
    virtual void visit(const ast::FunctionCall *) {}
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *node) { r = new NullStatement(node); }
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
    virtual void visit(const ast::ModuleFunction *) {}
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Statement *r;
private:
    StatementTransformer(const StatementTransformer &);
    StatementTransformer &operator=(const StatementTransformer &);
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

void compile_cli(CompilerSupport *support, const ast::Program *p)
{
    cli::g_type_cache.clear();
    cli::g_variable_cache.clear();
    cli::g_expression_cache.clear();
    cli::g_statement_cache.clear();

    cli::Program *ct = new cli::Program(support, p);
    ct->generate();
}
