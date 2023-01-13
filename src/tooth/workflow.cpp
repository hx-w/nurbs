#include "workflow.h"

#include <iostream>

#include "toolkit.h"
#include "engine.h"
#include "service.h"

using namespace std;

#define SERVICE_INST ToothEngine::get_instance()->get_service()
#define TOOLKIT_EXEC(func, prefix, ...) \
			string log_msg = ""; \
			auto _code = static_cast<int>(func(__VA_ARGS__ log_msg)); \
			switch (_code) { \
			case 0: SERVICE_INST->slot_add_log("error", prefix##" error, " + log_msg); break; \
			case 1: SERVICE_INST->slot_add_log("info", prefix##" successfully, " + log_msg); break; \
			case 2: SERVICE_INST->slot_add_log("warn", prefix##" suspended, " + log_msg); break; \
			default: break; \
			}


namespace ToothSpace {
	Workspace::Workspace() {
		TOOLKIT_EXEC(init_workenv, "init workspace", )
	}

	void Workspace::slot_fetch_filepath(const string& filepath, bool force) {
		cout << filepath.c_str() << endl;
		TOOLKIT_EXEC(preprocess_tooth_path, "project load", filepath, force,)
		if (_code == 2) {
			SERVICE_INST->slot_add_notice(
				"Force to load the project?##" + filepath,
				"Files changed since the project builded\n\n"
				"Cache and config will be replaced if confirmed\n\n"
				"This cannot be undone!\n\n\n"
			);
		}
	}

}