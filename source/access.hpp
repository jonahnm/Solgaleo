#pragma once
#include "buf_type.hpp"
#include "buffer.hpp"
namespace solgaleo {
class Access {
       public:
       static Access GetAccess() {
            Access A;
            A.buf = &s_shbuf;
            return A;
        };
        solgaleo::Buffer* buf;
        Access() { buf->is_writing = true; };
        ~Access() { buf->is_writing = false; };
    };
}