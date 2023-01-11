#include "service.h"
#include "engine.h"
#include "viewer.h"
#include "components/logger.h"

#include <memory>
#include <functional>
#include <communication/ContextHub.h>

namespace RenderSpace {
	class RenderWindowWidget;
}

using namespace std;
using namespace fundamental;
using namespace RenderSpace;


namespace GUISpace {
	GUIService::GUIService(GUIEngine& engine) noexcept :
		m_engine(engine), m_autobus(make_unique<AutoBus>()) {
		_subscribe_all();
		_register_all();
	}

	GUIService::~GUIService() {
		m_autobus.reset();
	}

	void GUIService::_subscribe_all() {
		m_autobus->subscribe<void()>(SignalPolicy::Sync, "render/render_setup",
			bind(&GUISpace::ImGuiViewer::setup));
		m_autobus->subscribe<void(shared_ptr<RenderWindowWidget>)>(SignalPolicy::Sync, "render/pre_redraw",
			bind(&GUISpace::ImGuiViewer::update, placeholders::_1));
		m_autobus->subscribe<void()>(SignalPolicy::Sync, "render/render_destroy",
			bind(&GUISpace::ImGuiViewer::destroy));
	}

	void GUIService::_register_all() {
		m_autobus->registerMethod<void(string&&, const string&)>(
			m_symbol + "/add_log",
			[this](string&& type, const string& msg) {
				LOG_LEVEL log_type = LOG_INFO;
				if (type == "info") log_type = LOG_INFO;
				else if (type == "error") log_type = LOG_ERROR;
				else if (type == "fatal") log_type = LOG_FATAL;
				else log_type = LOG_DEBUG;
				Logger::log(msg, log_type);
			}
		);
	}

	/// [sync invoke] -> render/load_mesh
	void GUIService::slot_load_mesh(const string& file_path) {
		auto _service = ContextHub::getInstance()->getServiceTable<uint32_t(const string&)>();
		_service->sync_invoke("render/load_mesh", file_path);
	}
}