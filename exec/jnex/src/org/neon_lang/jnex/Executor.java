package org.neon_lang.jnex;

import java.io.DataInput;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.math.BigDecimal;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import org.nevec.rjm.BigDecimalMath;

class Executor {
    private final boolean enable_assert = true;
    private final boolean enable_debug = false;

    private class ActivationFrame {
        int nesting_depth;
        ActivationFrame outer;
        Cell[] locals;
        int opstack_depth;

        ActivationFrame(int nesting_depth, ActivationFrame outer, int count, int opstack_depth)
        {
            this.nesting_depth = nesting_depth;
            this.outer = outer;
            locals = new Cell[count];
            for (int i = 0; i < count; i++) {
                locals[i] = new Cell();
            }
            this.opstack_depth = opstack_depth;
        }
    }

    Executor(DataInput in, String[] args)
    {
        predefined = new HashMap<String, GenericFunction>();
        predefined.put("builtin$array__append", this::array__append);
        predefined.put("builtin$array__concat", this::array__concat);
        predefined.put("builtin$array__extend", this::array__extend);
        predefined.put("builtin$array__find", this::array__find);
        predefined.put("builtin$array__remove", this::array__remove);
        predefined.put("builtin$array__reversed", this::array__reversed);
        predefined.put("builtin$array__size", this::array__size);
        predefined.put("builtin$array__slice", this::array__slice);
        predefined.put("builtin$array__splice", this::array__splice);
        predefined.put("builtin$array__toBytes__number", this::array__toBytes__number);
        predefined.put("builtin$array__toString__number", this::array__toString__number);
        predefined.put("builtin$array__toString__string", this::array__toString__string);
        predefined.put("builtin$array__toString__object", this::array__toString__object);
        predefined.put("builtin$boolean__toString", this::boolean__toString);
        predefined.put("builtin$bytes__append", this::bytes__append);
        predefined.put("builtin$bytes__concat", this::bytes__concat);
        predefined.put("global$Bytes__decodeUTF8", this::bytes__decodeUTF8);
        predefined.put("builtin$bytes__index", this::bytes__index);
        predefined.put("builtin$bytes__range", this::bytes__range);
        predefined.put("builtin$bytes__size", this::bytes__size);
        predefined.put("builtin$bytes__splice", this::bytes__splice);
        predefined.put("builtin$bytes__store", this::bytes__store);
        predefined.put("builtin$bytes__toArray", this::bytes__toArray);
        predefined.put("builtin$bytes__toString", this::bytes__toString);
        predefined.put("builtin$dictionary__keys", this::dictionary__keys);
        predefined.put("builtin$dictionary__remove", this::dictionary__remove);
        predefined.put("builtin$dictionary__toString__object", this::dictionary__toString__object);
        predefined.put("builtin$dictionary__toString__string", this::dictionary__toString__string);
        predefined.put("global$num", this::num);
        predefined.put("builtin$number__toString", this::number__toString);
        predefined.put("builtin$object__getArray", this::object__getArray);
        predefined.put("builtin$object__getBoolean", this::object__getBoolean);
        predefined.put("builtin$object__getBytes", this::object__getBytes);
        predefined.put("builtin$object__getDictionary", this::object__getDictionary);
        predefined.put("builtin$object__getNumber", this::object__getNumber);
        predefined.put("builtin$object__getString", this::object__getString);
        predefined.put("builtin$object__invokeMethod", this::object__invokeMethod);
        predefined.put("builtin$object__isNull", this::object__isNull);
        predefined.put("builtin$object__makeArray", this::object__makeArray);
        predefined.put("builtin$object__makeBoolean", this::object__makeBoolean);
        predefined.put("builtin$object__makeBytes", this::object__makeBytes);
        predefined.put("builtin$object__makeDictionary", this::object__makeDictionary);
        predefined.put("builtin$object__makeNull", this::object__makeNull);
        predefined.put("builtin$object__makeNumber", this::object__makeNumber);
        predefined.put("builtin$object__makeString", this::object__makeString);
        predefined.put("builtin$object__setProperty", this::object__setProperty);
        predefined.put("builtin$object__subscript", this::object__subscript);
        predefined.put("builtin$object__toString", this::object__toString);
        predefined.put("global$print", this::print);
        predefined.put("global$str", this::number__toString);
        predefined.put("builtin$string__append", this::string__append);
        predefined.put("builtin$string__concat", this::string__concat);
        predefined.put("builtin$string__index", this::string__index);
        predefined.put("builtin$string__length", this::string__length);
        predefined.put("builtin$string__substring", this::string__substring);
        predefined.put("builtin$string__encodeUTF8", this::string__encodeUTF8);
        predefined.put("builtin$string__toString", this::string__toString);
        predefined.put("math$abs", this::math$abs);
        predefined.put("math$atan", this::math$atan);
        predefined.put("math$ceil", this::math$ceil);
        predefined.put("math$cos", this::math$cos);
        predefined.put("math$exp", this::math$exp);
        predefined.put("math$floor", this::math$floor);
        predefined.put("math$log", this::math$log);
        predefined.put("math$odd", this::math$odd);
        predefined.put("math$sign", this::math$sign);
        predefined.put("math$sin", this::math$sin);
        predefined.put("math$sqrt", this::math$sqrt);
        predefined.put("math$trunc", this::math$trunc);
        predefined.put("random$uint32", this::random$uint32);
        predefined.put("runtime$assertionsEnabled", this::runtime$assertionsEnabled);
        predefined.put("runtime$debugEnabled", this::runtime$debugEnabled);
        predefined.put("runtime$createObject", this::runtime$createObject);
        predefined.put("runtime$executorName", this::runtime$executorName);
        predefined.put("runtime$moduleIsMain", this::runtime$moduleIsMain);
        predefined.put("string$fromCodePoint", this::string$fromCodePoint);
        predefined.put("string$toCodePoint", this::string$toCodePoint);
        predefined.put("sys$exit", this::sys$exit);
        predefined.put("textio$close", this::textio$close);
        predefined.put("textio$open", this::textio$open);
        predefined.put("textio$readLine", this::textio$readLine);
        predefined.put("textio$writeLine", this::textio$writeLine);
        predefined.put("time$now", this::time$now);

        this.args = new ArrayList<Cell>();
        for (String x: args) {
            this.args.add(new Cell(x));
        }

        object = new Bytecode(in);
        ip = 0;
        callstack = new ArrayDeque<Integer>();
        stack = new ArrayDeque<Cell>();
        globals = new Cell[object.global_size];
        for (int i = 0; i < object.global_size; i++) {
            globals[i] = new Cell();
        }
        frames = new ArrayDeque<ActivationFrame>();
    }

    void run()
    {
        ip = object.code.size();
        invoke(0);
        execLoop(0);
    }

    void execLoop(int min_callstack_depth)
    {
        while (callstack.size() > min_callstack_depth && ip < object.code.size()) {
            //System.err.println("ip=" + ip + " op=" + opcodes[object.code.get(ip)]);
            //for (Cell c: stack) { System.err.println("  " + c); }
            try {
                switch (opcodes[object.code.get(ip)]) {
                    case PUSHB: doPUSHB(); break;
                    case PUSHN: doPUSHN(); break;
                    case PUSHS: doPUSHS(); break;
                    case PUSHY: doPUSHY(); break;
                    case PUSHPG: doPUSHPG(); break;
                    case PUSHPPG: doPUSHPPG(); break;
                    //case PUSHPMG
                    case PUSHPL: doPUSHPL(); break;
                    case PUSHPOL: doPUSHPOL(); break;
                    case PUSHI: doPUSHI(); break;
                    case LOADB: doLOADB(); break;
                    case LOADN: doLOADN(); break;
                    case LOADS: doLOADS(); break;
                    case LOADY: doLOADY(); break;
                    case LOADA: doLOADA(); break;
                    case LOADD: doLOADD(); break;
                    case LOADP: doLOADP(); break;
                    case LOADJ: doLOADJ(); break;
                    case STOREB: doSTOREB(); break;
                    case STOREN: doSTOREN(); break;
                    case STORES: doSTORES(); break;
                    case STOREY: doSTOREY(); break;
                    case STOREA: doSTOREA(); break;
                    case STORED: doSTORED(); break;
                    case STOREP: doSTOREP(); break;
                    case STOREJ: doSTOREJ(); break;
                    case NEGN: doNEGN(); break;
                    case ADDN: doADDN(); break;
                    case SUBN: doSUBN(); break;
                    case MULN: doMULN(); break;
                    case DIVN: doDIVN(); break;
                    case MODN: doMODN(); break;
                    case EXPN: doEXPN(); break;
                    case EQB: doEQB(); break;
                    case NEB: doNEB(); break;
                    case EQN: doEQN(); break;
                    case NEN: doNEN(); break;
                    case LTN: doLTN(); break;
                    case GTN: doGTN(); break;
                    case LEN: doLEN(); break;
                    case GEN: doGEN(); break;
                    case EQS: doEQS(); break;
                    case NES: doNES(); break;
                    case LTS: doLTS(); break;
                    case GTS: doGTS(); break;
                    case LES: doLES(); break;
                    case GES: doGES(); break;
                    case EQY: doEQY(); break;
                    case NEY: doNEY(); break;
                    case LTY: doLTY(); break;
                    //case GTY
                    //case LEY
                    //case GEY
                    case EQA: doEQA(); break;
                    case NEA: doNEA(); break;
                    //case EQD
                    //case NED
                    case EQP: doEQP(); break;
                    case NEP: doNEP(); break;
                    //case ANDB
                    //case ORB
                    case NOTB: doNOTB(); break;
                    case INDEXAR: doINDEXAR(); break;
                    case INDEXAW: doINDEXAW(); break;
                    case INDEXAV: doINDEXAV(); break;
                    case INDEXAN: doINDEXAN(); break;
                    case INDEXDR: doINDEXDR(); break;
                    case INDEXDW: doINDEXDW(); break;
                    case INDEXDV: doINDEXDV(); break;
                    case INA: doINA(); break;
                    case IND: doIND(); break;
                    case CALLP: doCALLP(); break;
                    case CALLF: doCALLF(); break;
                    //case CALLMF
                    //case CALLI
                    case JUMP: doJUMP(); break;
                    case JF: doJF(); break;
                    case JT: doJT(); break;
                    case DUP: doDUP(); break;
                    case DUPX1: doDUPX1(); break;
                    case DROP: doDROP(); break;
                    case RET: doRET(); break;
                    case CONSA: doCONSA(); break;
                    case CONSD: doCONSD(); break;
                    case EXCEPT: doEXCEPT(); break;
                    case ALLOC: doALLOC(); break;
                    case PUSHNIL: doPUSHNIL(); break;
                    case RESETC: doRESETC(); break;
                    //case PUSHPEG
                    case JUMPTBL: doJUMPTBL(); break;
                    //case CALLX
                    case SWAP: doSWAP(); break;
                    //case DROPN
                    //case PUSHFP
                    case CALLV: doCALLV(); break;
                    case PUSHCI: doPUSHCI(); break;
                    default:
                        System.err.println("Unknown opcode: " + opcodes[object.code.get(ip)]);
                        System.exit(1);
                }
            } catch (NeonException x) {
                raiseLiteral(x.name, x.info);
            }
        }
    }

    public static void main(String[] args)
    {
        try {
            new Executor(new DataInputStream(new FileInputStream(args[0])), args).run();
        } catch (java.io.FileNotFoundException x) {
            System.err.println(x);
            System.exit(1);
        }
    }

    private int getVint()
    {
        int r = 0;
        while (true) {
            byte x = object.code.get(ip);
            ip++;
            if ((r & 0xff000000) != 0) {
                System.err.println("Integer value exceeds maximum");
                System.exit(1);
            }
            r = (r << 7) | (x & 0x7f);
            if ((x & 0x80) == 0) {
                break;
            }
        }
        return r;
    }

    private void doPUSHB()
    {
        ip++;
        boolean val = object.code.get(ip++) != 0;
        stack.addFirst(new Cell(val));
    }

    private void doPUSHN()
    {
        ip++;
        int val = getVint();
        stack.addFirst(new Cell(new BigDecimal(object.strtable[val])));
    }

    private void doPUSHS()
    {
        ip++;
        int val = getVint();
        stack.addFirst(new Cell(object.strtable[val]));
    }

    private void doPUSHY()
    {
        ip++;
        int val = getVint();
        stack.addFirst(new Cell(object.bytetable[val]));
    }

    private void doPUSHPG()
    {
        ip++;
        int addr = getVint();
        stack.addFirst(new Cell(globals[addr]));
    }

    private void doPUSHPPG()
    {
        ip++;
        int val = getVint();
        String s = object.strtable[val];
        switch (s) {
            case "sys$args":
                stack.addFirst(new Cell(new Cell(args)));
                break;
            case "textio$stderr":
                stack.addFirst(new Cell(new Cell(new NeObjectNative(System.err))));
                break;
            case "textio$stdout":
                stack.addFirst(new Cell(new Cell(new NeObjectNative(System.out))));
                break;
            default:
                assert false : s;
        }
    }

    private void doPUSHPL()
    {
        ip++;
        int addr = getVint();
        stack.addFirst(new Cell(frames.getFirst().locals[addr]));
    }

    private void doPUSHPOL()
    {
        ip++;
        int back = getVint();
        int addr = getVint();
        ActivationFrame frame = frames.getFirst();
        while (back > 0) {
            frame = frame.outer;
            back--;
        }
        stack.addFirst(new Cell(frame.locals[addr]));
    }

    private void doPUSHI()
    {
        ip++;
        int x = getVint();
        stack.addFirst(new Cell(BigDecimal.valueOf(x)));
    }

    private void doLOADB()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        stack.addFirst(new Cell(addr.getBoolean()));
    }

    private void doLOADN()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        stack.addFirst(new Cell(addr.getNumber()));
    }

    private void doLOADS()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        stack.addFirst(new Cell(addr.getString()));
    }

    private void doLOADY()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        stack.addFirst(new Cell(addr.getBytes()));
    }

    private void doLOADA()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        stack.addFirst(new Cell(addr.getArray()));
    }

    private void doLOADD()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        stack.addFirst(new Cell(addr.getDictionary()));
    }

    private void doLOADP()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        stack.addFirst(new Cell(addr.getAddress()));
    }

    private void doLOADJ()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        stack.addFirst(new Cell(addr.getObject()));
    }

    private void doSTOREB()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        boolean val = stack.removeFirst().getBoolean();
        addr.set(val);
    }

    private void doSTOREN()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        BigDecimal val = stack.removeFirst().getNumber();
        addr.set(val);
    }

    private void doSTORES()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        String val = stack.removeFirst().getString();
        addr.set(val);
    }

    private void doSTOREY()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        byte[] val = stack.removeFirst().getBytes();
        addr.set(val);
    }

    private void doSTOREA()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        List<Cell> val = stack.removeFirst().getArray();
        addr.set(val);
    }

    private void doSTORED()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        Map<String, Cell> val = stack.removeFirst().getDictionary();
        addr.set(val);
    }

    private void doSTOREP()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        Cell val = stack.removeFirst().getAddress();
        addr.set(val);
    }

    private void doSTOREJ()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        NeObject val = stack.removeFirst().getObject();
        addr.set(val);
    }

    private void doNEGN()
    {
        ip++;
        BigDecimal x = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(x.negate()));
    }

    private void doADDN()
    {
        ip++;
        BigDecimal b = stack.removeFirst().getNumber();
        BigDecimal a = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(a.add(b)));
    }

    private void doSUBN()
    {
        ip++;
        BigDecimal b = stack.removeFirst().getNumber();
        BigDecimal a = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(a.subtract(b)));
    }

    private void doMULN()
    {
        ip++;
        BigDecimal b = stack.removeFirst().getNumber();
        BigDecimal a = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(a.multiply(b)));
    }

    private void doDIVN()
    {
        ip++;
        BigDecimal b = stack.removeFirst().getNumber();
        BigDecimal a = stack.removeFirst().getNumber();
        if (b.signum() == 0) {
            raiseLiteral("NumberException.DivideByZero");
            return;
        }
        stack.addFirst(new Cell(a.divide(b, 34, java.math.RoundingMode.HALF_EVEN)));
    }

    private void doMODN()
    {
        ip++;
        BigDecimal b = stack.removeFirst().getNumber();
        BigDecimal a = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(a.remainder(b)));
    }

    private void doEXPN()
    {
        ip++;
        BigDecimal b = stack.removeFirst().getNumber();
        BigDecimal a = stack.removeFirst().getNumber();
        try {
            int p = b.intValue();
            if (p >= 0) {
                BigDecimal r = BigDecimal.ONE;
                while (p != 0) {
                    if ((p & 1) != 0) {
                        r = r.multiply(a);
                    }
                    a = a.multiply(a);
                    p >>= 1;
                }
                stack.addFirst(new Cell(r));
                return;
            }
        } catch (ArithmeticException x) {
        }
        stack.addFirst(new Cell(BigDecimalMath.pow(a, b)));
    }

    private void doEQB()
    {
        ip++;
        boolean b = stack.removeFirst().getBoolean();
        boolean a = stack.removeFirst().getBoolean();
        stack.addFirst(new Cell(a == b));
    }

    private void doNEB()
    {
        ip++;
        boolean b = stack.removeFirst().getBoolean();
        boolean a = stack.removeFirst().getBoolean();
        stack.addFirst(new Cell(a != b));
    }

    private void doEQN()
    {
        ip++;
        BigDecimal b = stack.removeFirst().getNumber();
        BigDecimal a = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(a.compareTo(b) == 0));
    }

    private void doNEN()
    {
        ip++;
        BigDecimal b = stack.removeFirst().getNumber();
        BigDecimal a = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(a.compareTo(b) != 0));
    }

    private void doLTN()
    {
        ip++;
        BigDecimal b = stack.removeFirst().getNumber();
        BigDecimal a = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(a.compareTo(b) < 0));
    }

    private void doGTN()
    {
        ip++;
        BigDecimal b = stack.removeFirst().getNumber();
        BigDecimal a = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(a.compareTo(b) > 0));
    }

    private void doLEN()
    {
        ip++;
        BigDecimal b = stack.removeFirst().getNumber();
        BigDecimal a = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(a.compareTo(b) <= 0));
    }

    private void doGEN()
    {
        ip++;
        BigDecimal b = stack.removeFirst().getNumber();
        BigDecimal a = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(a.compareTo(b) >= 0));
    }

    private void doEQS()
    {
        ip++;
        String b = stack.removeFirst().getString();
        String a = stack.removeFirst().getString();
        stack.addFirst(new Cell(a.equals(b)));
    }

    private void doNES()
    {
        ip++;
        String b = stack.removeFirst().getString();
        String a = stack.removeFirst().getString();
        stack.addFirst(new Cell(!a.equals(b)));
    }

    private void doLTS()
    {
        ip++;
        String b = stack.removeFirst().getString();
        String a = stack.removeFirst().getString();
        stack.addFirst(new Cell(a.compareTo(b) < 0));
    }

    private void doGTS()
    {
        ip++;
        String b = stack.removeFirst().getString();
        String a = stack.removeFirst().getString();
        stack.addFirst(new Cell(a.compareTo(b) > 0));
    }

    private void doLES()
    {
        ip++;
        String b = stack.removeFirst().getString();
        String a = stack.removeFirst().getString();
        stack.addFirst(new Cell(a.compareTo(b) <= 0));
    }

    private void doGES()
    {
        ip++;
        String b = stack.removeFirst().getString();
        String a = stack.removeFirst().getString();
        stack.addFirst(new Cell(a.compareTo(b) >= 0));
    }

    private void doEQY()
    {
        ip++;
        byte[] b = stack.removeFirst().getBytes();
        byte[] a = stack.removeFirst().getBytes();
        stack.addFirst(new Cell(Arrays.equals(a, b)));
    }

    private void doNEY()
    {
        ip++;
        byte[] b = stack.removeFirst().getBytes();
        byte[] a = stack.removeFirst().getBytes();
        stack.addFirst(new Cell(!Arrays.equals(a, b)));
    }

    private void doLTY()
    {
        ip++;
        byte[] b = stack.removeFirst().getBytes();
        byte[] a = stack.removeFirst().getBytes();
        boolean r = a.length < b.length;
        for (int i = 0; i < a.length; i++) {
            if (i >= b.length) {
                break;
            }
            if (a[i] < b[i]) {
                r = true;
                break;
            } else if (a[i] > b[i]) {
                r = false;
                break;
            }
        }
        stack.addFirst(new Cell(r));
    }

    private void doEQA()
    {
        ip++;
        Cell b = stack.removeFirst();
        Cell a = stack.removeFirst();
        stack.addFirst(new Cell(a.equals(b)));
    }

    private void doNEA()
    {
        ip++;
        Cell b = stack.removeFirst();
        Cell a = stack.removeFirst();
        stack.addFirst(new Cell(!a.equals(b)));
    }

    private void doEQP()
    {
        ip++;
        Cell b = stack.removeFirst().getAddress();
        Cell a = stack.removeFirst().getAddress();
        stack.addFirst(new Cell(a == b));
    }

    private void doNEP()
    {
        ip++;
        Cell b = stack.removeFirst().getAddress();
        Cell a = stack.removeFirst().getAddress();
        stack.addFirst(new Cell(a != b));
    }

    private void doNOTB()
    {
        ip++;
        boolean x = stack.removeFirst().getBoolean();
        stack.addFirst(new Cell(!x));
    }

    private void doINDEXAR()
    {
        ip++;
        BigDecimal index = stack.removeFirst().getNumber();
        List<Cell> array = stack.removeFirst().getAddress().getArray();
        try {
            index.intValueExact();
        } catch (ArithmeticException x) {
            raiseLiteral("PANIC", "Array index not an integer: " + index.toString());
            return;
        }
        if (index.signum() < 0) {
            raiseLiteral("PANIC", "Array index is negative: " + index.toString());
            return;
        }
        if (index.intValue() >= array.size()) {
            raiseLiteral("PANIC", "Array index exceeds size " + array.size() + ": " + index.toString());
            return;
        }
        stack.addFirst(new Cell(array.get(index.intValue())));
    }

    private void doINDEXAW()
    {
        ip++;
        BigDecimal index = stack.removeFirst().getNumber();
        List<Cell> array = stack.removeFirst().getAddress().getArray();
        try {
            index.intValueExact();
        } catch (ArithmeticException x) {
            raiseLiteral("PANIC", "Array index not an integer: " + index.toString());
            return;
        }
        if (index.signum() < 0) {
            raiseLiteral("PANIC", "Array index is negative: " + index.toString());
            return;
        }
        int i = index.intValue();
        while (i >= array.size()) {
            array.add(new Cell());
        }
        stack.addFirst(new Cell(array.get(i)));
    }

    private void doINDEXAV()
    {
        ip++;
        BigDecimal index = stack.removeFirst().getNumber();
        List<Cell> array = stack.removeFirst().getArray();
        try {
            index.intValueExact();
        } catch (ArithmeticException x) {
            raiseLiteral("PANIC", "Array index not an integer: " + index.toString());
            return;
        }
        if (index.signum() < 0) {
            raiseLiteral("PANIC", "Array index is negative: " + index.toString());
            return;
        }
        if (index.intValue() >= array.size()) {
            raiseLiteral("PANIC", "Array index exceeds size " + array.size() + ": " + index.toString());
            return;
        }
        stack.addFirst(array.get(index.intValue()));
    }

    private void doINDEXAN()
    {
        ip++;
        BigDecimal index = stack.removeFirst().getNumber();
        List<Cell> array = stack.removeFirst().getArray();
        try {
            index.intValueExact();
        } catch (ArithmeticException x) {
            raiseLiteral("PANIC", "Array index not an integer: " + index.toString());
            return;
        }
        if (index.signum() < 0) {
            raiseLiteral("PANIC", "Array index is negative: " + index.toString());
            return;
        }
        int i = index.intValue();
        if (i < array.size()) {
            stack.addFirst(array.get(i));
        } else {
            stack.addFirst(new Cell());
        }
    }

    private void doINDEXDR()
    {
        ip++;
        String key = stack.removeFirst().getString();
        Map<String, Cell> dictionary = stack.removeFirst().getAddress().getDictionary();
        Cell r = dictionary.get(key);
        if (r == null) {
            raiseLiteral("PANIC", "Dictionary key not found: " + key);
            return;
        }
        stack.addFirst(new Cell(r));
    }

    private void doINDEXDW()
    {
        ip++;
        String key = stack.removeFirst().getString();
        Map<String, Cell> dictionary = stack.removeFirst().getAddress().getDictionary();
        Cell r = dictionary.get(key);
        if (r == null) {
            r = new Cell();
            dictionary.put(key, r);
        }
        stack.addFirst(new Cell(r));
    }

    private void doINDEXDV()
    {
        ip++;
        String key = stack.removeFirst().getString();
        Map<String, Cell> dictionary = stack.removeFirst().getDictionary();
        Cell r = dictionary.get(key);
        if (r == null) {
            raiseLiteral("PANIC", "Dictionary key not found: " + key);
            return;
        }
        stack.addFirst(r);
    }

    private void doINA()
    {
        ip++;
        List<Cell> array = stack.removeFirst().getArray();
        Cell val = stack.removeFirst();
        for (Cell x: array) {
            if (x.equals(val)) {
                stack.addFirst(new Cell(true));
                return;
            }
        }
        stack.addFirst(new Cell(false));
    }

    private void doIND()
    {
        ip++;
        Map<String, Cell> dict = stack.removeFirst().getDictionary();
        String val = stack.removeFirst().getString();
        stack.addFirst(new Cell(dict.containsKey(val)));
    }

    private void doCALLP()
    {
        ip++;
        int func = getVint();
        GenericFunction f = predefined.get(object.strtable[func]);
        if (f != null) {
            f.call();
        } else {
            System.err.println("Unknown function: " + object.strtable[func]);
            System.exit(1);
        }
    }

    private void doCALLF()
    {
        ip++;
        int target = getVint();
        invoke(target);
    }

    private void doJUMP()
    {
        ip++;
        int target = getVint();
        ip = target;
    }

    private void doJF()
    {
        ip++;
        int target = getVint();
        boolean a = stack.removeFirst().getBoolean();
        if (!a) {
            ip = target;
        }
    }

    private void doJT()
    {
        ip++;
        int target = getVint();
        boolean a = stack.removeFirst().getBoolean();
        if (a) {
            ip = target;
        }
    }

    private void doDUP()
    {
        ip++;
        stack.addFirst(stack.getFirst().copy());
    }

    private void doDUPX1()
    {
        ip++;
        Cell a = stack.removeFirst();
        Cell b = stack.removeFirst();
        stack.addFirst(a.copy());
        stack.addFirst(b.copy());
        stack.addFirst(a.copy());
    }

    private void doDROP()
    {
        ip++;
        stack.removeFirst();
    }

    private void doRET()
    {
        frames.removeFirst();
        ip = callstack.removeFirst();
    }

    private void doCONSA()
    {
        ip++;
        int val = getVint();
        List<Cell> a = new ArrayList<Cell>();
        while (val > 0) {
            a.add(stack.removeFirst());
            val--;
        }
        stack.addFirst(new Cell(a));
    }

    private void doCONSD()
    {
        ip++;
        int val = getVint();
        Map<String, Cell> d = new TreeMap<String, Cell>();
        while (val > 0) {
            Cell value = stack.removeFirst();
            String key = stack.removeFirst().getString();
            d.put(key, value);
            val--;
        }
        stack.addFirst(new Cell(d));
    }

    private void doEXCEPT()
    {
        int start_ip = ip;
        ip++;
        int val = getVint();
        ip = start_ip;
        NeObject info = stack.removeFirst().getObject();
        raiseLiteral(object.strtable[val], info);
    }

    private void doALLOC()
    {
        ip++;
        int val = getVint();
        stack.addFirst(new Cell(new Cell(new ArrayList<Cell>(val))));
    }

    private void doPUSHNIL()
    {
        ip++;
        stack.addFirst(new Cell());
    }

    private void doRESETC()
    {
        ip++;
        Cell addr = stack.removeFirst().getAddress();
        addr.reset();
    }

    private void doJUMPTBL()
    {
        ip++;
        int val = getVint();
        BigDecimal n = stack.removeFirst().getNumber();
        try {
            int i = n.intValueExact();
            if (i >= 0 && i < val) {
                ip += 6 * i;
            } else {
                ip += 6 * val;
            }
        } catch (ArithmeticException x) {
            ip += 6 * val;
        }
    }

    private void doSWAP()
    {
        ip++;
        Cell a = stack.removeFirst();
        Cell b = stack.removeFirst();
        stack.addFirst(a);
        stack.addFirst(b);
    }

    private void doCALLV()
    {
        ip++;
        int val = getVint();
        List<Cell> pi = stack.removeFirst().getArray();
        Cell instance = pi.get(0).getAddress();
        int interface_index = pi.get(1).getNumber().intValueExact();
        //Module *m = reinterpret_cast<Module *>(instance->array_for_write()[0].array_for_write()[0].other());
        Bytecode.ClassInfo classinfo = (Bytecode.ClassInfo)instance.getArray().get(0).getGeneric()[1];
        invoke(classinfo.interfaces[interface_index][val]);
    }

    private void doPUSHCI()
    {
        ip++;
        int val = getVint();
        String name = object.strtable[val];
        if (!name.contains(".")) {
            for (Bytecode.ClassInfo c: object.classes) {
                if (c.name == val) {
                    Cell ci = new Cell(new Object[] {null, c});
                    stack.addFirst(ci);
                    return;
                }
            }
        } else {
            // TODO
        }
        System.err.println("neon: unknown class name " + name);
        System.exit(1);
    }

    private void invoke(int index)
    {
        callstack.addFirst(ip);
        ActivationFrame outer = null;
        int nest = object.functions[index].nest;
        int params = object.functions[index].params;
        int locals = object.functions[index].locals;
        if (frames.size() > 0) {
            assert nest <= frames.getFirst().nesting_depth+1;
            outer = frames.getFirst();
            while (outer != null && nest <= outer.nesting_depth) {
                assert outer.outer == null || outer.nesting_depth == outer.outer.nesting_depth+1;
                outer = outer.outer;
            }
        }
        ActivationFrame frame = new ActivationFrame(nest, outer, locals, stack.size() - params);
        frames.addFirst(frame);
        ip = object.functions[index].entry;
    }

    private void raiseLiteral(String name)
    {
        raiseLiteral(name, "");
    }

    private void raiseLiteral(String name, String info)
    {
        raiseLiteral(name, new NeObjectString(info));
    }

    private void raiseLiteral(String name, NeObject info)
    {
        Cell exceptionvar = new Cell(new ArrayList<Cell>(Arrays.asList(
            new Cell(name),
            new Cell(info),
            new Cell(BigDecimal.valueOf(ip))
        )));

        int tip = ip;
        Iterator<Integer> si = callstack.iterator();
        int depth = 0;
        for (;;) {
            for (Bytecode.ExceptionInfo e: object.exceptions) {
                if (tip >= e.start && tip < e.end) {
                    String handler = object.strtable[e.excid];
                    if (name.equals(handler) || (name.length() > handler.length() && name.startsWith(handler) && name.charAt(handler.length()) == '.')) {
                        ip = e.handler;
                        while (depth > 0) {
                            callstack.removeFirst();
                            depth--;
                        }
                        stack.addFirst(exceptionvar);
                        return;
                    }

                }
            }
            if (!si.hasNext()) {
                break;
            }
            depth++;
            tip = si.next();
        }

        System.err.println("Unhandled exception " + name + " (" + info + ")");
        System.exit(1);
    }

    public enum Opcode {
        PUSHB,
        PUSHN,
        PUSHS,
        PUSHY,
        PUSHPG,
        PUSHPPG,
        PUSHPMG,
        PUSHPL,
        PUSHPOL,
        PUSHI,
        LOADB,
        LOADN,
        LOADS,
        LOADY,
        LOADA,
        LOADD,
        LOADP,
        LOADJ,
        STOREB,
        STOREN,
        STORES,
        STOREY,
        STOREA,
        STORED,
        STOREP,
        STOREJ,
        NEGN,
        ADDN,
        SUBN,
        MULN,
        DIVN,
        MODN,
        EXPN,
        EQB,
        NEB,
        EQN,
        NEN,
        LTN,
        GTN,
        LEN,
        GEN,
        EQS,
        NES,
        LTS,
        GTS,
        LES,
        GES,
        EQY,
        NEY,
        LTY,
        GTY,
        LEY,
        GEY,
        EQA,
        NEA,
        EQD,
        NED,
        EQP,
        NEP,
        ANDB,
        ORB,
        NOTB,
        INDEXAR,
        INDEXAW,
        INDEXAV,
        INDEXAN,
        INDEXDR,
        INDEXDW,
        INDEXDV,
        INA,
        IND,
        CALLP,
        CALLF,
        CALLMF,
        CALLI,
        JUMP,
        JF,
        JT,
        DUP,
        DUPX1,
        DROP,
        RET,
        CONSA,
        CONSD,
        EXCEPT,
        ALLOC,
        PUSHNIL,
        RESETC,
        PUSHPEG,
        JUMPTBL,
        CALLX,
        SWAP,
        DROPN,
        PUSHFP,
        CALLV,
        PUSHCI,
    }

    private interface GenericFunction {
        void call();
    }

    private Opcode[] opcodes = Opcode.values();
    private Map<String, GenericFunction> predefined;

    private List<Cell> args;
    private Bytecode object;
    private int ip;
    private ArrayDeque<Integer> callstack;
    private ArrayDeque<Cell> stack;
    private Cell[] globals;
    private ArrayDeque<ActivationFrame> frames;
    java.util.Random gen = new java.util.Random();

    private void array__append()
    {
        Cell v = stack.removeFirst();
        List<Cell> a = stack.removeFirst().getAddress().getArray();
        a.add(v);
    }

    private void array__concat()
    {
        List<Cell> b = stack.removeFirst().getArray();
        List<Cell> a = stack.removeFirst().getArray();
        List<Cell> r = new ArrayList<Cell>();
        r.addAll(a);
        r.addAll(b);
        stack.addFirst(new Cell(r));
    }

    private void array__extend()
    {
        List<Cell> v = stack.removeFirst().getArray();
        List<Cell> a = stack.removeFirst().getAddress().getArray();
        a.addAll(v);
    }

    private void array__find()
    {
        Cell e = stack.removeFirst();
        List<Cell> a = stack.removeFirst().getArray();
        for (int i = 0; i < a.size(); i++) {
            if (a.get(i).equals(e)) {
                stack.addFirst(new Cell(BigDecimal.valueOf(i)));
                return;
            }
        }
        raiseLiteral("PANIC", "value not found in array");
    }

    private void array__remove()
    {
        int index = stack.removeFirst().getNumber().intValueExact();
        List<Cell> a = stack.removeFirst().getAddress().getArray();
        a.remove(index);
    }

    private void array__reversed()
    {
        List<Cell> a = stack.removeFirst().getArray();
        List<Cell> r = new ArrayList<Cell>();
        for (int i = 0; i < a.size(); i++) {
            r.add(a.get(i).copy());
        }
        java.util.Collections.reverse(r);
        stack.addFirst(new Cell(r));
    }

    private void array__size()
    {
        List<Cell> a = stack.removeFirst().getArray();
        stack.addFirst(new Cell(BigDecimal.valueOf(a.size())));
    }

    private void array__slice()
    {
        boolean last_from_end = stack.removeFirst().getBoolean();
        BigDecimal nlast = stack.removeFirst().getNumber();
        boolean first_from_end = stack.removeFirst().getBoolean();
        BigDecimal nfirst = stack.removeFirst().getNumber();
        List<Cell> a = stack.removeFirst().getArray();
        if (nfirst.stripTrailingZeros().scale() > 0) {
            raiseLiteral("PANIC", "First index not an integer: " + nfirst.toString());
            return;
        }
        if (nlast.stripTrailingZeros().scale() > 0) {
            raiseLiteral("PANIC", "Last index not an integer: " + nlast.toString());
            return;
        }
        int first = nfirst.intValue();
        int last = nlast.intValue();
        if (first_from_end) {
            first += a.size() - 1;
        }
        if (first < 0) {
            first = 0;
        }
        if (first > a.size()) {
            first = a.size();
        }
        if (last_from_end) {
            last += a.size() - 1;
        }
        if (last >= a.size()) {
            last = a.size() - 1;
        }
        if (last < first) {
            last = first - 1;
        }
        stack.addFirst(new Cell(a.subList(first, last+1)));
    }

    private void array__splice()
    {
        boolean last_from_end = stack.removeFirst().getBoolean();
        int last = stack.removeFirst().getNumber().intValueExact();
        boolean first_from_end = stack.removeFirst().getBoolean();
        int first = stack.removeFirst().getNumber().intValueExact();
        List<Cell> a = stack.removeFirst().getArray();
        List<Cell> b = stack.removeFirst().getArray();
        if (first_from_end) {
            first += a.size() - 1;
        }
        if (last_from_end) {
            last += a.size() - 1;
        }
        List<Cell> r = new ArrayList<Cell>();
        r.addAll(a.subList(0, first));
        r.addAll(b);
        r.addAll(a.subList(last+1, a.size()));
        stack.addFirst(new Cell(r));
    }

    private void array__toBytes__number()
    {
        List<Cell> a = stack.removeFirst().getArray();
        byte[] r = new byte[a.size()];
        int i = 0;
        for (Cell c: a) {
            int x = c.getNumber().intValue();
            if (x < 0 || x >= 256) {
                raiseLiteral("PANIC", "Byte value out of range at offset " + i + ": " + x);
                return;
            }
            r[i] = (byte) c.getNumber().intValue();
            i++;
        }
        stack.addFirst(new Cell(r));
    }

    private void array__toString__number()
    {
        List<Cell> a = stack.removeFirst().getArray();
        StringBuilder r = new StringBuilder("[");
        boolean first = true;
        for (Cell x: a) {
            if (first) {
                first = false;
            } else {
                r.append(", ");
            }
            r.append(x.getNumber());
        }
        r.append("]");
        stack.addFirst(new Cell(r.toString()));
    }

    private void array__toString__string()
    {
        List<Cell> a = stack.removeFirst().getArray();
        StringBuilder r = new StringBuilder("[");
        boolean first = true;
        for (Cell x: a) {
            if (first) {
                first = false;
            } else {
                r.append(", ");
            }
            r.append(Util.quoted(x.getString()));
        }
        r.append("]");
        stack.addFirst(new Cell(r.toString()));
    }

    private void array__toString__object()
    {
        List<Cell> a = stack.removeFirst().getArray();
        StringBuilder r = new StringBuilder("[");
        boolean first = true;
        for (Cell x: a) {
            if (first) {
                first = false;
            } else {
                r.append(", ");
            }
            r.append(x.getObject().toLiteralString());
        }
        r.append("]");
        stack.addFirst(new Cell(r.toString()));
    }

    private void boolean__toString()
    {
        boolean b = stack.removeFirst().getBoolean();
        stack.addFirst(new Cell(b ? "TRUE" : "FALSE"));
    }

    private void bytes__append()
    {
        byte[] b = stack.removeFirst().getBytes();
        Cell ac = stack.removeFirst().getAddress();
        byte[] a = ac.getBytes();
        byte[] t = new byte[a.length + b.length];
        System.arraycopy(a, 0, t, 0, a.length);
        System.arraycopy(b, 0, t, a.length, b.length);
        ac.set(t);
    }

    private void bytes__concat()
    {
        byte[] b = stack.removeFirst().getBytes();
        byte[] a = stack.removeFirst().getBytes();
        byte[] r = new byte[a.length + b.length];
        System.arraycopy(a, 0, r, 0, a.length);
        System.arraycopy(b, 0, r, a.length, b.length);
        stack.addFirst(new Cell(r));
    }

    private void bytes__decodeUTF8()
    {
        byte[] b = stack.removeFirst().getBytes();
        stack.addFirst(new Cell(new Cell[]{new Cell(BigDecimal.ZERO), new Cell(new String(b))}));
    }

    private void bytes__index()
    {
        int index = stack.removeFirst().getNumber().intValueExact();
        byte[] b = stack.removeFirst().getBytes();
        stack.addFirst(new Cell(BigDecimal.valueOf(b[index])));
    }

    private void bytes__range()
    {
        boolean last_from_end = stack.removeFirst().getBoolean();
        int last = stack.removeFirst().getNumber().intValueExact();
        boolean first_from_end = stack.removeFirst().getBoolean();
        int first = stack.removeFirst().getNumber().intValueExact();
        byte[] b = stack.removeFirst().getBytes();
        if (first_from_end) {
            first += b.length - 1;
        }
        if (last_from_end) {
            last += b.length - 1;
        }
        stack.addFirst(new Cell(Arrays.copyOfRange(b, first, last+1)));
    }

    private void bytes__size()
    {
        byte[] b = stack.removeFirst().getBytes();
        stack.addFirst(new Cell(BigDecimal.valueOf(b.length)));
    }

    private void bytes__splice()
    {
        boolean last_from_end = stack.removeFirst().getBoolean();
        int last = stack.removeFirst().getNumber().intValueExact();
        boolean first_from_end = stack.removeFirst().getBoolean();
        int first = stack.removeFirst().getNumber().intValueExact();
        byte[] a = stack.removeFirst().getBytes();
        byte[] b = stack.removeFirst().getBytes();
        if (first_from_end) {
            first += a.length - 1;
        }
        if (last_from_end) {
            last += a.length - 1;
        }
        byte[] r = new byte[first + b.length + a.length-(last+1)];
        System.arraycopy(a, 0, r, 0, first);
        System.arraycopy(b, 0, r, first, b.length);
        System.arraycopy(a, last+1, r, first+b.length, a.length-(last+1));
        stack.addFirst(new Cell(r));
    }

    private void bytes__store()
    {
        int index = stack.removeFirst().getNumber().intValueExact();
        byte[] s = stack.removeFirst().getAddress().getBytes();
        int b = stack.removeFirst().getNumber().intValueExact();
        s[index] = (byte)b;
    }

    private void bytes__toArray()
    {
        byte[] b = stack.removeFirst().getBytes();
        List<Cell> a = new ArrayList<Cell>();
        for (byte x: b) {
            a.add(new Cell(BigDecimal.valueOf(x & 0xff)));
        }
        stack.addFirst(new Cell(a));
    }

    private void bytes__toString()
    {
        byte[] b = stack.removeFirst().getBytes();
        StringBuilder r = new StringBuilder("HEXBYTES \"");
        boolean first = true;
        for (byte c: b) {
            if (!first) {
                r.append(' ');
            } else {
                first = false;
            }
            r.append(String.format("%02x", c & 0xff));
        }
        r.append("\"");
        stack.addFirst(new Cell(r.toString()));
    }

    private void dictionary__keys()
    {
        Map<String, Cell> d = stack.removeFirst().getDictionary();
        List<Cell> r = new ArrayList<Cell>();
        for (String k: d.keySet()) {
            r.add(new Cell(k));
        }
        stack.addFirst(new Cell(r));
    }

    private void dictionary__remove()
    {
        String key = stack.removeFirst().getString();
        Map<String, Cell> d = stack.removeFirst().getAddress().getDictionary();
        d.remove(key);
    }

    private void dictionary__toString__object()
    {
        Map<String, Cell> d = stack.removeFirst().getDictionary();
        StringBuilder r = new StringBuilder("{");
        boolean first = true;
        for (String k: d.keySet()) {
            if (first) {
                first = false;
            } else {
                r.append(", ");
            }
            r.append(Util.quoted(k));
            r.append(": ");
            r.append(d.get(k).getObject().toLiteralString());
        }
        r.append("}");
        stack.addFirst(new Cell(r.toString()));
    }

    private void dictionary__toString__string()
    {
        Map<String, Cell> d = stack.removeFirst().getDictionary();
        StringBuilder r = new StringBuilder("{");
        boolean first = true;
        for (String k: d.keySet()) {
            if (first) {
                first = false;
            } else {
                r.append(", ");
            }
            r.append(Util.quoted(k));
            r.append(": ");
            r.append(Util.quoted(d.get(k).getString()));
        }
        r.append("}");
        stack.addFirst(new Cell(r.toString()));
    }

    private void num()
    {
        String s = stack.removeFirst().getString();
        boolean any_digits = false;
        for (int i = 0; i < s.length(); i++) {
            if (s.charAt(i) >= '0' && s.charAt(i) <= '9') {
                any_digits = true;
                break;
            }
        }
        if (any_digits) {
            try {
                stack.addFirst(new Cell(new BigDecimal(s)));
            } catch (NumberFormatException x) {
                raiseLiteral("PANIC", "num() argument not a number");
            }
        } else {
            raiseLiteral("PANIC", "num() argument not a number");
        }
    }

    private void number__toString()
    {
        BigDecimal n = stack.removeFirst().getNumber();
        String s = n.toString();
        if (s.indexOf('.') >= 0) {
            while (s.length() > 1 && s.charAt(s.length()-1) == '0') {
                s = s.substring(0, s.length()-1);
            }
            if (s.charAt(s.length()-1) == '.') {
                s = s.substring(0, s.length()-1);
            }
        }
        stack.addFirst(new Cell(s));
    }

    private void object__getArray()
    {
        NeObject o = stack.removeFirst().getObject();
        if (o == null) {
            raiseLiteral("DynamicConversionException");
            return;
        }
        List<NeObject> a = o.getArray();
        if (a == null) {
            raiseLiteral("DynamicConversionException");
            return;
        }
        List<Cell> r = new ArrayList<Cell>();
        for (NeObject x: a) {
            r.add(new Cell(x));
        }
        stack.addFirst(new Cell(r));
    }

    private void object__getBoolean()
    {
        NeObject o = stack.removeFirst().getObject();
        if (o == null) {
            raiseLiteral("DynamicConversionException");
            return;
        }
        Boolean b = o.getBoolean();
        if (b == null) {
            raiseLiteral("DynamicConversionException");
            return;
        }
        stack.addFirst(new Cell(b));
    }

    private void object__getBytes()
    {
        NeObject o = stack.removeFirst().getObject();
        if (o == null) {
            raiseLiteral("DynamicConversionException");
            return;
        }
        byte[] b = o.getBytes();
        if (b == null) {
            raiseLiteral("DynamicConversionException");
            return;
        }
        stack.addFirst(new Cell(b));
    }

    private void object__getDictionary()
    {
        NeObject o = stack.removeFirst().getObject();
        if (o == null) {
            raiseLiteral("DynamicConversionException");
            return;
        }
        Map<String, NeObject> d = o.getDictionary();
        if (d == null) {
            raiseLiteral("DynamicConversionException");
            return;
        }
        Map<String, Cell> r = new TreeMap<String, Cell>();
        for (Map.Entry<String, NeObject> x: d.entrySet()) {
            r.put(x.getKey(), new Cell(x.getValue()));
        }
        stack.addFirst(new Cell(r));
    }

    private void object__getNumber()
    {
        NeObject o = stack.removeFirst().getObject();
        if (o == null) {
            raiseLiteral("DynamicConversionException");
            return;
        }
        BigDecimal n = o.getNumber();
        if (n == null) {
            raiseLiteral("DynamicConversionException");
            return;
        }
        stack.addFirst(new Cell(n));
    }

    private void object__getString()
    {
        NeObject o = stack.removeFirst().getObject();
        if (o == null) {
            raiseLiteral("DynamicConversionException");
            return;
        }
        String s = o.getString();
        if (s == null) {
            raiseLiteral("DynamicConversionException");
            return;
        }
        stack.addFirst(new Cell(s));
    }

    private void object__invokeMethod()
    {
        List<Cell> a = stack.removeFirst().getArray();
        String name = stack.removeFirst().getString();
        List<NeObject> args = new ArrayList<NeObject>();
        for (Cell x: a) {
            args.add(x.getObject());
        }
        NeObject r = stack.removeFirst().getObject().invokeMethod(name, args);
        stack.addFirst(new Cell(r));
    }

    private void object__isNull()
    {
        NeObject o = stack.removeFirst().getObject();
        stack.addFirst(new Cell(o == null));
    }

    private void object__makeArray()
    {
        List<Cell> a = stack.removeFirst().getArray();
        List<NeObject> r = new ArrayList<NeObject>();
        for (Cell x: a) {
            r.add(x.getObject());
        }
        stack.addFirst(new Cell(new NeObjectArray(r)));
    }

    private void object__makeBoolean()
    {
        boolean b = stack.removeFirst().getBoolean();
        stack.addFirst(new Cell(new NeObjectBoolean(b)));
    }

    private void object__makeBytes()
    {
        byte[] b = stack.removeFirst().getBytes();
        stack.addFirst(new Cell(new NeObjectBytes(b)));
    }

    private void object__makeDictionary()
    {
        Map<String, Cell> d = stack.removeFirst().getDictionary();
        Map<String, NeObject> r = new TreeMap<String, NeObject>();
        for (Map.Entry<String, Cell> x: d.entrySet()) {
            r.put(x.getKey(), x.getValue().getObject());
        }
        stack.addFirst(new Cell(new NeObjectDictionary(r)));
    }

    private void object__makeNull()
    {
        stack.addFirst(new Cell((NeObject)null));
    }

    private void object__makeNumber()
    {
        BigDecimal n = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(new NeObjectNumber(n)));
    }

    private void object__makeString()
    {
        String s = stack.removeFirst().getString();
        stack.addFirst(new Cell(new NeObjectString(s)));
    }

    private void object__setProperty()
    {
        NeObject i = stack.removeFirst().getObject();
        NeObject o = stack.removeFirst().getObject();
        NeObject v = stack.removeFirst().getObject();
        if (o == null) {
            raiseLiteral("DynamicConversionException", "object is null");
            return;
        }
        if (i == null) {
            raiseLiteral("DynamicConversionException", "index is null");
            return;
        }
        o.setProperty(i, v);
    }

    private void object__subscript()
    {
        NeObject i = stack.removeFirst().getObject();
        NeObject o = stack.removeFirst().getObject();
        if (o == null) {
            raiseLiteral("DynamicConversionException", "object is null");
            return;
        }
        if (i == null) {
            raiseLiteral("DynamicConversionException", "index is null");
            return;
        }
        NeObject r = o.subscript(i);
        if (r == null) {
            raiseLiteral("ObjectSubscriptException", i.toString());
            return;
        }
        stack.addFirst(new Cell(r));
    }

    private void object__toString()
    {
        NeObject o = stack.removeFirst().getObject();
        stack.addFirst(new Cell(o != null ? o.toString() : "null"));
    }

    private void print()
    {
        NeObject x = stack.removeFirst().getObject();
        if (x != null) {
            System.out.println(x.toString());
        } else {
            System.out.println("NIL");
        }
    }

    private void string__append()
    {
        String b = stack.removeFirst().getString();
        Cell a = stack.removeFirst().getAddress();
        a.set(a.getString() + b);
    }

    private void string__concat()
    {
        String b = stack.removeFirst().getString();
        String a = stack.removeFirst().getString();
        stack.addFirst(new Cell(a + b));
    }

    private void string__index()
    {
        int index = stack.removeFirst().getNumber().intValue();
        String s = stack.removeFirst().getString();
        stack.addFirst(new Cell(s.substring(index, index+1)));
    }

    private void string__length()
    {
        String s = stack.removeFirst().getString();
        stack.addFirst(new Cell(BigDecimal.valueOf(s.length())));
    }

    private void string__substring()
    {
        boolean last_from_end = stack.removeFirst().getBoolean();
        int last = stack.removeFirst().getNumber().intValue();
        boolean first_from_end = stack.removeFirst().getBoolean();
        int first = stack.removeFirst().getNumber().intValue();
        String s = stack.removeFirst().getString();
        if (first_from_end) {
            first += s.length() - 1;
        }
        if (last_from_end) {
            last += s.length() - 1;
        }
        stack.addFirst(new Cell(s.substring(first, last+1)));
    }

    private void string__encodeUTF8()
    {
        String s = stack.removeFirst().getString();
        stack.addFirst(new Cell(s.getBytes()));
    }

    private void string__toString()
    {
        String s = stack.removeFirst().getString();
        stack.addFirst(new Cell(s));
    }

    private void math$abs()
    {
        BigDecimal x = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(x.abs()));
    }

    private void math$atan()
    {
        BigDecimal x = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(BigDecimal.valueOf(Math.atan(x.doubleValue()))));
    }

    private void math$ceil()
    {
        BigDecimal x = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(x.divide(BigDecimal.ONE, BigDecimal.ROUND_CEILING)));
    }

    private void math$cos()
    {
        BigDecimal x = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(BigDecimal.valueOf(Math.cos(x.doubleValue()))));
    }

    private void math$exp()
    {
        BigDecimal x = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(BigDecimal.valueOf(Math.exp(x.doubleValue()))));
    }

    private void math$floor()
    {
        BigDecimal x = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(x.divide(BigDecimal.ONE, BigDecimal.ROUND_FLOOR)));
    }

    private void math$log()
    {
        BigDecimal x = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(BigDecimal.valueOf(Math.log(x.doubleValue()))));
    }

    private void math$odd()
    {
        BigDecimal x = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(x.remainder(new BigDecimal(2)).compareTo(BigDecimal.ZERO) != 0));
    }

    private void math$sign()
    {
        BigDecimal x = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(new BigDecimal(x.signum())));
    }

    private void math$sin()
    {
        BigDecimal x = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(BigDecimal.valueOf(Math.sin(x.doubleValue()))));
    }

    private void math$sqrt()
    {
        BigDecimal x = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(BigDecimal.valueOf(Math.sqrt(x.doubleValue()))));
    }

    private void math$trunc()
    {
        BigDecimal x = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(x.divide(BigDecimal.ONE, BigDecimal.ROUND_DOWN)));
    }

    private void random$uint32()
    {
        stack.addFirst(new Cell(BigDecimal.valueOf(gen.nextInt(0x10000000))));
    }

    private void runtime$assertionsEnabled()
    {
        stack.addFirst(new Cell(enable_assert));
    }

    private void runtime$createObject()
    {
        String name = stack.removeFirst().getString();
        try {
            Object obj = Class.forName(name).newInstance();
            stack.addFirst(new Cell(new NeObjectNative(obj)));
        } catch (ClassNotFoundException cnfe) {
            stack.addFirst(new Cell(new NeObjectNative(null))); // TODO: exception
        } catch (InstantiationException ie) {
            stack.addFirst(new Cell(new NeObjectNative(null))); // TODO: exception
        } catch (IllegalAccessException iae) {
            stack.addFirst(new Cell(new NeObjectNative(null))); // TODO: exception
        }
    }

    private void runtime$debugEnabled()
    {
        stack.addFirst(new Cell(enable_debug));
    }

    private void runtime$executorName()
    {
        stack.addFirst(new Cell("jnex"));
    }

    private void runtime$moduleIsMain()
    {
        stack.addFirst(new Cell(true)); // TODO modules
    }

    private void string$fromCodePoint()
    {
        BigDecimal n = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(new String(new int[] {n.intValue()}, 0, 1)));
    }

    private void string$toCodePoint()
    {
        String s = stack.removeFirst().getString();
        stack.addFirst(new Cell(BigDecimal.valueOf(s.charAt(0))));
    }

    private void sys$exit()
    {
        BigDecimal n = stack.removeFirst().getNumber();
        System.exit(n.intValue());
    }

    private void textio$close()
    {
        NeObject f = stack.removeFirst().getObject();
        try {
            ((java.io.Closeable)f.getNative()).close();
        } catch (java.io.IOException e) {
            raiseLiteral("TextioException");
        }
    }

    private void textio$open()
    {
        int mode = stack.removeFirst().getNumber().intValueExact();
        String name = stack.removeFirst().getString();
        try {
            Object f;
            switch (mode) {
                case 0: // TODO: enum read
                    f = new java.io.BufferedReader(new java.io.FileReader(name));
                    break;
                case 1: // TODO: enum write
                    f = new java.io.PrintStream(new java.io.FileOutputStream(name));
                    break;
                default:
                    raiseLiteral("TextioException.Open", "open error");
                    return;
            }
            stack.addFirst(new Cell(new NeObjectNative(f)));
        } catch (java.io.FileNotFoundException e) {
            raiseLiteral("TextioException.Open", "open error");
        }
    }

    private void textio$readLine()
    {
        NeObject f = stack.removeFirst().getObject();
        try {
            String s = ((java.io.BufferedReader)f.getNative()).readLine();
            stack.addFirst(new Cell(true));
            stack.addFirst(new Cell(s));
        } catch (java.io.IOException e) {
            raiseLiteral("TextioException");
        }
    }

    private void textio$writeLine()
    {
        String s = stack.removeFirst().getString();
        java.io.PrintStream f = (java.io.PrintStream)stack.removeFirst().getObject().getNative();
        f.println(s);
    }

    private void time$now()
    {
        stack.addFirst(new Cell(BigDecimal.valueOf(System.currentTimeMillis()).divide(BigDecimal.valueOf(1000))));
    }
}
