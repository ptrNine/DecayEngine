#ifndef UIBUILDER_UIBUILDER_HPP
#define UIBUILDER_UIBUILDER_HPP

#include "../../base/ftl/vector2.hpp"
#include "../../base/flags.hpp"
#include "../../base/ftl/vector.hpp"
#include "../../base/baseTypes.hpp"
#include <rapidxml/rapidxml.hpp>

// Todo: custom allocator impl

struct nk_context;
using UIFlags = base::Flags<std::uint32_t>;


class UIBase {
    friend class UIContainer;
    friend class UIBuilder;

protected:
    virtual void    present        (nk_context* context)        = 0;
    virtual UIBase* initFromXmlNode(rapidxml::xml_node<>* node) = 0;

    UIFlags _flags;
};


class UIContainer : public UIBase {
protected:
    inline void presentChildren(nk_context* context) {
        for (auto child : _children)
            child->present(context);
    }

    inline void initFromXmlNodeChildren(rapidxml::xml_node<>* node);

    ftl::Vector<UIBase*> _children;
};



class UIWindow : public UIContainer {
public:
    enum {
        BORDERED           = UIFlags::def<0>,
        MOVABLE            = UIFlags::def<1>,
        SCALABLE           = UIFlags::def<2>,
        CLOSABLE           = UIFlags::def<3>,
        MINIMIZABLE        = UIFlags::def<4>,
        NO_SCROLLBAR       = UIFlags::def<5>,
        TITLED             = UIFlags::def<6>,
        AUTO_HIDE_SCROLL   = UIFlags::def<7>,
        ALWAYS_BACKGROUND  = UIFlags::def<8>,
        SCALER_IN_LEFT     = UIFlags::def<9>,
        NO_INPUT           = UIFlags::def<10>,
        IS_SHOWN           = UIFlags::def<11>
    };

    void    present        (nk_context* context) override;
    UIBase* initFromXmlNode(rapidxml::xml_node<>* node) override;

protected:
    Vector2<uint32_t> _pos;
    Vector2<uint32_t> _size;
    std::string _title;
    std::string _id;
};




class UIButton : public UIBase {

};


class UIBuilder {
    using StrV = std::string_view;

public:
    UIBuilder(const StrV& name);
    void present(nk_context* context);

protected:
    rapidxml::xml_document<> _doc;
    ftl::Vector<char>        _file;
    ftl::Vector<UIBase*>     _children;
};


#endif //UIBUILDER_UIBUILDER_HPP
