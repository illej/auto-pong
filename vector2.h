#ifndef _VECTOR_2_
#define _VECTOR_2_

typedef union
{
    struct
    {
        float x, y;
    };
    struct
    {
        float w, h;
    };
} v2;

inline v2
v2_add (v2 a, v2 b)
{
    v2 result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;

    return result;
}

inline v2
v2_addf (v2 a, float f)
{
    v2 result;

    result.x = a.x + f;
    result.y = a.y + f;

    return result;
}

inline v2
v2_sub (v2 a, v2 b)
{
    v2 result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;

    return result;
}

inline v2
v2_neg (v2 v)
{
    v2 result;

    result.x = -v.x;
    result.y = -v.y;

    return result;
}

inline v2
v2_clamp (v2 v, v2 min, v2 max)
{
    v2 result;

    result.x = CLAMP (v.x, min.x, max.x);
    result.y = CLAMP (v.y, min.y, max.y);

    return result;
}

inline v2
v2_divf (v2 a, float f)
{
    v2 result = {0};

    result.x = a.x / f;
    result.y = a.y / f;

    return result;
}

inline float
v2_inner (v2 a, v2 b)
{
    return (a.x * b.x) + (a.y * b.y);
}

inline float
v2_len_sqrd (v2 a)
{
    return v2_inner (a, a);
}

inline float
v2_len (v2 a)
{
    float sqr = v2_len_sqrd (a);
    float len = sqrtf (sqr);

    return len;
}

inline v2
v2_norm (v2 a)
{
    float len = v2_len (a);
    v2 result = {0};

    if (len > 0)
    {
        result.x = a.x / len;
        result.y = a.y / len;
    }

    return result;
}

inline bool
v2_eq (v2 a, v2 b)
{
    return (a.x == b.x &&
            a.y == b.y);
}

static void
v2_test (void)
{
    v2 a = { 2.0f, 3.0f };
    v2 b = { 5.1f, 7.9f };
    v2 c = { 3.0f, 5.0f };
    float f = 3.4f;

    printf ("Vector2 Tests:\n");

    ASSERT (v2_eq (v2_add (a, b), (v2) { 7.1f, 10.9f }));
    ASSERT (v2_eq (v2_sub (b, a), (v2) { 3.1f, 4.9f }));
    ASSERT (v2_eq (v2_addf (a, 1.4f), (v2) { 3.4f, 4.4f }));
    ASSERT (v2_eq (v2_neg (a), (v2) { -2.0f, -3.0f }));
    ASSERT (v2_eq (v2_clamp (b, v2_neg (a), c), c));
    ASSERT (v2_eq (v2_divf (a, 2.0f), (v2) { 1.0f, 1.5f }));
    ASSERT (v2_inner (a, c) == 21.0f);
    ASSERT (v2_len_sqrd (a) == 13.0f);
    ASSERT (v2_len (a) == 3.60555127546f);

    printf ("OK\n");
}

#endif
