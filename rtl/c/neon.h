struct Ne_Number {
    double dval;
};

struct Ne_String {
    unsigned char *ptr;
    int len;
};

struct Ne_Object {
    struct Ne_String *str;
};

void Ne_Number_assign(struct Ne_Number *dest, const struct Ne_Number *src);
struct Ne_Number *Ne_number_literal(double n);
struct Ne_String *Ne_string_literal(const char *s);
struct Ne_Object *Ne_object__makeString(const struct Ne_String *s);
void Ne_print(struct Ne_Object *obj);
struct Ne_String *Ne_str(const struct Ne_Number *n);
