package main

import (
	"fmt"
	"io/ioutil"
	"math"
	"os"
	"strconv"
)

const (
	ENTER    = iota // enter function scope
	LEAVE    = iota // leave function scope
	PUSHB    = iota // push boolean immediate
	PUSHN    = iota // push number immediate
	PUSHS    = iota // push string immediate
	PUSHY    = iota // push bytes immediate
	PUSHPG   = iota // push pointer to global
	PUSHPPG  = iota // push pointer to predefined global
	PUSHPMG  = iota // push pointer to module global
	PUSHPL   = iota // push pointer to local
	PUSHPOL  = iota // push pointer to outer local
	PUSHI    = iota // push 32-bit integer immediate
	LOADB    = iota // load boolean
	LOADN    = iota // load number
	LOADS    = iota // load string
	LOADY    = iota // load bytes
	LOADA    = iota // load array
	LOADD    = iota // load dictionary
	LOADP    = iota // load pointer
	LOADJ    = iota // load object
	STOREB   = iota // store boolean
	STOREN   = iota // store number
	STORES   = iota // store string
	STOREY   = iota // store bytes
	STOREA   = iota // store array
	STORED   = iota // store dictionary
	STOREP   = iota // store pointer
	STOREJ   = iota // store object
	NEGN     = iota // negate number
	ADDN     = iota // add number
	SUBN     = iota // subtract number
	MULN     = iota // multiply number
	DIVN     = iota // divide number
	MODN     = iota // modulo number
	EXPN     = iota // exponentiate number
	EQB      = iota // compare equal boolean
	NEB      = iota // compare unequal boolean
	EQN      = iota // compare equal number
	NEN      = iota // compare unequal number
	LTN      = iota // compare less number
	GTN      = iota // compare greater number
	LEN      = iota // compare less equal number
	GEN      = iota // compare greater equal number
	EQS      = iota // compare equal string
	NES      = iota // compare unequal string
	LTS      = iota // compare less string
	GTS      = iota // compare greater string
	LES      = iota // compare less equal string
	GES      = iota // compare greater equal string
	EQY      = iota // compare equal bytes
	NEY      = iota // compare unequal bytes
	LTY      = iota // compare less bytes
	GTY      = iota // compare greater bytes
	LEY      = iota // compare less equal bytes
	GEY      = iota // compare greater equal bytes
	EQA      = iota // compare equal array
	NEA      = iota // compare unequal array
	EQD      = iota // compare equal dictionary
	NED      = iota // compare unequal dictionary
	EQP      = iota // compare equal pointer
	NEP      = iota // compare unequal pointer
	ANDB     = iota // and boolean
	ORB      = iota // or boolean
	NOTB     = iota // not boolean
	INDEXAR  = iota // index array for read
	INDEXAW  = iota // index array for write
	INDEXAV  = iota // index array value
	INDEXAN  = iota // index array value, no exception
	INDEXDR  = iota // index dictionary for read
	INDEXDW  = iota // index dictionary for write
	INDEXDV  = iota // index dictionary value
	INA      = iota // in array
	IND      = iota // in dictionary
	CALLP    = iota // call predefined
	CALLF    = iota // call function
	CALLMF   = iota // call module function
	CALLI    = iota // call indirect
	JUMP     = iota // unconditional jump
	JF       = iota // jump if false
	JT       = iota // jump if true
	JFCHAIN  = iota // jump and drop next if false
	DUP      = iota // duplicate
	DUPX1    = iota // duplicate under second value
	DROP     = iota // drop
	RET      = iota // return
	CALLE    = iota // call foreign
	CONSA    = iota // construct array
	CONSD    = iota // construct dictionary
	EXCEPT   = iota // throw exception
	ALLOC    = iota // allocate record
	PUSHNIL  = iota // push nil pointer
	JNASSERT = iota // jump if assertions disabled
	RESETC   = iota // reset cell
	PUSHPEG  = iota // push pointer to external global
	JUMPTBL  = iota // jump table
	CALLX    = iota // call extension
	SWAP     = iota // swap two top stack elements
	DROPN    = iota // drop element n
	PUSHM    = iota // push current module
	CALLV    = iota // call virtual
	PUSHCI   = iota // push class info
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

const (
	CELL_NONE       = iota
	CELL_ADDRESS    = iota
	CELL_BOOLEAN    = iota
	CELL_NUMBER     = iota
	CELL_STRING     = iota
	CELL_ARRAY      = iota
	CELL_DICTIONARY = iota
)

type cell struct {
	ctype int
	addr  *cell
	bool  bool
	num   float64
	str   string
	array []cell
	dict  map[string]cell
}

func make_cell_addr(a *cell) cell {
	return cell{
		ctype: CELL_ADDRESS,
		addr:  a,
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

func (self cell) Equal(other cell) bool {
	if self.ctype != other.ctype {
		return false
	}
	switch self.ctype {
	case CELL_ADDRESS:
		return self.addr == other.addr
	case CELL_BOOLEAN:
		return self.bool == other.bool
	case CELL_NUMBER:
		return self.num == other.num
	case CELL_STRING:
		return self.str == other.str
	case CELL_ARRAY:
		if len(self.array) != len(other.array) {
			return false
		}
		for i, v := range self.array {
			if v.Equal(other.array[i]) {
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
			if v.Equal(other.dict[k]) {
				return false
			}
		}
		return true
	}
	return false
}

type function struct {
	name  int
	entry int
}

type bytecode struct {
	source_hash []byte
	global_size int
	strtable    []string
	functions   []function
	code        []byte
}

func get_strtable(bytes []byte) []string {
	r := []string{}
	i := 0
	for i < len(bytes) {
		strlen := get_vint(bytes, &i)
		r = append(r, string(bytes[i:i+strlen]))
		i += strlen
	}
	return r
}

func make_bytecode(bytes []byte) bytecode {
	r := bytecode{}
	i := 0
	r.source_hash = bytes[0:32]
	i += 32
	r.global_size = get_vint(bytes, &i)
	strtablesize := get_vint(bytes, &i)
	r.strtable = get_strtable(bytes[i : i+strtablesize])
	i += strtablesize

	typesize := get_vint(bytes, &i)
	assert(typesize == 0, "typesize")

	constantsize := get_vint(bytes, &i)
	assert(constantsize == 0, "constantsize")

	variablesize := get_vint(bytes, &i)
	assert(variablesize == 0, "variablesize")

	exportfunctionsize := get_vint(bytes, &i)
	assert(exportfunctionsize == 0, "exportfunctionsize")

	exceptionexportsize := get_vint(bytes, &i)
	assert(exceptionexportsize == 0, "exceptionexportsize")

	interfaceexportsize := get_vint(bytes, &i)
	assert(interfaceexportsize == 0, "interfaceexportsize")

	importsize := get_vint(bytes, &i)
	assert(importsize == 0, "importsize")

	functionsize := get_vint(bytes, &i)
	for functionsize > 0 {
		f := function{}
		f.name = get_vint(bytes, &i)
		f.entry = get_vint(bytes, &i)
		r.functions = append(r.functions, f)
		functionsize--
	}

	exceptionsize := get_vint(bytes, &i)
	assert(exceptionsize == 0, "exceptionsize")

	classsize := get_vint(bytes, &i)
	assert(classsize == 0, "classsize")

	r.code = bytes[i:]

	return r
}

type executor struct {
	object    bytecode
	globals   []cell
	stack     []cell
	callstack []int
	frames    [][]cell
	ip        int
}

func make_executor(bytes []byte) executor {
	object := make_bytecode(bytes)
	return executor{
		object:    object,
		globals:   make([]cell, object.global_size),
		stack:     []cell{},
		callstack: []int{},
		ip:        0,
	}
}

func (self *executor) run() {
	self.callstack = append(self.callstack, len(self.object.code))
	for self.ip < len(self.object.code) {
		switch self.object.code[self.ip] {
		case ENTER:
			self.op_enter()
		case LEAVE:
			self.op_leave()
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
		case JFCHAIN:
			self.op_jfchain()
		case DUP:
			self.op_dup()
		case DUPX1:
			self.op_dupx1()
		case DROP:
			self.op_drop()
		case RET:
			self.op_ret()
		case CALLE:
			self.op_calle()
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
		case JNASSERT:
			self.op_jnassert()
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
		case PUSHM:
			self.op_pushm()
		case CALLV:
			self.op_callv()
		case PUSHCI:
			self.op_pushci()
		default:
			panic(fmt.Sprintf("unknown opcode %d", self.object.code[self.ip]))
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

func (self *executor) op_enter() {
	self.ip++
	nest := get_vint(self.object.code, &self.ip)
	assert(nest >= 0, "To avoid unused variable error")
	params := get_vint(self.object.code, &self.ip)
	assert(params >= 0, "To avoid unused variable error")
	locals := get_vint(self.object.code, &self.ip)
	f := make([]cell, locals)
	self.frames = append(self.frames, f)
}

func (self *executor) op_leave() {
	self.ip++
	self.frames = self.frames[:len(self.frames)-1]
}

func (self *executor) op_pushb() {
	self.ip++
	val := self.object.code[self.ip] != 0
	self.ip++
	self.push(make_cell_bool(val))
}

func (self *executor) op_pushn() {
	self.ip++
	val := get_vint(self.object.code, &self.ip)
	n, err := strconv.ParseFloat(self.object.strtable[val], 64)
	if err != nil {
		panic("number")
	}
	self.push(make_cell_num(n))
}

func (self *executor) op_pushs() {
	self.ip++
	val := get_vint(self.object.code, &self.ip)
	self.push(make_cell_str(self.object.strtable[val]))
}

func (self *executor) op_pushy() {
	assert(false, "unimplemented")
}

func (self *executor) op_pushpg() {
	self.ip++
	addr := get_vint(self.object.code, &self.ip)
	self.push(make_cell_addr(&self.globals[addr]))
}

func (self *executor) op_pushppg() {
	assert(false, "unimplemented")
}

func (self *executor) op_pushpmg() {
	assert(false, "unimplemented")
}

func (self *executor) op_pushpl() {
	self.ip += 1
	addr := get_vint(self.object.code, &self.ip)
	self.push(make_cell_addr(&self.frames[len(self.frames)-1][addr]))
}

func (self *executor) op_pushpol() {
	assert(false, "unimplemented")
}

func (self *executor) op_pushi() {
	self.ip++
	val := get_vint(self.object.code, &self.ip)
	self.push(make_cell_num(float64(val)))
}

func (self *executor) op_loadb() {
	self.ip++
	addr := self.pop().addr
	self.push(*addr)
}

func (self *executor) op_loadn() {
	self.ip++
	addr := self.pop().addr
	self.push(*addr)
}

func (self *executor) op_loads() {
	self.ip++
	addr := self.pop().addr
	self.push(*addr)
}

func (self *executor) op_loady() {
	assert(false, "unimplemented")
}

func (self *executor) op_loada() {
	self.ip++
	addr := self.pop().addr
	self.push(*addr)
}

func (self *executor) op_loadd() {
	self.ip++
	addr := self.pop().addr
	self.push(*addr)
}

func (self *executor) op_loadp() {
	self.ip++
	addr := self.pop().addr
	self.push(*addr)
}

func (self *executor) op_loadj() {
	assert(false, "unimplemented")
}

func (self *executor) op_storeb() {
	self.ip++
	addr := self.pop().addr
	val := self.pop()
	*addr = val
}

func (self *executor) op_storen() {
	self.ip++
	addr := self.pop().addr
	val := self.pop()
	*addr = val
}

func (self *executor) op_stores() {
	self.ip++
	addr := self.pop().addr
	val := self.pop()
	*addr = val
}

func (self *executor) op_storey() {
	assert(false, "unimplemented")
}

func (self *executor) op_storea() {
	self.ip++
	addr := self.pop().addr
	val := self.pop()
	*addr = val
}

func (self *executor) op_stored() {
	self.ip++
	addr := self.pop().addr
	val := self.pop()
	*addr = val
}

func (self *executor) op_storep() {
	self.ip++
	addr := self.pop().addr
	val := self.pop()
	*addr = val
}

func (self *executor) op_storej() {
	assert(false, "unimplemented")
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
	self.push(make_cell_num(a / b))
}

func (self *executor) op_modn() {
	self.ip++
	b := self.pop().num
	a := self.pop().num
	self.push(make_cell_num(math.Remainder(a, b)))
}

func (self *executor) op_expn() {
	self.ip++
	b := self.pop().num
	a := self.pop().num
	self.push(make_cell_num(math.Pow(a, b)))
}

func (self *executor) op_eqb() {
	assert(false, "unimplemented")
}

func (self *executor) op_neb() {
	assert(false, "unimplemented")
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
	assert(false, "unimplemented")
}

func (self *executor) op_lts() {
	assert(false, "unimplemented")
}

func (self *executor) op_gts() {
	self.ip++
	b := self.pop().str
	a := self.pop().str
	self.push(make_cell_bool(a > b))
}

func (self *executor) op_les() {
	assert(false, "unimplemented")
}

func (self *executor) op_ges() {
	assert(false, "unimplemented")
}

func (self *executor) op_eqy() {
	assert(false, "unimplemented")
}

func (self *executor) op_ney() {
	assert(false, "unimplemented")
}

func (self *executor) op_lty() {
	assert(false, "unimplemented")
}

func (self *executor) op_gty() {
	assert(false, "unimplemented")
}

func (self *executor) op_ley() {
	assert(false, "unimplemented")
}

func (self *executor) op_gey() {
	assert(false, "unimplemented")
}

func (self *executor) op_eqa() {
	assert(false, "unimplemented")
}

func (self *executor) op_nea() {
	assert(false, "unimplemented")
}

func (self *executor) op_eqd() {
	assert(false, "unimplemented")
}

func (self *executor) op_ned() {
	assert(false, "unimplemented")
}

func (self *executor) op_eqp() {
	assert(false, "unimplemented")
}

func (self *executor) op_nep() {
	assert(false, "unimplemented")
}

func (self *executor) op_andb() {
	assert(false, "unimplemented")
}

func (self *executor) op_orb() {
	assert(false, "unimplemented")
}

func (self *executor) op_notb() {
	self.ip++
	b := self.pop().bool
	self.push(make_cell_bool(!b))
}

func (self *executor) op_indexar() {
	self.ip++
	index := int(self.pop().num)
	addr := self.pop().addr
	self.push(make_cell_addr(&addr.array[index]))
}

func (self *executor) op_indexaw() {
	self.ip++
	index := int(self.pop().num)
	addr := self.pop().addr
	if index >= len(addr.array) {
		addr.array = append(addr.array, make([]cell, index-len(addr.array)+1)...)
	}
	self.push(make_cell_addr(&addr.array[index]))
}

func (self *executor) op_indexav() {
	self.ip++
	index := int(self.pop().num)
	a := self.pop().array
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
	assert(false, "unimplemented")
}

func (self *executor) op_indexdw() {
	self.ip++
	/*key :=*/ self.pop() /*.str*/
	/*addr :=*/ self.pop() /*.addr*/
	self.push(make_cell_addr(nil))
	//self.push(make_cell_addr(&addr.dict[key]))
}

func (self *executor) op_indexdv() {
	self.ip++
	key := self.pop().str
	dict := self.pop().addr.dict
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
	assert(false, "unimplemented")
}

func (self *executor) op_callp() {
	self.ip++
	val := get_vint(self.object.code, &self.ip)
	name := self.object.strtable[val]
	switch name {
	case "array__append":
		e := self.pop()
		a := self.pop().addr
		a.array = append(a.array, e)
	case "array__concat":
		b := self.pop().array
		a := self.pop().array
		self.push(make_cell_array(append(a, b...)))
	case "array__extend":
		b := self.pop().array
		addr := self.pop().addr
		addr.array = append(addr.array, b...)
	case "array__size":
		a := self.pop().array
		self.push(make_cell_num(float64(len(a))))
	case "array__slice":
		last_from_end := self.pop().bool
		last := int(self.pop().num)
		first_from_end := self.pop().bool
		first := int(self.pop().num)
		a := self.pop().array
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
		self.push(make_cell_array(a[first : last+1]))
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
	case "binary$and32":
		b := self.pop().num
		a := self.pop().num
		self.push(make_cell_num(float64(uint32(a) & uint32(b))))
	case "chr":
		c := self.pop().num
		self.push(make_cell_str(string([]byte{byte(c)})))
	case "concat":
		b := self.pop().str
		a := self.pop().str
		self.push(make_cell_str(a + b))
	case "dictionary__keys":
		d := self.pop().dict
		i := 0
		keys := make([]cell, len(d))
		for k := range d {
			keys[i] = make_cell_str(k)
			i++
		}
		self.push(make_cell_array(keys))
	case "math$abs":
		x := self.pop().num
		self.push(make_cell_num(math.Abs(x)))
	case "math$ceil":
		x := self.pop().num
		self.push(make_cell_num(math.Ceil(x)))
	case "math$sqrt":
		x := self.pop().num
		self.push(make_cell_num(math.Sqrt(x)))
	case "ord":
		s := self.pop().str
		self.push(make_cell_num(float64(s[0])))
	case "print":
		str := self.pop()
		fmt.Println(str.str)
	case "str", "number__toString":
		n := self.pop().num
		self.push(make_cell_str(fmt.Sprintf("%v", n)))
	case "strb", "boolean__toString":
		b := self.pop().bool
		if b {
			self.push(make_cell_str("TRUE"))
		} else {
			self.push(make_cell_str("FALSE"))
		}
	case "string__append":
		t := self.pop().str
		a := self.pop().addr
		a.str = a.str + t
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
		self.push(make_cell_str(s[:first] + t + s[last+1:]))
	case "string__substring":
		last_from_end := self.pop().bool
		last := int(self.pop().num)
		first_from_end := self.pop().bool
		first := int(self.pop().num)
		s := self.pop().str
		if first_from_end {
			first += len(s) - 1
		}
		if last_from_end {
			last += len(s) - 1
		}
		self.push(make_cell_str(s[first : last+1]))
	default:
		assert(false, fmt.Sprintf("unimplemented function: %s", self.object.strtable[val]))
	}
}

func (self *executor) op_callf() {
	self.ip++
	val := get_vint(self.object.code, &self.ip)
	self.callstack = append(self.callstack, self.ip)
	self.ip = val
}

func (self *executor) op_callmf() {
	assert(false, "unimplemented")
}

func (self *executor) op_calli() {
	assert(false, "unimplemented")
}

func (self *executor) op_jump() {
	self.ip++
	target := get_vint(self.object.code, &self.ip)
	self.ip = target
}

func (self *executor) op_jf() {
	self.ip++
	target := get_vint(self.object.code, &self.ip)
	a := self.pop().bool
	if !a {
		self.ip = target
	}
}

func (self *executor) op_jt() {
	self.ip++
	target := get_vint(self.object.code, &self.ip)
	a := self.pop().bool
	if a {
		self.ip = target
	}
}

func (self *executor) op_jfchain() {
	self.ip++
	target := get_vint(self.object.code, &self.ip)
	a := self.pop()
	if !a.bool {
		self.ip = target
		self.pop()
		self.push(a)
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
	self.ip = self.callstack[len(self.callstack)-1]
	self.callstack = self.callstack[:len(self.callstack)-1]
}

func (self *executor) op_calle() {
	assert(false, "unimplemented")
}

func (self *executor) op_consa() {
	self.ip++
	val := get_vint(self.object.code, &self.ip)
	r := []cell{}
	for val > 0 {
		r = append(r, self.pop())
		val--
	}
	self.push(make_cell_array(r))
}

func (self *executor) op_consd() {
	self.ip++
	val := get_vint(self.object.code, &self.ip)
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
	assert(false, "unimplemented")
}

func (self *executor) op_alloc() {
	assert(false, "unimplemented")
}

func (self *executor) op_pushnil() {
	self.ip += 1
	self.stack = append(self.stack, make_cell_addr(nil))
}

func (self *executor) op_jnassert() {
	assert(false, "unimplemented")
}

func (self *executor) op_resetc() {
	self.ip++
	addr := self.pop().addr
	addr.ctype = CELL_NONE
}

func (self *executor) op_pushpeg() {
	assert(false, "unimplemented")
}

func (self *executor) op_jumptbl() {
	assert(false, "unimplemented")
}

func (self *executor) op_callx() {
	assert(false, "unimplemented")
}

func (self *executor) op_swap() {
	self.ip++;
	top := len(self.stack)
	self.stack[top-2], self.stack[top-1] = self.stack[top-1], self.stack[top-2]
}

func (self *executor) op_dropn() {
	assert(false, "unimplemented")
}

func (self *executor) op_pushm() {
	assert(false, "unimplemented")
}

func (self *executor) op_callv() {
	assert(false, "unimplemented")
}

func (self *executor) op_pushci() {
	assert(false, "unimplemented")
}

func main() {
	bytes, err := ioutil.ReadFile(os.Args[1])
	if err != nil {
		panic(err)
	}
	executor := make_executor(bytes)
	executor.run()
}
