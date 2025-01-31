﻿#ifndef VEIN_HTML_DOCUMENT_HPP
#define VEIN_HTML_DOCUMENT_HPP

#include "vein/html/Tag.hpp"

#include "vein/Hash.hpp"

#include <unordered_map>
#include <string>


namespace vein::html {

struct Document
{
    Tag* head_tag = nullptr;
    Tag* title_tag = nullptr;
    Tag* description_tag = nullptr;
    Tag* link_rel_canonical_tag = nullptr;

    std::unordered_map<std::string, Tag*, string_hash, std::equal_to<>>
    name_tag, id_tag, form_action_tag;

    Tag::callback_type default_callback_;

    auto* tag_by_id(this auto&& self, std::string_view id)
    {
        auto const it = self.id_tag.find(id);
        if (it == self.id_tag.end()) {
            throw std::invalid_argument("tag with id \"" + std::string(id) + "\" not found");
        }
        return it->second;
    }
};

}

#endif
