/*****************************************************************//**
 * \file   node_flow.h
 * \brief  Tooth Workflow Editor using ImNodes
 * 
 * \author CarOL
 * \date   January 2023
 *********************************************************************/
#pragma once

#include "base.h"

namespace GUISpace {
    class NodeFlow : public GUIComponentBase {
    public:
        /// init nodes positions
        static void init();

        /// action delete link
        static void delete_selected_links();

        static void render();
    };
}
