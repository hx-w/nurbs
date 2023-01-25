// impl
#include "context.h"

#include "container.h"
#include "xwindow.h"
#include "geom_ext/drawable.h"
#include "service.h"

#include <mesh.h>

#include <iostream>
#include <line.hpp>

using namespace std;
using namespace geometry;

namespace RenderSpace {

    RenderContext::RenderContext(std::shared_ptr<RenderService> service,
                                 std::shared_ptr<RenderContainer> container,
                                 std::shared_ptr<RenderWindowWidget> window)
        : m_service(service), m_container(container), m_window(window) {
        // convert this pointer to shared_ptr

        m_window->init_context(static_cast<shared_ptr<RenderContext>>(this));
        m_service->init_context(static_cast<shared_ptr<RenderContext>>(this));
    }

    void RenderContext::ctx_update_and_draw() {
        m_container->update_all();
        m_container->draw_all();
    }

    DrawableID RenderContext::ctx_load_drawable(const string& filename) {
        // only mesh
        int status = 0;
        auto geom_mesh = Mesh::load_obj(filename, status);
        switch (status) {
        case 0:
            m_service->slot_add_log("error", "Cannot load mesh from file " + filename);
            return -1;
        case 1:
            m_service->slot_add_log("info", "Load mesh from file " + filename + " (trimesh)");
            break;
        case 2:
            m_service->slot_add_log("info", "Load mesh from file " + filename);
        }                                      
        return ctx_add_drawable(make_shared<Mesh>(geom_mesh), Props{});
    }

    DrawableID RenderContext::ctx_add_drawable(shared_ptr<GeometryBase> geom, Props& props, int type) {
        auto geom_type = static_cast<GeomType>(type);
        
        auto post_setup = [&](shared_ptr<DrawableBase> drawable) -> DrawableID {
			drawable->_shader() = m_container->shaders()["default"];
			drawable->get_ready();
			auto drawable_id = m_container->add_drawable(drawable);
            return drawable_id;
        };

        DrawableID drawable_id = -1;

        // decode properties
        /// color
        auto clr = Vector3f(0.5); // default
        if (props.find("color") != props.end()) {
            clr = any_cast<Vector3f>(props.at("color"));
        }

        if (geom_type == GeomTypeMesh) {
			auto geom_mesh = dynamic_pointer_cast<Mesh>(geom);
			auto drawable_mesh = make_shared<NewMeshDrawable>(*geom_mesh, clr);
			drawable_id = post_setup(drawable_mesh);
        }
        else if (geom_type == GeomTypeArrow) {
            // decode properties for arrow
            auto arrow_len = 1.0f;
            if (props.find("arrow_length") != props.end()) {
                arrow_len = any_cast<float>(props.at("arrow_length"));
            }

            auto geom_ray = dynamic_pointer_cast<Ray>(geom);
            auto drawable_ray = make_shared<ArrowDrawable>(*geom_ray, arrow_len, clr);
            drawable_ray->_shade_mode() = 0x01;
            drawable_id = post_setup(drawable_ray);
        }
        else if (geom_type == GeomTypePoint) {
            
        }
		m_service->slot_add_log("info", "Add drawable object with id " + to_string(drawable_id));
        return drawable_id;
    }

    bool RenderContext::ctx_remove_drawable(DrawableID id) {
        return m_container->remove_drawable(id);
    }

    shared_ptr<DrawableBase> RenderContext::ctx_get_drawable(DrawableID id) {
        return m_container->get_drawable_inst(id);
    }

    bool RenderContext::ctx_set_drawable_property(DrawableID id, const string& property, const std::any& value) {
        return m_container->set_drawable_property(id, property, value);
    }

    void RenderContext::ctx_pick_drawables(const Vector3f& origin, const Vector3f& direction, bool muti) {
        Ray pick_ray(origin, direction);

        vector<DrawableID> picked_ids;
        vector<Vector3f> picked_points;
        vector<Vector3f> picked_normals;

        auto success = m_container->pickcmd(
            move(pick_ray), picked_ids, picked_points, picked_normals, m_window->gizmo.getTransform(), muti
        );

        if (success) {
            /// [Notify] render/picked_drawables
            ctx_notify<void(vector<DrawableID>&, vector<Vector3f>&, vector<Vector3f>&)>(
                "/picked_points", picked_ids, picked_points, picked_normals
            );
        }
        else {
            // not picked
        }
    }

    void RenderContext::ctx_change_interact_mode(int mode) {
        m_window->set_interact_mode(static_cast<InteractMode>(mode));
    }

    void RenderContext::ctx_update_transform_mat(const glm::mat4& transf) {
        m_window->update_transform_mat(transf);
    }

}  // namespace RenderSpace
