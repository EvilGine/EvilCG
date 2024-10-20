#include <help/ecg_status.h>

namespace ecg {
	ecg_status_handler& ecg_status_handler::operator=(const ecg_status_handler& rhs) {
		m_status = rhs.m_status;
		if (m_status != SUCCESS)
			throw ecg_status_ex(m_status);
	}

	ecg_status_handler& ecg_status_handler::operator=(const ecg_status& rhs) {
		m_status = rhs;
		if(m_status != SUCCESS)
			throw ecg_status_ex(m_status);
	}

	ecg_status ecg_status_handler::get_status() const {
		return m_status;
	}
}