/* The part of ares's nall library its SPC700 core (third_party/ares) uses:
 * masked unsigned integers n1..n16 with .bit(i), a few integer aliases,
 * and the declaration-only types its disassembler/serializer members
 * name. Our code, not nall's. */
#ifndef CT_NALL_SHIM_HPP
#define CT_NALL_SHIM_HPP

#include <stdint.h>

#define NALL_NOINLINE __attribute__((noinline))
#define order_lsb2(a, b) a, b   /* little-endian host */

namespace ares {

struct string;       /* disassembler return type, never defined here */
struct serializer;   /* serialize() parameter, never defined here */

template <int Bits> struct NaturalStorage { typedef uint32_t type; };
template <> struct NaturalStorage<8> { typedef uint8_t type; };
template <> struct NaturalStorage<16> { typedef uint16_t type; };

/* An unsigned Bits-bit integer: every assignment wraps to Bits bits. Its
   size is that of its storage, so ares's register unions (an n16 over
   two n8 halves) lay out as on the real target. */
template <int Bits> struct Natural {
    typedef typename NaturalStorage<Bits>::type T;
    static constexpr uint32_t mask = Bits >= 32 ? 0xFFFFFFFFu : (1u << Bits) - 1;
    T v;

    Natural() = default;
    /* From anything that converts to an integer: other widths, flag
       structs with an integer conversion, plain integers (as nall's). */
    template <typename U> constexpr Natural(const U &x) : v((T)((uint32_t)x & mask)) {}
    operator uint32_t() const { return v; }

    template <typename U> Natural &operator=(const U &x) { v = (T)((uint32_t)x & mask); return *this; }
    Natural &operator+=(uint32_t x) { return *this = v + x; }
    Natural &operator-=(uint32_t x) { return *this = v - x; }
    Natural &operator|=(uint32_t x) { return *this = v | x; }
    Natural &operator&=(uint32_t x) { return *this = v & x; }
    Natural &operator^=(uint32_t x) { return *this = v ^ x; }
    Natural &operator<<=(uint32_t x) { return *this = v << x; }
    Natural &operator>>=(uint32_t x) { return *this = v >> x; }
    Natural &operator++() { return *this = v + 1; }
    Natural &operator--() { return *this = v - 1; }
    Natural operator++(int) { Natural old = *this; *this = v + 1; return old; }
    Natural operator--(int) { Natural old = *this; *this = v - 1; return old; }

    struct Bit {
        Natural &n;
        uint32_t i;
        operator bool() const { return n.v >> i & 1; }
        Bit &operator=(bool b) { n = (n.v & ~(1u << i)) | (uint32_t)b << i; return *this; }
        Bit &operator^=(bool b) { return *this = (bool)*this ^ b; }
    };
    Bit bit(uint32_t i) { return Bit{*this, i}; }
    bool bit(uint32_t i) const { return v >> i & 1; }
};

typedef Natural<1> n1;
typedef Natural<2> n2;
typedef Natural<3> n3;
typedef Natural<4> n4;
typedef Natural<8> n8;
typedef Natural<16> n16;
typedef int8_t i8;
typedef int32_t s32;
typedef uint32_t u32;

}  // namespace ares

#endif
