#include "iterator.hpp"
#include <wiredtiger.h>
#include <stdio.h>

namespace NAC {
    class TFSMetaDBIteratorImpl {
    public:
        TFSMetaDBIteratorImpl() = delete;
        TFSMetaDBIteratorImpl(const TFSMetaDBIteratorImpl&) = delete;
        TFSMetaDBIteratorImpl(TFSMetaDBIteratorImpl&&) = delete;

        TFSMetaDBIteratorImpl(const std::shared_ptr<void>& data)
            : Cursor(data, (WT_CURSOR*)data.get())
        {
        }

        bool Next(TFSMetaDBModelBase& out) {
            int result;

            if (First) {
                First = false;
                result = Cursor->search(Cursor.get());

            } else {
                result = Cursor->next(Cursor.get());
            }

            if (result == 0) {
                WT_ITEM value;
                result = Cursor->get_value(Cursor.get(), &value);

                if (result == 0) {
                    out.Load((void*)Cursor->session, value.size, value.data);

                    return true;
                }

            } else if (result != WT_NOTFOUND) {
                dprintf(
                    2,
                    "Failed to get next record: %s\n",
                    wiredtiger_strerror(result)
                );
            }

            return false;
        }

    private:
        std::shared_ptr<WT_CURSOR> Cursor;
        bool First = true;
    };

    TFSMetaDBIterator::TFSMetaDBIterator(const std::shared_ptr<void>& data)
        : Impl(new TFSMetaDBIteratorImpl(data))
    {
    }

    TFSMetaDBIterator::~TFSMetaDBIterator() {
        delete Impl;
    }

    bool TFSMetaDBIterator::Next(TFSMetaDBModelBase& out) {
        return Impl->Next(out);
    }
}
