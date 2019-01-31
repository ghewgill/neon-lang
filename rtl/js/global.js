neon = {
    global: {
        array__append: function(self, element) {
            self.push(element);
        },

        array__concat: function(left, right) {
            return left.concat(right);
        },

        array__extend: function(self, elements) {
            elements.forEach(function(x) {
                self.push(x);
            });
        },

        array__range: function(first, last, step) {
            var r = [];
            for (var i = first; i <= last; i += step) {
                r.push(i);
            }
            return r;
        },

        array__size: function(a) {
            return a.length;
        },

        array__toString__number: function(self) {
            return "[" + self.join(", ") + "]";
        },

        array__toString__string: function(self) {
            return "[" + self.map(function(x) { return "\"" + x + "\""; }).join(", ") + "]";
        },

        boolean__toString: function(x) {
            return x ? "TRUE" : "FALSE";
        },

        chr: function(x) {
            if (x != Math.trunc(x)) {
                throw new neon.NeonException("ValueRangeException", {info: "chr() argument not an integer"});
            }
            if (x < 0 || x > 0x10ffff) {
                throw new neon.NeonException("ValueRangeException", {info: "chr() argument out of range 0-0x10ffff"});
            }
            return String.fromCharCode(x);
        },

        concat: function(a, b) {
            return a + b;
        },

        dictionary__keys: function(self) {
            return Object.keys(self).sort();
        },

        divide: function(a, b) {
            if (b === 0) {
                throw new NeonException("DivideByZeroException");
            }
            return a / b;
        },

        int: function(x) {
            return x < 0 ? Math.ceil(x) : Math.floor(x);
        },

        max: function(x, y) {
            return Math.max(x, y);
        },

        min: function(x, y) {
            return Math.min(x, y);
        },

        num: function(s) {
            return parseFloat(s);
        },

        number__toString: function(x) {
            return x.toString();
        },

        object__toString: function(x) {
            return x.toString();
        },

        ord: function(s) {
            if (s.length != 1) {
                throw new neon.NeonException("ArrayIndexException", {info: "ord() requires string of length 1"});
            }
            return s.charCodeAt(0);
        },

        print: function(s) {
            console.log(s);
        },

        str: function(x) {
            return x.toString();
        },

        string__length: function(self) {
            return self.length;
        },

        substring: function(s, offset, length) {
            return s.substring(offset, offset + length);
        }
    },

    binary: {
        and32: function(x, y) {
            return (x & y) | 0;
        }
    },

    io: {
        fprint: function(f, s) {
            console.error(s);
        }
    },

    math: {
        abs: function(x) {
            return Math.abs(x);
        },

        acos: function(x) {
            return Math.acos(x);
        },

        acosh: function(x) {
            return Math.acosh(x);
        },

        asin: function(x) {
            return Math.asin(x);
        },

        asinh: function(x) {
            return Math.asinh(x);
        },

        atan: function(x) {
            return Math.atan(x);
        },

        atanh: function(x) {
            return Math.atanh(x);
        },

        atan2: function(y, x) {
            return Math.atan2(y, x);
        },

        cbrt: function(x) {
            return Math.cbrt(x);
        },

        ceil: function(x) {
            return Math.ceil(x);
        },

        cos: function(x) {
            return Math.cos(x);
        },

        cosh: function(x) {
            return Math.cosh(x);
        },

        erf: function(x) {
            return 0; // TODO Math.erf(x);
        },

        erfc: function(x) {
            return 0; // TODO Math.erfc(x);
        },

        exp: function(x) {
            return Math.exp(x);
        },

        exp2: function(x) {
            return Math.pow(2, x);
        },

        expm1: function(x) {
            return Math.expm1(x);
        },

        floor: function(x) {
            return Math.floor(x);
        },

        frexp: function(x) {
            return Math.frexp(x);
        },

        intdiv: function(x, y) {
            var r = x / y;
            return r < 0 ? Math.ceil(r) : Math.floor(r);
        },

        log: function(x) {
            return Math.log(x);
        },

        sign: function(x) {
            return Math.sign(x);
        },

        sin: function(x) {
            return Math.sin(x);
        },

        sqrt: function(x) {
            return Math.sqrt(x);
        },

        tan: function(x) {
            return Math.tan(x);
        }
    },

    os: {
        system: function(command) {
            var exec = require("child_process").exec;
            child = exec(command);
        }
    },

    sys: {
        exit: function(x) {
            process.exit(x);
        }
    },

    NeonException: function(name, info) {
        this.name = name;
        this.info = info.info;
        this.code = info.code;
        this.offset = 0;
    }
}
