#ifndef MICROBLAZE_INTC_HPP_
#define MICROBLAZE_INTC_HPP_
#ifdef __MICROBLAZE__

#include <array>
#include <tuple>
#include "xintc.h"


using IntrVecId = unsigned;
using IntrPayload = void*;
using Intr = std::tuple<IntrVecId, XInterruptHandler, IntrPayload>;

int intrRegister(Intr intr);
int init_intc();

extern XIntc intc;

#endif /* __MICROBLAZE__ */
#endif /* MICROBLAZE_INTC_HPP_ */
