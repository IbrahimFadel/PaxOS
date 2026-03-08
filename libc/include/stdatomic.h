#ifndef KLIBC_STDATOMIC_H
#define KLIBC_STDATOMIC_H

typedef enum {
  memory_order_relaxed = __ATOMIC_RELAXED,
  memory_order_consume = __ATOMIC_CONSUME,
  memory_order_acquire = __ATOMIC_ACQUIRE,
  memory_order_release = __ATOMIC_RELEASE,
  memory_order_acq_rel = __ATOMIC_ACQ_REL,
  memory_order_seq_cst = __ATOMIC_SEQ_CST
} memory_order;

typedef _Atomic(_Bool) atomic_bool;
typedef _Atomic(char) atomic_char;
typedef _Atomic(signed char) atomic_schar;
typedef _Atomic(unsigned char) atomic_uchar;
typedef _Atomic(short) atomic_short;
typedef _Atomic(unsigned short) atomic_ushort;
typedef _Atomic(int) atomic_int;
typedef _Atomic(unsigned int) atomic_uint;
typedef _Atomic(long) atomic_long;
typedef _Atomic(unsigned long) atomic_ulong;
typedef _Atomic(long long) atomic_llong;
typedef _Atomic(unsigned long long) atomic_ullong;

#define ATOMIC_VAR_INIT(value) (value)

typedef struct {
  atomic_bool __flag;
} atomic_flag;
#define ATOMIC_FLAG_INIT {ATOMIC_VAR_INIT(0)}

#define atomic_exchange_explicit(object, desired, order) __atomic_exchange_n(object, desired, order)
#define atomic_store_explicit(object, desired, order)    __atomic_store_n(object, desired, order)
#define atomic_load_explicit(object, order)              __atomic_load_n(object, order)

static __inline _Bool atomic_flag_test_and_set_explicit(volatile atomic_flag *__object,
                                                        memory_order __order) {
  return (atomic_exchange_explicit(&__object->__flag, 1, __order));
}

static __inline _Bool atomic_flag_test_and_set(volatile atomic_flag *__object) {
  return (atomic_flag_test_and_set_explicit(__object, memory_order_seq_cst));
}

static __inline void atomic_flag_clear_explicit(volatile atomic_flag *__object,
                                                memory_order __order) {
  atomic_store_explicit(&__object->__flag, 0, __order);
}

static __inline void atomic_thread_fence(memory_order __order) {
#ifdef __CLANG_ATOMICS
  __c11_atomic_thread_fence(__order);
#elif defined(__GNUC_ATOMICS)
  __atomic_thread_fence(__order);
#else
  __sync_synchronize();
#endif
}

#endif
