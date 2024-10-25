#include <help/ecg_status.h>

namespace ecg {
	ecg_status_handler& ecg_status_handler::operator=(const ecg_status_handler& rhs) {
		m_status = rhs.m_status;
		if (m_status != status_code::SUCCESS)
			throw ecg_status_ex(m_status);
	}

	ecg_status_handler& ecg_status_handler::operator=(const ecg_status& rhs) {
		m_status = rhs;
		if(m_status != status_code::SUCCESS)
			throw ecg_status_ex(m_status);
	}

	bool ecg_status_handler::operator==(const status_code& code) {
		return this->m_status == code;
	}

	ecg_status ecg_status_handler::get_status() const {
		return m_status;
	}
}