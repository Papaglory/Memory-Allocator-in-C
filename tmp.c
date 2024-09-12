typedef struct b b;
// the type b referes to a yet undefined type struct b

struct a {
    b * struct_b_pointer;
    int c;
};

struct b {
    struct a * a_pointer;
    void * d;
};

