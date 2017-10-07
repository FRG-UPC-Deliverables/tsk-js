#include "tsk_js.h"

#include <string.h>

namespace tsk_js {

using v8::Exception;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Persistent;
using v8::PropertyAttribute;

using v8::Local;
using v8::MaybeLocal;
using v8::Object;
using v8::Number;
using v8::String;
using v8::Boolean;
using v8::Value;
using v8::Data;
using v8::Array;

/* -------------------------------------------------------------------------- */
static TSK_WALK_RET_ENUM
add_content(TSK_FS_FILE *fs_file, TSK_OFF_T a_off, TSK_DADDR_T addr,
    char *buf, size_t size, TSK_FS_BLOCK_FLAG_ENUM flags, BUFFER_INFO *ptr)
{
    if (size == 0)
        return TSK_WALK_CONT;

    ptr->size += size;
    ptr->data = (char *) realloc(ptr->data, ptr->size);
    memcpy(ptr->data + a_off, buf, size);

    return TSK_WALK_CONT;
}
/* -------------------------------------------------------------------------- */

TskFile::TskFile(TSK_FS_FILE* fs_file)
{
    this->_fs_file = fs_file;
}

TskFile::~TskFile() { }

int
TskFile::get_content(v8::Isolate *isolate, BUFFER_INFO *buf)
{
    int fw_flags = 0;

    buf->data = NULL;
    buf->size = 0;

    if (tsk_fs_file_walk(this->_fs_file,
            (TSK_FS_FILE_WALK_FLAG_ENUM) fw_flags,
            (TSK_FS_FILE_WALK_CB) add_content, buf)) {
        return 0;
    }

    return 1;
}

int
TskFile::set_properties(Isolate *isolate, Object *obj)
{
    Local<Value> key;
    bool allocated, has_children;
    const char* type;
    TSK_FS_DIR *fs_dir;
    TSK_FS_FILE* fs_file;

    fs_file = this->_fs_file;
    key = String::NewFromUtf8(isolate, "name");
    obj->Set(key, String::NewFromUtf8(isolate, fs_file->name->name));

    key = String::NewFromUtf8(isolate, "allocated");
    allocated = fs_file->name->flags == TSK_FS_NAME_FLAG_ALLOC;
    obj->Set(key, Boolean::New(isolate, allocated));

    key = String::NewFromUtf8(isolate, "type");
    switch (fs_file->name->type) {
        case TSK_FS_NAME_TYPE_REG:
            type = "register";
            break;

        case TSK_FS_NAME_TYPE_DIR:
            type = "directory";
            break;

        case TSK_FS_NAME_TYPE_VIRT:
            type = "virtual";
            break;

        default:
            type = "unknown";
            break;
    }
    obj->Set(key, String::NewFromUtf8(isolate, type));

    key = String::NewFromUtf8(isolate, "inode");
    obj->Set(key, Number::New(isolate, fs_file->name->meta_addr));

    // get the list of entries in the directory
    if (fs_file->name->type == TSK_FS_NAME_TYPE_DIR) {
        if (fs_file->name->meta_addr == TSK_FS_ORPHANDIR_INUM(fs_file->fs_info)) {
            has_children = true;
        } else {
            if ((fs_dir = tsk_fs_dir_open_meta(fs_file->fs_info,
                    fs_file->name->meta_addr)) == NULL) {
                return 0;
            }

            has_children = fs_dir->names_used > 2;
        }
        key = String::NewFromUtf8(isolate, "hasChildren");
        obj->Set(key, Boolean::New(isolate, has_children));
    }
    
    return 1;
}

}
