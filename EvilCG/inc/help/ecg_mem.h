#ifndef ECG_MEM_H
#define ECG_MEM_H
#include <help/ecg_logger.h>
#include <ecg_global.h>

namespace ecg {
	template <class Type>
	struct handle_t {
		uint64_t handle;
		std::shared_ptr<Type> ptr;

		handle_t() {
			ptr = nullptr;
			handle = 0;
		}

		handle_t(std::shared_ptr<Type> p, uint64_t h) {
			handle = h;
			ptr = p;
		}
	};

	class ecg_mem {
	public:
		static ecg_mem& get_instance() {
			static ecg_mem instance;
			return instance;
		}

		void delete_memory(uint64_t handle);
		void delete_all_memory();

		template <typename Type>
		handle_t<Type> allocate() {
			try {
				std::scoped_lock lock(m_memory_lock);

				auto id = get_next_id();
				auto ptr = std::make_shared<Type>();
				m_memory_map[id] = std::static_pointer_cast<void>(ptr);

				return handle_t<Type>(ptr, id);
			}
			catch (const std::exception& ex) {
				spdlog::default_logger()->error("Error on memory allocation: {}", ex.what());
			}
			catch (...) {
				spdlog::default_logger()->error(g_unknown_error);
			}

			return handle_t<Type>();
		}

		template <typename Type>
		handle_t<Type> allocate(size_t size) {
			try {
				if (size == 0) return handle_t<Type>();
				std::scoped_lock lock(m_memory_lock);

				auto id = get_next_id();
				auto raw_ptr = new Type[size];
				auto vec_ptr = std::shared_ptr<Type>(raw_ptr, std::default_delete<Type[]>());
				m_memory_map[id] = std::static_pointer_cast<void>(vec_ptr);
				
				handle_t<Type> res;
				res.ptr = vec_ptr;
				res.handle = id;
				
				return res;
			}
			catch (const std::exception& ex) {
				spdlog::default_logger()->error("Error on memory allocation: {}", ex.what());
			}
			catch (...) {
				spdlog::default_logger()->error(g_unknown_error);
			}

			return handle_t<Type>();
		}

	protected:
		virtual ~ecg_mem() = default;
		ecg_mem() = default;

		uint64_t get_next_id() {
			uint64_t iter = 0;
			uint64_t max_iter = UINT64_MAX - 1;

			while ((ms_id == 0 || m_memory_map.find(ms_id) != m_memory_map.end()) &&
				iter < max_iter) {
				++ms_id;
				++iter;
			}

			if (iter == max_iter)
				throw std::out_of_range("Can't create ID for ecg_mem");

			return ms_id;
		}

	private:
		std::unordered_map<uint64_t, std::shared_ptr<void>> m_memory_map;
		std::mutex m_memory_lock;
		static uint64_t ms_id;
	};
}

#endif
