#include "smart_pointers.h"

namespace task {

    template <typename T>
    UniquePtr<T>::UniquePtr(T* ptr) {
        data = ptr;
    }

    template <typename T>
    UniquePtr<T>::UniquePtr(UniquePtr&& other) {
        this->data = other.data;
        other.data = nullptr;
    }

    template <typename T>
    UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr&& other) {
        reset(other.data);
        other.data = nullptr;

        return *this;
    }

    template <typename T>
    T* UniquePtr<T>::get() const {
        return data;
    }

    template <typename T>
    T& UniquePtr<T>::operator*() const {
        return *data;
    }

    template <typename T>
    T* UniquePtr<T>::operator->() const {
        return get();
    }

    template <typename T>
    T* UniquePtr<T>::get() {
        return data;
    }

    template <typename T>
    T& UniquePtr<T>::operator*() {
        return *data;
    }

    template <typename T>
    T* UniquePtr<T>::operator->() {
        return get();
    }

    template <typename T>
    T* UniquePtr<T>::release() {
        T* result = this->data;
        this->data = nullptr;
        return result;
    }

    template <typename T>
    void UniquePtr<T>::reset(T* ptr) {
        delete data;
        data = ptr;
    }

    template <typename T>
    void UniquePtr<T>::swap(UniquePtr& other) {
        T* temp_ptr = this->data;
        this->data = other.data;
        other.data = temp_ptr;
    }
    template <typename T>
    UniquePtr<T>::~UniquePtr() {
        delete data;
    }

    template <typename T>
    SharedPtr<T>::SharedPtr(T* ptr) {
        this->control_block = new ControlBlock<T>(ptr);
    }

    template <typename T>
    SharedPtr<T>::SharedPtr(const SharedPtr& other) {
        this->control_block = other.control_block;
        this->control_block->ref_count++;
    }

    template <typename T>
    SharedPtr<T>::SharedPtr(SharedPtr&& other) {
        this->control_block = other.control_block;
        other.control_block = nullptr;
    }

    template <typename T>
    SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other) {
        if (&other == this) {
            return *this;
        }
        if (this->control_block) {
            this->control_block->ref_count--;

            if (control_block->ref_count == 0) {
                delete control_block->ptr;
                control_block->ptr = nullptr;
            }

            if ((control_block->ref_count + control_block->weak_count) == 0) {
                delete this->control_block;
            }
        }

        this->control_block = other.control_block;
        this->control_block->ref_count++;

        return *this;
    }

    template <typename T>
    SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) {
        if (&other == this) {
            return *this;
        }
        if (this->control_block) {
            this->control_block->ref_count--;

            if (control_block->ref_count == 0) {
                delete control_block->ptr;
                control_block->ptr = nullptr;
            }

            if ((control_block->ref_count + control_block->weak_count) == 0) {
                delete this->control_block;
            }
        }

        this->control_block = other.control_block;
        other.control_block = nullptr;

        return *this;
    }

    template <typename T>
    SharedPtr<T>::SharedPtr(const WeakPtr<T>& other) {
        if (other.control_block) {
            this->control_block = other.control_block;
            this->control_block->ref_count++;
        } else {
            throw std::invalid_argument("Weak pointer should be non-empty");
        }
    }

    template <typename T>
    T* SharedPtr<T>::get() {
        return this->control_block->ptr;
    }

    template <typename T>
    T& SharedPtr<T>::operator*() {
        return *(this->control_block->ptr);
    }

    template <typename T>
    T* SharedPtr<T>::operator->() {
        return this->get();
    }

    template <typename T>
    size_t SharedPtr<T>::use_count() const {
        if (this->control_block->ptr) {
            return this->control_block->ref_count;
        } else {
            return 0;
        }
    }

    template <typename T>
    void SharedPtr<T>::reset(T* ptr) {
        this->control_block->ref_count--;

        if (control_block->ref_count == 0) {
            delete control_block->ptr;
            control_block->ptr = nullptr;
        }

        if (this->control_block->ref_count == 0 && this->control_block->weak_count == 0) {
            delete this->control_block;
        }

        this->control_block = new ControlBlock(ptr);
    }

    template <typename T>
    void SharedPtr<T>::swap(SharedPtr& other) {
        ControlBlock<T>* temp = this->control_block;
        this->control_block = other.control_block;
        other.control_block = temp;
    }

    template <typename T>
    SharedPtr<T>::~SharedPtr() {
        if (this->control_block) {
            control_block->ref_count--;

            if (control_block->ref_count == 0) {
                delete control_block->ptr;
                control_block->ptr = nullptr;
            }

            if ((control_block->ref_count + control_block->weak_count) == 0) {
                delete this->control_block;
            }
        }
    }

    template <typename T>
    WeakPtr<T>::WeakPtr() : WeakPtr(SharedPtr<T>()) {}

    template <typename T>
    WeakPtr<T>::WeakPtr(const SharedPtr<T>& other) {
        this->control_block = other.control_block;
        this->control_block->weak_count++;
    }

    template <typename T>
    WeakPtr<T>::WeakPtr(const WeakPtr& other) {
        this->control_block = other.control_block;
        this->control_block->weak_count++;
    }

    template <typename T>
    WeakPtr<T>::WeakPtr(WeakPtr&& other) {
        this->control_block = other.control_block;
        other.control_block = nullptr;
    }

    template <typename T>
    WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr& other) {
        if (this == &other) {
            return *this;
        }

        this->control_block->weak_count--;

        if ((this->control_block->ref_count + this->control_block->weak_count) == 0) {
            delete this->control_block;
        }

        this->control_block = other.control_block;
        this->control_block->weak_count++;

        return *this;
    }

    template <typename T>
    WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr&& other) {
        if (this == &other) {
            return *this;
        }

        this->control_block = other.control_block;
        other.control_block = nullptr;

        return *this;
    }

    template <typename T>
    WeakPtr<T>& WeakPtr<T>::operator=(const SharedPtr<T>& other) {
        this->control_block = other.control_block;
        this->control_block->weak_count++;

        return *this;
    }

    template <typename T>
    SharedPtr<T> WeakPtr<T>::lock() {
        if (expired()) {
            return SharedPtr<T>();
        } else {
            return SharedPtr<T>(*this);
        }
    }

    template <typename T>
    size_t WeakPtr<T>::use_count() {
        if (this->control_block) {
            return this->control_block->ref_count;
        } else {
            return 0;
        }
    }

    template <typename T>
    bool WeakPtr<T>::expired() {
        return use_count() == 0;
    }

    template <typename T>
    void WeakPtr<T>::reset() {
        this->control_block->weak_count--;

        if ((this->control_block->ref_count + this->control_block->weak_count) == 0) {
            delete this->control_block;
        }
    }

    template <typename T>
    void WeakPtr<T>::swap(WeakPtr& other) {
        ControlBlock<T> temp = this->control_block;
        this->control_block = other.control_block;
        other.control_block = temp;
    }

    template <typename T>
    WeakPtr<T>::~WeakPtr() {
        if (this->control_block) {
            this->control_block->weak_count--;

            if ((this->control_block->ref_count + this->control_block->weak_count) == 0) {
                delete control_block;
            }
        }
    }
}
