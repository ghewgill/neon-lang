package main

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"
	"io/ioutil"
	"math"
	"math/rand"
	"os"
	"os/exec"
	"path"
	"path/filepath"
	"runtime"
	"sort"
	"strconv"
	"strings"
	"time"
)

const BYTECODE_VERSION int = 3

const (
	PUSHB   = iota // push boolean immediate
	PUSHN   = iota // push number immediate
	PUSHS   = iota // push string immediate
	PUSHY   = iota // push bytes immediate
	PUSHPG  = iota // push pointer to global
	PUSHPPG = iota // push pointer to predefined global
	PUSHPMG = iota // push pointer to module global
	PUSHPL  = iota // push pointer to local
	PUSHPOL = iota // push pointer to outer local
	PUSHI   = iota // push 32-bit integer immediate
	LOADB   = iota // load boolean
	LOADN   = iota // load number
	LOADS   = iota // load string
	LOADY   = iota // load bytes
	LOADA   = iota // load array
	LOADD   = iota // load dictionary
	LOADP   = iota // load pointer
	LOADJ   = iota // load object
	LOADV   = iota // load voidptr
	STOREB  = iota // store boolean
	STOREN  = iota // store number
	STORES  = iota // store string
	STOREY  = iota // store bytes
	STOREA  = iota // store array
	STORED  = iota // store dictionary
	STOREP  = iota // store pointer
	STOREJ  = iota // store object
	STOREV  = iota // store voidptr
	NEGN    = iota // negate number
	ADDN    = iota // add number
	SUBN    = iota // subtract number
	MULN    = iota // multiply number
	DIVN    = iota // divide number
	MODN    = iota // modulo number
	EXPN    = iota // exponentiate number
	EQB     = iota // compare equal boolean
	NEB     = iota // compare unequal boolean
	EQN     = iota // compare equal number
	NEN     = iota // compare unequal number
	LTN     = iota // compare less number
	GTN     = iota // compare greater number
	LEN     = iota // compare less equal number
	GEN     = iota // compare greater equal number
	EQS     = iota // compare equal string
	NES     = iota // compare unequal string
	LTS     = iota // compare less string
	GTS     = iota // compare greater string
	LES     = iota // compare less equal string
	GES     = iota // compare greater equal string
	EQY     = iota // compare equal bytes
	NEY     = iota // compare unequal bytes
	LTY     = iota // compare less bytes
	GTY     = iota // compare greater bytes
	LEY     = iota // compare less equal bytes
	GEY     = iota // compare greater equal bytes
	EQA     = iota // compare equal array
	NEA     = iota // compare unequal array
	EQD     = iota // compare equal dictionary
	NED     = iota // compare unequal dictionary
	EQP     = iota // compare equal pointer
	NEP     = iota // compare unequal pointer
	EQV     = iota // compare equal voidptr
	NEV     = iota // compare unequal voidptr
	ANDB    = iota // and boolean
	ORB     = iota // or boolean
	NOTB    = iota // not boolean
	INDEXAR = iota // index array for read
	INDEXAW = iota // index array for write
	INDEXAV = iota // index array value
	INDEXAN = iota // index array value, no exception
	INDEXDR = iota // index dictionary for read
	INDEXDW = iota // index dictionary for write
	INDEXDV = iota // index dictionary value
	INA     = iota // in array
	IND     = iota // in dictionary
	CALLP   = iota // call predefined
	CALLF   = iota // call function
	CALLMF  = iota // call module function
	CALLI   = iota // call indirect
	JUMP    = iota // unconditional jump
	JF      = iota // jump if false
	JT      = iota // jump if true
	DUP     = iota // duplicate
	DUPX1   = iota // duplicate under second value
	DROP    = iota // drop
	RET     = iota // return
	CONSA   = iota // construct array
	CONSD   = iota // construct dictionary
	EXCEPT  = iota // throw exception
	ALLOC   = iota // allocate record
	PUSHNIL = iota // push nil pointer
	RESETC  = iota // reset cell
	PUSHPEG = iota // push pointer to external global
	JUMPTBL = iota // jump table
	CALLX   = iota // call extension
	SWAP    = iota // swap two top stack elements
	DROPN   = iota // drop element n
	PUSHFP  = iota // push function pointer
	CALLV   = iota // call virtual
	PUSHCI  = iota // push class info
)

func assert(b bool, msg string) {
	if !b {
		panic(msg)
	}
}

func get_vint(bytes []byte, i *int) int {
	r := 0
	for {
		x := bytes[*i]
		*i++
		r = (r << 7) | int(x&0x7f)
		if x&0x80 == 0 {
			break
		}
	}
	return r
}

type neonexception struct {
	name string
	info object
}

func (e *neonexception) Error() string {
	return fmt.Sprintf("%s (%s)", e.name, e.info)
}

type object interface {
	getBoolean() (bool, error)
	getNumber() (float64, error)
	getString() (string, error)
	getBytes() ([]byte, error)
	getArray() ([]object, error)
	getDictionary() (map[string]object, error)
	subscript(index object) (object, error)
	toLiteralString() string
	toString() string
}

type objectBoolean struct {
	bool bool
}

func (obj objectBoolean) getBoolean() (bool, error) {
	return obj.bool, nil
}

func (obj objectBoolean) getNumber() (float64, error) {
	return 0, errors.New("type error")
}

func (obj objectBoolean) getString() (string, error) {
	return "", errors.New("type error")
}

func (obj objectBoolean) getBytes() ([]byte, error) {
	return nil, errors.New("type error")
}

func (obj objectBoolean) getArray() ([]object, error) {
	return nil, errors.New("type error")
}

func (obj objectBoolean) getDictionary() (map[string]object, error) {
	return nil, errors.New("type error")
}

func (obj objectBoolean) subscript(index object) (object, error) {
	return nil, errors.New("type error")
}

func (obj objectBoolean) toLiteralString() string {
	return obj.toString()
}

func (obj objectBoolean) toString() string {
	if obj.bool {
		return "TRUE"
	} else {
		return "FALSE"
	}
}

type objectNumber struct {
	num float64
}

func (obj objectNumber) getBoolean() (bool, error) {
	return false, errors.New("type error")
}

func (obj objectNumber) getNumber() (float64, error) {
	return obj.num, nil
}

func (obj objectNumber) getString() (string, error) {
	return "", errors.New("type error")
}

func (obj objectNumber) getBytes() ([]byte, error) {
	return nil, errors.New("type error")
}

func (obj objectNumber) getArray() ([]object, error) {
	return nil, errors.New("type error")
}

func (obj objectNumber) getDictionary() (map[string]object, error) {
	return nil, errors.New("type error")
}

func (obj objectNumber) subscript(index object) (object, error) {
	return nil, errors.New("type error")
}

func (obj objectNumber) toLiteralString() string {
	return obj.toString()
}

func (obj objectNumber) toString() string {
	return strconv.FormatFloat(obj.num, 'g', -1, 64)
}

type objectString struct {
	str string
}

func (obj objectString) getBoolean() (bool, error) {
	return false, errors.New("type error")
}

func (obj objectString) getNumber() (float64, error) {
	return 0, errors.New("type error")
}

func (obj objectString) getString() (string, error) {
	return obj.str, nil
}

func (obj objectString) getBytes() ([]byte, error) {
	return nil, errors.New("type error")
}

func (obj objectString) getArray() ([]object, error) {
	return nil, errors.New("type error")
}

func (obj objectString) getDictionary() (map[string]object, error) {
	return nil, errors.New("type error")
}

func (obj objectString) subscript(index object) (object, error) {
	return nil, errors.New("type error")
}

func (obj objectString) toLiteralString() string {
	return quoted(obj.str)
}

func (obj objectString) toString() string {
	return obj.str
}

type objectBytes struct {
	bytes []byte
}

func (obj objectBytes) getBoolean() (bool, error) {
	return false, errors.New("type error")
}

func (obj objectBytes) getNumber() (float64, error) {
	return 0, errors.New("type error")
}

func (obj objectBytes) getString() (string, error) {
	return "", errors.New("type error")
}

func (obj objectBytes) getBytes() ([]byte, error) {
	return obj.bytes, nil
}

func (obj objectBytes) getArray() ([]object, error) {
	return nil, errors.New("type error")
}

func (obj objectBytes) getDictionary() (map[string]object, error) {
	return nil, errors.New("type error")
}

func (obj objectBytes) subscript(index object) (object, error) {
	return nil, errors.New("type error")
}

func (obj objectBytes) toLiteralString() string {
	return obj.toString()
}

func (obj objectBytes) toString() string {
	r := "HEXBYTES \""
	for i, x := range obj.bytes {
		if i > 0 {
			r += " "
		}
		r += fmt.Sprintf("%02x", x)
	}
	r += "\""
	return r
}

type objectArray struct {
	array []object
}

func (obj objectArray) getBoolean() (bool, error) {
	return false, errors.New("type error")
}

func (obj objectArray) getNumber() (float64, error) {
	return 0, errors.New("type error")
}

func (obj objectArray) getString() (string, error) {
	return "", errors.New("type error")
}

func (obj objectArray) getBytes() ([]byte, error) {
	return nil, errors.New("type error")
}

func (obj objectArray) getArray() ([]object, error) {
	return obj.array, nil
}

func (obj objectArray) getDictionary() (map[string]object, error) {
	return nil, errors.New("type error")
}

func (obj objectArray) subscript(index object) (object, error) {
	if i, err := index.getNumber(); err == nil {
		if int(i) < len(obj.array) {
			return obj.array[int(i)], nil
		} else {
			return nil, &neonexception{"ArrayIndexException", objectString{fmt.Sprintf("%g", i)}}
		}
	} else {
		return nil, &neonexception{"DynamicConversionException", objectString{"to Number"}}
	}
}

func (obj objectArray) toLiteralString() string {
	return obj.toString()
}

func (obj objectArray) toString() string {
	r := "["
	for i, x := range obj.array {
		if i > 0 {
			r += ", "
		}
		if x != nil {
			r += x.toLiteralString()
		} else {
			r += "null"
		}
	}
	r += "]"
	return r
}

type objectDictionary struct {
	dict map[string]object
}

func (obj objectDictionary) getBoolean() (bool, error) {
	return false, errors.New("type error")
}

func (obj objectDictionary) getNumber() (float64, error) {
	return 0, errors.New("type error")
}

func (obj objectDictionary) getString() (string, error) {
	return "", errors.New("type error")
}

func (obj objectDictionary) getBytes() ([]byte, error) {
	return nil, errors.New("type error")
}

func (obj objectDictionary) getArray() ([]object, error) {
	return nil, errors.New("type error")
}

func (obj objectDictionary) getDictionary() (map[string]object, error) {
	return obj.dict, nil
}

func (obj objectDictionary) subscript(index object) (object, error) {
	if k, err := index.getString(); err == nil {
		if r, ok := obj.dict[k]; ok {
			return r, nil
		} else {
			return nil, &neonexception{"ObjectSubscriptException", objectString{k}}
		}
	} else {
		return nil, &neonexception{"DynamicConversionException", objectString{"to String"}}
	}
}

func (obj objectDictionary) toLiteralString() string {
	return obj.toString()
}

func (obj objectDictionary) toString() string {
	r := "{"
	first := true
	keys := []string{}
	for k := range obj.dict {
		keys = append(keys, k)
	}
	sort.Strings(keys)
	for _, k := range keys {
		if first {
			first = false
		} else {
			r += ", "
		}
		r += quoted(k) + ": "
		if obj.dict[k] != nil {
			r += obj.dict[k].toLiteralString()
		} else {
			r += "null"
		}
	}
	r += "}"
	return r
}

type reference interface {
	load() cell
	store(c cell)
}

type referenceDirect struct {
	addr *cell
}

func (r referenceDirect) load() cell {
	return *r.addr
}

func (r referenceDirect) store(c cell) {
	*r.addr = c
}

type referenceDictionary struct {
	dict reference
	key  string
}

func (r referenceDictionary) load() cell {
	return r.dict.load().dict[r.key]
}

func (r referenceDictionary) store(c cell) {
	d := r.dict.load()
	if d.dict == nil {
		d.dict = map[string]cell{}
		r.dict.store(d)
	}
	d.dict[r.key] = c
}

const (
	CELL_NONE       = iota
	CELL_REFERENCE  = iota
	CELL_BOOLEAN    = iota
	CELL_NUMBER     = iota
	CELL_STRING     = iota
	CELL_BYTES      = iota
	CELL_OBJECT     = iota
	CELL_ARRAY      = iota
	CELL_DICTIONARY = iota
	CELL_MODULE     = iota
	CELL_CLASSINFO  = iota
	CELL_OTHER      = iota
)

type moduleclassinfo struct {
	module    *module
	classinfo *classinfo
}

type cell struct {
	ctype     int
	ref       reference
	bool      bool
	num       float64
	str       string
	bytes     []byte
	obj       object
	array     []cell
	dict      map[string]cell
	module    *module
	classinfo moduleclassinfo
	other     interface{}
}

func make_cell_none() cell {
	return cell{
		ctype: CELL_NONE,
	}
}

func make_cell_addr(a *cell) cell {
	return cell{
		ctype: CELL_REFERENCE,
		ref:   referenceDirect{a},
	}
}

func make_cell_ref(r reference) cell {
	return cell{
		ctype: CELL_REFERENCE,
		ref:   r,
	}
}

func make_cell_bool(b bool) cell {
	return cell{
		ctype: CELL_BOOLEAN,
		bool:  b,
	}
}

func make_cell_num(n float64) cell {
	return cell{
		ctype: CELL_NUMBER,
		num:   n,
	}
}

func make_cell_str(s string) cell {
	return cell{
		ctype: CELL_STRING,
		str:   s,
	}
}

func make_cell_bytes(b []byte) cell {
	return cell{
		ctype: CELL_BYTES,
		bytes: b,
	}
}

func make_cell_obj(o object) cell {
	return cell{
		ctype: CELL_OBJECT,
		obj:   o,
	}
}

func make_cell_array(a []cell) cell {
	return cell{
		ctype: CELL_ARRAY,
		array: a,
	}
}

func make_cell_dict(d map[string]cell) cell {
	return cell{
		ctype: CELL_DICTIONARY,
		dict:  d,
	}
}

func make_cell_module(m *module) cell {
	return cell{
		ctype:  CELL_MODULE,
		module: m,
	}
}

func make_cell_classinfo(m *module, ci *classinfo) cell {
	return cell{
		ctype:     CELL_CLASSINFO,
		classinfo: moduleclassinfo{m, ci},
	}
}

func make_cell_other(p interface{}) cell {
	return cell{
		ctype: CELL_OTHER,
		other: p,
	}
}

func (self cell) Equal(other cell) bool {
	if self.ctype != other.ctype {
		return false
	}
	switch self.ctype {
	case CELL_REFERENCE:
		panic("compare references")
		return false
	case CELL_BOOLEAN:
		return self.bool == other.bool
	case CELL_NUMBER:
		return self.num == other.num
	case CELL_STRING:
		return self.str == other.str
	case CELL_BYTES:
		panic("compare bytes")
		return false //self.bytes == other.bytes
	case CELL_OBJECT:
		return self.obj == other.obj
	case CELL_ARRAY:
		if len(self.array) != len(other.array) {
			return false
		}
		for i, v := range self.array {
			if !v.Equal(other.array[i]) {
				return false
			}
		}
		return true
	case CELL_DICTIONARY:
		if len(self.dict) != len(other.dict) {
			return false
		}
		for k, v := range self.dict {
			_, exists := other.dict[k]
			if !exists {
				return false
			}
			if !v.Equal(other.dict[k]) {
				return false
			}
		}
		return true
	case CELL_MODULE:
		return self.module == other.module
	default:
		panic("unknown ctype in cell.Equal")
	}
	return false
}

func quoted(s string) string {
	var r bytes.Buffer
	r.WriteString("\"")
	for i := 0; i < len(s); i++ {
		var c uint32 = uint32(s[i])

		// UTF8 decoder. This won't be the right place for this, but here it is.
		if (c & 0x80) != 0 {
			n := 0
			if (c & 0xe0) == 0xc0 {
				c &= 0x1f
				n = 1
			} else if (c & 0xf0) == 0xe0 {
				c &= 0x0f
				n = 2
			} else if (c & 0xf8) == 0xf0 {
				c &= 0x07
				n = 3
			} else if (c & 0xfc) == 0xf8 {
				c &= 0x03
				n = 4
			} else if (c & 0xfe) == 0xfc {
				c &= 0x01
				n = 5
			}
			for n > 0 {
				i++
				if (s[i] & 0xc0) != 0x80 {
					// Garbage data, give up.
					break
				}
				c = (c << 6) | uint32(s[i]&0x3f)
				n--
			}
		}

		switch c {
		case '\b':
			r.WriteString("\\b")
		case '\f':
			r.WriteString("\\f")
		case '\n':
			r.WriteString("\\n")
		case '\r':
			r.WriteString("\\r")
		case '\t':
			r.WriteString("\\t")
		case '"', '\\':
			r.WriteString("\\")
			r.WriteByte(byte(c))
		default:
			if c >= ' ' && c < 0x7f {
				r.WriteByte(byte(c))
			} else if c < 0x10000 {
				r.WriteString(fmt.Sprintf("\\u%04x", c))
			} else {
				r.WriteString(fmt.Sprintf("\\U%08x", c))
			}
		}
	}
	r.WriteString("\"")
	return r.String()
}

type exception struct {
	start       int
	end         int
	excid       int
	handler     int
	stack_depth int
}

type exporttype struct {
	name       int
	descriptor int
}

type exportconstant struct {
	name  int
	vtype int
	value []byte
}

type exportvariable struct {
	name  int
	vtype int
	index int
}

type exportfunction struct {
	name       int
	descriptor int
	index      int
}

type exportexception struct {
	name int
}

type exportinterface struct {
	name               int
	method_descriptors []byte
}

type function struct {
	name   int
	nest   int
	params int
	locals int
	entry  int
}

type imported struct {
	name     int
	optional bool
	hash     []byte
}

type classinfo struct {
	name       int
	interfaces [][]int
}

type bytecode struct {
	version           int
	source_hash       []byte
	global_size       int
	strtable          [][]byte
	export_types      []exporttype
	export_constants  []exportconstant
	export_variables  []exportvariable
	export_functions  []exportfunction
	export_exceptions []exportexception
	export_interfaces []exportinterface
	functions         []function
	imports           []imported
	exceptions        []exception
	classes           []classinfo
	code              []byte
}

func get_strtable(bytes []byte) [][]byte {
	r := [][]byte{}
	i := 0
	for i < len(bytes) {
		strlen := get_vint(bytes, &i)
		r = append(r, bytes[i:i+strlen])
		i += strlen
	}
	return r
}

func make_bytecode(bytes []byte) bytecode {
	r := bytecode{}
	i := 0

	sig := bytes[0:4]
	assert(sig[0] == 0x4e && sig[1] == 0x65 && sig[2] == 0x00 && sig[3] == 0x6e, "signature not found")
	i += 4

	r.version = get_vint(bytes, &i)
	assert(r.version == BYTECODE_VERSION, "bytecode version mismatch")

	r.source_hash = bytes[i : i+32]
	i += 32
	r.global_size = get_vint(bytes, &i)
	strtablesize := get_vint(bytes, &i)
	r.strtable = get_strtable(bytes[i : i+strtablesize])
	i += strtablesize

	typesize := get_vint(bytes, &i)
	for typesize > 0 {
		et := exporttype{}
		et.name = get_vint(bytes, &i)
		et.descriptor = get_vint(bytes, &i)
		r.export_types = append(r.export_types, et)
		typesize--
	}

	constantsize := get_vint(bytes, &i)
	for constantsize > 0 {
		ec := exportconstant{}
		ec.name = get_vint(bytes, &i)
		ec.vtype = get_vint(bytes, &i)
		size := get_vint(bytes, &i)
		ec.value = bytes[i : i+size]
		i += size
		r.export_constants = append(r.export_constants, ec)
		constantsize--
	}

	variablesize := get_vint(bytes, &i)
	for variablesize > 0 {
		ev := exportvariable{}
		ev.name = get_vint(bytes, &i)
		ev.vtype = get_vint(bytes, &i)
		ev.index = get_vint(bytes, &i)
		r.export_variables = append(r.export_variables, ev)
		variablesize--
	}

	exportfunctionsize := get_vint(bytes, &i)
	for exportfunctionsize > 0 {
		ef := exportfunction{}
		ef.name = get_vint(bytes, &i)
		ef.descriptor = get_vint(bytes, &i)
		ef.index = get_vint(bytes, &i)
		r.export_functions = append(r.export_functions, ef)
		exportfunctionsize--
	}

	exceptionexportsize := get_vint(bytes, &i)
	for exceptionexportsize > 0 {
		exc := exportexception{}
		exc.name = get_vint(bytes, &i)
		r.export_exceptions = append(r.export_exceptions, exc)
		exceptionexportsize--
	}

	interfaceexportsize := get_vint(bytes, &i)
	for interfaceexportsize > 0 {
		iface := exportinterface{}
		iface.name = get_vint(bytes, &i)
		methoddescriptorsize := get_vint(bytes, &i)
		for methoddescriptorsize > 0 {
			/*first :=*/ get_vint(bytes, &i)
			/*second :=*/ get_vint(bytes, &i)
			methoddescriptorsize--
		}
		r.export_interfaces = append(r.export_interfaces, iface)
		interfaceexportsize--
	}

	importsize := get_vint(bytes, &i)
	for importsize > 0 {
		imp := imported{}
		imp.name = get_vint(bytes, &i)
		imp.optional = get_vint(bytes, &i) != 0
		imp.hash = bytes[i : i+32]
		i += 32
		r.imports = append(r.imports, imp)
		importsize--
	}

	functionsize := get_vint(bytes, &i)
	for functionsize > 0 {
		f := function{}
		f.name = get_vint(bytes, &i)
		f.nest = get_vint(bytes, &i)
		f.params = get_vint(bytes, &i)
		f.locals = get_vint(bytes, &i)
		f.entry = get_vint(bytes, &i)
		r.functions = append(r.functions, f)
		functionsize--
	}

	exceptionsize := get_vint(bytes, &i)
	for exceptionsize > 0 {
		e := exception{}
		e.start = get_vint(bytes, &i)
		e.end = get_vint(bytes, &i)
		e.excid = get_vint(bytes, &i)
		e.handler = get_vint(bytes, &i)
		e.stack_depth = get_vint(bytes, &i)
		r.exceptions = append(r.exceptions, e)
		exceptionsize--
	}

	classsize := get_vint(bytes, &i)
	for classsize > 0 {
		c := classinfo{}
		c.name = get_vint(bytes, &i)
		interfacecount := get_vint(bytes, &i)
		for interfacecount > 0 {
			methods := []int{}
			methodcount := get_vint(bytes, &i)
			for methodcount > 0 {
				methods = append(methods, get_vint(bytes, &i))
				methodcount--
			}
			c.interfaces = append(c.interfaces, methods)
			interfacecount--
		}
		r.classes = append(r.classes, c)
		classsize--
	}

	r.code = bytes[i:]

	return r
}

type frame struct {
	nesting_depth int
	outer         *frame
	locals        []cell
	opstack_depth int
}

type module struct {
	name    string
	object  bytecode
	globals []cell
}

type returnaddress struct {
	module *module
	ip     int
}

type executor struct {
	modulefilename        string
	param_recursion_limit int
	predefined_globals    map[string]*cell
	modules               map[string]*module
	init_order            []*module
	stack                 []cell
	callstack             []returnaddress
	frames                []frame
	module                *module
	ip                    int
}

func make_executor(modulefilename string, bytes []byte) executor {
	r := executor{
		modulefilename:        modulefilename,
		param_recursion_limit: 1000,
		predefined_globals:    map[string]*cell{},
		modules:               map[string]*module{},
		stack:                 []cell{},
		callstack:             []returnaddress{},
		frames:                []frame{},
		module:                nil,
		ip:                    0,
	}

	args := []cell{}
	for i := 1; i < len(os.Args); i++ {
		args = append(args, make_cell_str(os.Args[i]))
	}
	a := make_cell_array(args)
	r.predefined_globals["sys$args"] = &a

	r.importModule("", make_bytecode(bytes))
	r.module = r.modules[""]
	return r
}

func (self *executor) importModule(name string, object bytecode) {
	_, found := self.modules[name]
	if found {
		return
	}
	m := new(module)
	*m = module{
		name:    name,
		object:  object,
		globals: make([]cell, object.global_size),
	}
	self.modules[name] = m
	for _, imp := range object.imports {
		name := string(object.strtable[imp.name])
		var bytes []byte
		var err error
		if strings.Contains(name, "/") {
			bytes, err = ioutil.ReadFile(name + ".neonx")
		} else {
			bytes, err = ioutil.ReadFile(path.Join("lib", name+".neonx"))
			if err != nil {
				bytes, err = ioutil.ReadFile(path.Join(path.Dir(self.modulefilename), name+".neonx"))
			}
		}
		if err != nil {
			panic(err)
		}
		self.importModule(name, make_bytecode(bytes))
	}
	if name != "" {
		self.init_order = append([]*module{m}, self.init_order...)
	}
}

func (self *executor) run() {
	self.ip = len(self.module.object.code)
	self.invoke(self.module, 0)
	for _, m := range self.init_order {
		self.invoke(m, 0)
	}
	for self.ip < len(self.module.object.code) {
		switch self.module.object.code[self.ip] {
		case PUSHB:
			self.op_pushb()
		case PUSHN:
			self.op_pushn()
		case PUSHS:
			self.op_pushs()
		case PUSHY:
			self.op_pushy()
		case PUSHPG:
			self.op_pushpg()
		case PUSHPPG:
			self.op_pushppg()
		case PUSHPMG:
			self.op_pushpmg()
		case PUSHPL:
			self.op_pushpl()
		case PUSHPOL:
			self.op_pushpol()
		case PUSHI:
			self.op_pushi()
		case LOADB:
			self.op_loadb()
		case LOADN:
			self.op_loadn()
		case LOADS:
			self.op_loads()
		case LOADY:
			self.op_loady()
		case LOADA:
			self.op_loada()
		case LOADD:
			self.op_loadd()
		case LOADP:
			self.op_loadp()
		case LOADJ:
			self.op_loadj()
		case LOADV:
			self.op_loadv()
		case STOREB:
			self.op_storeb()
		case STOREN:
			self.op_storen()
		case STORES:
			self.op_stores()
		case STOREY:
			self.op_storey()
		case STOREA:
			self.op_storea()
		case STORED:
			self.op_stored()
		case STOREP:
			self.op_storep()
		case STOREJ:
			self.op_storej()
		case STOREV:
			self.op_storev()
		case NEGN:
			self.op_negn()
		case ADDN:
			self.op_addn()
		case SUBN:
			self.op_subn()
		case MULN:
			self.op_muln()
		case DIVN:
			self.op_divn()
		case MODN:
			self.op_modn()
		case EXPN:
			self.op_expn()
		case EQB:
			self.op_eqb()
		case NEB:
			self.op_neb()
		case EQN:
			self.op_eqn()
		case NEN:
			self.op_nen()
		case LTN:
			self.op_ltn()
		case GTN:
			self.op_gtn()
		case LEN:
			self.op_len()
		case GEN:
			self.op_gen()
		case EQS:
			self.op_eqs()
		case NES:
			self.op_nes()
		case LTS:
			self.op_lts()
		case GTS:
			self.op_gts()
		case LES:
			self.op_les()
		case GES:
			self.op_ges()
		case EQY:
			self.op_eqy()
		case NEY:
			self.op_ney()
		case LTY:
			self.op_lty()
		case GTY:
			self.op_gty()
		case LEY:
			self.op_ley()
		case GEY:
			self.op_gey()
		case EQA:
			self.op_eqa()
		case NEA:
			self.op_nea()
		case EQD:
			self.op_eqd()
		case NED:
			self.op_ned()
		case EQP:
			self.op_eqp()
		case NEP:
			self.op_nep()
		case EQV:
			self.op_eqv()
		case NEV:
			self.op_nev()
		case ANDB:
			self.op_andb()
		case ORB:
			self.op_orb()
		case NOTB:
			self.op_notb()
		case INDEXAR:
			self.op_indexar()
		case INDEXAW:
			self.op_indexaw()
		case INDEXAV:
			self.op_indexav()
		case INDEXAN:
			self.op_indexan()
		case INDEXDR:
			self.op_indexdr()
		case INDEXDW:
			self.op_indexdw()
		case INDEXDV:
			self.op_indexdv()
		case INA:
			self.op_ina()
		case IND:
			self.op_ind()
		case CALLP:
			self.op_callp()
		case CALLF:
			self.op_callf()
		case CALLMF:
			self.op_callmf()
		case CALLI:
			self.op_calli()
		case JUMP:
			self.op_jump()
		case JF:
			self.op_jf()
		case JT:
			self.op_jt()
		case DUP:
			self.op_dup()
		case DUPX1:
			self.op_dupx1()
		case DROP:
			self.op_drop()
		case RET:
			self.op_ret()
		case CONSA:
			self.op_consa()
		case CONSD:
			self.op_consd()
		case EXCEPT:
			self.op_except()
		case ALLOC:
			self.op_alloc()
		case PUSHNIL:
			self.op_pushnil()
		case RESETC:
			self.op_resetc()
		case PUSHPEG:
			self.op_pushpeg()
		case JUMPTBL:
			self.op_jumptbl()
		case CALLX:
			self.op_callx()
		case SWAP:
			self.op_swap()
		case DROPN:
			self.op_dropn()
		case PUSHFP:
			self.op_pushfp()
		case CALLV:
			self.op_callv()
		case PUSHCI:
			self.op_pushci()
		default:
			panic(fmt.Sprintf("unknown opcode %d", self.module.object.code[self.ip]))
		}
	}
}

func (self *executor) pop() cell {
	r := self.stack[len(self.stack)-1]
	self.stack = self.stack[:len(self.stack)-1]
	return r
}

func (self *executor) push(c cell) {
	self.stack = append(self.stack, c)
}

func (self *executor) op_pushb() {
	self.ip++
	val := self.module.object.code[self.ip] != 0
	self.ip++
	self.push(make_cell_bool(val))
}

func (self *executor) op_pushn() {
	self.ip++
	val := get_vint(self.module.object.code, &self.ip)
	n, err := strconv.ParseFloat(string(self.module.object.strtable[val]), 64)
	if err != nil {
		panic("number")
	}
	self.push(make_cell_num(n))
}

func (self *executor) op_pushs() {
	self.ip++
	val := get_vint(self.module.object.code, &self.ip)
	self.push(make_cell_str(string(self.module.object.strtable[val])))
}

func (self *executor) op_pushy() {
	self.ip++
	val := get_vint(self.module.object.code, &self.ip)
	self.push(make_cell_bytes(self.module.object.strtable[val]))
}

func (self *executor) op_pushpg() {
	self.ip++
	addr := get_vint(self.module.object.code, &self.ip)
	self.push(make_cell_addr(&self.module.globals[addr]))
}

func (self *executor) op_pushppg() {
	self.ip++
	name := get_vint(self.module.object.code, &self.ip)
	self.push(make_cell_addr(self.predefined_globals[string(self.module.object.strtable[name])]))
}

func (self *executor) op_pushpmg() {
	self.ip++
	mod := get_vint(self.module.object.code, &self.ip)
	name := get_vint(self.module.object.code, &self.ip)
	for _, m := range self.modules {
		if m.name == string(self.module.object.strtable[mod]) {
			for _, v := range m.object.export_variables {
				if string(m.object.strtable[v.name]) == string(self.module.object.strtable[name]) {
					self.push(make_cell_addr(&m.globals[v.index]))
					return
				}
			}
			panic("variable not found: " + string(self.module.object.strtable[name]))
		}
	}
	panic("module not found: " + string(self.module.object.strtable[mod]))
}

func (self *executor) op_pushpl() {
	self.ip += 1
	addr := get_vint(self.module.object.code, &self.ip)
	self.push(make_cell_addr(&self.frames[len(self.frames)-1].locals[addr]))
}

func (self *executor) op_pushpol() {
	self.ip++
	back := get_vint(self.module.object.code, &self.ip)
	addr := get_vint(self.module.object.code, &self.ip)
	frame := &self.frames[len(self.frames)-1]
	for back > 0 {
		frame = frame.outer
		back--
	}
	self.push(make_cell_addr(&frame.locals[addr]))
}

func (self *executor) op_pushi() {
	self.ip++
	val := get_vint(self.module.object.code, &self.ip)
	self.push(make_cell_num(float64(val)))
}

func (self *executor) op_loadb() {
	self.ip++
	ref := self.pop().ref
	self.push(ref.load())
}

func (self *executor) op_loadn() {
	self.ip++
	ref := self.pop().ref
	self.push(ref.load())
}

func (self *executor) op_loads() {
	self.ip++
	ref := self.pop().ref
	self.push(ref.load())
}

func (self *executor) op_loady() {
	self.ip++
	ref := self.pop().ref
	self.push(ref.load())
}

func (self *executor) op_loada() {
	self.ip++
	ref := self.pop().ref
	self.push(ref.load())
}

func (self *executor) op_loadd() {
	self.ip++
	ref := self.pop().ref
	self.push(ref.load())
}

func (self *executor) op_loadp() {
	self.ip++
	ref := self.pop().ref
	self.push(ref.load())
}

func (self *executor) op_loadj() {
	self.ip++
	ref := self.pop().ref
	self.push(ref.load())
}

func (self *executor) op_loadv() {
	self.ip++
	ref := self.pop().ref
	self.push(ref.load())
}

func (self *executor) op_storeb() {
	self.ip++
	ref := self.pop().ref
	val := self.pop()
	ref.store(val)
}

func (self *executor) op_storen() {
	self.ip++
	ref := self.pop().ref
	val := self.pop()
	ref.store(val)
}

func (self *executor) op_stores() {
	self.ip++
	ref := self.pop().ref
	val := self.pop()
	ref.store(val)
}

func (self *executor) op_storey() {
	self.ip++
	ref := self.pop().ref
	val := self.pop()
	ref.store(val)
}

func (self *executor) op_storea() {
	self.ip++
	ref := self.pop().ref
	val := self.pop()
	ref.store(val)
}

func (self *executor) op_stored() {
	self.ip++
	ref := self.pop().ref
	val := self.pop()
	ref.store(val)
}

func (self *executor) op_storep() {
	self.ip++
	ref := self.pop().ref
	val := self.pop()
	ref.store(val)
}

func (self *executor) op_storej() {
	self.ip++
	ref := self.pop().ref
	val := self.pop()
	ref.store(val)
}

func (self *executor) op_storev() {
	self.ip++
	ref := self.pop().ref
	val := self.pop()
	ref.store(val)
}

func (self *executor) op_negn() {
	self.ip++
	x := self.pop().num
	self.push(make_cell_num(-x))
}

func (self *executor) op_addn() {
	self.ip++
	b := self.pop().num
	a := self.pop().num
	self.push(make_cell_num(a + b))
}

func (self *executor) op_subn() {
	self.ip++
	b := self.pop().num
	a := self.pop().num
	self.push(make_cell_num(a - b))
}

func (self *executor) op_muln() {
	self.ip++
	b := self.pop().num
	a := self.pop().num
	self.push(make_cell_num(a * b))
}

func (self *executor) op_divn() {
	self.ip++
	b := self.pop().num
	a := self.pop().num
	if b == 0 {
		self.raise_literal("NumberException.DivideByZero", objectString{""})
		return
	}
	self.push(make_cell_num(a / b))
}

func (self *executor) op_modn() {
	self.ip++
	b := self.pop().num
	a := self.pop().num
	self.push(make_cell_num(math.Mod(a, b)))
}

func (self *executor) op_expn() {
	self.ip++
	b := self.pop().num
	a := self.pop().num
	self.push(make_cell_num(math.Pow(a, b)))
}

func (self *executor) op_eqb() {
	self.ip++
	b := self.pop().bool
	a := self.pop().bool
	self.push(make_cell_bool(a == b))
}

func (self *executor) op_neb() {
	self.ip++
	b := self.pop().bool
	a := self.pop().bool
	self.push(make_cell_bool(a != b))
}

func (self *executor) op_eqn() {
	self.ip++
	b := self.pop().num
	a := self.pop().num
	self.push(make_cell_bool(a == b))
}

func (self *executor) op_nen() {
	self.ip++
	b := self.pop().num
	a := self.pop().num
	self.push(make_cell_bool(a != b))
}

func (self *executor) op_ltn() {
	self.ip++
	b := self.pop().num
	a := self.pop().num
	self.push(make_cell_bool(a < b))
}

func (self *executor) op_gtn() {
	self.ip++
	b := self.pop().num
	a := self.pop().num
	self.push(make_cell_bool(a > b))
}

func (self *executor) op_len() {
	self.ip++
	b := self.pop().num
	a := self.pop().num
	self.push(make_cell_bool(a <= b))
}

func (self *executor) op_gen() {
	self.ip++
	b := self.pop().num
	a := self.pop().num
	self.push(make_cell_bool(a >= b))
}

func (self *executor) op_eqs() {
	self.ip++
	b := self.pop().str
	a := self.pop().str
	self.push(make_cell_bool(a == b))
}

func (self *executor) op_nes() {
	self.ip++
	b := self.pop().str
	a := self.pop().str
	self.push(make_cell_bool(a != b))
}

func (self *executor) op_lts() {
	self.ip++
	b := self.pop().str
	a := self.pop().str
	self.push(make_cell_bool(a < b))
}

func (self *executor) op_gts() {
	self.ip++
	b := self.pop().str
	a := self.pop().str
	self.push(make_cell_bool(a > b))
}

func (self *executor) op_les() {
	self.ip++
	b := self.pop().str
	a := self.pop().str
	self.push(make_cell_bool(a <= b))
}

func (self *executor) op_ges() {
	self.ip++
	b := self.pop().str
	a := self.pop().str
	self.push(make_cell_bool(a >= b))
}

func (self *executor) op_eqy() {
	self.ip++
	b := self.pop().bytes
	a := self.pop().bytes
	r := false
	if len(a) == len(b) {
		r = true
		for i := range a {
			if a[i] != b[i] {
				r = false
				break
			}
		}
	}
	self.push(make_cell_bool(r))
}

func (self *executor) op_ney() {
	self.ip++
	b := self.pop().bytes
	a := self.pop().bytes
	r := true
	if len(a) == len(b) {
		r = false
		for i := range a {
			if a[i] != b[i] {
				r = true
				break
			}
		}
	}
	self.push(make_cell_bool(r))
}

func (self *executor) op_lty() {
	self.ip++
	b := self.pop().bytes
	a := self.pop().bytes
	r := len(a) < len(b)
	for i, _ := range a {
		if i >= len(b) {
			break
		}
		if a[i] < b[i] {
			r = true
			break
		} else if a[i] > b[i] {
			r = false
			break
		}
	}
	self.push(make_cell_bool(r))
}

func (self *executor) op_gty() {
	assert(false, "unimplemented gty")
}

func (self *executor) op_ley() {
	assert(false, "unimplemented ley")
}

func (self *executor) op_gey() {
	assert(false, "unimplemented gey")
}

func (self *executor) op_eqa() {
	self.ip++
	b := self.pop()
	a := self.pop()
	self.push(make_cell_bool(a.Equal(b)))
}

func (self *executor) op_nea() {
	self.ip++
	b := self.pop()
	a := self.pop()
	self.push(make_cell_bool(!a.Equal(b)))
}

func (self *executor) op_eqd() {
	self.ip++
	b := self.pop()
	a := self.pop()
	self.push(make_cell_bool(a.Equal(b)))
}

func (self *executor) op_ned() {
	assert(false, "unimplemented ned")
}

func (self *executor) op_eqp() {
	self.ip++
	b := self.pop().ref
	a := self.pop().ref
	self.push(make_cell_bool(a == b))
}

func (self *executor) op_nep() {
	self.ip++
	b := self.pop().ref
	a := self.pop().ref
	self.push(make_cell_bool(a != b))
}

func (self *executor) op_eqv() {
	assert(false, "unimplemented eqv")
}

func (self *executor) op_nev() {
	assert(false, "unimplemented nev")
}

func (self *executor) op_andb() {
	assert(false, "unimplemented andb")
}

func (self *executor) op_orb() {
	assert(false, "unimplemented orb")
}

func (self *executor) op_notb() {
	self.ip++
	b := self.pop().bool
	self.push(make_cell_bool(!b))
}

func (self *executor) op_indexar() {
	self.ip++
	nindex := self.pop().num
	ref := self.pop().ref
	a := ref.load().array
	if math.Trunc(nindex) != nindex {
		self.raise_literal("ArrayIndexException", objectString{fmt.Sprintf("%g", nindex)})
		return
	}
	if nindex < 0 {
		self.raise_literal("ArrayIndexException", objectString{fmt.Sprintf("%g", nindex)})
		return
	}
	index := int(nindex)
	if index >= len(a) {
		self.raise_literal("ArrayIndexException", objectString{fmt.Sprintf("%g", nindex)})
		return
	}
	self.push(make_cell_addr(&a[index]))
}

func (self *executor) op_indexaw() {
	self.ip++
	nindex := self.pop().num
	ref := self.pop().ref
	a := ref.load().array
	if math.Trunc(nindex) != nindex {
		self.raise_literal("ArrayIndexException", objectString{fmt.Sprintf("%g", nindex)})
		return
	}
	if nindex < 0 {
		self.raise_literal("ArrayIndexException", objectString{fmt.Sprintf("%g", nindex)})
		return
	}
	index := int(nindex)
	if index >= len(a) {
		a = append(a, make([]cell, index-len(a)+1)...)
		ref.store(make_cell_array(a))
	}
	self.push(make_cell_addr(&a[index]))
}

func (self *executor) op_indexav() {
	self.ip++
	nindex := self.pop().num
	a := self.pop().array
	if math.Trunc(nindex) != nindex {
		self.raise_literal("ArrayIndexException", objectString{fmt.Sprintf("%g", nindex)})
		return
	}
	if nindex < 0 {
		self.raise_literal("ArrayIndexException", objectString{fmt.Sprintf("%g", nindex)})
		return
	}
	index := int(nindex)
	self.push(a[index])
}

func (self *executor) op_indexan() {
	self.ip++
	index := int(self.pop().num)
	a := self.pop().array
	if index < len(a) {
		self.push(a[index])
	} else {
		self.push(cell{})
	}
}

func (self *executor) op_indexdr() {
	self.ip++
	key := self.pop().str
	ref := self.pop().ref
	if _, present := ref.load().dict[key]; !present {
		self.raise_literal("DictionaryIndexException", objectString{key})
		return
	}
	self.push(make_cell_ref(referenceDictionary{ref, key}))
}

func (self *executor) op_indexdw() {
	self.ip++
	key := self.pop().str
	ref := self.pop().ref
	self.push(make_cell_ref(referenceDictionary{ref, key}))
}

func (self *executor) op_indexdv() {
	self.ip++
	key := self.pop().str
	dict := self.pop().dict
	self.push(dict[key])
}

func (self *executor) op_ina() {
	self.ip++
	array := self.pop().array
	val := self.pop()
	for _, v := range array {
		if v.Equal(val) {
			self.push(make_cell_bool(true))
			return
		}
	}
	self.push(make_cell_bool(false))
}

func (self *executor) op_ind() {
	self.ip++
	dict := self.pop().dict
	key := self.pop().str
	_, found := dict[key]
	self.push(make_cell_bool(found))
}

func (self *executor) op_callp() {
	self.ip++
	val := get_vint(self.module.object.code, &self.ip)
	name := string(self.module.object.strtable[val])
	switch name {
	case "array__append":
		e := self.pop()
		r := self.pop().ref
		a := r.load()
		a.array = append(a.array, e)
		r.store(a)
	case "array__concat":
		b := self.pop().array
		a := self.pop().array
		self.push(make_cell_array(append(a, b...)))
	case "array__extend":
		b := self.pop().array
		r := self.pop().ref
		a := r.load()
		a.array = append(a.array, b...)
		r.store(a)
	case "array__find":
		e := self.pop()
		a := self.pop().array
		found := false
		for i, x := range a {
			if x.Equal(e) {
				self.push(make_cell_num(float64(i)))
				found = true
				break
			}
		}
		if !found {
			self.raise_literal("ArrayIndexException", objectString{"value not found in array"})
		}
	case "array__range":
		step := self.pop().num
		last := self.pop().num
		first := self.pop().num
		if step == 0 {
			self.raise_literal("ValueRangeException", objectString{fmt.Sprintf("%g", step)})
		} else {
			r := []cell{}
			if step < 0 {
				for i := first; i >= last; i += step {
					r = append(r, make_cell_num(i))
				}
			} else {
				for i := first; i <= last; i += step {
					r = append(r, make_cell_num(i))
				}
			}
			self.push(make_cell_array(r))
		}
	case "array__remove":
		index := self.pop().num
		if index != math.Trunc(index) || index < 0 {
			self.raise_literal("ArrayIndexException", objectString{fmt.Sprintf("%g", index)})
		} else {
			index := int(index)
			r := self.pop().ref
			a := r.load().array
			a = append(a[:index], a[index+1:]...)
			r.store(make_cell_array(a))
		}
	case "array__resize":
		size := self.pop().num
		if size != math.Trunc(size) || size < 0 {
			self.raise_literal("ArrayIndexException", objectString{fmt.Sprintf("%g", size)})
		} else {
			size := int(size)
			r := self.pop().ref
			a := r.load().array
			if size < len(a) {
				a = a[:size]
			} else {
				for len(a) < size {
					a = append(a, make_cell_none())
				}
			}
			r.store(make_cell_array(a))
		}
	case "array__reversed":
		a := self.pop().array
		r := make([]cell, len(a))
		for i, x := range a {
			r[len(a)-1-i] = x
		}
		self.push(make_cell_array(r))
	case "array__size":
		a := self.pop().array
		self.push(make_cell_num(float64(len(a))))
	case "array__slice":
		last_from_end := self.pop().bool
		nlast := self.pop().num
		first_from_end := self.pop().bool
		nfirst := self.pop().num
		a := self.pop().array
		if nfirst != math.Trunc(nfirst) {
			self.raise_literal("ArrayIndexException", objectString{fmt.Sprintf("%g", nfirst)})
		} else if nlast != math.Trunc(nlast) {
			self.raise_literal("ArrayIndexException", objectString{fmt.Sprintf("%g", nlast)})
		} else {
			first := int(nfirst)
			last := int(nlast)
			if first_from_end {
				first += len(a) - 1
			}
			if first < 0 {
				first = 0
			}
			if first > len(a) {
				first = len(a)
			}
			if last_from_end {
				last += len(a) - 1
			}
			if last < -1 {
				last = -1
			}
			if last >= len(a) {
				last = len(a) - 1
			}
			if last < first {
				last = first - 1
			}
			self.push(make_cell_array(a[first : last+1]))
		}
	case "array__splice":
		last_from_end := self.pop().bool
		last := int(self.pop().num)
		first_from_end := self.pop().bool
		first := int(self.pop().num)
		a := self.pop().array
		b := self.pop().array
		if first_from_end {
			first += len(a) - 1
		}
		if first < 0 {
			first = 0
		}
		if first > len(a) {
			first = len(a)
		}
		if last_from_end {
			last += len(a) - 1
		}
		if last < -1 {
			last = -1
		}
		if last >= len(a) {
			last = len(a) - 1
		}
		r := make([]cell, first)
		copy(r, a[:first])
		r = append(r, b...)
		r = append(r, a[last+1:]...)
		self.push(make_cell_array(r))
	case "array__toBytes__number":
		a := self.pop().array
		b := make([]byte, len(a))
		for i, x := range a {
			if x.num < 0 || x.num >= 256 {
				self.raise_literal("ByteOutOfRangeException", objectString{fmt.Sprintf("%g", x.num)})
			}
			b[i] = byte(x.num)
		}
		self.push(make_cell_bytes(b))
	case "array__toString__number":
		a := self.pop().array
		r := "["
		first := true
		for _, v := range a {
			if first {
				first = false
			} else {
				r += ", "
			}
			r += fmt.Sprintf("%v", v.num)
		}
		r += "]"
		self.push(make_cell_str(r))
	case "array__toString__object":
		a := self.pop().array
		r := "["
		for i, x := range a {
			if i > 0 {
				r += ", "
			}
			r += x.obj.toLiteralString()
		}
		r += "]"
		self.push(make_cell_str(r))
	case "array__toString__string":
		a := self.pop().array
		r := "["
		for i, x := range a {
			if i > 0 {
				r += ", "
			}
			r += quoted(x.str)
		}
		r += "]"
		self.push(make_cell_str(r))
	case "binary$and32":
		b := uint32(self.pop().num)
		a := uint32(self.pop().num)
		self.push(make_cell_num(float64(a & b)))
	case "binary$and64":
		b := uint64(self.pop().num)
		a := uint64(self.pop().num)
		self.push(make_cell_num(float64(a & b)))
	case "binary$andBytes":
		b := self.pop().bytes
		a := self.pop().bytes
		r := make([]byte, len(a))
		for i := range a {
			r[i] = a[i] & b[i]
		}
		self.push(make_cell_bytes(r))
	case "binary$extract32":
		w := uint32(self.pop().num)
		n := uint32(self.pop().num)
		x := uint32(self.pop().num)
		self.push(make_cell_num(float64((x >> n) & ((1 << w) - 1))))
	case "binary$get32":
		n := uint32(self.pop().num)
		x := uint32(self.pop().num)
		self.push(make_cell_bool((x & (1 << n)) != 0))
	case "binary$not32":
		x := uint32(self.pop().num)
		self.push(make_cell_num(float64(^x)))
	case "binary$not64":
		x := uint64(self.pop().num)
		self.push(make_cell_num(float64(^x)))
	case "binary$notBytes":
		a := self.pop().bytes
		r := make([]byte, len(a))
		for i := range a {
			r[i] = ^a[i]
		}
		self.push(make_cell_bytes(r))
	case "binary$or32":
		b := uint32(self.pop().num)
		a := uint32(self.pop().num)
		self.push(make_cell_num(float64(a | b)))
	case "binary$orBytes":
		b := self.pop().bytes
		a := self.pop().bytes
		r := make([]byte, len(a))
		for i := range a {
			r[i] = a[i] | b[i]
		}
		self.push(make_cell_bytes(r))
	case "binary$replace32":
		y := uint32(self.pop().num)
		w := uint32(self.pop().num)
		n := uint32(self.pop().num)
		x := uint32(self.pop().num)
		self.push(make_cell_num(float64((x & ^(((1 << w) - 1) << n)) | (y << n))))
	case "binary$set32":
		v := self.pop().bool
		n := uint32(self.pop().num)
		x := uint32(self.pop().num)
		if v {
			self.push(make_cell_num(float64(x | (1 << n))))
		} else {
			self.push(make_cell_num(float64(x & ^(1 << n))))
		}
	case "binary$shiftLeft32":
		n := uint32(self.pop().num)
		x := uint32(self.pop().num)
		self.push(make_cell_num(float64(x << n)))
	case "binary$shiftRight32":
		n := uint32(self.pop().num)
		x := uint32(self.pop().num)
		self.push(make_cell_num(float64(x >> n)))
	case "binary$shiftRightSigned32":
		n := uint32(self.pop().num)
		x := int32(self.pop().num)
		self.push(make_cell_num(float64(x >> n)))
	case "binary$shiftRight64":
		n := uint32(self.pop().num)
		x := uint64(self.pop().num)
		self.push(make_cell_num(float64(x >> n)))
	case "binary$xor32":
		b := uint32(self.pop().num)
		a := uint32(self.pop().num)
		self.push(make_cell_num(float64(a ^ b)))
	case "binary$xorBytes":
		b := self.pop().bytes
		a := self.pop().bytes
		r := make([]byte, len(a))
		for i := range a {
			r[i] = a[i] ^ b[i]
		}
		self.push(make_cell_bytes(r))
	case "bytes__concat":
		b := self.pop().bytes
		a := self.pop().bytes
		self.push(make_cell_bytes(append(a, b...)))
	case "bytes__decodeToString":
		b := self.pop().bytes
		self.push(make_cell_str(string(b)))
	case "bytes__index":
		nindex := self.pop().num
		b := self.pop().bytes
		if nindex != math.Trunc(nindex) {
			self.raise_literal("BytesIndexException", objectString{fmt.Sprintf("%g", nindex)})
		} else {
			index := int(nindex)
			if index < 0 || index >= len(b) {
				self.raise_literal("BytesIndexException", objectString{fmt.Sprintf("%g", index)})
			} else {
				self.push(make_cell_num(float64(b[index])))
			}
		}
	case "bytes__range":
		last_from_end := self.pop().bool
		nlast := self.pop().num
		first_from_end := self.pop().bool
		nfirst := self.pop().num
		b := self.pop().bytes
		if nfirst != math.Trunc(nfirst) {
			self.raise_literal("BytesIndexException", objectString{fmt.Sprintf("%g", nfirst)})
		} else if nlast != math.Trunc(nlast) {
			self.raise_literal("BytesIndexException", objectString{fmt.Sprintf("%g", nlast)})
		} else {
			first := int(nfirst)
			last := int(nlast)
			if first_from_end {
				first += len(b) - 1
			}
			if last_from_end {
				last += len(b) - 1
			}
			if first < 0 {
				first = 0
			} else if first > len(b) {
				first = len(b)
			}
			if last >= len(b) {
				last = len(b) - 1
			} else if last < 0 {
				last = -1
			}
			if last < first {
				self.push(make_cell_bytes([]byte{}))
			} else {
				self.push(make_cell_bytes(b[first : last+1]))
			}
		}
	case "bytes__size":
		b := self.pop().bytes
		self.push(make_cell_num(float64(len(b))))
	case "bytes__splice":
		last_from_end := self.pop().bool
		last := int(self.pop().num)
		first_from_end := self.pop().bool
		first := int(self.pop().num)
		b := self.pop().bytes
		t := self.pop().bytes
		if first_from_end {
			first += len(b) - 1
		}
		if last_from_end {
			last += len(b) - 1
		}
		self.push(make_cell_bytes(append(b[:first], append(t, b[last+1:]...)...)))
	case "bytes__toArray":
		b := self.pop().bytes
		a := make([]cell, len(b))
		for i, x := range b {
			a[i] = make_cell_num(float64(x))
		}
		self.push(make_cell_array(a))
	case "bytes__toString":
		b := self.pop().bytes
		s := "HEXBYTES \""
		for i, x := range b {
			if i > 0 {
				s += " "
			}
			s += fmt.Sprintf("%02x", x)
		}
		s += "\""
		self.push(make_cell_str(s))
	case "console$input":
		prompt := self.pop().str
		reader := bufio.NewReader(os.Stdin)
		fmt.Print(prompt)
		r, err := reader.ReadString('\n')
		if err == nil {
			r = strings.TrimRight(r, "\n")
			self.push(make_cell_str(r))
		} else {
			self.raise_literal("EndOfFileException", objectString{err.Error()})
		}
	case "dictionary__keys":
		d := self.pop().dict
		i := 0
		keys := make([]string, len(d))
		for k := range d {
			keys[i] = k
			i++
		}
		sort.Strings(keys)
		ckeys := make([]cell, len(d))
		for i, k := range keys {
			ckeys[i] = make_cell_str(k)
		}
		self.push(make_cell_array(ckeys))
	case "dictionary__remove":
		key := self.pop().str
		r := self.pop().ref
		d := r.load().dict
		delete(d, key)
	case "dictionary__toString__object":
		d := self.pop().dict
		r := "{"
		keys := []string{}
		for k := range d {
			keys = append(keys, k)
		}
		sort.Strings(keys)
		for i, k := range keys {
			if i > 0 {
				r += ", "
			}
			r += quoted(k)
			r += ": "
			r += d[k].obj.toLiteralString()
		}
		r += "}"
		self.push(make_cell_str(r))
	case "dictionary__toString__string":
		d := self.pop().dict
		r := "{"
		keys := []string{}
		for k := range d {
			keys = append(keys, k)
		}
		sort.Strings(keys)
		for i, k := range keys {
			if i > 0 {
				r += ", "
			}
			r += quoted(k)
			r += ": "
			r += quoted(d[k].str)
		}
		r += "}"
		self.push(make_cell_str(r))
	case "exceptiontype__toString":
		et := self.pop().array
		self.push(make_cell_str(fmt.Sprintf("<ExceptionType:%s,%s,%v>", et[0].str, et[1].obj.toString(), et[2].num)))
	case "file$_CONSTANT_Separator":
		self.push(make_cell_str(string(os.PathSeparator)))
	case "file$copy":
		dstname := self.pop().str
		srcname := self.pop().str
		dst, err := os.OpenFile(dstname, os.O_CREATE|os.O_EXCL, 0644)
		if err != nil && os.IsExist(err) {
			self.push(make_cell_array([]cell{make_cell_num(1), make_cell_str(err.Error())})) // error
		} else {
			if err != nil {
				panic(err)
			}
			defer dst.Close()
			src, err := os.Open(srcname)
			if err != nil {
				panic(err)
			}
			defer src.Close()
			io.Copy(src, dst)
			self.push(make_cell_array([]cell{make_cell_num(0)})) // ok
		}
	case "file$copyOverwriteIfExists":
		dstname := self.pop().str
		srcname := self.pop().str
		neon_file_copyOverwriteIfExists(srcname, dstname)
		self.push(make_cell_array([]cell{make_cell_num(0)})) // ok
	case "file$delete":
		name := self.pop().str
		err := os.Remove(name)
		if err != nil && !os.IsNotExist(err) {
			panic(err)
		}
		self.push(make_cell_array([]cell{make_cell_num(0)})) // ok
	case "file$exists":
		name := self.pop().str
		_, err := os.Stat(name)
		if err != nil && !os.IsNotExist(err) {
			panic(err)
		}
		self.push(make_cell_bool(err == nil))
	case "file$files":
		dir := self.pop().str
		fileinfos, err := ioutil.ReadDir(dir)
		if err != nil {
			panic(err)
		}
		a := []cell{}
		for _, fi := range fileinfos {
			a = append(a, make_cell_str(fi.Name()))
		}
		self.push(make_cell_array(a))
	case "file$getInfo":
		name := self.pop().str
		fileinfo, err := os.Stat(name)
		if err != nil {
			panic(err)
		}
		a := make([]cell, 9)
		a[0] = make_cell_str(filepath.Base(name))
		a[1] = make_cell_num(float64(fileinfo.Size()))
		a[2] = make_cell_bool((fileinfo.Mode() & 0x04) != 0)
		a[3] = make_cell_bool((fileinfo.Mode() & 0x02) != 0)
		a[4] = make_cell_bool((fileinfo.Mode() & 0x01) != 0)
		if (fileinfo.Mode() & os.ModeDir) != 0 {
			a[5] = make_cell_num(1) // FileType.directory
		} else /*if os.IsRegular(fileinfo.Mode())*/ {
			a[5] = make_cell_num(0) // FileType.normal
		} /*else {
		    a[5] = make_cell_num(2) // FileType.special
		}*/
		a[6] = make_cell_num(0)
		a[7] = make_cell_num(0)
		a[8] = make_cell_num(float64(fileinfo.ModTime().Unix()))
		self.push(make_cell_array([]cell{make_cell_num(0), make_cell_array(a)})) // info
	case "file$isDirectory":
		name := self.pop().str
		fileinfo, err := os.Stat(name)
		if err == nil && (fileinfo.Mode()&os.ModeDir) != 0 {
			self.push(make_cell_bool(true))
		} else {
			self.push(make_cell_bool(false))
		}
	case "file$mkdir":
		name := self.pop().str
		err := os.Mkdir(name, 0755)
		if err != nil && os.IsExist(err) {
			self.push(make_cell_array([]cell{make_cell_num(1), make_cell_str(name)})) // error
		} else if err != nil {
			panic(err)
		} else {
			self.push(make_cell_array([]cell{make_cell_num(0)})) // ok
		}
	case "file$readBytes":
		name := self.pop().str
		b, err := ioutil.ReadFile(name)
		if err != nil {
			panic(err)
		}
		self.push(make_cell_array([]cell{make_cell_num(0), make_cell_bytes(b)})) // data
	case "file$readLines":
		name := self.pop().str
		f, err := os.Open(name)
		if err != nil {
			panic(err)
		}
		defer f.Close()
		reader := bufio.NewReader(f)
		a := []cell{}
		for {
			s, _ := reader.ReadString('\n')
			if len(s) == 0 {
				break
			}
			a = append(a, make_cell_str(s[:len(s)-1]))
		}
		self.push(make_cell_array([]cell{make_cell_num(0), make_cell_array(a)})) // lines
	case "file$removeEmptyDirectory":
		name := self.pop().str
		err := os.Remove(name)
		if err != nil {
			self.push(make_cell_array([]cell{make_cell_num(1), make_cell_str(err.Error())})) // error
		} else {
			self.push(make_cell_array([]cell{make_cell_num(0)})) // ok
		}
	case "file$rename":
		newname := self.pop().str
		oldname := self.pop().str
		err := os.Rename(oldname, newname)
		if err != nil {
			self.push(make_cell_array([]cell{make_cell_num(1), make_cell_str(err.Error())})) // error
		} else {
			self.push(make_cell_array([]cell{make_cell_num(0)})) // ok
		}
	case "file$writeBytes":
		b := self.pop().bytes
		name := self.pop().str
		ioutil.WriteFile(name, b, 0644)
		self.push(make_cell_array([]cell{make_cell_num(0)})) // ok
	case "file$writeLines":
		a := self.pop().array
		name := self.pop().str
		f, err := os.Create(name)
		if err != nil {
			panic(err)
		}
		defer f.Close()
		for _, s := range a {
			f.WriteString(s.str + "\n")
		}
		self.push(make_cell_array([]cell{make_cell_num(0)})) // ok
	case "math$abs":
		x := self.pop().num
		self.push(make_cell_num(math.Abs(x)))
	case "math$acos":
		x := self.pop().num
		self.push(make_cell_num(math.Acos(x)))
	case "math$acosh":
		x := self.pop().num
		self.push(make_cell_num(math.Acosh(x)))
	case "math$asin":
		x := self.pop().num
		self.push(make_cell_num(math.Asin(x)))
	case "math$asinh":
		x := self.pop().num
		self.push(make_cell_num(math.Asinh(x)))
	case "math$atan":
		x := self.pop().num
		self.push(make_cell_num(math.Atan(x)))
	case "math$atanh":
		x := self.pop().num
		self.push(make_cell_num(math.Atanh(x)))
	case "math$atan2":
		x := self.pop().num
		y := self.pop().num
		self.push(make_cell_num(math.Atan2(y, x)))
	case "math$cbrt":
		x := self.pop().num
		self.push(make_cell_num(math.Cbrt(x)))
	case "math$ceil":
		x := self.pop().num
		self.push(make_cell_num(math.Ceil(x)))
	case "math$cos":
		x := self.pop().num
		self.push(make_cell_num(math.Cos(x)))
	case "math$cosh":
		x := self.pop().num
		self.push(make_cell_num(math.Cosh(x)))
	case "math$erf":
		x := self.pop().num
		self.push(make_cell_num(math.Erf(x)))
	case "math$erfc":
		x := self.pop().num
		self.push(make_cell_num(math.Erfc(x)))
	case "math$exp":
		x := self.pop().num
		self.push(make_cell_num(math.Exp(x)))
	case "math$exp2":
		x := self.pop().num
		self.push(make_cell_num(math.Exp2(x)))
	case "math$expm1":
		x := self.pop().num
		self.push(make_cell_num(math.Expm1(x)))
	case "math$floor":
		x := self.pop().num
		self.push(make_cell_num(math.Floor(x)))
	case "math$frexp":
		x := self.pop().num
		frac, exp := math.Frexp(x)
		self.push(make_cell_num(frac))
		self.push(make_cell_num(float64(exp)))
	case "math$hypot":
		y := self.pop().num
		x := self.pop().num
		self.push(make_cell_num(math.Hypot(x, y)))
	case "math$intdiv":
		y := self.pop().num
		x := self.pop().num
		self.push(make_cell_num(math.Trunc(x / y)))
	case "math$ldexp":
		exp := self.pop().num
		x := self.pop().num
		self.push(make_cell_num(math.Ldexp(x, int(exp))))
	case "math$lgamma":
		x := self.pop().num
		r, _ := math.Lgamma(x)
		self.push(make_cell_num(r))
	case "math$log":
		x := self.pop().num
		self.push(make_cell_num(math.Log(x)))
	case "math$log10":
		x := self.pop().num
		self.push(make_cell_num(math.Log10(x)))
	case "math$log1p":
		x := self.pop().num
		self.push(make_cell_num(math.Log1p(x)))
	case "math$log2":
		x := self.pop().num
		self.push(make_cell_num(math.Log2(x)))
	case "math$max":
		y := self.pop().num
		x := self.pop().num
		self.push(make_cell_num(math.Max(x, y)))
	case "math$min":
		y := self.pop().num
		x := self.pop().num
		self.push(make_cell_num(math.Min(x, y)))
	case "math$nearbyint":
		x := self.pop().num
		// TODO self.push(make_cell_num(math.Round(x)))
		self.push(make_cell_num(x))
	case "math$odd":
		n := self.pop().num
		if n != math.Trunc(n) {
			self.raise_literal("ValueRangeException", objectString{"odd() requires integer"})
		} else {
			self.push(make_cell_bool((int(n) & 1) != 0))
		}
	case "math$round":
		x := self.pop().num
		n := int(self.pop().num)
		p := math.Pow10(n)
		self.push(make_cell_num(math.Trunc(x*p+0.5) / p))
	case "math$sign":
		x := self.pop().num
		self.push(make_cell_num(math.Copysign(1, x)))
	case "math$sin":
		x := self.pop().num
		self.push(make_cell_num(math.Sin(x)))
	case "math$sinh":
		x := self.pop().num
		self.push(make_cell_num(math.Sinh(x)))
	case "math$sqrt":
		x := self.pop().num
		self.push(make_cell_num(math.Sqrt(x)))
	case "math$tan":
		x := self.pop().num
		self.push(make_cell_num(math.Tan(x)))
	case "math$tanh":
		x := self.pop().num
		self.push(make_cell_num(math.Tanh(x)))
	case "math$tgamma":
		x := self.pop().num
		self.push(make_cell_num(math.Gamma(x)))
	case "math$trunc":
		x := self.pop().num
		self.push(make_cell_num(math.Trunc(x)))
	case "num":
		s := self.pop().str
		n, err := strconv.ParseFloat(s, 64)
		if err == nil {
			self.push(make_cell_num(n))
		} else {
			self.raise_literal("ValueRangeException", objectString{"num() argument not a number"})
		}
	case "object__invokeMethod":
		args := self.pop().array
		name := self.pop().str
		o := self.pop().obj
		if s, err := o.getString(); err == nil {
			if name == "length" {
				if len(args) == 0 {
					self.push(make_cell_obj(objectNumber{float64(len(s))}))
				} else {
					self.raise_literal("DynamicConversionException", objectString{"invalid number of arguments to length() (expected 0)"})
				}
			} else {
				self.raise_literal("DynamicConversionException", objectString{"string object does not support this method"})
			}
		} else if a, err := o.getArray(); err == nil {
			if name == "size" {
				if len(args) == 0 {
					self.push(make_cell_obj(objectNumber{float64(len(a))}))
				} else {
					self.raise_literal("DynamicConversionException", objectString{"invalid number of arguments to size() (expected 0)"})
				}
			} else {
				self.raise_literal("DynamicConversionException", objectString{"array object does not support this method"})
			}
		} else if d, err := o.getDictionary(); err == nil {
			if name == "keys" {
				if len(args) == 0 {
					i := 0
					keys := make([]string, len(d))
					for k := range d {
						keys[i] = k
						i++
					}
					sort.Strings(keys)
					okeys := make([]object, len(d))
					for i, k := range keys {
						okeys[i] = objectString{k}
					}
					self.push(make_cell_obj(objectArray{okeys}))
				} else {
					self.raise_literal("DynamicConversionException", objectString{"invalid number of arguments to keys() (expected 0)"})
				}
			} else if name == "size" {
				if len(args) == 0 {
					self.push(make_cell_obj(objectNumber{float64(len(d))}))
				} else {
					self.raise_literal("DynamicConversionException", objectString{"invalid number of arguments to size() (expected 0)"})
				}
			} else {
				self.raise_literal("DynamicConversionException", objectString{"dictionary object does not support this method"})
			}
		} else {
			self.raise_literal("DynamicConversionException", objectString{"object does not support method calls"})
		}
	case "object__isNull":
		o := self.pop().obj
		if o == nil {
			self.push(make_cell_bool(true))
		} else {
			self.push(make_cell_bool(false))
		}
	case "object__getBoolean":
		o := self.pop().obj
		if b, err := o.getBoolean(); err == nil {
			self.push(make_cell_bool(b))
		} else {
			self.raise_literal("DynamicConversionException", objectString{"to Boolean"})
		}
	case "object__getNumber":
		o := self.pop().obj
		if n, err := o.getNumber(); err == nil {
			self.push(make_cell_num(n))
		} else {
			self.raise_literal("DynamicConversionException", objectString{"to Number"})
		}
	case "object__getString":
		o := self.pop().obj
		if s, err := o.getString(); err == nil {
			self.push(make_cell_str(s))
		} else {
			self.raise_literal("DynamicConversionException", objectString{"to String"})
		}
	case "object__getBytes":
		o := self.pop().obj
		if s, err := o.getBytes(); err == nil {
			self.push(make_cell_bytes(s))
		} else {
			self.raise_literal("DynamicConversionException", objectString{"to Bytes"})
		}
	case "object__getArray":
		o := self.pop().obj
		if a, err := o.getArray(); err == nil {
			b := make([]cell, len(a))
			for i, x := range a {
				b[i] = make_cell_obj(x)
			}
			self.push(make_cell_array(b))
		} else {
			self.raise_literal("DynamicConversionException", objectString{"to Array"})
		}
	case "object__getDictionary":
		o := self.pop().obj
		if d, err := o.getDictionary(); err == nil {
			b := make(map[string]cell)
			for k, x := range d {
				b[k] = make_cell_obj(x)
			}
			self.push(make_cell_dict(b))
		} else {
			self.raise_literal("DynamicConversionException", objectString{"to Dictionary"})
		}
	case "object__makeNull":
		self.push(make_cell_obj(nil))
	case "object__makeBoolean":
		b := self.pop().bool
		self.push(make_cell_obj(objectBoolean{b}))
	case "object__makeNumber":
		n := self.pop().num
		self.push(make_cell_obj(objectNumber{n}))
	case "object__makeString":
		s := self.pop().str
		self.push(make_cell_obj(objectString{s}))
	case "object__makeBytes":
		b := self.pop().bytes
		self.push(make_cell_obj(objectBytes{b}))
	case "object__makeArray":
		a := self.pop().array
		b := make([]object, len(a))
		for i, x := range a {
			b[i] = x.obj
		}
		self.push(make_cell_obj(objectArray{b}))
	case "object__makeDictionary":
		d := self.pop().dict
		b := make(map[string]object)
		for k, x := range d {
			b[k] = x.obj
		}
		self.push(make_cell_obj(objectDictionary{b}))
	case "object__subscript":
		i := self.pop().obj
		o := self.pop().obj
		if o == nil {
			self.raise_literal("DynamicConversionException", objectString{"object is null"})
		} else if i == nil {
			self.raise_literal("DynamicConversionException", objectString{"index is null"})
		} else if x, err := o.subscript(i); err == nil {
			self.push(make_cell_obj(x))
		} else {
			if ne, ok := err.(*neonexception); ok {
				self.raise_literal(ne.name, ne.info)
			} else {
				self.raise_literal("ObjectSubscriptException", objectString{i.toString()})
			}
		}
	case "object__toString":
		o := self.pop().obj
		if o != nil {
			self.push(make_cell_str(o.toString()))
		} else {
			self.push(make_cell_str("null"))
		}
	case "os$platform":
		self.push(make_cell_str(runtime.GOOS))
	case "os$spawn":
		cmdline := self.pop().str
		cmd := exec.Command("/bin/sh", "-c", cmdline)
		err := cmd.Start()
		if err != nil {
			self.raise_literal("OsException.Spawn", objectString{""})
		} else {
			self.push(make_cell_other(cmd))
		}
	case "os$system":
		cmdline := self.pop().str
		cmd := exec.Command("/bin/sh", "-c", cmdline)
		cmd.Stdin = os.Stdin
		cmd.Stdout = os.Stdout
		cmd.Stderr = os.Stderr
		err := cmd.Run()
		if err != nil {
			// TODO: ExitCode() is only available in Go >=1.12
			self.push(make_cell_num(1 /*cmd.ProcessState.ExitCode()*/))
		} else {
			self.push(make_cell_num(0))
		}
	case "os$wait":
		cmd := self.pop().ref.load().other.(*exec.Cmd)
		err := cmd.Wait()
		if err != nil {
			// TODO: ExitCode() is only available in Go >=1.12
			self.push(make_cell_num(1 /*cmd.ProcessState.ExitCode()*/))
		} else {
			self.push(make_cell_num(0))
		}
	case "pointer__toString":
		p := self.pop().ref.(referenceDirect).addr
		self.push(make_cell_str(fmt.Sprintf("<p:%p>", p)))
	case "print":
		x := self.pop()
		if x.obj != nil {
			fmt.Println(x.obj.toString())
		} else {
			fmt.Println("NIL")
		}
	case "random$uint32":
		self.push(make_cell_num(float64(rand.Uint32())))
	case "runtime$assertionsEnabled":
		self.push(make_cell_bool(true)) // TODO: enable_assertions
	case "runtime$executorName":
		self.push(make_cell_str("gonex"))
	case "runtime$isModuleImported":
		name := self.pop().str
		_, found := self.modules[name]
		self.push(make_cell_bool(found))
	case "runtime$moduleIsMain":
		self.push(make_cell_bool(self.module == self.modules[""]))
	case "runtime$setRecursionLimit":
		self.param_recursion_limit = int(self.pop().num)
	case "str", "number__toString":
		n := self.pop().num
		self.push(make_cell_str(fmt.Sprintf("%v", n)))
	case "boolean__toString":
		b := self.pop().bool
		if b {
			self.push(make_cell_str("TRUE"))
		} else {
			self.push(make_cell_str("FALSE"))
		}
	case "string$find":
		t := self.pop().str
		s := self.pop().str
		r := strings.Index(s, t)
		if r < 0 {
			self.push(make_cell_array([]cell{make_cell_num(0)})) // notfound
		} else {
			self.push(make_cell_array([]cell{make_cell_num(1), make_cell_num(float64(r))})) // index
		}
	case "string$fromCodePoint":
		c := self.pop().num
		if c != math.Trunc(c) {
			self.raise_literal("ValueRangeException", objectString{"fromCodePoint() argument not an integer"})
		} else if c < 0 || c > 0x10ffff {
			self.raise_literal("ValueRangeException", objectString{"fromCodePoint() argument out of range 0-0x10ffff"})
		} else {
			self.push(make_cell_str(string([]byte{byte(c)})))
		}
	case "string$lower":
		s := self.pop().str
		self.push(make_cell_str(strings.ToLower(s)))
	case "string$quoted":
		s := self.pop().str
		self.push(make_cell_str(quoted(s)))
	case "string$split":
		d := self.pop().str
		s := self.pop().str
		a := strings.Split(s, d)
		r := []cell{}
		for _, x := range a {
			r = append(r, make_cell_str(x))
		}
		self.push(make_cell_array(r))
	case "string$splitLines":
		s := self.pop().str
		r := []cell{}
		i := 0
		for i < len(s) {
			nl := i
			for nl < len(s) && s[nl] != '\r' && s[nl] != '\n' {
				nl++
			}
			r = append(r, make_cell_str(s[i:nl]))
			if nl == len(s) {
				break
			}
			if s[nl] == '\r' && nl+1 < len(s) && s[nl+1] == '\n' {
				i = nl + 2
			} else {
				i = nl + 1
			}
		}
		self.push(make_cell_array(r))
	case "string$toCodePoint":
		s := self.pop().str
		if len(s) != 1 {
			self.raise_literal("StringIndexException", objectString{"toCodePoint() requires string of length 1"})
		} else {
			self.push(make_cell_num(float64(s[0])))
		}
	case "string$trimCharacters":
		trailing := self.pop().str
		leading := self.pop().str
		s := self.pop().str
		self.push(make_cell_str(strings.TrimRight(strings.TrimLeft(s, leading), trailing)))
	case "string$upper":
		s := self.pop().str
		self.push(make_cell_str(strings.ToUpper(s)))
	case "string__append":
		t := self.pop().str
		r := self.pop().ref
		s := r.load()
		s.str = s.str + t
		r.store(s)
	case "string__concat":
		b := self.pop().str
		a := self.pop().str
		self.push(make_cell_str(a + b))
	case "string__index":
		nindex := self.pop().num
		s := self.pop().str
		if nindex != math.Trunc(nindex) {
			self.raise_literal("StringIndexException", objectString{fmt.Sprintf("%v", nindex)})
		} else {
			index := int(nindex)
			if index < 0 {
				self.raise_literal("StringIndexException", objectString{fmt.Sprintf("%v", index)})
			} else if index >= len(s) {
				self.raise_literal("StringIndexException", objectString{fmt.Sprintf("%v", index)})
			} else {
				self.push(make_cell_str(s[index : index+1]))
			}
		}
	case "string__length":
		s := self.pop().str
		self.push(make_cell_num(float64(len(s))))
	case "string__splice":
		last_from_end := self.pop().bool
		last := int(self.pop().num)
		first_from_end := self.pop().bool
		first := int(self.pop().num)
		s := self.pop().str
		t := self.pop().str
		if first_from_end {
			first += len(s) - 1
		}
		if last_from_end {
			last += len(s) - 1
		}
		if first < 0 {
			self.raise_literal("StringIndexException", objectString{fmt.Sprintf("%g", first)})
		} else if last < first-1 {
			self.raise_literal("StringIndexException", objectString{fmt.Sprintf("%g", last)})
		} else {
			padding := ""
			if first > len(s) {
				padding = strings.Repeat(" ", first-len(s))
				first = len(s)
			}
			if last >= len(s) {
				last = len(s) - 1
			}
			self.push(make_cell_str(s[:first] + padding + t + s[last+1:]))
		}
	case "string__substring":
		last_from_end := self.pop().bool
		nlast := self.pop().num
		first_from_end := self.pop().bool
		nfirst := self.pop().num
		s := self.pop().str
		if nfirst != math.Trunc(nfirst) {
			self.raise_literal("StringIndexException", objectString{fmt.Sprintf("%g", nfirst)})
		} else if nlast != math.Trunc(nlast) {
			self.raise_literal("StringIndexException", objectString{fmt.Sprintf("%g", nlast)})
		} else {
			first := int(nfirst)
			last := int(nlast)
			if first_from_end {
				first += len(s) - 1
			}
			if last_from_end {
				last += len(s) - 1
			}
			if first < 0 {
				first = 0
			} else if first > len(s) {
				first = len(s)
			}
			if last >= len(s) {
				last = len(s) - 1
			} else if last < 0 {
				last = -1
			}
			if last < first {
				self.push(make_cell_bytes([]byte{}))
			} else {
				self.push(make_cell_str(s[first : last+1]))
			}
		}
	case "string__toBytes":
		s := self.pop().str
		self.push(make_cell_bytes([]byte(s)))
	case "string__toString":
		s := self.pop().str
		self.push(make_cell_str(s))
	case "sys$exit":
		n := self.pop().num
		if n != math.Trunc(n) {
			self.raise_literal("InvalidValueException", objectString{fmt.Sprintf("sys.exit invalid parameter: %g", n)})
		} else if n < 0 || n > 255 {
			self.raise_literal("InvalidValueException", objectString{fmt.Sprintf("sys.exit invalid parameter: %g", n)})
		} else {
			os.Exit(int(n))
		}
	case "time$now":
		t := time.Now().UnixNano()
		self.push(make_cell_num(float64(t / 1.0e9)))
	case "time$sleep":
		t := self.pop().num
		time.Sleep(time.Duration(t * 1e9))
	default:
		assert(false, fmt.Sprintf("unimplemented function: %s", name))
	}
}

func (self *executor) op_callf() {
	self.ip++
	val := get_vint(self.module.object.code, &self.ip)
	if len(self.callstack) >= self.param_recursion_limit {
		self.raise_literal("StackOverflowException", objectString{""})
		return
	}
	self.invoke(self.module, val)
}

func (self *executor) op_callmf() {
	self.ip++
	mod := get_vint(self.module.object.code, &self.ip)
	fun := get_vint(self.module.object.code, &self.ip)
	if len(self.callstack) >= self.param_recursion_limit {
		self.raise_literal("StackOverflowException", objectString{""})
		return
	}
	m, found := self.modules[string(self.module.object.strtable[mod])]
	if !found {
		panic("module not found: " + string(self.module.object.strtable[mod]))
	}
	for _, ef := range m.object.export_functions {
		if string(m.object.strtable[ef.name])+","+string(m.object.strtable[ef.descriptor]) == string(self.module.object.strtable[fun]) {
			self.invoke(m, ef.index)
			return
		}
	}
	panic("function not found: " + string(self.module.object.strtable[fun]))
}

func (self *executor) op_calli() {
	self.ip++
	if len(self.callstack) >= self.param_recursion_limit {
		self.raise_literal("StackOverflowException", objectString{""})
		return
	}
	a := self.pop().array
	m := a[0].module
	index := int(a[1].num)
	self.invoke(m, index)
}

func (self *executor) op_jump() {
	self.ip++
	target := get_vint(self.module.object.code, &self.ip)
	self.ip = target
}

func (self *executor) op_jf() {
	self.ip++
	target := get_vint(self.module.object.code, &self.ip)
	a := self.pop().bool
	if !a {
		self.ip = target
	}
}

func (self *executor) op_jt() {
	self.ip++
	target := get_vint(self.module.object.code, &self.ip)
	a := self.pop().bool
	if a {
		self.ip = target
	}
}

func (self *executor) op_dup() {
	self.ip++
	x := self.stack[len(self.stack)-1]
	self.push(x)
}

func (self *executor) op_dupx1() {
	self.ip++
	a := self.pop()
	b := self.pop()
	self.push(a)
	self.push(b)
	self.push(a)
}

func (self *executor) op_drop() {
	self.ip++
	self.pop()
}

func (self *executor) op_ret() {
	self.frames = self.frames[:len(self.frames)-1]
	self.module = self.callstack[len(self.callstack)-1].module
	self.ip = self.callstack[len(self.callstack)-1].ip
	self.callstack = self.callstack[:len(self.callstack)-1]
}

func (self *executor) op_consa() {
	self.ip++
	val := get_vint(self.module.object.code, &self.ip)
	r := []cell{}
	for val > 0 {
		r = append(r, self.pop())
		val--
	}
	self.push(make_cell_array(r))
}

func (self *executor) op_consd() {
	self.ip++
	val := get_vint(self.module.object.code, &self.ip)
	r := map[string]cell{}
	for val > 0 {
		value := self.pop()
		key := self.pop().str
		r[key] = value
		val--
	}
	self.push(make_cell_dict(r))
}

func (self *executor) op_except() {
	start_ip := self.ip
	self.ip++
	val := get_vint(self.module.object.code, &self.ip)
	self.ip = start_ip
	info := self.pop().obj
	self.raise_literal(string(self.module.object.strtable[val]), info)
}

func (self *executor) op_alloc() {
	self.ip++
	val := get_vint(self.module.object.code, &self.ip)
	cell := make_cell_array(make([]cell, val))
	self.push(make_cell_addr(&cell))
}

func (self *executor) op_pushnil() {
	self.ip += 1
	self.stack = append(self.stack, make_cell_addr(nil))
}

func (self *executor) op_resetc() {
	self.ip++
	r := self.pop().ref
	r.store(make_cell_none())
}

func (self *executor) op_pushpeg() {
	assert(false, "unimplemented pushpeg")
}

func (self *executor) op_jumptbl() {
	self.ip++
	val := get_vint(self.module.object.code, &self.ip)
	n := self.pop().num
	if n == math.Trunc(n) && n >= 0 {
		i := int(n)
		if i < val {
			self.ip += 6 * i
		} else {
			self.ip += 6 * val
		}
	} else {
		self.ip += 6 * val
	}
}

func (self *executor) op_callx() {
	assert(false, "unimplemented callx")
}

func (self *executor) op_swap() {
	self.ip++
	top := len(self.stack)
	self.stack[top-2], self.stack[top-1] = self.stack[top-1], self.stack[top-2]
}

func (self *executor) op_dropn() {
	assert(false, "unimplemented dropn")
}

func (self *executor) op_pushfp() {
	self.ip++
	val := get_vint(self.module.object.code, &self.ip)
	r := []cell{make_cell_module(self.module), make_cell_num(float64(val))}
	self.push(make_cell_array(r))
}

func (self *executor) op_callv() {
	self.ip++
	val := get_vint(self.module.object.code, &self.ip)
	if len(self.callstack) >= self.param_recursion_limit {
		self.raise_literal("StackOverflowException", objectString{""})
		return
	}
	pi := self.pop().array
	instance := pi[0].ref
	interface_index := int(pi[1].num)
	//m :=
	classinfo := instance.load().array[0].classinfo
	self.invoke(classinfo.module, classinfo.classinfo.interfaces[interface_index][val])
}

func (self *executor) op_pushci() {
	self.ip++
	val := get_vint(self.module.object.code, &self.ip)
	dot := strings.Index(string(self.module.object.strtable[val]), ".")
	if dot < 0 {
		for _, c := range self.module.object.classes {
			if c.name == val {
				ci := make_cell_classinfo(self.module, &c)
				self.push(ci)
				return
			}
		}
	} else {
		modname := string(self.module.object.strtable[val][:dot])
		methodname := string(self.module.object.strtable[val][dot+1:])
		if m, found := self.modules[modname]; found {
			for _, c := range m.object.classes {
				if string(m.object.strtable[c.name]) == methodname {
					ci := make_cell_classinfo(m, &c)
					self.push(ci)
					return
				}
			}
		}
	}
	panic("neon: unknown class name")
}

func (self *executor) raise_literal(exception string, info object) {
	exceptionvar := make_cell_array([]cell{
		make_cell_str(exception),
		make_cell_obj(info),
		make_cell_num(float64(self.ip)),
	})

	tmodule := self.module
	tip := self.ip
	sp := len(self.callstack)
	for {
		for _, e := range tmodule.object.exceptions {
			if tip >= e.start && tip < e.end {
				handler := string(tmodule.object.strtable[e.excid])
				if exception == handler || (len(exception) > len(handler) && exception[0:len(handler)] == handler && exception[len(handler)] == '.') {
					self.module = tmodule
					self.ip = e.handler
					if len(self.frames) > 0 {
						for len(self.stack) > self.frames[len(self.frames)-1].opstack_depth+e.stack_depth {
							self.pop()
						}
					} else {
						for len(self.stack) > e.stack_depth {
							self.pop()
						}
					}
					self.callstack = self.callstack[:sp]
					self.push(exceptionvar)
					return
				}
			}
		}
		if sp == 0 {
			break
		}
		sp--
		if len(self.frames) > 0 {
			self.frames = self.frames[:len(self.frames)-1]
		}
		tmodule = self.callstack[sp].module
		tip = self.callstack[sp].ip
	}

	fmt.Fprintf(os.Stderr, "Unhandled exception %s (%s)\n", exception, info.toString())
	os.Exit(1)
}

func neon_file_copyOverwriteIfExists(srcname string, dstname string) {
	dst, err := os.Create(dstname)
	if err != nil {
		panic(err)
	}
	defer dst.Close()
	src, err := os.Open(srcname)
	if err != nil {
		panic(err)
	}
	defer src.Close()
	io.Copy(src, dst)
}

func (self *executor) invoke(mod *module, index int) {
	self.callstack = append(self.callstack, returnaddress{self.module, self.ip})
	nest := mod.object.functions[index].nest
	params := mod.object.functions[index].params
	locals := mod.object.functions[index].locals
	var outer *frame
	if len(self.frames) > 0 {
		assert(nest <= self.frames[len(self.frames)-1].nesting_depth+1, "internal nesting error")
		outer = &self.frames[len(self.frames)-1]
		for outer != nil && nest <= outer.nesting_depth {
			assert(outer.outer == nil || outer.nesting_depth == outer.outer.nesting_depth+1, "internal nesting error")
			outer = outer.outer
		}
	}
	f := frame{
		nesting_depth: nest,
		outer:         outer,
		locals:        make([]cell, locals),
		opstack_depth: len(self.stack) - params,
	}
	self.frames = append(self.frames, f)
	self.module = mod
	self.ip = self.module.object.functions[index].entry
}

func main() {
	bytes, err := ioutil.ReadFile(os.Args[1])
	if err != nil {
		panic(err)
	}
	executor := make_executor(os.Args[1], bytes)
	executor.run()
}
