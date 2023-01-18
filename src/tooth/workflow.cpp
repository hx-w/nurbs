#include "workflow.h"

#include <iostream>

#include "toolkit.h"
#include "engine.h"
#include "service.h"
#include "tooth_pack.h"

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
		atomic_init(&_curr_wkflow_id, 1);
	}

	void Workspace::fetch_filepath(const string& filepath, bool force) {
		cout << filepath.c_str() << endl;
		TOOLKIT_EXEC(preprocess_tooth_path, "project load", filepath, force,)
		if (_code == 2) {
			SERVICE_INST->slot_add_notice(
				"Force to load the project?##" + filepath,
				"Files changed since the project builded\n\n"
				"Cache and config will be replaced if confirmed\n\n"
				"This cannot be undone!\n\n\n"
			);
			return;
		}
		if (_code != 1) return;
		// open workflow editor
		auto wkflow_id = _gen_wkflow_id();
		_tooth_packs.emplace_back(
			make_shared<ToothPack>(wkflow_id, filepath)
		);

		/// notice GUI module to setup
		SERVICE_INST->slot_open_workflow(_tooth_packs.back()->get_context());
	}

	/// status = 0, failed; = 1, success
	void Workspace::confirm_workflow(int flow_id, int status) {
		auto iter = _tooth_packs.begin();
		for (; iter != _tooth_packs.end(); ++iter) {
			if ((*iter)->get_context()->flow_id == flow_id) {
				break; // found, and must be found
			}
		}
		auto& flow_name = (*iter)->get_context()->flow_name;

		if (status == 0) {
			// failed case
			SERVICE_INST->slot_add_log("warn", "Discard workflow: " + flow_name);

			// do something
		}
		else if (status == 1) {
			// success case

			// save to cache
			save_tooth_pack_cache((*iter).get());

			SERVICE_INST->slot_add_log("info", "Confirm workflow: " + flow_name);
			// load mesh to renderer
			load_meshes_to_renderer((*iter).get());

			SERVICE_INST->slot_add_tooth_pack((*iter));
		}
		else {
			// invalid case
		}
	}

	int Workspace::_gen_wkflow_id() {
		return _curr_wkflow_id++;
	}
}
