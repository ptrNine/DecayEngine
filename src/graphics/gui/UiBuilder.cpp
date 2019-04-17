#include "UiBuilder.hpp"
#include <iostream>
#include <fstream>
#include "../../base/aton.hpp"

bool scmp(const char* r, const char* l) {
    while(*r && *l)
        if (*r++ != *l++)
            return false;
    return *r == *l;
}

bool r_bool(const char* str) {
    if      (scmp(str, "true"))  return true;
    else if (scmp(str, "false")) return false;

    ABORTF("'{}' is not 'false' or 'true'", str);
    return false;
}

static auto createFromNode(rapidxml::xml_node<>* node) -> UIBase* {
    auto name = node->name();

    if      (scmp(name, "Window")) return new UIWindow;
    //else if (scmp(name, ""))

    return nullptr;
}

void UIContainer::initFromXmlNodeChildren(rapidxml::xml_node<>* node) {
    for (auto n = node->first_node(); n; n = n->next_sibling())
        _children.push_back(createFromNode(n)->initFromXmlNode(n));
}

void UIWindow::present(nk_context* context) {
    std::cout << "Present window" << std::endl;
    presentChildren(context);
}

UIBase* UIWindow::initFromXmlNode(rapidxml::xml_node<>* node) {
    std::cout << "Init window!" << std::endl;
    for (auto attrib = node->first_attribute(); attrib; attrib = attrib->next_attribute()) {
        auto name  = attrib->name();
        auto value = attrib->value();

        if      (scmp(name, "id"))         _id = value;
        else if (scmp(name, "title"))      _title = value;

        else if (scmp(name, "position_x")) base::aton(value, _pos.x());
        else if (scmp(name, "position_y")) base::aton(value, _pos.y());
        else if (scmp(name, "width"))      base::aton(value, _size.x());
        else if (scmp(name, "height"))     base::aton(value, _size.y());

        else if (scmp(name, "bordered"))         _flags.set_if(BORDERED,          r_bool(value));
        else if (scmp(name, "movable"))          _flags.set_if(MOVABLE,           r_bool(value));
        else if (scmp(name, "scalable"))         _flags.set_if(SCALABLE,          r_bool(value));
        else if (scmp(name, "closable"))         _flags.set_if(CLOSABLE,          r_bool(value));
        else if (scmp(name, "minimizable"))      _flags.set_if(MINIMIZABLE,       r_bool(value));
        else if (scmp(name, "noScrollbar"))      _flags.set_if(NO_SCROLLBAR,      r_bool(value));
        else if (scmp(name, "titled"))           _flags.set_if(TITLED,            r_bool(value));
        else if (scmp(name, "autoHideScroll"))   _flags.set_if(AUTO_HIDE_SCROLL,  r_bool(value));
        else if (scmp(name, "alwaysBackground")) _flags.set_if(ALWAYS_BACKGROUND, r_bool(value));
        else if (scmp(name, "scalerInLeft"))     _flags.set_if(SCALER_IN_LEFT,    r_bool(value));
        else if (scmp(name, "noInput"))          _flags.set_if(NO_INPUT,          r_bool(value));
        else if (scmp(name, "isShown"))          _flags.set_if(IS_SHOWN,          r_bool(value));
    }

    initFromXmlNodeChildren(node);

    return this;
}


UIBuilder::UIBuilder(const UIBuilder::StrV& name) {
    auto stream = std::ifstream(name.data(), std::ios_base::in | std::ios_base::binary | std::ios_base::ate);

    // Todo: stream.is_open assert!

    auto last = stream.tellg();
    stream.seekg(0, std::ios_base::beg);

    auto size = static_cast<SizeT>(last - stream.tellg());
    _file.reserve(size + 1);
    _file.resize(size + 1);

    stream.read(_file.data(), size);

    _doc.parse<0>(_file.data());

    for (auto n = _doc.first_node(); n; n = n->next_sibling())
        _children.push_back(createFromNode(n)->initFromXmlNode(n));
}

void UIBuilder::present(nk_context *context) {
    for (auto child : _children)
        child->present(context);
}