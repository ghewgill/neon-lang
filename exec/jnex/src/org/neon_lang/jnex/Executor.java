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

    Executor(DataInput in)
    {
        predefined = new HashMap<String, GenericFunction>();
        predefined.put("array__append", this::array__append);
        predefined.put("array__concat", this::array__concat);
        predefined.put("array__extend", this::array__extend);
        predefined.put("array__size", this::array__size);
        predefined.put("array__slice", this::array__slice);
        predefined.put("array__splice", this::array__splice);
        predefined.put("array__toBytes__number", this::array__toBytes__number);
        predefined.put("array__toString__number", this::array__toString__number);
        predefined.put("boolean__toString", this::boolean__toString);
        predefined.put("bytes__decodeToString", this::bytes__decodeToString);
        predefined.put("bytes__range", this::bytes__range);
        predefined.put("bytes__size", this::bytes__size);
        predefined.put("bytes__splice", this::bytes__splice);
        predefined.put("bytes__toArray", this::bytes__toArray);
        predefined.put("bytes__toString", this::bytes__toString);
        predefined.put("chr", this::chr);
        predefined.put("concat", this::concat);
        predefined.put("dictionary__keys", this::dictionary__keys);
        predefined.put("number__toString", this::number__toString);
        predefined.put("ord", this::ord);
        predefined.put("print", this::print);
        predefined.put("str", this::number__toString);
        predefined.put("strb", this::boolean__toString);
        predefined.put("string__append", this::string__append);
        predefined.put("string__length", this::string__length);
        predefined.put("string__substring", this::string__substring);
        predefined.put("string__toBytes", this::string__toBytes);

        object = new Bytecode(in);
        ip = 0;
        callstack = new ArrayDeque<Integer>();
        stack = new ArrayDeque<Cell>();
        globals = new Cell[object.global_size];
        for (int i = 0; i < object.global_size; i++) {
            globals[i] = new Cell();
        }
        frames = new ArrayDeque<Cell[]>();
    }

    void run()
    {
        callstack.addFirst(object.code.size());
        while (ip < object.code.size()) {
            //System.err.println("ip=" + ip + " op=" + opcodes[object.code.get(ip)]);
            //for (Cell c: stack) { System.err.println("  " + c); }
            switch (opcodes[object.code.get(ip)]) {
                case ENTER: doENTER(); break;
                case LEAVE: doLEAVE(); break;
                case PUSHB: doPUSHB(); break;
                case PUSHN: doPUSHN(); break;
                case PUSHS: doPUSHS(); break;
                case PUSHT: doPUSHT(); break;
                case PUSHPG: doPUSHPG(); break;
                //case PUSHPPG
                //case PUSHPMG
                case PUSHPL: doPUSHPL(); break;
                case PUSHPOL: doPUSHPOL(); break;
                case PUSHI: doPUSHI(); break;
                case LOADB: doLOADB(); break;
                case LOADN: doLOADN(); break;
                case LOADS: doLOADS(); break;
                case LOADT: doLOADT(); break;
                case LOADA: doLOADA(); break;
                case LOADD: doLOADD(); break;
                case LOADP: doLOADP(); break;
                //case LOADJ
                case STOREB: doSTOREB(); break;
                case STOREN: doSTOREN(); break;
                case STORES: doSTORES(); break;
                case STORET: doSTORET(); break;
                case STOREA: doSTOREA(); break;
                case STORED: doSTORED(); break;
                case STOREP: doSTOREP(); break;
                //case STOREJ
                case NEGN: doNEGN(); break;
                case ADDN: doADDN(); break;
                case SUBN: doSUBN(); break;
                case MULN: doMULN(); break;
                case DIVN: doDIVN(); break;
                case MODN: doMODN(); break;
                case EXPN: doEXPN(); break;
                //case EQB
                //case NEB
                case EQN: doEQN(); break;
                case NEN: doNEN(); break;
                case LTN: doLTN(); break;
                case GTN: doGTN(); break;
                case LEN: doLEN(); break;
                case GEN: doGEN(); break;
                case EQS: doEQS(); break;
                //case NES
                //case LTS
                case GTS: doGTS(); break;
                //case LES
                //case GES
                //case EQT
                //case NET
                //case LTT
                //case GTT
                //case LET
                //case GES
                //case EQA
                //case NEA
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
                //case INA
                //case IND
                case CALLP: doCALLP(); break;
                case CALLF: doCALLF(); break;
                //case CALLMF
                //case CALLI
                case JUMP: doJUMP(); break;
                case JF: doJF(); break;
                case JT: doJT(); break;
                case JFCHAIN: doJFCHAIN(); break;
                case DUP: doDUP(); break;
                case DUPX1: doDUPX1(); break;
                case DROP: doDROP(); break;
                case RET: doRET(); break;
                //case CALLE
                case CONSA: doCONSA(); break;
                case CONSD: doCONSD(); break;
                case EXCEPT: doEXCEPT(); break;
                case ALLOC: doALLOC(); break;
                case PUSHNIL: doPUSHNIL(); break;
                case JNASSERT: doJNASSERT(); break;
                case RESETC: doRESETC(); break;
                //case PUSHPEG
                case JUMPTBL: doJUMPTBL(); break;
                //case CALLX
                //case SWAP
                //case DROPN
                //case PUSHM
                //case CALLV
                case PUSHCI: doPUSHCI(); break;
                default:
                    System.err.println("Unknown opcode: " + opcodes[object.code.get(ip)]);
                    System.exit(1);
            }
        }
    }

    public static void main(String[] args)
    {
        try {
            new Executor(new DataInputStream(new FileInputStream(args[0]))).run();
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

    private void doENTER()
    {
        ip++;
        int nest = getVint();
        int val = getVint();
        Cell[] frame = new Cell[val];
        for (int i = 0; i < frame.length; i++) {
            frame[i] = new Cell();
        }
        frames.addFirst(frame);
    }

    private void doLEAVE()
    {
        ip++;
        frames.removeFirst();
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

    private void doPUSHT()
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

    private void doPUSHPL()
    {
        ip++;
        int addr = getVint();
        stack.addFirst(new Cell(frames.getFirst()[addr]));
    }

    private void doPUSHPOL()
    {
        assert false : "PUSHPOL";
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

    private void doLOADT()
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

    private void doSTORET()
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
            raiseLiteral("DivideByZeroException");
            return;
        }
        stack.addFirst(new Cell(a.divide(b)));
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
        stack.addFirst(new Cell(BigDecimalMath.pow(a, b)));
    }

    private void doEQN()
    {
        ip++;
        BigDecimal b = stack.removeFirst().getNumber();
        BigDecimal a = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(a.equals(b)));
    }

    private void doNEN()
    {
        ip++;
        BigDecimal b = stack.removeFirst().getNumber();
        BigDecimal a = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(!a.equals(b)));
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

    private void doGTS()
    {
        ip++;
        String b = stack.removeFirst().getString();
        String a = stack.removeFirst().getString();
        stack.addFirst(new Cell(a.compareTo(b) > 0));
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
            raiseLiteral("ArrayIndexException", index.toString());
            return;
        }
        if (index.signum() < 0 || index.intValue() >= array.size()) {
            raiseLiteral("ArrayIndexException", index.toString());
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
            raiseLiteral("ArrayIndexException", index.toString());
            return;
        }
        if (index.signum() < 0) {
            raiseLiteral("ArrayIndexException", index.toString());
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
            raiseLiteral("ArrayIndexException", index.toString());
            return;
        }
        if (index.signum() < 0 || index.intValue() >= array.size()) {
            raiseLiteral("ArrayIndexException", index.toString());
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
            raiseLiteral("ArrayIndexException", index.toString());
            return;
        }
        if (index.signum() < 0) {
            raiseLiteral("ArrayIndexException", index.toString());
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
            raiseLiteral("DictionaryIndexException", key);
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
            raiseLiteral("DictionaryIndexException", key);
            return;
        }
        stack.addFirst(r);
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
        callstack.addFirst(ip);
        ip = target;
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

    private void doJFCHAIN()
    {
        ip++;
        int target = getVint();
        Cell a = stack.removeFirst();
        if (!a.getBoolean()) {
            ip = target;
            stack.removeFirst();
            stack.addFirst(a);
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
        List<Cell> a = stack.removeFirst().getArray();
        ExceptionInfo ei = new ExceptionInfo();
        if (a.size() >= 1) {
            ei.info = a.get(0).getString();
        }
        if (a.size() >= 2) {
            ei.code = a.get(1).getNumber();
        }
        raiseLiteral(object.strtable[val], ei);
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
        stack.addFirst(new Cell((Cell)null));
    }

    private void doJNASSERT()
    {
        ip++;
        int target = getVint();
        if (!enable_assert) {
            ip = target;
        }
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

    private void raiseLiteral(String name)
    {
        raiseLiteral(name, new ExceptionInfo());
    }

    private void raiseLiteral(String name, String info)
    {
        ExceptionInfo ei = new ExceptionInfo();
        ei.info = info;
        raiseLiteral(name, ei);
    }

    private void raiseLiteral(String name, ExceptionInfo info)
    {
        Cell exceptionvar = new Cell(new ArrayList<Cell>(Arrays.asList(
            new Cell(name),
            new Cell(info.info),
            new Cell(info.code),
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

        System.err.println("Unhandled exception " + name + " (" + info.info + ") (code " + info.code + ")");
        System.exit(1);
    }

    private class ExceptionInfo {
        String info = "";
        BigDecimal code = BigDecimal.valueOf(0);
    }

    public enum Opcode {
        ENTER,
        LEAVE,
        PUSHB,
        PUSHN,
        PUSHS,
        PUSHT,
        PUSHPG,
        PUSHPPG,
        PUSHPMG,
        PUSHPL,
        PUSHPOL,
        PUSHI,
        LOADB,
        LOADN,
        LOADS,
        LOADT,
        LOADA,
        LOADD,
        LOADP,
        LOADJ,
        STOREB,
        STOREN,
        STORES,
        STORET,
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
        EQT,
        NET,
        LTT,
        GTT,
        LET,
        GET,
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
        JFCHAIN,
        DUP,
        DUPX1,
        DROP,
        RET,
        CALLE,
        CONSA,
        CONSD,
        EXCEPT,
        ALLOC,
        PUSHNIL,
        JNASSERT,
        RESETC,
        PUSHPEG,
        JUMPTBL,
        CALLX,
        SWAP,
        DROPN,
        PUSHM,
        CALLV,
        PUSHCI,
    }

    private interface GenericFunction {
        void call();
    }

    private Opcode[] opcodes = Opcode.values();
    private Map<String, GenericFunction> predefined;

    private Bytecode object;
    private int ip;
    private ArrayDeque<Integer> callstack;
    private ArrayDeque<Cell> stack;
    private Cell[] globals;
    private ArrayDeque<Cell[]> frames;

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

    private void array__size()
    {
        List<Cell> a = stack.removeFirst().getArray();
        stack.addFirst(new Cell(BigDecimal.valueOf(a.size())));
    }

    private void array__slice()
    {
        boolean last_from_end = stack.removeFirst().getBoolean();
        int last = stack.removeFirst().getNumber().intValueExact();
        boolean first_from_end = stack.removeFirst().getBoolean();
        int first = stack.removeFirst().getNumber().intValueExact();
        List<Cell> a = stack.removeFirst().getArray();
        if (first_from_end) {
            first += a.size() - 1;
        }
        if (last_from_end) {
            last += a.size() - 1;
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

    private void boolean__toString()
    {
        boolean b = stack.removeFirst().getBoolean();
        stack.addFirst(new Cell(b ? "TRUE" : "FALSE"));
    }

    private void bytes__decodeToString()
    {
        byte[] b = stack.removeFirst().getBytes();
        stack.addFirst(new Cell(new String(b)));
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

    private void chr()
    {
        BigDecimal n = stack.removeFirst().getNumber();
        stack.addFirst(new Cell(new String(new int[] {n.intValue()}, 0, 1)));
    }

    private void concat()
    {
        String b = stack.removeFirst().getString();
        String a = stack.removeFirst().getString();
        stack.addFirst(new Cell(a + b));
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

    private void ord()
    {
        String s = stack.removeFirst().getString();
        stack.addFirst(new Cell(BigDecimal.valueOf(s.charAt(0))));
    }

    private void print()
    {
        System.out.println(stack.removeFirst().getString());
    }

    private void string__append()
    {
        String b = stack.removeFirst().getString();
        Cell a = stack.removeFirst().getAddress();
        a.set(a.getString() + b);
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

    private void string__toBytes()
    {
        String s = stack.removeFirst().getString();
        stack.addFirst(new Cell(s.getBytes()));
    }
}
