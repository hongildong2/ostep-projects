// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  uint pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
};

struct mylock_t {
  uint turn;
  uint ticket;
  struct cpu* cpu; // The cpu holding the lock
  uint pcs[10]; // Tha call stack that locked the lock
};