#pragma once
#include <cstdint>
#include <set>
#include "types.hpp"

namespace riscv
{
	template <int W>
	struct AtomicMemory
	{
		using address_t = address_type<W>;
		static constexpr size_t MAX_RESV = 48;

		void load_reserve(int size, address_t addr) RISCV_INTERNAL
		{
			check_alignment(size, addr);
			if (LIKELY(m_reservations.size() < MAX_RESV))
				m_reservations.insert(addr);
			else
				throw MachineException(DEADLOCK_REACHED,
					"Not enough room for memory reservations", addr);
		}

		// Volume I: RISC-V Unprivileged ISA V20190608 p.49:
		// An SC can only pair with the most recent LR in program order.
		bool store_conditional(int size, address_t addr) RISCV_INTERNAL
		{
			check_alignment(size, addr);
			bool result = (m_reservations.count(addr) != 0);
			// Regardless of success or failure, executing an SC.W
			// instruction invalidates any reservation held by this hart.
			m_reservations.clear();
			return result;
		}

	private:
		inline void check_alignment(int size, address_t addr) RISCV_INTERNAL
		{
			if (UNLIKELY(addr & (size-1))) {
				throw MachineException(INVALID_ALIGNMENT,
					"Load-Reserved address is misaligned", addr);
			}
		}

		std::set<address_t> m_reservations;
	};
}
